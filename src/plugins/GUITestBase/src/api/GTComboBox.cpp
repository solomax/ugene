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

#include "GTComboBox.h"
#include "GTWidget.h"

#include "GTMouseDriver.h"
#include "GTKeyboardDriver.h"

namespace U2 {

#define GT_CLASS_NAME "GTComboBox"

#define GT_METHOD_NAME "setCurrentIndex"
void GTComboBox::setCurrentIndex(U2OpStatus& os, QComboBox *comboBox, int index) {

    GT_CHECK(comboBox != NULL, "QComboBox* == NULL");

    if(comboBox->currentIndex() == index){
        return;
    }

    int comboCount = comboBox->count();
    GT_CHECK(index>=0 && index<comboCount, "invalid index");

    GTWidget::setFocus(os, comboBox);
    GTGlobals::sleep();
    int currIndex = comboBox->currentIndex() == -1 ? 0 : comboBox->currentIndex();
    QString directionKey = index > currIndex ? "down" : "up";

    int pressCount = qAbs(index-currIndex);
    for (int i=0; i<pressCount; i++) {
        GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key[directionKey]);
        GTGlobals::sleep(100);
    }
    GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key["enter"]);
    GTGlobals::sleep(500);

    currIndex = comboBox->currentIndex();
    GT_CHECK(currIndex == index, "Can't set index");
}

void GTComboBox::setIndexWithText(U2OpStatus& os, QComboBox *comboBox, const QString& text) {
    GT_CHECK(comboBox != NULL, "QComboBox* == NULL");

    int index = comboBox->findText(text,Qt::MatchContains );
    GT_CHECK(index != -1, "Text was not found");

    setCurrentIndex(os, comboBox, index);
    CHECK_OP(os, );

    QString currentText = comboBox->currentText();
    GT_CHECK(currentText == text, "Can't set text");
}

#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
