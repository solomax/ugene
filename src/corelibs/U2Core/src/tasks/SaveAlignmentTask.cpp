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

#include <QFileInfo>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/GHints.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/MAlignmentImporter.h>
#include <U2Core/MAlignmentObject.h>

#include "SaveAlignmentTask.h"

namespace U2 {

SaveAlignmentTask::SaveAlignmentTask(const MAlignment& _ma, const QString& _fileName, DocumentFormatId _f, const QVariantMap& _hints)
: Task("", TaskFlag_None),
  ma(_ma),
  fileName(_fileName),
  hints(_hints),
  format(_f)
{
    GCOUNTER( cvar, tvar, "ExportAlignmentTask" );
    setTaskName(tr("Export alignment to '%1'").arg(QFileInfo(fileName).fileName()));
    setVerboseLogMode(true);

    if (ma.isEmpty()) {
        setError(tr("An alignment is empty"));
    }
}

void SaveAlignmentTask::run() {
    DocumentFormatRegistry* r = AppContext::getDocumentFormatRegistry();
    DocumentFormat* f = r->getFormatById(format);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(fileName));
    doc.reset(f->createNewLoadedDocument(iof, fileName, stateInfo));

    MAlignmentObject* obj = MAlignmentImporter::createAlignment(doc->getDbiRef(), ma, stateInfo);
    CHECK_OP(stateInfo, );

    GHints* docHints = doc->getGHints();
    foreach(const QString& key, hints.keys()){
        docHints->set(key, hints[key]);
    }

    doc->addObject(obj);
    f->storeDocument(doc.data(), stateInfo);
}

} // U2
