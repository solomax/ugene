/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2016 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MCA_EDITOR_H_
#define _U2_MCA_EDITOR_H_

#include "MaEditor.h"

#include "view_rendering/MaEditorWgt.h"

#include <U2Core/MultipleChromatogramAlignmentObject.h>

namespace U2 {

class McaEditor : public MaEditor {
    Q_OBJECT
public:
    McaEditor(const QString& viewName, GObject* obj);

    MultipleChromatogramAlignmentObject* getMaObject() const { return qobject_cast<MultipleChromatogramAlignmentObject*>(maObject); }

    virtual void buildStaticToolbar(QToolBar* tb);

    virtual void buildStaticMenu(QMenu* m);

    virtual int getRowHeight() const;

protected slots:
    void sl_showHideChromatograms(bool show);

protected:
    QWidget* createWidget();

    bool              showChromatograms;

    QAction*          showChromatogramsAction;
};

class McaEditorWgt : public MaEditorWgt {
    Q_OBJECT
public:
    McaEditorWgt(MaEditor* editor);

    McaEditor* getEditor() const { return qobject_cast<McaEditor* >(editor); }

protected:
    void initSeqArea(GScrollBar* shBar, GScrollBar* cvBar);
};

} // namespace

#endif // _U2_MCA_EDITOR_H_
