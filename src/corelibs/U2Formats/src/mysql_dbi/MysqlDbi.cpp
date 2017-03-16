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

#include <QtSql/QSqlError>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

#include "MysqlAssemblyDbi.h"
#include "MysqlAttributeDbi.h"
#include "MysqlCrossDatabaseReferenceDbi.h"
#include "MysqlDbi.h"
#include "MysqlFeatureDbi.h"
#include "MysqlModDbi.h"
#include "MysqlMsaDbi.h"
#include "MysqlObjectDbi.h"
#include "MysqlObjectRelationsDbi.h"
#include "MysqlSequenceDbi.h"
#include "MysqlUdrDbi.h"
#include "MysqlVariantDbi.h"
#include "util/upgraders/MysqlUpgraderFrom_1_14_To_1_15.h"
#include "util/upgraders/MysqlUpgraderFrom_1_15_To_1_16.h"
#include "util/upgraders/MysqlUpgraderFrom_1_16_To_1_17.h"
#include "util/upgraders/MysqlUpgraderFrom_1_16_To_1_24.h"
#include "util/upgraders/MysqlUpgraderFrom_1_24_To_1_25.h"

namespace U2 {

MysqlDbi::MysqlDbi() : GenericSqlDbi(MysqlDbiFactory::ID) {
    db = new GenericSqlDbRef;

    assemblyDbi =           new MysqlAssemblyDbi(this);
    attributeDbi =          new MysqlAttributeDbi(this);
    crossDbi =              new MysqlCrossDatabaseReferenceDbi(this);
    featureDbi =            new MysqlFeatureDbi(this);
    modDbi =                new MysqlModDbi(this);
    msaDbi =                new MysqlMsaDbi(this);
    objectDbi =             new MysqlObjectDbi(this);
    objectRelationsDbi =    new MysqlObjectRelationsDbi(this);
    sequenceDbi =           new MysqlSequenceDbi(this);
    udrDbi =                new MysqlUdrDbi(this);
    variantDbi =            new MysqlVariantDbi(this);

    upgraders << new MysqlUpgraderFrom_1_14_To_1_15(this);
    upgraders << new MysqlUpgraderFrom_1_15_To_1_16(this);
    upgraders << new MysqlUpgraderFrom_1_16_To_1_17(this);
    upgraders << new MysqlUpgraderFrom_1_16_To_1_24(this);
    upgraders << new MysqlUpgraderFrom_1_24_To_1_25(this);
}

MysqlDbi::~MysqlDbi() {
    delete variantDbi;
    delete udrDbi;
    delete sequenceDbi;
    delete objectDbi;
    delete msaDbi;
    delete modDbi;
    delete featureDbi;
    delete crossDbi;
    delete attributeDbi;
    delete assemblyDbi;

    delete db;

    QSqlDatabase::removeDatabase(QString::number((qint64)QThread::currentThread()));
}

bool MysqlDbi::isInitialized(U2OpStatus &os) {
    if (!tablesAreCreated) {
        U2SqlQuery q("SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = :name and TABLE_TYPE='BASE TABLE'", db, os);
        q.bindString(":name", db->handle.databaseName());

        const int countOfTables = q.selectInt64();
        CHECK_OP(os, false);

        tablesAreCreated = 0 != countOfTables;
    }
    return tablesAreCreated;
}

QString MysqlDbi::getDriverType() {
    return "QMYSQL";
}

QString MysqlDbi::getConnectOptions() {
    return "CLIENT_COMPRESS=1;MYSQL_OPT_RECONNECT=1;";
}

U2DbiRegistry::DbVendor MysqlDbi::getVendor() const {
    return U2DbiRegistry::MYSQL;
}

void MysqlDbi::populateDefaultSchema(U2OpStatus& os) {
    GenericSqlTransaction transaction(db, os);
    Q_UNUSED(transaction);

    // meta table, stores general db info
    U2SqlQuery("CREATE TABLE Meta(name TEXT NOT NULL, value TEXT NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8", db, os).execute();
    CHECK_DB_INIT(os);

    objectDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    sequenceDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    assemblyDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    attributeDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    crossDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    featureDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    modDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    msaDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    objectRelationsDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    udrDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);
    variantDbi->initSqlSchema(os);
    CHECK_DB_INIT(os);

    setVersionProperties(Version::minVersionForMySQL(), os);
    CHECK_DB_INIT(os);
}

void MysqlDbi::setupProperties(const QHash<QString, QString> &props, U2OpStatus &os) {
    foreach (const QString& key, props.keys()) {
        if (key.startsWith("mysql-")) {
            setProperty(key, props.value(key), os);
            CHECK_OP(os, );
        }
    }
}

