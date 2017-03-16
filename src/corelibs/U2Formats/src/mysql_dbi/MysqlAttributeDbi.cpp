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

#include "MysqlAttributeDbi.h"
#include "dbi/util/GenericSqlHelpers.h"

namespace U2 {

MysqlAttributeDbi::MysqlAttributeDbi(GenericSqlDbi *dbi) : GenericSqlAttributeDbi(dbi) {}

MysqlAttributeDbi::~MysqlAttributeDbi() {}

void MysqlAttributeDbi::initSqlSchema( U2OpStatus &os ) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    // object attribute main table
    // object -> object id this attribute is for
    // child -> optional object id in case if this attribute shows relation between 2 objects
    // otype, ctype -> object and child types
    // oextra, cextra -> object and child db extra
    // version -> object version is attribute is valid for
    // name -> name of the attribute
    U2SqlQuery( "CREATE TABLE Attribute (id BIGINT PRIMARY KEY AUTO_INCREMENT, type INTEGER NOT NULL, "
        "object BIGINT, child BIGINT, otype INTEGER NOT NULL, ctype INTEGER, oextra LONGBLOB NOT NULL, "
        "cextra LONGBLOB, version BIGINT NOT NULL, name LONGTEXT NOT NULL, "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    //TODO: check if index is efficient for getting attribute for specific object
    U2SqlQuery("CREATE INDEX Attribute_object on Attribute(object)" , db, os).execute();

    U2SqlQuery("CREATE TABLE IntegerAttribute (attribute BIGINT, value BIGINT NOT NULL, "
        " FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8" , db, os).execute();
    U2SqlQuery("CREATE INDEX IntegerAttribute_attribute on IntegerAttribute(attribute)", db, os).execute();

    U2SqlQuery("CREATE TABLE RealAttribute (attribute BIGINT, value DOUBLE NOT NULL, "
        " FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    U2SqlQuery("CREATE INDEX RealAttribute_attribute on RealAttribute(attribute)" , db, os).execute();

    U2SqlQuery("CREATE TABLE StringAttribute (attribute BIGINT, value LONGTEXT NOT NULL, "
        " FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8" , db, os).execute();
    U2SqlQuery("CREATE INDEX StringAttribute_attribute on StringAttribute(attribute)" , db, os).execute();

    U2SqlQuery("CREATE TABLE ByteArrayAttribute (attribute BIGINT, value LONGBLOB NOT NULL, "
        " FOREIGN KEY(attribute) REFERENCES Attribute(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8" , db, os).execute();
    U2SqlQuery("CREATE INDEX ByteArrayAttribute_attribute on ByteArrayAttribute(attribute)" , db, os).execute();
}

}   // namespace U2
