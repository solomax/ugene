/**
* UGENE - Integrated Bioinformatics Tools.
* Copyright (C) 2008-2016 UniPro <ugene@unipro.ru>
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
#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include "Hmmer3BuildFromFileTask.h"

namespace U2 {

Hmmer3BuildFromFileTask::Hmmer3BuildFromFileTask(const Hmmer3BuildSettings &settings, const QString &msaUrl)
: Task(tr("Build HMMER 3 profile from file"), TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
settings(settings), msaUrl(msaUrl), loadTask(NULL)
{

}

void Hmmer3BuildFromFileTask::prepare() {
    DocumentFormatId msaFormat = detectMsaFormat();
    CHECK_OP(stateInfo, );

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(msaUrl));
    loadTask = new LoadDocumentTask(msaFormat, msaUrl, iof);
    addSubTask(loadTask);
}

QList<Task*> Hmmer3BuildFromFileTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    if (loadTask == subTask) {
        Document *doc = loadTask->getDocument();
        SAFE_POINT(NULL != doc, L10N::nullPointerError("Document"), result);

        QList<MAlignment> msaList = getLoadedAlignments(doc);
        CHECK_OP(stateInfo, result);
        result << new Hmmer3BuildTask(settings, msaList.first());
    }
    return result;
}

QString Hmmer3BuildFromFileTask::generateReport() const {
    return generateReport(settings, msaUrl, this);
}

QString Hmmer3BuildFromFileTask::generateReport(const Hmmer3BuildSettings &settings, const QString &msaUrl, const Task *task) {
    QString res;

    res += "<table>";
    if( !msaUrl.isEmpty() ) {
        res += "<tr><td width=200><b>" + tr("Source alignment") + "</b></td><td>" + msaUrl + "</td></tr>";
    }
    res += "<tr><td><b>" + tr("Profile name") + "</b></td><td>" + settings.profileUrl + "</td></tr>";

    const UHMM3BuildSettings & bldSettings = settings.inner;

    res += "<tr><td><b>" + tr( "Options:" ) + "</b></td></tr>";
    res += "<tr><td><b>" + tr( "Model construction strategies" ) + "</b></td><td>";
    switch( bldSettings.archStrategy ) {
        case p7_ARCH_FAST: res += "fast"; break;
        case p7_ARCH_HAND: res += "hand"; break;
        default: assert( false );
    }
    res += "</td></tr>";

    res += "<tr><td><b>" + tr( "Relative model construction strategies" ) + "</b></td><td>";
    switch( bldSettings.wgtStrategy ) {
        case p7_WGT_GSC:    res += tr("Gerstein/Sonnhammer/Chothia tree weights"); break;
        case p7_WGT_BLOSUM: res += tr("Henikoff simple filter weights" ); break;
        case p7_WGT_PB:     res += tr("Henikoff position-based weights" ); break;
        case p7_WGT_NONE:   res += tr("No relative weighting; set all to 1" ); break;
        case p7_WGT_GIVEN:  res += tr("Weights given in MSA file" ); break;
        default: assert( false );
    }
    res += "</td></tr>";

    res += "<tr><td><b>" + tr( "Effective sequence weighting strategies" ) + "</b></td><td>";
    switch( bldSettings.effnStrategy ) {
        case p7_EFFN_ENTROPY:   res += tr( "adjust effective sequence number to achieve relative entropy target" ); break;
        case p7_EFFN_CLUST:     res += tr( "effective sequence number is number of single linkage clusters" ); break;
        case p7_EFFN_NONE:      res += tr( "no effective sequence number weighting: just use number of sequences" ); break;
        case p7_EFFN_SET:       res += tr( "set effective sequence number for all models to: %1" ).arg( bldSettings.eset ); break;
        default: assert( false );
    }
    res += "</td></tr>";

    if( task->hasError() ) {
        res += "<tr><td width=200><b>" + tr( "Task finished with error: '%1'" ).arg( task->getError() ) + "</b></td><td></td></tr>";
    }
    res += "</table>";

    return res;
}

DocumentFormatId Hmmer3BuildFromFileTask::detectMsaFormat() {
    DocumentFormatConstraints constraints;
    constraints.supportedObjectTypes += GObjectTypes::MULTIPLE_ALIGNMENT;
    constraints.checkRawData = true;
    constraints.rawData = IOAdapterUtils::readFileHeader(msaUrl);
    constraints.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    QList<DocumentFormatId> formats = AppContext::getDocumentFormatRegistry()->selectFormats(constraints);
    if (formats.isEmpty()) {
        setError(tr("Unrecognized input alignment file format"));
        return DocumentFormatId();
    }
    return formats.first();
}

QList<MAlignment> Hmmer3BuildFromFileTask::getLoadedAlignments(Document *doc) {
    QList<MAlignment> result;

    QList<GObject*> objects = doc->findGObjectByType(GObjectTypes::MULTIPLE_ALIGNMENT);
    if (objects.isEmpty()) {
        setError(tr("There are no multiple alignments in the file"));
        return result;
    }

    foreach (GObject *object, objects) {
        MAlignmentObject *msaObject = qobject_cast<MAlignmentObject*>(object);
        result << msaObject->getMAlignment();
    }
    return result;
}

} // U2
