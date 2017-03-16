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

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UdrSchemaRegistry.h>

#include "dbi/util/GenericSqlHelpers.h"
#include "MysqlUdrDbi.h"

static const QString PLACEHOLDER_MARK = ":";

namespace U2 {

MysqlUdrDbi::MysqlUdrDbi(GenericSqlDbi *dbi) : GenericSqlUdrDbi(dbi)
{
}

MysqlUdrDbi::~MysqlUdrDbi() {}

/************************************************************************/
/* SQL initialization */
/************************************************************************/
void MysqlUdrDbi::createTable(const UdrSchema *schema, U2OpStatus &os) {
    CHECK_EXT(schema->size() > 0, os.setError("Empty schema"), );

    QString query = tableStartDef(schema->getId());
    CHECK_OP(os, );
    for (int i=0; i<schema->size(); i++) {
        UdrSchema::FieldDesc field = schema->getField(i, os);
        CHECK_OP(os, );
        query += ", " + fieldDef(field);
        CHECK_OP(os, );
    }
    query += foreignKeysDef(schema, os);
    CHECK_OP(os, );
    query += ") ENGINE=InnoDB DEFAULT CHARSET=utf8";

    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery(query, db, os).execute();
}

void MysqlUdrDbi::createIndex(const UdrSchemaId &schemaId, const QStringList &fields, U2OpStatus &os) {
    QString query = "CREATE INDEX "
        + tableName(schemaId) + "_" + fields.join("_") + " "
        + "on " + tableName(schemaId)
        + "("
        + fields.join(", ")
        + ")";

    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery(query, db, os).execute();
}

/************************************************************************/
/* Utilities */
/************************************************************************/
QString MysqlUdrDbi::tableStartDef(const UdrSchemaId &schemaId) {
    return "CREATE TABLE " + tableName(schemaId) + " (" +
        UdrSchema::RECORD_ID_FIELD_NAME + " BIGINT PRIMARY KEY AUTO_INCREMENT";
}

QString MysqlUdrDbi::fieldDef(const UdrSchema::FieldDesc &field) {
    QString def = field.getName() + " ";
    switch (field.getDataType()) {
        case UdrSchema::INTEGER:
            def += "BIGINT";
            break;
        case UdrSchema::DOUBLE:
            def += "DOUBLE";
            break;
        case UdrSchema::STRING:
            def += "LONGTEXT";
            break;
        case UdrSchema::BLOB:
            def += "LONGBLOB";
            break;
        case UdrSchema::ID:
            def += "BIGINT NOT NULL";
            break;
        default:
            FAIL("Unknown UDR data type detected!", QString());
    }
    return def;
}

QString MysqlUdrDbi::foreignKeysDef(const UdrSchema *schema, U2OpStatus &os) {
    QString result;

    for (int i=0; i<schema->size(); i++) {
        const UdrSchema::FieldDesc field = schema->getField(i, os);
        CHECK_OP(os, "");

        if (UdrSchema::ID == field.getDataType()) {
            result += ", FOREIGN KEY(" + field.getName() + ") REFERENCES Object(id) ON DELETE CASCADE";
        }
    }

    return result;
}

} // U2
