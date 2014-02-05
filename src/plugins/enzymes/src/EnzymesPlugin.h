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

#ifndef _U2_ENZYMES_PLUGIN_H_
#define _U2_ENZYMES_PLUGIN_H_

#include <U2Core/PluginModel.h>
#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class EnzymesADVContext;

class EnzymesPlugin : public Plugin  {
    Q_OBJECT
public:
    EnzymesPlugin();
    ~EnzymesPlugin();
    void createToolsMenu();
private slots:
    void sl_onOpenDigestSequenceDialog();
    void sl_onOpenConstructMoleculeDialog();
    void sl_onOpenCreateFragmentDialog();
private:
    EnzymesADVContext*  ctxADV;
    QAction* openDigestSequenceDialog;
    QAction* openConstructMoleculeDialog;
    QAction* openCreateFragmentDialog;
};

class EnzymesADVContext: public GObjectViewWindowContext {
    Q_OBJECT
public:
    EnzymesADVContext(QObject* p, const QList<QAction*>& cloningActions);

protected slots:
    void sl_search();
    void sl_createPCRProduct();

protected:
    virtual void buildMenu(GObjectView* v, QMenu* m);
    virtual void initViewContext(GObjectView* view);
    QList<QAction*> cloningActions;
    QAction* createPCRProductAction;
};



} //namespace

#endif
