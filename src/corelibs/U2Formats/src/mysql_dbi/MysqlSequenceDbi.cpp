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
#include <U2Core/U2SequenceUtils.h>

#include "dbi/util/GenericSqlHelpers.h"
#include "MysqlSequenceDbi.h"

namespace U2 {

MysqlSequenceDbi::MysqlSequenceDbi(GenericSqlDbi* dbi) : GenericSqlSequenceDbi(dbi)
{
}

MysqlSequenceDbi::~MysqlSequenceDbi() {}

void MysqlSequenceDbi::initSqlSchema(U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    // sequence object
    U2SqlQuery("CREATE TABLE Sequence (object BIGINT PRIMARY KEY, length BIGINT NOT NULL DEFAULT 0, alphabet TEXT NOT NULL, "
                            "circular TINYINT NOT NULL DEFAULT 0, "
                             "FOREIGN KEY(object) REFERENCES Object(id) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    // part of the sequence, starting with 'sstart'(inclusive) and ending at 'send'(not inclusive)
    U2SqlQuery("CREATE TABLE SequenceData (sequence BIGINT, sstart BIGINT NOT NULL, send BIGINT NOT NULL, data LONGBLOB NOT NULL, "
        "FOREIGN KEY(sequence) REFERENCES Sequence(object) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();

    U2SqlQuery("CREATE INDEX SequenceData_sequence_region on SequenceData(sstart, send)", db, os).execute();
}

}   // namespace U2
