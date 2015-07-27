/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2015 UniPro <ugene@unipro.ru>
 * http://ugene.unipro.ru
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <QCoreApplication>
#include <QDir>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/GenbankFeatures.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/UserApplicationsSettings.h>

#include "ExtractProductTask.h"

namespace U2 {

QString ExtractProductTask::getProductName(const QString &sequenceName, qint64 sequenceLength, const U2Region &region, bool fileName) {
    qint64 endPos = region.endPos();
    if (endPos > sequenceLength) {
        endPos = endPos % sequenceLength;
    }

    return QString("%1%2%3-%4")
        .arg(sequenceName)
        .arg(fileName ? "_" : ":")
        .arg(region.startPos + 1)
        .arg(endPos);
}

ExtractProductTask::ExtractProductTask(const InSilicoPcrProduct &product, const U2EntityRef &sequenceRef, const QString &outputFile)
: Task(tr("Extract PCR product"), TaskFlags_FOSE_COSC), product(product), sequenceRef(sequenceRef), outputFile(outputFile), result(NULL)
{
    GCOUNTER(cvar, tvar, "ExtractProductTask");
}

ExtractProductTask::~ExtractProductTask() {
    delete result;
}

DNASequence ExtractProductTask::getProductSequence(U2OpStatus &os) const {
    DNASequence sequence = extractTargetSequence(os);
    CHECK_OP(os, sequence);
    sequence.seq = toProductSequence(sequence.seq, product.forwardPrimer, product.reversePrimer, product.forwardPrimerMatchLength, product.reversePrimerMatchLength);
    return sequence;
}

DNASequence ExtractProductTask::extractTargetSequence(U2OpStatus &os) const {
    DNASequence result("", "");
    DbiConnection connection(sequenceRef.dbiRef, os);
    CHECK_OP(os, result);
    SAFE_POINT_EXT(NULL != connection.dbi, os.setError(L10N::nullPointerError("DBI")), result);
    U2SequenceDbi *sequenceDbi = connection.dbi->getSequenceDbi();
    SAFE_POINT_EXT(NULL != sequenceDbi, os.setError(L10N::nullPointerError("Sequence DBI")), result);

    U2Sequence sequence = sequenceDbi->getSequenceObject(sequenceRef.entityId, os);
    CHECK_OP(os, result);

    result.seq = sequenceDbi->getSequenceData(sequenceRef.entityId, product.region, os);
    CHECK_OP(os, result);
    if (product.region.endPos() > sequence.length) {
        U2Region tail(0, product.region.endPos() % sequence.length);
        result.seq += sequenceDbi->getSequenceData(sequenceRef.entityId, tail, os);
        CHECK_OP(os, result);
    }

    result.setName(getProductName(sequence.visualName, sequence.length, product.region));
    return result;
}

QByteArray ExtractProductTask::toProductSequence(const QByteArray &targetSequence, const QByteArray &forwardPrimer, const QByteArray &reversePrimer, int forwardPrimerMatchLength, int reversePrimerMatchLength) {
    int startPos = forwardPrimerMatchLength;
    int length = targetSequence.length() - forwardPrimerMatchLength - reversePrimerMatchLength;
    return forwardPrimer + targetSequence.mid(startPos, length) + DNASequenceUtils::reverseComplement(reversePrimer);
}

SharedAnnotationData ExtractProductTask::getPrimerAnnotation(const QByteArray &primer, int matchLengh, U2Strand::Direction strand, int sequenceLength) {
    SharedAnnotationData result(new AnnotationData);
    U2Region region;
    if (U2Strand::Direct == strand) {
        region = U2Region(0, matchLengh);
    } else {
        region = U2Region(sequenceLength - matchLengh, matchLengh);
    }
    result->location->regions << region;
    result->location->strand = U2Strand(strand);

    result->name = GBFeatureUtils::getKeyInfo(GBFeatureKey_misc_feature).text;
    result->qualifiers << U2Qualifier(GBFeatureUtils::QUALIFIER_NOTE, "primer");
    return result;
}

void ExtractProductTask::run() {
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    SAFE_POINT_EXT(NULL != iof, setError(L10N::nullPointerError("IOAdapterFactory")), );

    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);
    SAFE_POINT_EXT(NULL != format, setError(L10N::nullPointerError("Genbank Format")), );
    QScopedPointer<Document> doc(format->createNewLoadedDocument(iof, outputFile, stateInfo));
    CHECK_OP(stateInfo, );

