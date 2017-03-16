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

#include "GenericSqlCrossDatabaseReferenceDbi.h"
#include "GenericSqlObjectDbi.h"
#include "util/GenericSqlHelpers.h"

namespace U2 {

GenericSqlCrossDatabaseReferenceDbi::GenericSqlCrossDatabaseReferenceDbi(GenericSqlDbi* dbi) :
    U2CrossDatabaseReferenceDbi(dbi),
    GenericSqlChildDbiCommon(dbi)
{
}

GenericSqlCrossDatabaseReferenceDbi::~GenericSqlCrossDatabaseReferenceDbi() {}

void GenericSqlCrossDatabaseReferenceDbi::createCrossReference(U2CrossDatabaseReference& reference, const QString& folder, U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    dbi->getSqlObjectDbi()->createObject(reference, folder, U2DbiObjectRank_TopLevel, os);
    CHECK_OP(os, );

    static const QString queryString = "INSERT INTO CrossDatabaseReference(object, factory, dbi, rid, version) VALUES(:object, :factory, :dbi, :rid, :version)";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", reference.id);
    q.bindString(":factory", reference.dataRef.dbiRef.dbiFactoryId);
    q.bindString(":dbi", reference.dataRef.dbiRef.dbiId);
    q.bindBlob(":rid", reference.dataRef.entityId);
    q.bindInt64(":version", reference.dataRef.version);
    q.execute();
}

void GenericSqlCrossDatabaseReferenceDbi::removeCrossReferenceData(const U2DataId &referenceId, U2OpStatus &os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "DELETE FROM CrossDatabaseReference WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", referenceId);
    q.execute();
}

U2CrossDatabaseReference GenericSqlCrossDatabaseReferenceDbi::getCrossReference(const U2DataId& objectId, U2OpStatus& os) {
    U2CrossDatabaseReference res(objectId, dbi->getDbiId(), 0);

    static const QString queryString = "SELECT r.factory, r.dbi, r.rid, r.version, o.name, o.version FROM CrossDatabaseReference AS r, Object AS o WHERE o.id = :id AND r.object = o.id";
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":id", objectId);
    if (q.step())  {
        res.dataRef.dbiRef.dbiFactoryId= q.getString(0);
        res.dataRef.dbiRef.dbiId = q.getString(1);
        res.dataRef.entityId = q.getBlob(2);
        res.dataRef.version = q.getInt64(3);
        res.visualName = q.getString(4);
        res.version = q.getInt64(5);
        q.ensureDone();
    }

    return res;
}

void GenericSqlCrossDatabaseReferenceDbi::updateCrossReference(const U2CrossDatabaseReference& reference, U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString queryString = "UPDATE CrossDatabaseReference SET factory = :factory, dbi = :dbi, rid = :rid, version = :version WHERE object = :object";
    U2SqlQuery q(queryString, db, os);
    q.bindString(":factory", reference.dataRef.dbiRef.dbiFactoryId);
    q.bindString(":dbi", reference.dataRef.dbiRef.dbiId);
    q.bindBlob(":rid", reference.dataRef.entityId);
    q.bindInt64(":version", reference.dataRef.version);
    q.bindDataId(":object", reference.id);
    q.execute();
}

}   // namespace U2
