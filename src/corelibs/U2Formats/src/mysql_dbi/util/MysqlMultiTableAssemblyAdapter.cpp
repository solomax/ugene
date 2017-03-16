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
#include "MysqlMultiTableAssemblyAdapter.h"
#include "MysqlSingleTableAssemblyAdapter.h"

namespace U2 {

MysqlMultiTableAssemblyAdapter::MysqlMultiTableAssemblyAdapter(GenericSqlDbi* dbi, const U2DataId& assemblyId, const AssemblyCompressor* compressor, GenericSqlDbRef* ref, U2OpStatus& os)
    : GenericSqlMultiTableAssemblyAdapter(dbi, assemblyId, compressor, ref, os)
{
}

MysqlMultiTableAssemblyAdapter::~MysqlMultiTableAssemblyAdapter() {}

GenericSqlMtaSingleTableAdapter* MysqlMultiTableAssemblyAdapter::createAdapter(int rowPos, int elenPos, U2OpStatus& os) {
    SAFE_POINT(0 <= rowPos && rowPos < adaptersGrid.size(), "Out of range", NULL);
    SAFE_POINT(0 <= elenPos && elenPos < adaptersGrid.at(rowPos).size(), "Out of range", NULL);
    SAFE_POINT(NULL == adaptersGrid.at(rowPos).at(elenPos), "Adapter is already created", NULL);

    const QString suffix = getTableSuffix(rowPos, elenPos);
    const U2Region& elenRange = elenRanges.at(elenPos);
    const QByteArray idExtra = getIdExtra(rowPos, elenPos);

    GenericSqlSingleTableAssemblyAdapter* sa = new MysqlSingleTableAssemblyAdapter(dbi, assemblyId, 'M', suffix, compressor, db, os);
    sa->enableRangeTableMode(elenRange.startPos, elenRange.endPos());

    GenericSqlMtaSingleTableAdapter* ma = new GenericSqlMtaSingleTableAdapter(sa, rowPos, elenPos, idExtra);

    adapters << ma;
    idExtras << idExtra;
    adaptersGrid[rowPos][elenPos] =  ma;

    return ma;
}

}   // namespace U2
