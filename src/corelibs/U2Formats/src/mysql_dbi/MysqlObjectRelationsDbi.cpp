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

#include <U2Core/U2ObjectTypeUtils.h>
#include <U2Core/U2SafePoints.h>

#include "dbi/util/GenericSqlHelpers.h"
#include "MysqlObjectRelationsDbi.h"

namespace U2 {

MysqlObjectRelationsDbi::MysqlObjectRelationsDbi(GenericSqlDbi *dbi)
    : GenericSqlObjectRelationsDbi( dbi )
{
}

MysqlObjectRelationsDbi::~MysqlObjectRelationsDbi() {}

void MysqlObjectRelationsDbi::initSqlSchema( U2OpStatus &os ) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery( "CREATE TABLE ObjectRelation (object BIGINT NOT NULL, "
        "reference BIGINT NOT NULL, role INTEGER NOT NULL, "
        "PRIMARY KEY(object, reference), "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE,"
        "FOREIGN KEY(reference) REFERENCES Object(id) ON DELETE CASCADE) "
        "ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os ).execute( );
    CHECK_OP( os, );

    U2SqlQuery( "CREATE INDEX ObjectRelationRole ON ObjectRelation(role)", db, os ).execute( );
}

} // namespace U2
