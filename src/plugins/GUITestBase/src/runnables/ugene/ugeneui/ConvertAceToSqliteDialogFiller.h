/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2014 UniPro <ugene@unipro.ru>
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


#ifndef CONVERTACETOSQLITEDIALOGFILLER_H
#define CONVERTACETOSQLITEDIALOGFILLER_H

#include "GTUtilsDialog.h"

namespace U2 {

class ConvertAceToSqliteDialogFiller : public Filler
{
public:
    enum OutFileAction { NOT_SET, REPLACE, APPEND, CANCEL };
    ConvertAceToSqliteDialogFiller(U2OpStatus& _os, QString _leDest, OutFileAction action = NOT_SET): Filler(_os, "AceImportDialog"),
        leDestUrl(_leDest), action(action) {}
    virtual void run();
private:
    QString leDestUrl;
    OutFileAction action;
};
}
#endif // CONVERTACETOSQLITEDIALOGFILLER_H
