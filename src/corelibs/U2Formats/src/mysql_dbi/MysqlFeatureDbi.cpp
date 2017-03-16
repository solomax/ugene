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

#include "MysqlFeatureDbi.h"
#include "dbi/util/GenericSqlHelpers.h"

namespace U2 {

MysqlFeatureDbi::MysqlFeatureDbi(GenericSqlDbi* dbi) : GenericSqlFeatureDbi(dbi)
{
}

MysqlFeatureDbi::~MysqlFeatureDbi() {}

void MysqlFeatureDbi::initSqlSchema(U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    //nameHash is used for better indexing
    U2SqlQuery("CREATE TABLE Feature (id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT, "
        "class INTEGER NOT NULL, type INTEGER NOT NULL, parent BIGINT, root BIGINT, nameHash INTEGER, name TEXT, "
        "sequence BIGINT, strand INTEGER NOT NULL DEFAULT 0, start BIGINT NOT NULL DEFAULT 0, "
        "len BIGINT NOT NULL DEFAULT 0, end BIGINT NOT NULL DEFAULT 0) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    U2SqlQuery("CREATE TABLE FeatureKey (id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT, feature BIGINT NOT NULL, "
        " name TEXT NOT NULL, value TEXT, "
        " FOREIGN KEY(feature) REFERENCES Feature(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    U2SqlQuery("CREATE TABLE AnnotationTable (object BIGINT PRIMARY KEY, rootId BIGINT NOT NULL, "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE, "
        "FOREIGN KEY(rootId) REFERENCES Feature(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    U2SqlQuery("CREATE INDEX FeatureRootIndex ON Feature(root, class)", db, os).execute();
    U2SqlQuery("CREATE INDEX FeatureParentIndex ON Feature(parent)", db, os).execute();
    U2SqlQuery("CREATE INDEX FeatureLocationIndex ON Feature(start, end)", db, os).execute();
    U2SqlQuery("CREATE INDEX FeatureNameIndex ON Feature(root, nameHash)", db, os).execute();

    //FeatureKey index
    U2SqlQuery("CREATE INDEX FeatureKeyIndex ON FeatureKey(feature)", db, os).execute();
}

}   // namespace U2
