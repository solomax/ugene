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

#ifndef _U2_MYSQL_DBI_H_
#define _U2_MYSQL_DBI_H_

#include <QStack>

#include <U2Formats/GenericSqlDbi.h>

namespace U2 {

class MysqlAttributeDbi;
class MysqlAssemblyDbi;
class MysqlCrossDatabaseReferenceDbi;
class MysqlFeatureDbi;
class MysqlModDbi;
class MysqlMsaDbi;
class MysqlObjectDbi;
class MysqlObjectRelationsDbi;
class MysqlSequenceDbi;
class MysqlTransaction;
class MysqlUdrDbi;
class MysqlVariantDbi;

class U2FORMATS_EXPORT MysqlDbi : public GenericSqlDbi {
public:
    MysqlDbi();
    ~MysqlDbi();

    virtual bool isInitialized(U2OpStatus &os);

    virtual void populateDefaultSchema(U2OpStatus& os);

protected:
    virtual QString getDriverType();
    virtual QString getConnectOptions();
    virtual U2DbiRegistry::DbVendor getVendor() const;

    virtual void setupProperties(const QHash<QString, QString>& props, U2OpStatus& os);
    virtual void setupFeatures();
    virtual void setupTransactions(U2OpStatus& os);
    void checkUserPermissions(U2OpStatus& os);
    virtual void checkVersion(U2OpStatus &os);
};

class U2FORMATS_EXPORT MysqlDbiFactory : public GenericSqlDbiFactory {
public:
    MysqlDbiFactory();
    virtual ~MysqlDbiFactory();

    /** Creates new DBI instance */
    virtual GenericSqlDbi *createDbi();

    /** Returns DBI type ID */
    virtual U2DbiFactoryId getId() const;

    static const U2DbiFactoryId ID;
};

}   // namespace U2

#endif // _U2_MYSQL_DBI_H_
