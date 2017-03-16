/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
 * http://ugene.net
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

#include <QVarLengthArray>

#include <U2Core/AppContext.h>
#include <U2Core/Timer.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

#include <U2Formats/BAMUtils.h>

#include "MysqlAssemblyDbi.h"
#include "util/MysqlMultiTableAssemblyAdapter.h"

namespace U2 {

MysqlAssemblyDbi::MysqlAssemblyDbi(GenericSqlDbi* dbi) : GenericSqlAssemblyDbi(dbi) {
}

MysqlAssemblyDbi::~MysqlAssemblyDbi() {
    SAFE_POINT(adaptersById.isEmpty(), "Adapters list is not empty", );
}

void MysqlAssemblyDbi::initSqlSchema(U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    // assembly object
    // reference            - reference sequence id
    // imethod - indexing method - method used to handle read location
    // cmethod - method used to handle compression of reads data
    // idata - additional indexing method data
    // cdata - additional compression method data
    U2SqlQuery("CREATE TABLE Assembly (object BIGINT PRIMARY KEY, reference BIGINT, imethod LONGTEXT NOT NULL,"
        " cmethod LONGTEXT NOT NULL, idata LONGBLOB, cdata LONGBLOB, "
        " FOREIGN KEY(object) REFERENCES Object(id), "
        " FOREIGN KEY(reference) REFERENCES Object(id) ON DELETE SET NULL ) "
        " ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
}

GenericSqlAssemblyAdapter* MysqlAssemblyDbi::getAdapter(const U2DataId& assemblyId, U2OpStatus& os) {
    qint64 dbiId = U2DbiUtils::toDbiId(assemblyId);
    GenericSqlAssemblyAdapter* res = adaptersById.value(dbiId);
    if (res != NULL) {
        return res;
    }

    static const QString qString = "SELECT imethod FROM Assembly WHERE object = :object";
    U2SqlQuery q(qString, db, os);
    q.bindDataId(":object", assemblyId);
    if (!q.step()) {
        os.setError(U2DbiL10n::tr("There is no assembly object with the specified id."));
        return NULL;
    }

    res = new MysqlMultiTableAssemblyAdapter(dbi, assemblyId, NULL, db, os);
//    const QString indexMethod = q.getString(0);
//    if (indexMethod == MYSQL_DBI_ASSEMBLY_READ_ELEN_METHOD_SINGLE_TABLE) {
//        res = new SingleTableAssemblyAdapter(dbi, assemblyId, 'S', "", NULL, db, os);
//    } else if (indexMethod == MYSQL_DBI_ASSEMBLY_READ_ELEN_METHOD_MULTITABLE_V1) {
//    } else if (indexMethod == MYSQL_DBI_ASSEMBLY_READ_ELEN_METHOD_RTREE) {
//        res = new RTreeAssemblyAdapter(dbi, assemblyId, NULL, db, os);
//    } else {
//        os.setError(U2DbiL10n::tr("Unsupported reads storage type: %1").arg(indexMethod));
//        return NULL;
//    }

    adaptersById[dbiId] = res;
    return res;
}

}   // namespace U2
