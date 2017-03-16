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

#include <QCryptographicHash>
#include <QSqlError>
#include <QSqlQuery>

#include <U2Core/Folder.h>
#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

#include "MysqlObjectDbi.h"
#include "dbi/util/GenericSqlHelpers.h"

namespace U2 {

MysqlObjectDbi::MysqlObjectDbi(GenericSqlDbi* dbi) : GenericSqlObjectDbi(dbi) {}

MysqlObjectDbi::~MysqlObjectDbi() {}

void MysqlObjectDbi::initSqlSchema(U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    // objects table - stores IDs and types for all objects. It also stores 'top_level' flag to simplify queries
    // rank: see U2DbiObjectRank
    // name is a visual name of the object shown to user.
    U2SqlQuery("CREATE TABLE Object (id BIGINT PRIMARY KEY AUTO_INCREMENT, type INTEGER NOT NULL, "
                                    "version BIGINT NOT NULL DEFAULT 1, rank INTEGER NOT NULL, "
                                    "name TEXT NOT NULL, trackMod INTEGER NOT NULL DEFAULT 0) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    U2SqlQuery("CREATE TABLE ObjectAccessTrack (object BIGINT PRIMARY KEY, lastAccessTime TIMESTAMP, "
                "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    // parent-child object relation
    U2SqlQuery("CREATE TABLE Parent (parent BIGINT, child BIGINT, "
                       "PRIMARY KEY (parent, child), "
                       "FOREIGN KEY(parent) REFERENCES Object(id) ON DELETE CASCADE, "
                       "FOREIGN KEY(child) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    U2SqlQuery("CREATE INDEX Parent_parent_child on Parent(parent, child)", db, os).execute();
    U2SqlQuery("CREATE INDEX Parent_child on Parent(child)", db, os).execute();
    CHECK_OP(os, );

    // folders
    U2SqlQuery("CREATE TABLE Folder (id BIGINT PRIMARY KEY AUTO_INCREMENT, path LONGTEXT NOT NULL, hash VARCHAR(32) UNIQUE NOT NULL, previousPath LONGTEXT, "
               "vlocal BIGINT NOT NULL DEFAULT 1, vglobal BIGINT NOT NULL DEFAULT 1) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    // folder-object relation
    U2SqlQuery("CREATE TABLE FolderContent (folder BIGINT, object BIGINT, "
              "PRIMARY KEY (folder, object), "
              "FOREIGN KEY(folder) REFERENCES Folder(id) ON DELETE CASCADE,"
              "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );
    U2SqlQuery("CREATE INDEX FolderContent_object on FolderContent(object)", db, os).execute();
    CHECK_OP(os, );

    createFolder(ROOT_FOLDER, os);
    CHECK_OP(os, );
    createFolder(PATH_SEP + RECYCLE_BIN_FOLDER, os);
}

}   // namespace U2
