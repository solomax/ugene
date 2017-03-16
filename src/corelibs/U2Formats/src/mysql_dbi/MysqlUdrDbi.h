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

#ifndef _U2_MYSQL_UDR_DBI_H_
#define _U2_MYSQL_UDR_DBI_H_

#include "dbi/GenericSqlUdrDbi.h"

namespace U2 {

class U2SqlQuery;

class MysqlUdrDbi : public GenericSqlUdrDbi {
public:
    MysqlUdrDbi(GenericSqlDbi *dbi);
    virtual ~MysqlUdrDbi();

protected:
    virtual void createTable(const UdrSchema *schema, U2OpStatus &os);
    virtual void createIndex(const UdrSchemaId &schemaId, const QStringList &fields, U2OpStatus &os);

    /* Utilities */
    virtual QString tableStartDef(const UdrSchemaId &schemaId);
    virtual QString fieldDef(const UdrSchema::FieldDesc &field);
    virtual QString foreignKeysDef(const UdrSchema *schema, U2OpStatus &os);
};

} // U2

#endif // _U2_MYSQL_UDR_DBI_H_