    U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo);
    CHECK_OP(stateInfo, );

    DNASequence productSequence = getProductSequence(stateInfo);
    CHECK_OP(stateInfo, );
    U2EntityRef productRef = U2SequenceUtils::import(dbiRef, productSequence, stateInfo);
    CHECK_OP(stateInfo, );

    U2SequenceObject *sequenceObject = new U2SequenceObject(productSequence.getName(), productRef);
    doc->addObject(sequenceObject);
    AnnotationTableObject *annotations = new AnnotationTableObject(productSequence.getName() + " features", dbiRef);
    annotations->addAnnotations(QList<SharedAnnotationData>() << getPrimerAnnotation(product.forwardPrimer, product.forwardPrimerMatchLength,
        U2Strand::Direct, productSequence.length()));
    annotations->addAnnotations(QList<SharedAnnotationData>() << getPrimerAnnotation(product.reversePrimer, product.reversePrimerMatchLength,
        U2Strand::Complementary, productSequence.length()));
    annotations->addObjectRelation(GObjectRelation(GObjectReference(sequenceObject), ObjectRole_Sequence));
    doc->addObject(annotations);

    result = doc.take();
}

Document * ExtractProductTask::takeResult() {
    CHECK(NULL != result, NULL);
    if (result->thread() != QCoreApplication::instance()->thread()) {
        result->moveToThread(QCoreApplication::instance()->thread());
    }
    Document *returnValue = result;
    result = NULL;
    return returnValue;
}

/************************************************************************/
/* ExtractProductWrapperTask */
/************************************************************************/
ExtractProductWrapperTask::ExtractProductWrapperTask(const InSilicoPcrProduct &product, const U2EntityRef &sequenceRef, const QString &sequenceName, qint64 sequenceLength)
: Task(tr("Extract PCR product and open document"), TaskFlags_NR_FOSE_COSC)
{
    prepareUrl(product, sequenceName, sequenceLength);
    CHECK_OP(stateInfo, );
    extractTask = new ExtractProductTask(product, sequenceRef, outputFile);
}

void ExtractProductWrapperTask::prepare() {
    addSubTask(extractTask);
}

QList<Task*> ExtractProductWrapperTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    CHECK(extractTask == subTask, result);
    SaveDocFlags flags;
    flags |= SaveDoc_OpenAfter;
    flags |= SaveDoc_DestroyAfter;
    flags |= SaveDoc_Overwrite;
    QFile::remove(outputFile);
    result << new SaveDocumentTask(extractTask->takeResult(), flags);
    return result;
}

Task::ReportResult ExtractProductWrapperTask::report() {
    if (extractTask->isCanceled()) {
        QFile::remove(outputFile);
    }
    return ReportResult_Finished;
}

void ExtractProductWrapperTask::prepareUrl(const InSilicoPcrProduct &product, const QString &sequenceName, qint64 sequenceLength) {
    // generate file name
    QString fileName = ExtractProductTask::getProductName(sequenceName, sequenceLength, product.region) + ".gb";
    QRegExp regExp("[^A-z0-9_\\-\\s\\.\\(\\)]");
    fileName.replace(regExp, "_");

    QString outputDir = AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath() + QDir::separator() + "pcr";
    QString url = GUrlUtils::prepareDirLocation(outputDir, stateInfo) + QDir::separator() + fileName;
    CHECK_OP(stateInfo, );

    outputFile = GUrlUtils::rollFileName(url, "_", QSet<QString>());

    // reserve file
    QFile file(outputFile);
    file.open(QIODevice::WriteOnly);
    file.close();
}

} // U2
