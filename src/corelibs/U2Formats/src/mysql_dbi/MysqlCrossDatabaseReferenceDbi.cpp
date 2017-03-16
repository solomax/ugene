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

#include "MysqlCrossDatabaseReferenceDbi.h"
#include "dbi/util/GenericSqlHelpers.h"

namespace U2 {

MysqlCrossDatabaseReferenceDbi::MysqlCrossDatabaseReferenceDbi(GenericSqlDbi* dbi) :
    GenericSqlCrossDatabaseReferenceDbi(dbi)
{
}

MysqlCrossDatabaseReferenceDbi::~MysqlCrossDatabaseReferenceDbi() {}

void MysqlCrossDatabaseReferenceDbi::initSqlSchema(U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    // cross database reference object
    // factory - remote dbi factory
    // dbi - remote dbi id (url)
    // rid  - remote object id
    // version - remove object version
    U2SqlQuery("CREATE TABLE CrossDatabaseReference (object BIGINT, factory LONGTEXT NOT NULL, dbi TEXT NOT NULL, "
               "rid BLOB NOT NULL, version INTEGER NOT NULL, "
               " FOREIGN KEY(object) REFERENCES Object(id) ) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
}

}   // namespace U2
