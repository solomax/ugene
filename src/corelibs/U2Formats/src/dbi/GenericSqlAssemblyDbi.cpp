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

#include <QVarLengthArray>

#include <U2Core/AppContext.h>
#include <U2Core/Timer.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SqlHelpers.h>

#include <U2Formats/BAMUtils.h>

#include "GenericSqlAssemblyDbi.h"
#include "GenericSqlObjectDbi.h"
#include "util/GenericSqlMultiTableAssemblyAdapter.h"
#include "util/GenericSqlSingleTableAssemblyAdapter.h"

namespace U2 {

GenericSqlAssemblyDbi::GenericSqlAssemblyDbi(GenericSqlDbi* dbi) : U2AssemblyDbi(dbi), GenericSqlChildDbiCommon(dbi) {}

GenericSqlAssemblyDbi::~GenericSqlAssemblyDbi() {}

void GenericSqlAssemblyDbi::shutdown(U2OpStatus& os) {
    foreach(GenericSqlAssemblyAdapter* a, adaptersById.values()) {
        a->shutdown(os);
        delete a;
    }

    adaptersById.clear();
}

U2Assembly GenericSqlAssemblyDbi::getAssemblyObject(const U2DataId& assemblyId, U2OpStatus& os) {
    U2Assembly res;

    DBI_TYPE_CHECK(assemblyId, U2Type::Assembly, os, res);
    dbi->getObjectDbi()->getObject(res, assemblyId, os);

    CHECK_OP(os, res);

    U2SqlQuery q("SELECT Assembly.reference, Object.type, '' FROM Assembly, Object "
                " WHERE Assembly.object = :object AND Object.id = Assembly.reference", db, os);
    q.bindDataId(":object", assemblyId);
    if (q.step())  {
        res.referenceId = q.getDataIdExt(0);
        q.ensureDone();
    }

    return res;
}

qint64 GenericSqlAssemblyDbi::countReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os) {
    GTIMER(c2, t2, "GenericSqlAssemblyDbi::countReadsAt");
    GenericSqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return -1;
    }
    return a->countReads(r, os);
}


U2DbiIterator<U2AssemblyRead>* GenericSqlAssemblyDbi::getReads(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os, bool sortedHint) {
    GTIMER(c2, t2, "GenericSqlAssemblyDbi::getReadsAt");
    GenericSqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        return a->getReads(r, os, sortedHint);
    }
    return NULL;
}

U2DbiIterator<U2AssemblyRead>* GenericSqlAssemblyDbi::getReadsByRow(const U2DataId& assemblyId, const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) {
    GTIMER(c2, t2, "GenericSqlAssemblyDbi::getReadsAt");

    quint64 t0 = GTimer::currentTimeMicros();
    GenericSqlAssemblyAdapter* a = getAdapter(assemblyId, os);

    if ( a == NULL ) {
        return NULL;
    }

    U2DbiIterator<U2AssemblyRead>* res = a->getReadsByRow(r, minRow, maxRow, os);

    t2.stop();
    perfLog.trace(QString("Assembly: reads 2D select time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));

    return res;
}

U2DbiIterator<U2AssemblyRead>* GenericSqlAssemblyDbi::getReadsByName(const U2DataId& assemblyId, const QByteArray& name, U2OpStatus& os)  {
    GTIMER(c2, t2, "GenericSqlAssemblyDbi::getReadsByName");
    GenericSqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        return a->getReadsByName(name, os);
    }
    return NULL;
}


qint64 GenericSqlAssemblyDbi::getMaxPackedRow(const U2DataId& assemblyId, const U2Region& r, U2OpStatus& os) {
    quint64 t0 = GTimer::currentTimeMicros();

    GenericSqlAssemblyAdapter* a = getAdapter(assemblyId, os);

    if ( a == NULL ) {
        return -1;
    }
    qint64 res = a->getMaxPackedRow(r, os);

    perfLog.trace(QString("Assembly: get max packed row: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / (1000*1000)));
    return res;
}



qint64 GenericSqlAssemblyDbi::getMaxEndPos(const U2DataId& assemblyId, U2OpStatus& os) {
    quint64 t0 = GTimer::currentTimeMicros();

    GenericSqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return -1;
    }
    quint64 res = a->getMaxEndPos(os);

    perfLog.trace(QString("Assembly: get max end pos: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / (1000*1000)));
    return res;
}



void GenericSqlAssemblyDbi::createAssemblyObject(U2Assembly& assembly,
                                            const QString& folder,
                                            U2DbiIterator<U2AssemblyRead>* it,
                                            U2AssemblyReadsImportInfo& importInfo,
                                            U2OpStatus& os)
{
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    U2Object fakeObject;
    fakeObject.visualName = assembly.visualName;
    fakeObject.trackModType = assembly.trackModType;

    dbi->getSqlObjectDbi()->createObject(fakeObject, folder, U2DbiObjectRank_TopLevel, os);
    SAFE_POINT_OP(os, );

    assembly.id = U2DbiUtils::toU2DataId(U2DbiUtils::toDbiId(fakeObject.id), U2Type::Assembly, U2DbiUtils::toDbExtra(fakeObject.id));
    assembly.dbiId = fakeObject.dbiId;
    assembly.version = fakeObject.version;

    QString elenMethod = "multi-table-v1";
    //QString elenMethod = dbi->getProperty(Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_KEY, Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_RTREE, os);
//    QString elenMethod = dbi->getProperty(Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_KEY, Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_MULTITABLE_V1, os);
    //QString elenMethod = dbi->getProperty(Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_KEY, Mysql_DBI_ASSEMBLY_READ_ELEN_METHOD_SINGLE_TABLE, os);

    U2SqlQuery q("INSERT INTO Assembly(object, reference, imethod, cmethod) VALUES(:object, :reference, :imethod, :cmethod)", db, os);
    q.bindDataId(":object", assembly.id);
    q.bindDataId(":reference", assembly.referenceId);
    q.bindString(":imethod", elenMethod);
    q.bindString(":cmethod", "no-compression");
    q.insert();
    SAFE_POINT_OP(os,);

    GenericSqlAssemblyAdapter* a = getAdapter(assembly.id, os);
    SAFE_POINT_OP(os,);

    a->createReadsTables(os);
    SAFE_POINT_OP(os,);

    if (it != NULL) {
        addReads(a, it, importInfo, os);
        SAFE_POINT_OP(os,);
    }

    a->createReadsIndexes(os);
    SAFE_POINT_OP(os, );
}

void GenericSqlAssemblyDbi::finalizeAssemblyObject(U2Assembly &assembly, U2OpStatus &os) {
    U2OpStatusImpl correctTypeOs;
    U2OpStatusImpl removeObjectOs;

    correctAssemblyType(assembly, correctTypeOs);

    if (os.isCoR() || correctTypeOs.isCoR()) {
        dbi->getObjectDbi()->removeObject(assembly.id, true, removeObjectOs);
    }

    if (!os.isCoR() && correctTypeOs.isCoR()) {
        os.setError(correctTypeOs.getError());
    } else if (!os.isCoR() && removeObjectOs.isCoR()) {
        os.setError(removeObjectOs.getError());
    }
}

void GenericSqlAssemblyDbi::removeAssemblyData(const U2DataId &assemblyId, U2OpStatus &os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);
    CHECK_OP(os, );

    removeTables(assemblyId, os);
    CHECK_OP(os, );
    removeAssemblyEntry(assemblyId, os);
}

void GenericSqlAssemblyDbi::updateAssemblyObject(U2Assembly& assembly, U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery q("UPDATE Assembly SET reference = :reference WHERE object = :object", db, os);
    q.bindDataId(":reference", assembly.referenceId);
    q.bindDataId(":object", assembly.id);
    q.execute();

    SAFE_POINT_OP(os, );

    dbi->getSqlObjectDbi()->updateObject(assembly, os);
    SAFE_POINT_OP(os, );

    GenericSqlObjectDbi::incrementVersion(assembly.id, db, os);
    SAFE_POINT_OP(os, );
}

void GenericSqlAssemblyDbi::removeReads(const U2DataId& assemblyId, const QList<U2DataId>& rowIds, U2OpStatus& os){
    GenericSqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        a->removeReads(rowIds, os);
    }
}

void GenericSqlAssemblyDbi::addReads(GenericSqlAssemblyAdapter* a, U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os) {
    GTIMER(c2, t2, "GenericSqlAssemblyDbi::addReads");

    quint64 t0 = GTimer::currentTimeMicros();

    a->addReads(it, ii, os);

    t2.stop();
    perfLog.trace(QString("Assembly: %1 reads added in %2 seconds. Auto-packing: %3")
                  .arg(ii.nReads).arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)).arg(ii.packStat.readsCount > 0 ? "yes" : "no"));
}

