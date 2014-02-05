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

#ifndef GTSEQUENCEREADINGMODEDIALOG_H
#define GTSEQUENCEREADINGMODEDIALOG_H

#include <U2Core/U2OpStatus.h>
#include <QDialogButtonBox>
#include <QString>
#include "api/GTGlobals.h"

namespace U2 {

class GTSequenceReadingModeDialog {
public:
    enum sequenceMode {Separate, Merge};

    static GTGlobals::UseMethod useMethod;
    static sequenceMode mode;
    static QDialogButtonBox::StandardButton button;
    static int numSymbolParts;
    static int numSymbolFiles;
    static bool saveDocument;
    static QString newDocName;
};

} // namespace

#endif // GTSEQUENCEREADINGMODEDIALOG_H
