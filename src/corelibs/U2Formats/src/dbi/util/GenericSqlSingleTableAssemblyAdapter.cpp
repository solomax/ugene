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

#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2SafePoints.h>

#include "GenericSqlAssemblyUtils.h"
#include "GenericSqlHelpers.h"
#include "GenericSqlSingleTableAssemblyAdapter.h"
#include "dbi/GenericSqlDbi.h"
#include "dbi/GenericSqlObjectDbi.h"

namespace U2 {

// :end -> end of the region
// :start -> start of the region  (- max read size)
// :realStart -> real start of the region
const QString GenericSqlSingleTableAssemblyAdapter::DEFAULT_RANGE_CONDITION_CHECK = " (gstart < :end AND gstart + elen > :start) ";
const QString GenericSqlSingleTableAssemblyAdapter::RTM_RANGE_CONDITION_CHECK = " ((gstart < :end AND gstart > :start) AND gstart + elen > :realStart) ";
const QString GenericSqlSingleTableAssemblyAdapter::RTM_RANGE_CONDITION_CHECK_COUNT = "  (gstart < :end AND gstart > :start) ";
const QString GenericSqlSingleTableAssemblyAdapter::ALL_READ_FIELDS = " id, prow, gstart, elen, flags, mq, data";
const QString GenericSqlSingleTableAssemblyAdapter::SORTED_READS = " ORDER BY gstart ASC ";

GenericSqlSingleTableAssemblyAdapter::GenericSqlSingleTableAssemblyAdapter(GenericSqlDbi* dbi,
                                                       const U2DataId& assemblyId,
                                                       char tablePrefix,
                                                       const QString& tableSuffix,
                                                       const AssemblyCompressor* compressor,
                                                       GenericSqlDbRef* db,
                                                       U2OpStatus& ) :
    GenericSqlAssemblyAdapter(assemblyId, compressor, db),
    dbi(dbi),
    readsTable(getReadsTableName(assemblyId, tablePrefix, tableSuffix)),
    rangeConditionCheck(DEFAULT_RANGE_CONDITION_CHECK),
    rangeConditionCheckForCount(DEFAULT_RANGE_CONDITION_CHECK),
    minReadLength(0),
    maxReadLength(0),
    rangeMode(false),
    inited(false)
{
}

qint64 GenericSqlSingleTableAssemblyAdapter::countReads(const U2Region& r, U2OpStatus& os) {
    if (r == U2_REGION_MAX) {
        return U2SqlQuery(QString("SELECT COUNT(*) FROM %1").arg(readsTable), db, os).selectInt64();
    }

    static const QString qStr = "SELECT COUNT(*) FROM %1 WHERE %2";
    U2SqlQuery q(qStr.arg(readsTable).arg(rangeConditionCheckForCount), db, os);
    bindRegion(q, r, true);
    return q.selectInt64();
}

qint64 GenericSqlSingleTableAssemblyAdapter::countReadsPrecise(const U2Region& r, U2OpStatus& os) {
    if (!rangeMode) {
        return countReads(r, os);
    }

    //here we use not-optimized rangeConditionCheck but not rangeConditionCheckForCount
    static const QString qStr = "SELECT COUNT(*) FROM %1 WHERE %2";
    U2SqlQuery q(qStr.arg(readsTable).arg(rangeConditionCheck), db, os);
    bindRegion(q, r, false);
    return q.selectInt64();
}

qint64 GenericSqlSingleTableAssemblyAdapter::getMaxPackedRow(const U2Region& r, U2OpStatus& os) {
    static const QString queryString = "SELECT MAX(prow) FROM %1 WHERE %2";
    U2SqlQuery q(queryString.arg(readsTable).arg(rangeConditionCheck), db, os);
    bindRegion(q, r);
    return q.selectInt64();
}

qint64 GenericSqlSingleTableAssemblyAdapter::getMaxEndPos(U2OpStatus& os) {
    static const QString queryString = "SELECT MAX(gstart + elen) FROM %1";
    return U2SqlQuery(queryString.arg(readsTable), db, os).selectInt64();
}

U2DbiIterator<U2AssemblyRead>* GenericSqlSingleTableAssemblyAdapter::getReads(const U2Region& r, U2OpStatus& os, bool sortedHint) {
    static const QString qStr = "SELECT " + ALL_READ_FIELDS + " FROM %1 WHERE %2";

    QSharedPointer<U2SqlQuery> q (new U2SqlQuery(qStr.arg(readsTable).arg(rangeConditionCheck) + (sortedHint ? SORTED_READS : ""), db, os));
    bindRegion(*q, r);
    return new GenericSqlRSIterator<U2AssemblyRead>(q, new GenericSqlSimpleAssemblyReadLoader(), NULL, U2AssemblyRead(), os);
}

U2DbiIterator<U2AssemblyRead>* GenericSqlSingleTableAssemblyAdapter::getReadsByRow(const U2Region& r, qint64 minRow, qint64 maxRow, U2OpStatus& os) {
    static const QString qStr = "SELECT " + ALL_READ_FIELDS + " FROM %1 WHERE %2 AND (prow >= :minRow AND prow < :maxRow)";
    QSharedPointer<U2SqlQuery> q (new U2SqlQuery(qStr.arg(readsTable).arg(rangeConditionCheck), db, os));
    bindRegion(*q, r);
    q->bindInt64(":minRow", minRow);
    q->bindInt64(":maxRow", maxRow);
    return new GenericSqlRSIterator<U2AssemblyRead>(q, new GenericSqlSimpleAssemblyReadLoader(), NULL, U2AssemblyRead(), os);
}

U2DbiIterator<U2AssemblyRead>* GenericSqlSingleTableAssemblyAdapter::getReadsByName(const QByteArray& name, U2OpStatus& os) {
    static const QString qStr = "SELECT " + ALL_READ_FIELDS + " FROM %1 WHERE name = :name";
    QSharedPointer<U2SqlQuery> q (new U2SqlQuery(qStr.arg(readsTable), db, os));
    int hash = qHash(name);
    q->bindInt64(":name", hash);
    return new GenericSqlRSIterator<U2AssemblyRead>(q, new GenericSqlSimpleAssemblyReadLoader(),
        new GenericSqlAssemblyNameFilter(name), U2AssemblyRead(), os);
}

void GenericSqlSingleTableAssemblyAdapter::addReads(U2DbiIterator<U2AssemblyRead>* it, U2AssemblyReadsImportInfo& ii, U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    if (!inited) {
        createReadsTables(os);
    }

    static const QString q = "INSERT INTO %1(name, prow, flags, gstart, elen, mq, data) VALUES (:name, :prow, :flags, :gstart, :elen, :mq, :data)";

    while (it->hasNext() && !os.isCoR()) {
        U2SqlQuery insertQ(q.arg(readsTable), db, os);

        U2AssemblyRead read = it->next();
        bool dnaExt = false; //TODO:
        qint64 flags = read->flags;
        flags = flags | (dnaExt ? DnaExtAlphabet : 0);

        if (rangeMode) {
            SAFE_POINT(read->effectiveLen >= minReadLength && read->effectiveLen < maxReadLength, "Effective read length must be precomputed in the range mode", );
        } else {
            int readLen = read->readSequence.length();
            int effectiveReadLength = readLen + U2AssemblyUtils::getCigarExtraLength(read->cigar);
            read->effectiveLen = effectiveReadLength;
        }

        int hash = qHash(read->name);
        QByteArray packedData = GenericSqlAssemblyUtils::packData(GenericSqlAssemblyDataMethod_NSCQ, read, os);

        insertQ.bindInt64(":name", hash);
        insertQ.bindInt64(":prow", read->packedViewRow);
        insertQ.bindInt64(":flags", flags);
        insertQ.bindInt64(":gstart", read->leftmostPos);
        insertQ.bindInt64(":elen", read->effectiveLen);
        insertQ.bindInt32(":mq", read->mappingQuality);
        insertQ.bindBlob(":data", packedData);
        insertQ.insert();
        CHECK_OP(os, );

        GenericSqlAssemblyUtils::addToCoverage(ii.coverageInfo, read);

        ii.nReads++;
        ii.onReadImported();
    }
}

void GenericSqlSingleTableAssemblyAdapter::removeReads(const QList<U2DataId>& readIds, U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    //TODO: add transaction per pack or reads
    //TODO: remove multiple reads in 1 SQL at once
    //SQLiteObjectDbi* objDbi = dbi->getSQLiteObjectDbi();
    foreach(U2DataId readId, readIds) {
        GenericSqlUtils::remove(readsTable, "id", readId, 1, db, os);
        CHECK_OP_BREAK(os);
    }

    GenericSqlObjectDbi::incrementVersion(assemblyId, db, os);
}

void GenericSqlSingleTableAssemblyAdapter::dropReadsTables(U2OpStatus &os) {
    static QString queryString = "DROP TABLE IF EXISTS %1";
    U2SqlQuery(queryString.arg(readsTable), db, os).execute();
    CHECK_OP(os, );
    GenericSqlObjectDbi::incrementVersion(assemblyId, db, os);
}

void GenericSqlSingleTableAssemblyAdapter::pack(U2AssemblyPackStat& stat, U2OpStatus& os) {
    GenericSqlSingleTablePackAlgorithmAdapter packAdapter(db, readsTable);
    AssemblyPackAlgorithm::pack(packAdapter, stat, os);
    createReadsIndexes(os);
}

void GenericSqlSingleTableAssemblyAdapter::calculateCoverage(const U2Region& r, U2AssemblyCoverageStat& coverage, U2OpStatus& os) {
    QString queryString = "SELECT gstart, elen, data FROM " + readsTable;
    bool rangeArgs = (r != U2_REGION_MAX);

    if (rangeArgs) {
        queryString += " WHERE " + rangeConditionCheck;
    }

    U2SqlQuery q(queryString, db, os);

    if (rangeArgs) {
        bindRegion(q, r, false);
    }

    GenericSqlAssemblyUtils::calculateCoverage(q, r, coverage, os);
}

const QString& GenericSqlSingleTableAssemblyAdapter::getReadsTableName() const {
    return readsTable;
}

void GenericSqlSingleTableAssemblyAdapter::enableRangeTableMode(int minLen, int maxLen) {
    rangeMode = true;
    minReadLength = minLen;
    maxReadLength = maxLen;
    rangeConditionCheck = RTM_RANGE_CONDITION_CHECK;
    rangeConditionCheckForCount = RTM_RANGE_CONDITION_CHECK_COUNT;
}

QString GenericSqlSingleTableAssemblyAdapter::getReadsTableName(const U2DataId& assemblyId, char prefix, const QString& suffix) {
    return QString("AssemblyRead_%1%2_%3").arg(prefix).arg(U2DbiUtils::toDbiId(assemblyId)).arg(suffix);;
}

void GenericSqlSingleTableAssemblyAdapter::dropReadsIndexes(U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    static const QString q1 = "DROP INDEX IF EXISTS %1_gstart";
    U2SqlQuery(q1.arg(readsTable), db, os).execute();

    static const QString q2 = "DROP INDEX IF EXISTS %1_name";
    U2SqlQuery(q2.arg(readsTable), db, os).execute();
}

qint64 GenericSqlSingleTableAssemblyAdapter::getMinReadLength() const {
    return minReadLength;
}

qint64 GenericSqlSingleTableAssemblyAdapter::getMaxReadLength() const {
    return maxReadLength;
}

void GenericSqlSingleTableAssemblyAdapter::bindRegion(U2SqlQuery& q, const U2Region& r, bool forCount) {
    if (rangeMode) {
        q.bindInt64(":end", r.endPos());
        q.bindInt64(":start", r.startPos - maxReadLength);
        if (!forCount) {
            q.bindInt64(":realStart", r.startPos);
        }
    } else {
        q.bindInt64(":end", r.endPos());
        q.bindInt64(":start", r.startPos);
    }
}

//////////////////////////////////////////////////////////////////////////
// pack adapter

GenericSqlSingleTablePackAlgorithmAdapter::GenericSqlSingleTablePackAlgorithmAdapter(GenericSqlDbRef* db, const QString& readsTable) :
    db(db),
    readsTable(readsTable),
    updateQuery(NULL)
{
}

GenericSqlSingleTablePackAlgorithmAdapter::~GenericSqlSingleTablePackAlgorithmAdapter() {
    releaseDbResources();
}

U2DbiIterator<PackAlgorithmData>* GenericSqlSingleTablePackAlgorithmAdapter::selectAllReads(U2OpStatus& os) {
    QSharedPointer<U2SqlQuery> q (new U2SqlQuery("SELECT id, gstart, elen FROM " + readsTable + " ORDER BY gstart", db, os));
    return new GenericSqlRSIterator<PackAlgorithmData>(q, new GenericSqlSimpleAssemblyReadPackedDataLoader(), NULL, PackAlgorithmData(), os);
}

void GenericSqlSingleTablePackAlgorithmAdapter::assignProw(const U2DataId& readId, qint64 prow, U2OpStatus& os) {
    if (updateQuery != NULL) {
        delete updateQuery;
    }

    updateQuery = new U2SqlQuery("UPDATE " + readsTable + " SET prow = :prow WHERE id = :id", db, os);
    updateQuery->bindInt64(":prow", prow);
    updateQuery->bindDataId(":id", readId);
    updateQuery->execute();
}

void GenericSqlSingleTablePackAlgorithmAdapter::releaseDbResources() {
    delete updateQuery;
    updateQuery = NULL;
}

} //namespace
