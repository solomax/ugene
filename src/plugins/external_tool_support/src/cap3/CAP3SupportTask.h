/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2011 UniPro <ugene@unipro.ru>
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

#ifndef _U2_CLUSTALW_SUPPORT_TASK_H
#define _U2_CLUSTALW_SUPPORT_TASK_H

#include <QtCore/QFile>

#include <U2Core/Task.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include "utils/ExportTasks.h"

#include <U2Core/MAlignmentObject.h>

#include "ExternalToolRunTask.h"

namespace U2 {

class CAP3LogParser;

class CAP3SupportTaskSettings {
public:
    CAP3SupportTaskSettings() {}
    QString inputFilePath, outputFilePath;
};

class PrepareInputForCAP3Task : public Task {
    Q_OBJECT
public:
    PrepareInputForCAP3Task(const QStringList& inputFiles, const QString& outputDirPath);
    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    QList<LoadDocumentTask*> loadTasks;
    QStringList inputUrls;
    QString ouputUrl;
    int numSequences;
};


class CAP3SupportTask : public Task {
    Q_OBJECT
public:
    CAP3SupportTask(const CAP3SupportTaskSettings& settings);
    void prepare();
    Task::ReportResult report();
    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    MAlignmentObject*           mAObject;
    QString                     outputUrl;
    Document*                   newDoc;
    LoadDocumentTask*           loadTmpDocumentTask;
    ExternalToolRunTask*        cap3Task;
    CAP3LogParser*              logParser;
    CAP3SupportTaskSettings     settings;
};

class CAP3LogParser : public ExternalToolLogParser {
public:
    CAP3LogParser();
    int getProgress();
private:
    int countSequencesInMSA;
};

}//namespace
#endif // _U2_CLUSTALW_SUPPORT_TASK_H
