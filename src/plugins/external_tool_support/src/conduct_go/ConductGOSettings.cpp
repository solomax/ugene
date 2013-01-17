/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2012 UniPro <ugene@unipro.ru>
 * http://ugene.unipro.ru
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


#include "ConductGOSettings.h"

#include <U2Core/GUrlUtils.h>

namespace U2 {

ConductGOSettings::ConductGOSettings() {
    initDefault();
}

void ConductGOSettings::initDefault(){
    outDir = "";
    title = "Default";
    geneUniverse = "hgu133a";
}

QStringList ConductGOSettings::getArguments( const QString& treatFilePath){
    QStringList result;

//    go_analysis.py  '$title' '$diff_expr_file' '$logmeta' '$diff_expr_file.dbkey', '$annotation'

    result << GUrlUtils::getQuotedString(title);

    result << GUrlUtils::getQuotedString(treatFilePath);

    result << "logmeta.txt";

    result << GUrlUtils::getQuotedString(geneUniverse) + ".dbkey";

    result << GUrlUtils::getQuotedString(geneUniverse);

    return result;
}

} // U2
