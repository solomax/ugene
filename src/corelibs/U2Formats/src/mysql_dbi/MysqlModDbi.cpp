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

#include <QtCore/QCoreApplication>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "dbi/util/GenericSqlHelpers.h"
#include "MysqlModDbi.h"

namespace U2 {

/************************************************************************/
/* MysqlModDbi                                                          */
/************************************************************************/

MysqlModDbi::MysqlModDbi(GenericSqlDbi *dbi) : GenericSqlModDbi(dbi) {
}

MysqlModDbi::~MysqlModDbi() {}

void MysqlModDbi::initSqlSchema(U2OpStatus &os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    // UserModStep - user modification steps
    //   id                     - id of the user modifications step
    //   object, otype, oextra  - data id of the master object (i.e. object for which "undo/redo" was initiated)
    //   version                - master object was modified from this version
    U2SqlQuery("CREATE TABLE UserModStep (id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT,"
        " object BIGINT NOT NULL,"
        " otype INTEGER NOT NULL,"
        " oextra LONGBLOB NOT NULL,"
        " version LONGBLOB NOT NULL, "
        " FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    // MultiModStep - multiple modifications step with reference to a user modifications step
    //   id          - id of the multiple modifications step
    //   userStepId  - id of the user modifications step
    U2SqlQuery("CREATE TABLE MultiModStep (id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT,"
        " userStepId BIGINT NOT NULL,"
        " FOREIGN KEY(userStepId) REFERENCES UserModStep(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    // SingleModStep - single modification of a dbi object
    //   id                    - id of the modification
    //   object, otype, oextra - data id of the object that was modified
    //   version               - this is a modification from 'version' to 'version + 1' of the object
    //   modType               - type of the object modification
    //   details               - detailed description of the object modification
    //   multiStepId           - id of the multiModStep
    U2SqlQuery("CREATE TABLE SingleModStep (id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT,"
        " object BIGINT NOT NULL,"
        " otype INTEGER NOT NULL,"
        " oextra LONGBLOB NOT NULL,"
        " version BIGINT NOT NULL,"
        " modType INTEGER NOT NULL,"
        " details LONGTEXT NOT NULL,"
        " multiStepId BIGINT NOT NULL, "
        " FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE, "
        " FOREIGN KEY(multiStepId) REFERENCES MultiModStep(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    U2SqlQuery("CREATE INDEX SingleModStep_object ON SingleModStep(object)", db, os).execute();
    U2SqlQuery("CREATE INDEX SingleModStep_object_version ON SingleModStep(object, version)", db, os).execute();
}

}   // namespace U2
