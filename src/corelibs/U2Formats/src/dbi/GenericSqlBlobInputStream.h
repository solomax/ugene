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

#ifndef _U2_GENERIC_SQL_BLOB_INPUT_STREAM_H_
#define _U2_GENERIC_SQL_BLOB_INPUT_STREAM_H_

#include <U2Core/InputStream.h>

namespace U2 {

class GenericSqlDbRef;

class GenericSqlBlobInputStream : public InputStream {
    Q_DISABLE_COPY(GenericSqlBlobInputStream)
public:
    GenericSqlBlobInputStream(GenericSqlDbRef *db, const QByteArray &tableId, const QByteArray &columnId,
        const U2DataId &rowId, U2OpStatus &os);

    qint64 available();
    void close();
    int read(char *buffer, int length, U2OpStatus &os);
    qint64 skip(qint64 n, U2OpStatus &os);

protected:
    QByteArray blobData;
    qint64 size;
    qint64 offset;
};

} // U2

#endif // _U2_GENERIC_SQL_BLOB_INPUT_STREAM_H_

