/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2014 UniPro <ugene@unipro.ru>
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

#include "CufflinksSupport.h"

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AssemblyObject.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/DbiDataStorage.h>

#include "tophat/TopHatSettings.h"

#include "CuffdiffSupportTask.h"

namespace U2 {

const QString CuffdiffSupportTask::outSubDirBaseName("cuffdiff_out");

CuffdiffSupportTask::CuffdiffSupportTask(const CuffdiffSettings &_settings)
: ExternalToolSupportTask(tr("Running Cuffdiff task"), TaskFlags_NR_FOSE_COSC), settings(_settings)
{
    diffTask = NULL;
}

namespace {
    QStringList prepareAssemblyUrlsArgs(bool groupBySamples, const QMap<QString, QStringList> &assemblyUrls) {
        QStringList result;

        if (groupBySamples) {
            result << "-L";
            result << QStringList(assemblyUrls.keys()).join(",");
            foreach (const QStringList &urls, assemblyUrls.values()) {
                result << urls.join(",");
            }
        } else {
            foreach (const QStringList &urls, assemblyUrls.values()) {
                result << urls;
            }
        }
        return result;
    }

    int getSamplesCount(bool groupBySamples, const QMap<QString, QStringList> &assemblyUrls) {
        if (groupBySamples) {
            return assemblyUrls.size();
        } else {
            QStringList allUrls;
            foreach (const QStringList &urls, assemblyUrls.values()) {
                allUrls << urls;
            }
            return allUrls.size();
        }
    }
}

void CuffdiffSupportTask::prepare() {
    int samplesCount = getSamplesCount(settings.groupBySamples, settings.assemblyUrls);
    CHECK_EXT(samplesCount >= 2,
        stateInfo.setError(tr("At least 2 sets of assemblies are required for Cuffdiff")), );

    setupWorkingDir();
    CHECK_OP(stateInfo, );

    settings.outDir = GUrlUtils::createDirectory(
                settings.outDir + "/" + outSubDirBaseName,
                "_", stateInfo);
    CHECK_OP(stateInfo, );

    Task *t = createTranscriptTask();
    CHECK_OP(stateInfo, );
    addSubTask(t);
}

void CuffdiffSupportTask::setupWorkingDir() {
    if (0 == QString::compare(settings.workingDir, "default", Qt::CaseInsensitive)) {
        workingDir = ExternalToolSupportUtils::createTmpDir(CUFFDIFF_TMP_DIR, stateInfo);
    } else {
        workingDir = ExternalToolSupportUtils::createTmpDir(settings.workingDir, CUFFDIFF_TMP_DIR, stateInfo);
    }
}

QList<Task*> CuffdiffSupportTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> tasks;
    if (saveTasks.contains(subTask)) {
        saveTasks.removeOne(subTask);
    }
    if (saveTasks.isEmpty()) {
        if (NULL == diffTask) {
            tasks << createCuffdiffTask();
        } else {
            addOutFiles();
        }
    }
    return tasks;
}

Task::ReportResult CuffdiffSupportTask::report() {
    settings.cleanup();
    return ReportResult_Finished;
}

Task * CuffdiffSupportTask::createTranscriptTask() {
    createTranscriptDoc();
    CHECK_OP(stateInfo, NULL);
    addTranscriptObject();

    SaveDocumentTask *t = new SaveDocumentTask(transcriptDoc.data(), transcriptDoc->getIOAdapterFactory(), transcriptUrl);
    saveTasks << t;
    return t;
}

