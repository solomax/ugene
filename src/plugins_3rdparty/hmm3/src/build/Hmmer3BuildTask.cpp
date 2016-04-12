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
#include <QDir>
#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/SaveAlignmentTask.h>
#include <U2Core/UserApplicationsSettings.h>
#include <build/Hmmer3BuildFromFileTask.h>
#include <format/uHMMFormat.h>
#include <util/uhmm3Utilities.h>

#include "Hmmer3BuildTask.h"

namespace U2 {

Hmmer3BuildSettings::Hmmer3BuildSettings()
: loadProfile(false)
{
    setDefaultUHMM3BuildSettings(&inner);
}

Hmmer3BuildTask::Hmmer3BuildTask(const Hmmer3BuildSettings &settings, const MAlignment &msa)
: Task(tr("Build HMMER 3 profile"), TaskFlags_NR_FOSE_COSC | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled),
settings(settings), msa(msa), profile(NULL), saveTask(NULL), hmmerTask(NULL), loadTask(NULL)
{

}

Hmmer3BuildTask::~Hmmer3BuildTask() {
    if (NULL != profile) {
        p7_hmm_Destroy(profile);
    }
}

namespace {
    const QString HMMER3_TEMP_DIR = "HMMER3";

    QString getTaskTempDirName(const QString &prefix, Task *task) {
        return prefix + QString::number(task->getTaskId()) + "_" +
            QDate::currentDate().toString("dd.MM.yyyy") + "_" +
            QTime::currentTime().toString("hh.mm.ss.zzz") + "_" +
            QString::number(QCoreApplication::applicationPid());
    }
}

void Hmmer3BuildTask::prepare() {
    QString tempDirName = getTaskTempDirName("HMMER3_build_", this);
    QString tempDirPath = AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath(HMMER3_TEMP_DIR) + "/" + tempDirName;
    QString msaUrl = tempDirPath + "/msa.sto";
    if (settings.profileUrl.isEmpty()) {
        settings.profileUrl = tempDirPath + "/profile.hmm";
    }

    QDir tempDir(tempDirPath);
    if (tempDir.exists()){
        ExternalToolSupportUtils::removeTmpDir(tempDirPath, stateInfo);
        CHECK_OP(stateInfo, );
    }
    if (!tempDir.mkpath(tempDirPath)){
        setError(tr("Cannot create a directory for temporary files."));
        return;
    }

    saveTask = new SaveAlignmentTask(msa, msaUrl, BaseDocumentFormats::STOCKHOLM);
    saveTask->setSubtaskProgressWeight(5);
    addSubTask(saveTask);
}

QList<Task*> Hmmer3BuildTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    if (saveTask == subTask) {
        result << createBuildTask();
    } else if (hmmerTask == subTask) {
        if (!QFileInfo(settings.profileUrl).exists()) {
            setError(tr("A error occurred during HMM profile building. See the log for details"));
            return result;
        }
        if (settings.loadProfile) {
            IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(settings.profileUrl));
            loadTask = new LoadDocumentTask(UHMMFormat::UHHMER_FORMAT_ID, settings.profileUrl, iof);
            loadTask->setSubtaskProgressWeight(5);
            result << loadTask;
        }
    } else if (loadTask == subTask) {
        takeLoadedProfile(loadTask->getDocument());
    }
    return result;
}

QString Hmmer3BuildTask::generateReport() const {
    return Hmmer3BuildFromFileTask::generateReport(settings, "", this);
}

P7_HMM * Hmmer3BuildTask::takeHMM() {
    P7_HMM *result = profile;
    profile = NULL;
    return result;
}

void Hmmer3BuildTask::takeLoadedProfile(Document *doc) {
    QList<P7_HMM *> profiles = UHMM3Utilities::takeHmmsFromDocument(loadTask->getDocument(), stateInfo);
    if (profiles.isEmpty()) {
        setError(tr("Cannot read a profile"));
        return;
    }
    profile = profiles.takeFirst();

    if (!profiles.isEmpty()) { // it can't happen
        stateInfo.addWarning(tr("There are more than one HMM profile in the output file"));
        foreach(P7_HMM *profile, profiles) {
            p7_hmm_Destroy(profile);
        }
    }
}

Task * Hmmer3BuildTask::createBuildTask() {
    QStringList arguments;
    switch (settings.inner.archStrategy) {
    case p7_ARCH_FAST:
        arguments << "--fast";
        arguments << "--symfrac" << QString::number(settings.inner.symfrac);
        break;
    case p7_ARCH_HAND:
        arguments << "--hand";
        break;
    default:
        FAIL(tr("Unknown model construction strategy"), NULL);
    }
    switch (settings.inner.wgtStrategy) {
    case p7_WGT_NONE:
        arguments << "--wnone";
        break;
    case p7_WGT_GIVEN:
        arguments << "--wgiven";
        break;
    case p7_WGT_GSC:
        arguments << "--wgsc";
        break;
    case p7_WGT_PB:
        arguments << "--wpb";
        break;
    case p7_WGT_BLOSUM:
        arguments << "--wblosum";
        arguments << "--wid" << QString::number(settings.inner.wid);
        break;
    default:
        FAIL(tr("Unknown relative sequence weighting strategy"), NULL);
    }
    switch (settings.inner.effnStrategy) {
    case p7_EFFN_NONE:
        arguments << "--enone";
        break;
    case p7_EFFN_SET:
        arguments << "--eset" << QString::number(settings.inner.eset);
        break;
    case p7_EFFN_CLUST:
        arguments << "--eclust";
        arguments << "--eid" << QString::number(settings.inner.eid);
        break;
    case p7_EFFN_ENTROPY:
        arguments << "--eent";
        if (settings.inner.ere > 0) {
            arguments << "--ere" << QString::number(settings.inner.ere);
        }
        arguments << "--esigma" << QString::number(settings.inner.esigma);
        break;
    default:
        FAIL(tr("Unknown effective sequence weighting strategy"), NULL);
    }
    arguments << "--seed" << QString::number(settings.inner.seed);
    arguments << "--fragthresh" << QString::number(settings.inner.fragtresh);
    arguments << "--EmL" << QString::number(settings.inner.eml);
    arguments << "--EmN" << QString::number(settings.inner.emn);
    arguments << "--EvL" << QString::number(settings.inner.evl);
    arguments << "--EvN" << QString::number(settings.inner.evn);
    arguments << "--EfL" << QString::number(settings.inner.efl);
    arguments << "--EfN" << QString::number(settings.inner.efn);
    arguments << "--Eft" << QString::number(settings.inner.eft);
    arguments << settings.profileUrl << saveTask->getUrl();

    hmmerTask = new ExternalToolRunTask("HMMER 3 build", arguments, new ExternalToolLogParser());
    hmmerTask->setSubtaskProgressWeight(90);
    return hmmerTask;
}

} // U2
