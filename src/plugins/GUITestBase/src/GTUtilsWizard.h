/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2015 UniPro <ugene@unipro.ru>
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


#ifndef GTUTILSWIZARD_H
#define GTUTILSWIZARD_H

#include "GTUtilsDialog.h"
//class QVariant;
namespace U2 {

class GTUtilsWizard{
public:
    enum WizardButton{Next, Back, Apply, Run, Cancel, Defaults, Setup};
    static void setInputFiles(U2OpStatus &os, QList<QStringList >  list);
    static void setAllParameters(U2OpStatus &os, QMap<QString, QVariant> map);
    static void setParameter(U2OpStatus &os, QString parName, QVariant parValue);
    static QVariant getParameter(U2OpStatus &os, QString parName);
    static void setValue(U2OpStatus &os, QWidget* w, QVariant value);
    static void clickButton(U2OpStatus &os, WizardButton button);
    static QString getPageTitle(U2OpStatus &os);
private:
    static const QMap<QString, WizardButton> buttonMap;
    static QMap<QString, WizardButton> initButtonMap();
};

class TuxedoWizardFiller: public Filler{
public:
    TuxedoWizardFiller(U2OpStatus &os):Filler(os, "Tuxedo Wizard"){}
    TuxedoWizardFiller(U2OpStatus &os, CustomScenario* c):Filler(os, "Tuxedo Wizard", c){}
    void commonScenario();
};

}

#endif // GTUTILSWIZARD_H