Task * CuffdiffSupportTask::createCuffdiffTask() {
    // prepare arguments
    QStringList arguments;
    arguments << "-p" << QString::number(TopHatSettings::getThreadsCount());
    arguments << "--output-dir" << settings.outDir;
    if (settings.timeSeriesAnalysis) {
        arguments << "--time-series";
    }
    if (settings.upperQuartileNorm) {
        arguments << "--upper-quartile-norm";
    }
    if (CuffdiffSettings::Compatible == settings.hitsNorm) {
        arguments << "--compatible-hits-norm";
    } else if (CuffdiffSettings::Total == settings.hitsNorm) {
        arguments << "--total-hits-norm";
    }
    if (!settings.fragBiasCorrect.isEmpty()) {
        arguments << "--frag-bias-correct" << settings.fragBiasCorrect;
    }
    if (settings.multiReadCorrect) {
        arguments << "--multi-read-correct";
    }
    arguments << "--library-type";
    if (CuffdiffSettings::StandardIllumina == settings.libraryType) {
        arguments << "fr-unstranded";
    } else if (CuffdiffSettings::dUTP_NSR_NNSR == settings.libraryType) {
        arguments << "fr-firststrand";
    } else if (CuffdiffSettings::Ligation_StandardSOLiD == settings.libraryType) {
        arguments << "fr-secondstrand";
    }
    if (!settings.maskFile.isEmpty()) {
        arguments << "--mask-file" << settings.maskFile;
    }
    arguments << "--min-alignment-count" << QString::number(settings.minAlignmentCount);
    arguments << "--FDR" << QString::number(settings.fdr);
    arguments << "--max-mle-iterations" << QString::number(settings.maxMleIterations);
    if (settings.emitCountTables) {
        arguments << "--emit-count-tables";
    }

    arguments << transcriptUrl;
    arguments << prepareAssemblyUrlsArgs(settings.groupBySamples, settings.assemblyUrls);

    // create task
    logParser.reset(new LogParser());
    diffTask = new ExternalToolRunTask(ET_CUFFDIFF,
        arguments,
        logParser.data(),
        workingDir);
    setListenerForTask(diffTask);

    return diffTask;
}

void CuffdiffSupportTask::addFile(const QString &fileName) {
    ExternalToolSupportUtils::appendExistingFile(settings.outDir + "/" + fileName, outputFiles);
}

void CuffdiffSupportTask::addOutFiles() {
    addFile("splicing.diff");
    addFile("promoters.diff");
    addFile("cds.diff");
    addFile("cds_exp.diff");
    addFile("cds.fpkm_tracking");
    addFile("tss_group_exp.diff");
    addFile("tss_groups.fpkm_tracking");
    addFile("gene_exp.diff");
    addFile("genes.fpkm_tracking");
    addFile("isoform_exp.diff");
    addFile("isoforms.fpkm_tracking");
}

void CuffdiffSupportTask::createTranscriptDoc() {
    DocumentFormat *f = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::GTF);
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    transcriptUrl = workingDir + "/transcripts.gtf";
    transcriptDoc.reset(f->createNewLoadedDocument(iof, transcriptUrl, stateInfo));
    CHECK_OP(stateInfo, );
    transcriptDoc->setDocumentOwnsDbiResources(false);
}

void CuffdiffSupportTask::addTranscriptObject() {
    AnnotationTableObject *aobj = new AnnotationTableObject( "anns", transcriptDoc->getDbiRef( ) );
    bool first = true;
    foreach ( const AnnotationData& ann, settings.transcript ) {
        if (first) {
            aobj->setGObjectName( ann.name );
            first = false;
        }
        QStringList list;
        aobj->addAnnotation( ann );
    }
    transcriptDoc->addObject(aobj);
}

QStringList CuffdiffSupportTask::getOutputFiles() const {
    return outputFiles;
}

CuffdiffSupportTask::LogParser::LogParser()
: ExternalToolLogParser()
{

}

void CuffdiffSupportTask::LogParser::parseErrOutput(const QString &partOfLog) {
    ExternalToolLogParser::parseErrOutput(partOfLog);
    QString error = this->getLastError();
    if (!error.isEmpty()) {
        if (error.contains("found spliced alignment without XS attribute")) {
            setLastError("");
        }
    }
}

/************************************************************************/
/* CuffdiffSettings */
/************************************************************************/
CuffdiffSettings::CuffdiffSettings() {
    timeSeriesAnalysis = false;
    upperQuartileNorm = false;
    hitsNorm = Compatible;
    fragBiasCorrect = "";
    multiReadCorrect = false;
    libraryType = StandardIllumina;
    maskFile = "";
    minAlignmentCount = 10;
    fdr = 0.05;
    maxMleIterations = 5000;
    emitCountTables = false;
    storage = NULL;
    workingDir = "default";
    groupBySamples = false;
}

void CuffdiffSettings::cleanup() {
    transcript.clear();
}

} // U2
