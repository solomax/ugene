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

#ifndef _U2_GENERIC_SQL_DBI_H_
#define _U2_GENERIC_SQL_DBI_H_

#include <QtCore/QStack>

#include <U2Core/U2AbstractDbi.h>
#include <U2Core/U2DbiRegistry.h>

namespace U2 {

class GenericSqlAssemblyDbi;
class GenericSqlAttributeDbi;
class GenericSqlCrossDatabaseReferenceDbi;
class GenericSqlDbRef;
class GenericSqlFeatureDbi;
class GenericSqlModDbi;
class GenericSqlMsaDbi;
class GenericSqlObjectDbi;
class GenericSqlObjectRelationsDbi;
class GenericSqlSequenceDbi;
class GenericSqlTransaction;
class GenericSqlUdrDbi;
class GenericSqlVariantDbi;

#define CHECK_DB_INIT(os) \
    if (os.hasError()) { \
        db->handle.close(); \
        setState(U2DbiState_Void); \
        return; \
    }

class U2FORMATS_EXPORT GenericSqlDbi : public U2AbstractDbi {
public:
    GenericSqlDbi(const U2DbiFactoryId& fid);
    virtual ~GenericSqlDbi();

    /**
    Boots the database up to functional state.
    Can be called again after successful shutdown, to re-load the database.
    */
    virtual void init(const QHash<QString, QString>& properties, const QVariantMap& persistentData, U2OpStatus& os);

    /** Stops the database and frees up used resources. */
    virtual QVariantMap shutdown(U2OpStatus& os);

    /**
    Ensures that dbi state is synchronized with storage
    Return 'true' of operation is successful
    */
    virtual bool flush(U2OpStatus& os);

    /** Unique database id. Used for cross-database references. */
    virtual QString getDbiId() const;

    virtual bool isInitialized(U2OpStatus &os) = 0;

    virtual void populateDefaultSchema(U2OpStatus& os) = 0;

    /** Returns database meta-info. Any set of properties to be shown to user */
    virtual QHash<QString, QString> getDbiMetaInfo(U2OpStatus& os);

    /** Returns type of the entity referenced by the given ID */
    virtual U2DataType getEntityTypeById(const U2DataId& id) const;

    virtual U2AssemblyDbi*                  getAssemblyDbi();
    virtual U2AttributeDbi*                 getAttributeDbi();
    virtual U2CrossDatabaseReferenceDbi*    getCrossDatabaseReferenceDbi();
    virtual U2FeatureDbi*                   getFeatureDbi();
    virtual U2ModDbi*                       getModDbi();
    virtual U2MsaDbi*                       getMsaDbi();
    virtual U2ObjectDbi*                    getObjectDbi();
    virtual U2ObjectRelationsDbi*           getObjectRelationsDbi();
    virtual U2SequenceDbi*                  getSequenceDbi();
    virtual UdrDbi*                         getUdrDbi();
    virtual U2VariantDbi*                   getVariantDbi();
    GenericSqlObjectDbi*                    getSqlObjectDbi() { return objectDbi; }
    GenericSqlModDbi*                       getSqlModDbi() { return modDbi; }
    GenericSqlMsaDbi*                       getSqlMsaDbi() { return msaDbi; }
    GenericSqlSequenceDbi*                  getSqlSequenceDbi() { return sequenceDbi; }

    GenericSqlDbRef* getDbRef();

    /** Returns properties used to initialized the database */
    virtual QHash<QString, QString> getInitProperties() const {return initProperties;}

    virtual QString getProperty(const QString& name, const QString& defaultValue, U2OpStatus& os);

    virtual void setProperty(const QString& name, const QString& value, U2OpStatus& os);

    virtual void startOperationsBlock(U2OpStatus& os);

    virtual void stopOperationBlock(U2OpStatus &os);

    virtual bool isReadOnly() const;

    bool isTransactionActive() const;

    static const int BIND_PARAMETERS_LIMIT;

protected:
    void createHandle(const QHash<QString, QString>& props);

    virtual QString getDriverType() = 0;
    virtual QString getConnectOptions() = 0;
    virtual U2DbiRegistry::DbVendor getVendor() const = 0;

    void open(const QHash<QString, QString>& props, U2OpStatus& os);

    void setState(U2DbiState state);

    void internalInit(const QHash<QString, QString>& props, U2OpStatus& os);

    virtual void setupProperties(const QHash<QString, QString>& props, U2OpStatus& os) = 0;

    virtual void checkVersion(U2OpStatus &os) = 0;

    virtual void setupFeatures() = 0;

    virtual void setupTransactions(U2OpStatus& os) = 0;

    virtual void checkUserPermissions(U2OpStatus& os) = 0;

    GenericSqlDbRef*                        db;

    GenericSqlAssemblyDbi*                  assemblyDbi;
    GenericSqlAttributeDbi*                 attributeDbi;
    GenericSqlCrossDatabaseReferenceDbi*    crossDbi;
    GenericSqlFeatureDbi*                   featureDbi;
    GenericSqlModDbi*                       modDbi;
    GenericSqlMsaDbi*                       msaDbi;
    GenericSqlObjectDbi*                    objectDbi;
    GenericSqlObjectRelationsDbi*           objectRelationsDbi;
    GenericSqlSequenceDbi*                  sequenceDbi;
    GenericSqlUdrDbi*                       udrDbi;
    GenericSqlVariantDbi*                   variantDbi;

    QStack<GenericSqlTransaction*>          operationsBlockTransactions;

    bool                               tablesAreCreated;
    QString                            minCompatibleAppVersion;
};

class U2FORMATS_EXPORT GenericSqlDbiFactory : public U2DbiFactory {
public:
    GenericSqlDbiFactory();
    virtual ~GenericSqlDbiFactory();

    /** Creates new DBI instance */
    virtual GenericSqlDbi *createDbi() = 0;

    /** Returns DBI type ID */
    virtual U2DbiFactoryId getId() const = 0;

    /** Checks that data pointed by properties is a valid DBI resource */
    virtual FormatCheckResult isValidDbi(const QHash<QString, QString>& properties, const QByteArray& rawData, U2OpStatus& os) const;

    virtual GUrl id2Url(const U2DbiId& id) const {return GUrl(id, GUrl_Network);}

    virtual bool isDbiExists(const U2DbiId& id) const;
};

class GenericSqlChildDbiCommon {
public:
    GenericSqlChildDbiCommon(GenericSqlDbi* dbi);
    virtual ~GenericSqlChildDbiCommon() {}

    virtual void initSqlSchema(U2OpStatus& os) = 0;
    virtual void shutdown(U2OpStatus&) {}

protected:
    GenericSqlDbi*   dbi;
    GenericSqlDbRef* db;
};

}   // namespace U2

#endif // _U2_GENERIC_SQL_DBI_H_
