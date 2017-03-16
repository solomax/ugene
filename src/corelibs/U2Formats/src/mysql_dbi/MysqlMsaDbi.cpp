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

#include <U2Core/U2DbiPackUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

#include "MysqlMsaDbi.h"
#include "dbi/util/GenericSqlHelpers.h"

namespace U2 {

MysqlMsaDbi::MysqlMsaDbi(GenericSqlDbi* dbi) : GenericSqlMsaDbi(dbi)
{
}

MysqlMsaDbi::~MysqlMsaDbi() {}

void MysqlMsaDbi::initSqlSchema(U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    // MSA object
    U2SqlQuery("CREATE TABLE Msa (object BIGINT PRIMARY KEY, length BIGINT NOT NULL, "
        "alphabet TEXT NOT NULL, numOfRows INTEGER NOT NULL, "
        "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

     // MSA object row
     //   msa      - msa object id
     //   rowId    - id of the row in the msa
     //   sequence - sequence object id
     //   pos      - positional number of a row in the msa (initially, equals 'id', but can be changed, e.g. in GUI by moving rows)
     //   gstart   - offset of the first element in the sequence
     //   gend     - offset of the last element in the sequence (non-inclusive)
     //   length   - sequence and gaps length (trailing gap are not taken into account)
    U2SqlQuery("CREATE TABLE MsaRow (msa BIGINT NOT NULL, "
        "rowId BIGINT NOT NULL PRIMARY KEY AUTO_INCREMENT, "
        "sequence BIGINT NOT NULL, pos BIGINT NOT NULL, gstart BIGINT NOT NULL, "
        "gend BIGINT NOT NULL, length BIGINT NOT NULL, "
        "FOREIGN KEY(msa) REFERENCES Msa(object) ON DELETE CASCADE, "
        "FOREIGN KEY(sequence) REFERENCES Sequence(object) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    U2SqlQuery("CREATE INDEX MsaRow_msa_rowId ON MsaRow(msa, rowId)", db, os).execute();
    CHECK_OP(os, );
    U2SqlQuery("CREATE INDEX MsaRow_length ON MsaRow(length)", db, os).execute();
    CHECK_OP(os, );

     // Gap info for a MSA row:
     //   msa       - msa object id
     //   rowId     - id of the row in the msa
     //   gapStart  - start of the gap, the coordinate is relative to the gstart coordinate of the row
     //   gapEnd    - end of the gap, the coordinate is relative to the gstart coordinate of the row (non-inclusive)
     // Note! there is invariant: gend - gstart (of the row) == gapEnd - gapStart
    U2SqlQuery("CREATE TABLE MsaRowGap (msa BIGINT NOT NULL, rowId BIGINT NOT NULL, "
        "gapStart BIGINT NOT NULL, gapEnd BIGINT NOT NULL, "
        "FOREIGN KEY(rowId) REFERENCES MsaRow(rowId) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_OP(os, );

    U2SqlQuery("CREATE INDEX MsaRowGap_msa_rowId ON MsaRowGap(msa, rowId)", db, os).execute();
    CHECK_OP(os, );
}

}   // namespace U2
