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

#ifndef _U2_HMMER3_BUILD_TASK_H_
#define _U2_HMMER3_BUILD_TASK_H_

#include <U2Core/MAlignment.h>
#include <U2Core/Task.h>
#include <hmmer3/hmmer.h>

namespace U2 {

class Document;
class ExternalToolRunTask;
class LoadDocumentTask;
class SaveAlignmentTask;

class Hmmer3BuildSettings {
public:
    Hmmer3BuildSettings();

    UHMM3BuildSettings inner;
    QString profileUrl;
    bool loadProfile;
};

class Hmmer3BuildTask : public Task {
    Q_OBJECT
public:
    Hmmer3BuildTask(const Hmmer3BuildSettings &settings, const MAlignment &msa);
    ~Hmmer3BuildTask();

    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);
    QString generateReport() const;
    P7_HMM * takeHMM();

private:
    void takeLoadedProfile(Document *doc);
    Task * createBuildTask();

private:
    Hmmer3BuildSettings settings;
    MAlignment msa;
    P7_HMM *profile;
    SaveAlignmentTask *saveTask;
    ExternalToolRunTask *hmmerTask;
    LoadDocumentTask *loadTask;
};

} // U2

#endif // _U2_HMMER3_BUILD_TASK_H_
