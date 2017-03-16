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

#include "GenericSqlAssemblyDbi.h"
#include "GenericSqlAttributeDbi.h"
#include "GenericSqlCrossDatabaseReferenceDbi.h"
#include "GenericSqlDbi.h"
#include "GenericSqlFeatureDbi.h"
#include "GenericSqlModDbi.h"
#include "GenericSqlMsaDbi.h"
#include "GenericSqlObjectDbi.h"
#include "GenericSqlObjectRelationsDbi.h"
#include "GenericSqlSequenceDbi.h"
#include "GenericSqlUdrDbi.h"
#include "GenericSqlVariantDbi.h"
#include "util/GenericSqlDbiUtils.h"
#include "util/GenericSqlHelpers.h"

namespace U2 {

const int GenericSqlDbi::BIND_PARAMETERS_LIMIT = USHRT_MAX;

GenericSqlDbi::GenericSqlDbi(const U2DbiFactoryId& fid) : U2AbstractDbi(fid), tablesAreCreated(false) {}

GenericSqlDbi::~GenericSqlDbi() {}

U2AssemblyDbi* GenericSqlDbi::getAssemblyDbi() {
    return assemblyDbi;
}

U2AttributeDbi * GenericSqlDbi::getAttributeDbi() {
    return attributeDbi;
}

U2CrossDatabaseReferenceDbi* GenericSqlDbi::getCrossDatabaseReferenceDbi() {
    return crossDbi;
}

U2FeatureDbi* GenericSqlDbi::getFeatureDbi() {
    return featureDbi;
}

U2ModDbi* GenericSqlDbi::getModDbi() {
    return modDbi;
}

U2MsaDbi* GenericSqlDbi::getMsaDbi() {
    return msaDbi;
}

U2ObjectDbi* GenericSqlDbi::getObjectDbi() {
    return objectDbi;
}

U2ObjectRelationsDbi * GenericSqlDbi::getObjectRelationsDbi() {
    return objectRelationsDbi;
}

U2SequenceDbi* GenericSqlDbi::getSequenceDbi() {
    return sequenceDbi;
}

UdrDbi* GenericSqlDbi::getUdrDbi() {
    return udrDbi;
}

U2VariantDbi* GenericSqlDbi::getVariantDbi() {
    return variantDbi;
}

GenericSqlDbRef* GenericSqlDbi::getDbRef() {
    return db;
}

QString GenericSqlDbi::getProperty(const QString& name, const QString& defaultValue, U2OpStatus& os) {
    const bool appVersionRequested = U2DbiOptions::APP_MIN_COMPATIBLE_VERSION == name;

    if (appVersionRequested && !minCompatibleAppVersion.isEmpty()) {
        return minCompatibleAppVersion;
    }

    U2SqlQuery q("SELECT value FROM Meta WHERE name = :name", db, os);
    q.bindString(":name", name);
    QStringList res = q.selectStrings();
    CHECK_OP(os, defaultValue);

    if (!res.isEmpty()) {
        if (U2DbiOptions::APP_MIN_COMPATIBLE_VERSION == name) {
            CHECK_EXT(minCompatibleAppVersion.isEmpty(), os.setError("Unexpected value of minimum application version"), QString());
            minCompatibleAppVersion = res.first();
        }
        return res.first();
    }

    return defaultValue;
}

void GenericSqlDbi::setProperty(const QString& name, const QString& value, U2OpStatus& os) {
    GenericSqlTransaction t(db, os);
    Q_UNUSED(t);

    U2SqlQuery q1("DELETE FROM Meta WHERE name = :name", db, os);
    q1.bindString(":name", name);
    q1.execute();
    CHECK_OP(os, );

    U2SqlQuery q2("INSERT INTO Meta(name, value) VALUES (:name, :value)", db, os);
    q2.bindString(":name", name);
    q2.bindString(":value", value);
    q2.execute();

    if (U2DbiOptions::APP_MIN_COMPATIBLE_VERSION == name) {
        minCompatibleAppVersion = value;
    }
}

void GenericSqlDbi::startOperationsBlock(U2OpStatus& os) {
    operationsBlockTransactions.push(new GenericSqlTransaction(db, os));
}

void GenericSqlDbi::stopOperationBlock(U2OpStatus& os) {
    SAFE_POINT_EXT(!operationsBlockTransactions.isEmpty(), os.setError("There is no transaction to delete"), );
    delete operationsBlockTransactions.pop();
}

bool GenericSqlDbi::isReadOnly() const {
    return features.contains(U2DbiFeature_GlobalReadOnly);
}

bool GenericSqlDbi::isTransactionActive() const {
    return !db->transactionStack.isEmpty();
}

void GenericSqlDbi::createHandle(const QHash<QString, QString> &props) {
    const QString url = props.value(U2DbiOptions::U2_DBI_OPTION_URL);
    const QString connectionName = url + "_" + QString::number((qint64)QThread::currentThread());
    QSqlDatabase database = QSqlDatabase::database(connectionName);
    if (!database.isValid()) {
        database = QSqlDatabase::addDatabase(getDriverType(), connectionName);
    }
    db->handle = database;
}

void GenericSqlDbi::open(const QHash<QString, QString> &props, U2OpStatus &os) {
    QString userName;
    const QString password = props.value(U2DbiOptions::U2_DBI_OPTION_PASSWORD);
    QString host;
    int port = -1;
    QString dbName;
    U2DbiRegistry::DbVendor vendor;

    bool parseResult = U2DbiUtils::parseFullDbiUrl(props.value(U2DbiOptions::U2_DBI_OPTION_URL), userName, vendor, host, port, dbName);
    if (!parseResult) {
        os.setError(U2DbiL10n::tr("Database url is incorrect"));
        setState(U2DbiState_Void);
        return;
    }

    if (userName.isEmpty()) {
        os.setError(U2DbiL10n::tr("User login is not specified"));
        setState(U2DbiState_Void);
        return;
    }

    if (host.isEmpty()) {
        os.setError(U2DbiL10n::tr("Host is not specified"));
        setState(U2DbiState_Void);
        return;
    }

    if (dbName.isEmpty()) {
        os.setError(U2DbiL10n::tr("Database name is not specified"));
        setState(U2DbiState_Void);
        return;
    }

    db->handle.setUserName(userName);
    db->handle.setPassword(password);
    db->handle.setHostName(host);
    if (port != -1) {
        db->handle.setPort(port);
    }
    db->handle.setDatabaseName(dbName);

    db->handle.setConnectOptions(getConnectOptions());

    if (!db->handle.open() ) {
        os.setError(U2DbiL10n::tr("Error opening database: %1").arg(db->handle.lastError().text()));
        setState(U2DbiState_Void);
        return;
    }
}

void GenericSqlDbi::setState(U2DbiState s) {
    state = s;
}

void GenericSqlDbi::internalInit(const QHash<QString, QString>& props, U2OpStatus& os) {
    if (isInitialized(os)) {
        checkVersion(os);
        CHECK_OP(os, );
        setupProperties(props, os);
        CHECK_OP(os, );
    }

    setupFeatures();
    checkUserPermissions(os);
    CHECK_OP(os, );
    setupTransactions(os);
}

void GenericSqlDbi::checkVersion(U2OpStatus& os) {
    const QString appVersionText = getProperty(U2DbiOptions::APP_MIN_COMPATIBLE_VERSION, "", os);
    CHECK_OP(os, );

    if (appVersionText.isEmpty()) {
        // Not an error since other databases might be opened with this interface
        coreLog.info(U2DbiL10n::tr("Not a %1 database: %2, %3")
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

void GenericSqlDbi::init(const QHash<QString, QString>& props, const QVariantMap&, U2OpStatus& os) {
    createHandle(props);
    QMutexLocker(&db->mutex);

    CHECK_EXT(state == U2DbiState_Void, os.setError(U2DbiL10n::tr("Illegal database state: %1").arg(state)), );

    setState(U2DbiState_Starting);

    if (!db->handle.isOpen()) {
        open(props, os);
        CHECK_OP(os, );
    }

    initProperties = props;

    dbiId = U2DbiUtils::createFullDbiUrl(db->handle.userName(), getVendor(), db->handle.hostName(), db->handle.port(), db->handle.databaseName());
    internalInit(props, os);
    if (os.hasError()) {
        db->handle.close();
        setState(U2DbiState_Void);
        return;
    }
    setState(U2DbiState_Ready);
}

QVariantMap GenericSqlDbi::shutdown(U2OpStatus& os) {
    CHECK(db->handle.isOpen(), QVariantMap());

    CHECK_EXT(state == U2DbiState_Ready,
                db->handle.close();
              , QVariantMap());


    if (!flush(os)) {
        CHECK_OP(os, QVariantMap());
        os.setError(U2DbiL10n::tr("Can't synchronize database state"));
        return QVariantMap();
    }

    QMutexLocker(&db->mutex);

    assemblyDbi->shutdown(os);
    attributeDbi->shutdown(os);
    crossDbi->shutdown(os);
    featureDbi->shutdown(os);
    modDbi->shutdown(os);
    msaDbi->shutdown(os);
    objectDbi->shutdown(os);
    objectRelationsDbi->shutdown(os);
    sequenceDbi->shutdown(os);
    variantDbi->shutdown(os);

    setState(U2DbiState_Stopping);
    db->handle.close();

    initProperties.clear();

    setState(U2DbiState_Void);
    return QVariantMap();
}

bool GenericSqlDbi::flush(U2OpStatus& ) {
    return true;
}

QString GenericSqlDbi::getDbiId() const {
    return dbiId;
}

QHash<QString, QString> GenericSqlDbi::getDbiMetaInfo(U2OpStatus& ) {
    QHash<QString, QString> res;
    res[U2DbiOptions::U2_DBI_OPTION_URL] = U2DbiUtils::ref2Url(getDbiRef());
    return res;
}

U2DataType GenericSqlDbi::getEntityTypeById(const U2DataId& id) const {
    return U2DbiUtils::toType(id);
}


GenericSqlChildDbiCommon::GenericSqlChildDbiCommon(GenericSqlDbi* dbi) :
    dbi(dbi),
    db(NULL == dbi ? NULL : dbi->getDbRef())
{
}

GenericSqlDbiFactory::GenericSqlDbiFactory() : U2DbiFactory() {}

GenericSqlDbiFactory::~GenericSqlDbiFactory() {}

FormatCheckResult GenericSqlDbiFactory::isValidDbi(const QHash<QString, QString>& properties, const QByteArray& rawData, U2OpStatus& ) const {
    Q_UNUSED(properties);
    Q_UNUSED(rawData);
    // TODO: check the result
    return FormatDetection_Matched;
}

bool GenericSqlDbiFactory::isDbiExists(const U2DbiId& id) const {
    // TODO: check the connection
    Q_UNUSED(id);
//    QString host;
//    int port;
//    QString dbName;
//    MysqlDbiUtils::parseDbiId(id, host, port, dbName);

//    U2OpStatusImpl os;
//    U2SqlQuery q("SELECT DATA LIKE :dbName", db, os);
//    q.bindString(":dbName", dbName);
//    return !q.selectStrings().isEmpty();
    return false;
}

} // namespace U2
