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

#ifndef _U2_GENERIC_SQL_ASSEMBLY_ADAPTER_H_
#define _U2_GENERIC_SQL_ASSEMBLY_ADAPTER_H_

#include <U2Core/U2Assembly.h>

#include "GenericSqlHelpers.h"
#include "util/AssemblyAdapter.h"
#include "util/AssemblyPackAlgorithm.h"

namespace U2 {

class U2OpStatus;
class U2AssemblyReadsImportInfo;

class GenericSqlAssemblyAdapter : public AssemblyAdapter {
public:
    GenericSqlAssemblyAdapter(const U2DataId& assemblyId, const AssemblyCompressor* compressor, GenericSqlDbRef* ref);

protected:
    GenericSqlDbRef* db;
};

class GenericSqlAssemblyNameFilter : public GenericSqlRSFilter<U2AssemblyRead> {
public:
    GenericSqlAssemblyNameFilter(const QByteArray& expectedName);
    virtual bool filter(const U2AssemblyRead& r);

protected:
    QByteArray name;
};

class GenericSqlSimpleAssemblyReadLoader: public GenericSqlRSLoader<U2AssemblyRead> {
public:
    U2AssemblyRead load(U2SqlQuery* q);
};

class GenericSqlSimpleAssemblyReadPackedDataLoader : public GenericSqlRSLoader<PackAlgorithmData> {
public:
    virtual PackAlgorithmData load(U2SqlQuery* q);
};

}   // namespace U2

#endif // _U2_GENERIC_SQL_ASSEMBLY_ADAPTER_H_
