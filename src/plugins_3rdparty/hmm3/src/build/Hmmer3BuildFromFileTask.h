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

#ifndef _U2_HMMER3_BUILD_FROM_FILE_TASK_H_
#define _U2_HMMER3_BUILD_FROM_FILE_TASK_H_

#include "Hmmer3BuildTask.h"

namespace U2 {

class Document;

class Hmmer3BuildFromFileTask : public Task {
    Q_OBJECT
public:
    Hmmer3BuildFromFileTask(const Hmmer3BuildSettings &settings, const QString &msaUrl);
    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);
    QString generateReport() const;

    static QString generateReport(const Hmmer3BuildSettings &settings, const QString &msaUrl, const Task *task);

private:
    DocumentFormatId detectMsaFormat();
    QList<MAlignment> getLoadedAlignments(Document *doc);

private:
    Hmmer3BuildSettings settings;
    QString msaUrl;
    LoadDocumentTask *loadTask;
};

} // U2

#endif // _U2_HMMER3_BUILD_FROM_FILE_TASK_H_