void GenericSqlAssemblyDbi::removeTables(const U2DataId &assemblyId, U2OpStatus &os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);
    CHECK_OP(os, );

    AssemblyAdapter* adapter = getAdapter(assemblyId, os);
    CHECK(NULL != adapter, );
    adapter->dropReadsTables(os);
}

void GenericSqlAssemblyDbi::removeAssemblyEntry(const U2DataId &assemblyId, U2OpStatus &os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);
    CHECK_OP(os, );

    static const QString queryString("DELETE FROM Assembly WHERE object = :object");
    U2SqlQuery q(queryString, db, os);
    q.bindDataId(":object", assemblyId);
    q.execute();
}

void GenericSqlAssemblyDbi::correctAssemblyType(U2Assembly &assembly, U2OpStatus &os) {
    dbi->getSqlObjectDbi()->updateObjectType(assembly, os);
    SAFE_POINT_OP(os, );
}

void GenericSqlAssemblyDbi::addReads(const U2DataId& assemblyId, U2DbiIterator<U2AssemblyRead>* it, U2OpStatus& os) {
    GenericSqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a != NULL ) {
        U2AssemblyReadsImportInfo ii;
        addReads(a, it, ii, os);
    }
}


/**  Packs assembly rows: assigns packedViewRow value for every read in assembly */
void GenericSqlAssemblyDbi::pack(const U2DataId& assemblyId, U2AssemblyPackStat& stat, U2OpStatus& os) {
    GTIMER(c2, t2, "GenericSqlAssemblyDbi::pack");

    quint64 t0 = GTimer::currentTimeMicros();

    GenericSqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return;
    }
    stat.readsCount = a->countReads(U2_REGION_MAX, os);
    a->pack(stat, os);
    perfLog.trace(QString("Assembly: full pack time: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)));
}

void GenericSqlAssemblyDbi::calculateCoverage(const U2DataId& assemblyId, const U2Region& region, U2AssemblyCoverageStat& coverage, U2OpStatus& os) {
    GTIMER(c2, t2, "GenericSqlAssemblyDbi::calculateCoverage");

    quint64 t0 = GTimer::currentTimeMicros();

    GenericSqlAssemblyAdapter* a = getAdapter(assemblyId, os);
    if ( a == NULL ) {
        return;
    }
    a->calculateCoverage(region, coverage, os);
    perfLog.trace(QString("Assembly: full coverage calculation time for %2..%3: %1 seconds").arg((GTimer::currentTimeMicros() - t0) / float(1000*1000)).arg(region.startPos).arg(region.endPos()));
}

}   // namespace U2
