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

#include "GTLineEdit.h"
#include "GTWidget.h"

#include "GTMouseDriver.h"
#include "GTKeyboardDriver.h"

namespace U2 {

#define CHECK_LINEEDIT(method) CHECK_SET_ERR(lineEdit != NULL, "GTLineEdit::" #method ": lineEdit is NULL")

void GTLineEdit::setText(U2OpStatus& os, QLineEdit* lineEdit, const QString &str) {

    CHECK_LINEEDIT(setText);

    clear(os, lineEdit);
    GTKeyboardDriver::keySequence(os, str);
    GTGlobals::sleep(500);

    QString s = lineEdit->text();
    CHECK_SET_ERR(s == str, "Can't set text, set text differs from a given string");
}

void GTLineEdit::clear(U2OpStatus& os, QLineEdit* lineEdit) {

    CHECK_LINEEDIT(clear);

    GTWidget::setFocus(os, lineEdit);

    GTKeyboardDriver::keyClick(os, 'a', GTKeyboardDriver::key["ctrl"]);
    GTGlobals::sleep(100);
    GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key["delete"]);
    GTGlobals::sleep(1000);

    QString s = lineEdit->text();
    CHECK_SET_ERR(s.isEmpty() == true, "Can't clear text, lineEdit is not empty");
}

void GTLineEdit::pasteClipboard(U2OpStatus& os, QLineEdit* lineEdit, PasteMethod pasteMethod) {

    CHECK_LINEEDIT(pasteClipboard);

    clear(os, lineEdit);
    switch(pasteMethod) {
        case Mouse:
            os.setError("Not implemented: GTLineEdit::pasteClipboard: Paste by mouse");
            break;

        default:
        case Shortcut:
            GTKeyboardDriver::keyClick(os, 'v', GTKeyboardDriver::key["ctrl"]);
            break;
    }

    GTGlobals::sleep(500);
}

void GTLineEdit::checkTextSize(U2OpStatus& os, QLineEdit* lineEdit) {

    CHECK_LINEEDIT(checkTextSize);

    QMargins lineEditMargins = lineEdit->textMargins();
    QFontMetrics fontMetrics = lineEdit->fontMetrics();
    int textWidth = lineEditMargins.left() + lineEditMargins.right() + fontMetrics.width(lineEdit->text());
    int rectWidth = lineEdit->rect().width();

    CHECK_SET_ERR(textWidth <= rectWidth, "Text is not inside LineEdit's rect");
}

}