void MysqlDbi::setupFeatures() {
    features.insert(U2DbiFeature_ReadAssembly);
    features.insert(U2DbiFeature_ReadAttributes);
    features.insert(U2DbiFeature_ReadCrossDatabaseReferences);
    features.insert(U2DbiFeature_ReadFeatures);
    features.insert(U2DbiFeature_ReadModifications);
    features.insert(U2DbiFeature_ReadMsa);
    features.insert(U2DbiFeature_ReadProperties);
    features.insert(U2DbiFeature_ReadSequence);
    features.insert(U2DbiFeature_ReadUdr);
    features.insert(U2DbiFeature_ReadVariant);
    features.insert(U2DbiFeature_ReadRelations);

    features.insert(U2DbiFeature_WriteAssembly);
    features.insert(U2DbiFeature_WriteAttributes);
    features.insert(U2DbiFeature_WriteCrossDatabaseReferences);
    features.insert(U2DbiFeature_WriteFeatures);
    features.insert(U2DbiFeature_WriteModifications);
    features.insert(U2DbiFeature_WriteMsa);
    features.insert(U2DbiFeature_WriteProperties);
    features.insert(U2DbiFeature_WriteSequence);
    features.insert(U2DbiFeature_WriteUdr);
    features.insert(U2DbiFeature_WriteVariant);
    features.insert(U2DbiFeature_WriteRelations);

    features.insert(U2DbiFeature_AssemblyCoverageStat);
    features.insert(U2DbiFeature_AssemblyReadsPacking);
    features.insert(U2DbiFeature_RemoveObjects);
}

void MysqlDbi::checkUserPermissions(U2OpStatus& os) {
    const QString databaseName = db->handle.databaseName();
    const QString userName = db->handle.userName();
    CHECK_EXT(!databaseName.isEmpty() && !userName.isEmpty(), os.setError("Unable to check user permissions, database is not connected"), );

    const QString selectPrivilegeStr = "SELECT";
    const QString updatePrivilegeStr = "UPDATE";
    const QString deletePrivilegeStr = "DELETE";
    const QString insertPrivilegeStr = "INSERT";

    bool selectEnabled = false;
    bool updateEnabled = false;
    bool deleteEnabled = false;
    bool insertEnabled = false;

    const QString schemaQueryString = "SELECT DISTINCT PRIVILEGE_TYPE FROM information_schema.schema_privileges "
        "WHERE GRANTEE LIKE :userName AND TABLE_SCHEMA = :tableSchema";
    U2SqlQuery sq(schemaQueryString, db, os);
    sq.bindString(":userName", QString("'%1'%").arg(userName));
    sq.bindString(":tableSchema", databaseName);

    while (sq.step() && !(selectEnabled && updateEnabled && deleteEnabled && insertEnabled)) {
        const QString grantString = sq.getString(0);
        CHECK_OP(os, );

        selectEnabled |= grantString == selectPrivilegeStr;
        updateEnabled |= grantString == updatePrivilegeStr;
        deleteEnabled |= grantString == deletePrivilegeStr;
        insertEnabled |= grantString == insertPrivilegeStr;
    }

    const QString userQueryString = "SELECT DISTINCT PRIVILEGE_TYPE FROM information_schema.user_privileges "
        "WHERE GRANTEE LIKE :userName";
    U2SqlQuery uq(userQueryString, db, os);
    uq.bindString(":userName", QString("'%1'%").arg(userName));

    while (!(selectEnabled && updateEnabled && deleteEnabled && insertEnabled) && uq.step()) {
        const QString grantString = uq.getString(0);
        CHECK_OP(os, );

        selectEnabled |= grantString == selectPrivilegeStr;
        updateEnabled |= grantString == updatePrivilegeStr;
        deleteEnabled |= grantString == deletePrivilegeStr;
        insertEnabled |= grantString == insertPrivilegeStr;
    }

    if (!updateEnabled || !deleteEnabled || !insertEnabled) {
        if (selectEnabled) {
            features.insert(U2DbiFeature_GlobalReadOnly);
        } else {
            os.setError(QObject::tr("Invalid database user permissions set, so UGENE unable to use this database. "
                "Connect to your system administrator to fix the issue."));
        }
    }
}

void MysqlDbi::setupTransactions(U2OpStatus &os) {
    U2SqlQuery("SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED", db, os).execute();
    CHECK_OP(os, );
    U2SqlQuery("SET autocommit = 0", db, os).execute();
}

void MysqlDbi::checkVersion(U2OpStatus& os) {
    const QString appVersionText = getProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, "", os);
    CHECK_OP(os, );

    if (appVersionText.isEmpty()) {
        // Not an error since other databases might be opened with this interface
        coreLog.info(U2DbiL10n::tr("Not a %1 MySQL database: %2, %3")
            .arg(U2_PRODUCT_NAME).arg(db->handle.hostName()).arg(db->handle.databaseName()));
    } else {
        Version dbAppVersion = Version::parseVersion(appVersionText);
        Version currentVersion = Version::appVersion();
        if (dbAppVersion > currentVersion) {
            coreLog.info(U2DbiL10n::tr("Warning! The database was created with a newer %1 version: "
                "%2. Not all database features may be supported! Current %1 version: %3.")
                .arg(U2_PRODUCT_NAME).arg(dbAppVersion.text).arg(currentVersion.text));
        }
    }
}

// MysqlDbiFactory

const U2DbiFactoryId MysqlDbiFactory::ID = MYSQL_DBI_ID;

MysqlDbiFactory::MysqlDbiFactory() : GenericSqlDbiFactory() {}

MysqlDbiFactory::~MysqlDbiFactory() {}

GenericSqlDbi *MysqlDbiFactory::createDbi() {
    return new MysqlDbi();
}

U2DbiFactoryId MysqlDbiFactory::getId() const {
    return ID;
}

} // namespace U2
