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

#include <U2Core/U2SafePoints.h>
#include "MysqlSingleTableAssemblyAdapter.h"

namespace U2 {

MysqlSingleTableAssemblyAdapter::MysqlSingleTableAssemblyAdapter(GenericSqlDbi* dbi,
                                                       const U2DataId& assemblyId,
                                                       char tablePrefix,
                                                       const QString& tableSuffix,
                                                       const AssemblyCompressor* compressor,
                                                       GenericSqlDbRef* db,
                                                       U2OpStatus& st) :
    GenericSqlSingleTableAssemblyAdapter(dbi, assemblyId, tablePrefix, tableSuffix, compressor, db, st)
{
}

void MysqlSingleTableAssemblyAdapter::createReadsTables(U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    // id - id of the read
    // name - read name hash
    // prow - packed view row
    // gstart - start of the read
    // elen - effective length of the read
    // flags - read flags
    // mq - mapping quality
    // data - packed data: CIGAR, read sequence, quality string
    static QString q = "CREATE TABLE IF NOT EXISTS %1 (id BIGINT PRIMARY KEY AUTO_INCREMENT, name BIGINT NOT NULL, prow BIGINT NOT NULL, "
        "gstart BIGINT NOT NULL, elen BIGINT NOT NULL, flags BIGINT NOT NULL, mq TINYINT UNSIGNED NOT NULL, data LONGBLOB NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8";

    U2SqlQuery(q.arg(readsTable), db, os).execute();
    CHECK_OP(os, );
    inited = true;
}

//! %1 - database, %2 - table name, %3 - index name, %4 - index column
//! Example of working query:
// select if (EXISTS(SELECT distinct index_name FROM INFORMATION_SCHEMA.STATISTICS
// WHERE table_schema = 'uu' AND table_name = 'AssemblyRead_M2_800_4000_0' and index_name like 'AssemblyRead_M2_800_4000_0_name'),
// "SELECT id FROM AssemblyRead_M2_800_4000_0;", "CREATE INDEX AssemblyRead_M2_800_4000_0_name ON uu.AssemblyRead_M2_800_4000_0(name);") into @a;
// prepare smt from @a; execute smt; deallocate prepare smt;
static const QString CREATE_INDEX_IF_NOT_EXISTS_QUERY =
        "select if"
        "("
            "EXISTS(SELECT distinct index_name FROM INFORMATION_SCHEMA.STATISTICS "
            "WHERE table_schema = '%1' AND table_name = '%2' and index_name like '%3')"
        ","
            "\"SELECT %4 FROM %2;\""
        ","
            "\"CREATE INDEX %3 ON %1.%2(%4);\""
        ") into @a; prepare smt from @a; execute smt; deallocate prepare smt;";

void MysqlSingleTableAssemblyAdapter::createReadsIndexes(U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery(CREATE_INDEX_IF_NOT_EXISTS_QUERY.arg(db->handle.databaseName())
               .arg(readsTable).arg(readsTable + "_gstart").arg("gstart"), db, os).execute();
    CHECK_OP(os, );

    U2SqlQuery(CREATE_INDEX_IF_NOT_EXISTS_QUERY.arg(db->handle.databaseName())
               .arg(readsTable).arg(readsTable + "_name").arg("name"), db, os).execute();
}

} //namespace
