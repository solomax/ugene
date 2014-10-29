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

#include <QLineEdit>

#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include "AssemblyOptionsPanelSavableTab.h"

namespace U2 {

AssemblyOptionsPanelSavableTab::AssemblyOptionsPanelSavableTab(QWidget *wrappedWidget, MWMDIWindow *contextWindow)
    : U2SavableWidget(wrappedWidget, contextWindow)
{

}

AssemblyOptionsPanelSavableTab::~AssemblyOptionsPanelSavableTab() {
    U2WidgetStateStorage::saveWidgetState(*this);
    widgetStateSaved = true;
}

bool AssemblyOptionsPanelSavableTab::childCanBeSaved(QWidget *child) const {
    return (NULL != qobject_cast<QLineEdit *>(child)
        || NULL != qobject_cast<ShowHideSubgroupWidget *>(child))
        && child->objectName() != "qt_spinbox_lineedit"; // skip fake line edit inside a spin box
}

} // namespace U2