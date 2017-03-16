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

#include <U2Core/U2SqlHelpers.h>
#include <U2Core/U2SafePoints.h>

#include "MysqlVariantDbi.h"
#include "dbi/util/GenericSqlHelpers.h"

namespace U2 {

/********************************************************************/
/* MysqlVariantDbi */
/********************************************************************/

MysqlVariantDbi::MysqlVariantDbi(GenericSqlDbi* dbi) : GenericSqlVariantDbi(dbi)
{
}

MysqlVariantDbi::~MysqlVariantDbi() {}

void MysqlVariantDbi::initSqlSchema(U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    // Variant track object
    U2SqlQuery(" CREATE TABLE VariantTrack (object BIGINT PRIMARY KEY, sequence BIGINT, sequenceName TEXT NOT NULL,"
        " trackType INTEGER DEFAULT 1, fileHeader LONGTEXT,"
        " FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    // Variant element
    // track - Variant track object id
    // startPos - variation start position
    // endPos - variation end position
    // refData - reference sequence part
    // obsData - observed variation of the reference
    // comment - comment visible for user
    // publicId - identifier visible for user
    // additionalInfo - added for vcf4 support
    U2SqlQuery("CREATE TABLE Variant(id BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT, track BIGINT, startPos BIGINT, endPos BIGINT, "
        " refData BLOB NOT NULL, obsData BLOB NOT NULL, publicId TEXT NOT NULL, additionalInfo TEXT,"
        " FOREIGN KEY(track) REFERENCES VariantTrack(object) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
}

}   // namespace U2
