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

#include <U2Core/U2ObjectDbi.h>

#include <U2Test/GUITest.h>

#include "GTDatabaseConfig.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsLog.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsSharedDatabaseDocument.h"
#include "GTUtilsTaskTreeView.h"
#include "api/GTFileDialog.h"
#include "api/GTKeyboardDriver.h"
#include "api/GTLineEdit.h"
#include "api/GTMenu.h"
#include "api/GTMouseDriver.h"
#include "api/GTTreeWidget.h"
#include "api/GTWidget.h"
#include "runnables/qt/MessageBoxFiller.h"
#include "runnables/qt/PopupChooser.h"
#include "runnables/ugene/corelibs/U2Gui/AddFolderDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditConnectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportChromatogramFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportToDatabaseDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/SharedConnectionsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/ugeneui/SelectDocumentFormatDialogFiller.h"

#include "GTTestsSharedDatabase.h"

namespace U2 {
namespace GUITest_common_scenarios_shared_database {

GUI_TEST_CLASS_DEFINITION(cm_test_0001) {
    //1. File -> Shared databases.
    //Expected: The dialog appears.
    //2. Click "Add"
    //3. Fill the parameters:
    //    Connection name: cm_test_0001: new shared database
    //4. Click "OK".
    //Expected: the new connection item appears in the list.
    //5. Choose the new item and click "Connect".
    //Expected: the connection is established:
    //          1) The connection item has the "Connected" icon.
    //          2) The database document appears in the project.
    //          3) There are no errors in the log.
    //          4) The "Edit" button is disabled.
    GTLogTracer lt;
    QString conName = "cm_test_0001: new shared database";
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::ADD);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    {
        EditConnectionDialogFiller::Parameters params;
        params.connectionName = conName;
        GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, params, EditConnectionDialogFiller::FROM_SETTINGS));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(cm_test_0002) {
    //1. File -> Shared databases.
    //Expected: The dialog appears.
    //2. Choose the "cm_test_0002: new shared database" item and click "Connect" (or create it).
    //Expected: the connection is established:
    //          1) The connection item has the "Connected" icon.
    //          2) The database document appears in the project.
    //          3) There are no errors in the log.
    //          4) The "Edit" button is disabled.
    //3. Click "Delete".
    //Expected: the connection is removed:
    //          1) The connection item disappears.
    //          2) The database document disappears from the project.
    //          3) There are no errors in the log.
    GTLogTracer lt;
    QString conName = "cm_test_0002: new shared database";
    GTDatabaseConfig::initTestConnectionInfo(conName);

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::DELETE, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(cm_test_0003) {
    //1. File -> Shared databases.
    //Expected: The dialog appears.
    //2. Choose the "cm_test_0003: new shared database" item (or create it with the "wrong_login" login).
    //3. Click "Edit".
    //4. Fill the parameters:
    //    Connection name: cm_test_0003: new shared database 1
    //    Login: ugene
    //5. Click "OK".
    //Expected: the new connection name is changed in the list.
    //6. Click "Connect".
    //Expected: 1) The "Edit" button is disabled.
    //          2) The connection is established:
    //            a) The connection item has the "Connected" icon.
    //            b) The database document appears in the project.
    //            c) There are no errors in the log.
    //            d) The "Edit" button is disabled.
    //7. Click "Disconnect".
    //Expected: the "Edit" button is enabled.
    GTLogTracer lt;
    QString conName = "cm_test_0003: new shared database";
    QString newConName = "cm_test_0003: new shared database 1";
    GTDatabaseConfig::initTestConnectionInfo(conName, GTDatabaseConfig::database(), false);

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::EDIT, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, newConName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, newConName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    {
        EditConnectionDialogFiller::Parameters params;
        params.connectionName = newConName;
        GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, params, EditConnectionDialogFiller::FROM_SETTINGS));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, newConName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::DISCONNECT, newConName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(cm_test_0004) {
    //1. File -> Shared databases.
    //Expected: The dialog appears.
    //2. Choose the "ugene_gui_test" item and click "Connect" (or create it).
    //Expected: the connection is established:
    //          1) The connection item has the "Connected" icon.
    //          2) The database document appears in the project.
    //          3) There are no errors in the log.
    //          4) The "Edit" button is disabled.
    GTLogTracer lt;
    QString conName = "ugene_gui_test";
    GTDatabaseConfig::initTestConnectionInfo(conName);
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(cm_test_0005) {
    //1. File -> Shared databases.
    //Expected: The dialog appears.
    //2. Choose the "ugene_gui_test" item and click "Connect".
    //Expected: the connection is established:
    //          1) The connection item has the "Connected" icon.
    //          2) The database document appears in the project.
    //          3) There are no errors in the log.
    //          4) The "Edit" button is disabled.
    //3. Click "Disconnect".
    //Expected: the connection is removed:
    //          1) The "Connected" icon disappears from the item.
    //          2) The database document disappears from the project.
    //          3) There are no errors in the log.
    GTLogTracer lt;
    QString conName = "ugene_gui_test";
    GTDatabaseConfig::initTestConnectionInfo(conName);
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::DISCONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(cm_test_0006) {
    //1. File -> Shared databases.
    //Expected: The dialog appears.
    //2. Choose the "cm_test_0006: uninitialized database" item and click "Connect" (or create it).
    //Expected: the question about database initializations appears.
    //3. Click "No".
    //Expected: the connection is not established.
    //4. Click "Connect" again.
    //Expected: the question appears again.
    //5. Click "Yes".
    //Expected: 1) The initialization task is started and finished.
    //          2) The connection is established:
    //            a) The connection item has the "Connected" icon.
    //            b) The database document appears in the project.
    //            c) There are no errors in the log.
    //            d) The "Edit" button is disabled.
    GTLogTracer lt;
    QString conName = "cm_test_0006: uninitialized database";
    GTDatabaseConfig::initTestConnectionInfo(conName, GTDatabaseConfig::uninitializedDatabase());
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);

        SharedConnectionsDialogFiller::Action cAction(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        cAction.dbName = GTDatabaseConfig::uninitializedDatabase();
        cAction.expectedResult = SharedConnectionsDialogFiller::Action::DONT_INITIALIZE;
        actions << cAction;

        cAction.expectedResult = SharedConnectionsDialogFiller::Action::INITIALIZE;
        actions << cAction;

        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(cm_test_0007) {
    //1. File -> Shared databases.
    //Expected: The dialog appears.
    //2. Choose the "cm_test_0007: incompatible version database" item and click "Connect" (or create it).
    //Expected: the connection is not established; the error message about version appears.
    QString conName = "cm_test_0007: uninitialized database";
    GTDatabaseConfig::initTestConnectionInfo(conName, GTDatabaseConfig::versionDatabase());
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);

        SharedConnectionsDialogFiller::Action cAction(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        cAction.dbName = GTDatabaseConfig::versionDatabase();
        cAction.expectedResult = SharedConnectionsDialogFiller::Action::VERSION;
        actions << cAction;

        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);
}

GUI_TEST_CLASS_DEFINITION(cm_test_0008) {
    //Establish several connections
    //1. File -> Shared databases.
    //Expected: The dialog appears.
    //2. Choose the "ugene_gui_test" item and click "Connect" (or create it).
    //Expected: the connection is established:
    //          1) The connection item has the "Connected" icon.
    //          2) The database document appears in the project.
    //          3) There are no errors in the log.
    //          4) The "Edit" button is disabled.
    //2. Choose the "ugene_gui_test_2" item and click "Connect" (or create it).
    //Expected: the connection is established:
    //          1) The connection item has the "Connected" icon.
    //          2) The database document appears in the project.
    //          3) There are no errors in the log.
    //          4) The "Edit" button is disabled.
    GTLogTracer lt;
    QString conName1 = "ugene_gui_test";
    QString conName2 = "ugene_gui_test_2";
    GTDatabaseConfig::initTestConnectionInfo(conName1);
    GTDatabaseConfig::initTestConnectionInfo(conName2, GTDatabaseConfig::secondDatabase());
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName1);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName1);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName2);
        SharedConnectionsDialogFiller::Action cAction(SharedConnectionsDialogFiller::Action::CONNECT, conName2);
        cAction.dbName = GTDatabaseConfig::secondDatabase();
        actions << cAction;
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(cm_test_0009) {
    //1. File -> Shared databases.
    //Expected: The dialog appears.
    //2. Click "Add"
    //3. Fill the parameters:
    //    Connection name: cm_test_0009: new shared database
    //    Host: ugene-quad-ubuntu
    //    Port: 3306
    //    Database: ugene_gui_test
    //    Login: wronglogin
    //4. Click "OK".
    //Expected: the message about wrong connection data is shown.
    QString conName = "cm_test_0009: new shared database";
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::ADD);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        SharedConnectionsDialogFiller::Action cAction(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        cAction.expectedResult = SharedConnectionsDialogFiller::Action::WRONG_DATA;
        actions << cAction;
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    {
        EditConnectionDialogFiller::Parameters params;
        params.connectionName = conName;
        params.host = GTDatabaseConfig::host();
        params.port = QString::number(GTDatabaseConfig::port());
        params.database = GTDatabaseConfig::database();
        params.login = "wrongLogin";
        params.password = "wrongPassword";
        GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, params, EditConnectionDialogFiller::MANUAL));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);
}

GUI_TEST_CLASS_DEFINITION(cm_test_0013) {
    //1. File -> Shared databases.
    //Expected: The dialog appears.
    //2. Click "Add"
    //3. Fill the parameters:
    //    Connection name: cm_test_0013: new shared database
    //    Host: ugene-quad-ubuntu
    //    Port: 3306
    //    Database: ugene_gui_test
    //4. Click "OK".
    //5. Choose the new item and click "Connect".
    //Expected: the dialog about login and password appears.
    //6. Fill the parameters:
    //    Login: ugene
    //7. Click "OK".
    //Expected: the connection is established:
    //          1) The connection item has the "Connected" icon.
    //          2) The database document appears in the project.
    //          3) There are no errors in the log.
    //          4) The "Edit" button is disabled.
    GTLogTracer lt;
    QString conName = "cm_test_0013: new shared database";
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::ADD);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        SharedConnectionsDialogFiller::Action cAction(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        cAction.expectedResult = SharedConnectionsDialogFiller::Action::LOGIN;
        actions << cAction;
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    {
        EditConnectionDialogFiller::Parameters params;
        params.connectionName = conName;
        params.host = GTDatabaseConfig::host();
        params.port = QString::number(GTDatabaseConfig::port());
        params.database = GTDatabaseConfig::database();
        GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, params, EditConnectionDialogFiller::MANUAL));
    }
    {
        GTUtilsDialog::waitForDialog(os, new AuthenticationDialogFiller(os, GTDatabaseConfig::login(), GTDatabaseConfig::password()));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

namespace {

QString connectToTestDatabase(U2OpStatus &os) {
    GTLogTracer lt;
    QString conName = "ugene_gui_test";
    GTDatabaseConfig::initTestConnectionInfo(conName);
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMenuItemByName(os, GTMenu::showMainMenu(os, MWMENU_FILE), QStringList() << ACTION_PROJECTSUPPORT__ACCESS_SHARED_DB);

    CHECK_SET_ERR_RESULT(!lt.hasError(), "errors in log", conName);

    return GTDatabaseConfig::database();
}

}

GUI_TEST_CLASS_DEFINITION(proj_test_0001) {
    //1. Connect to the "ugene_gui_test" database.
    //2. Double click the folder "/proj_test_0001".
    //Expected: the folder is expanded; there are three subfolders: dir1, dir2, dir3.
    //3. Double click the folder "/proj_test_0001/dir2".
    //Expected: the folder is expanded; there is one object: pt0001_human_T1.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "proj_test_0001");
    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    QAbstractItemModel *model = treeView->model();
    CHECK_OP(os, );
    CHECK_SET_ERR(3 == model->rowCount(dirItem), "Wrong child count");

    GTUtilsProjectTreeView::findIndex(os, "pt0001_dir1");
    QModelIndex dir2Item = GTUtilsProjectTreeView::findIndex(os, "pt0001_dir2");
    GTUtilsProjectTreeView::findIndex(os, "pt0001_dir3");
    CHECK_OP(os, );
    CHECK_SET_ERR(1 == model->rowCount(dir2Item), "Wrong child count");

    QModelIndex obj = GTUtilsProjectTreeView::findIndex(os, "pt0001_human_T1");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(proj_test_0002) {
    //1. Connect to the "ugene_gui_test" database.
    //2. Right click the folder "/proj_test_0002" -> Create folder.
    //3. Enter the name "pt0002_dir" and press Enter.
    //Expected: the subfolder folder is created.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");
    QAbstractItemModel *model = treeView->model();

    const QModelIndex parentDir = GTUtilsProjectTreeView::findIndex(os, "proj_test_0002");
    CHECK_SET_ERR(0 == model->rowCount(parentDir), "Invalid child item count");

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__ADD_MENU << ACTION_PROJECT__CREATE_FOLDER));
    GTUtilsDialog::waitForDialog(os, new AddFolderDialogFiller(os, "pt0002_dir", GTGlobals::UseKeyBoard));
    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "proj_test_0002"));
    GTMouseDriver::click(os, Qt::RightButton);

    CHECK_SET_ERR(1 == model->rowCount(parentDir), "Invalid child item count");
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(proj_test_0003) {
    //1. Connect to the "ugene_gui_test" database.
    //2. Double click the folder "/proj_test_0003".
    //Expected: the folder is expanded; there is one subfolder.
    //3. Right on the subfolder -> Rename.
    //4. Enter the new name "new name" and press Enter.
    //Expected: the subfolder folder is renamed.
    //5. Press F2.
    //Expected: it is proposed to rename the subfolder.
    //6. Press Esc.
    //Expected: the subfolder folder is not renamed.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    const QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "proj_test_0003");
    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "proj_test_0003"));
    GTMouseDriver::doubleClick(os);

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "An invalid project tree view");

    CHECK_SET_ERR(treeView->isExpanded(dirItem), "The folder item has not expanded after double click");

    QAbstractItemModel *model = treeView->model();
    CHECK_SET_ERR(1 == model->rowCount(dirItem), "Invalid child item count");

    const QModelIndex subfolderItem = model->index(0, 0, dirItem);

    GTUtilsProjectTreeView::rename(os, subfolderItem.data().toString(), "pt0003_new_name");
    CHECK_OP(os, );
    CHECK_SET_ERR("pt0003_new_name" == subfolderItem.data().toString(), "Renaming failed");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "pt0003_new_name"));
    GTMouseDriver::click(os);
    GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key["F2"]);
    GTGlobals::sleep();
    GTKeyboardDriver::keyClick(os, GTKeyboardDriver::key["esc"]);

    CHECK_SET_ERR("pt0003_new_name" == subfolderItem.data().toString(), "The folder was renamed");
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(proj_test_0004) {
    //Object drag'n'drop
    //1. Connect to the "ugene_gui_test" database.
    //2. Expand the folder "/proj_test_0004/pt0004_dir1/pt0004_dir2".
    //Expected: the folder is expanded; there is one object: pt0004_human_T1.
    //3. Drag'n'drop the object to the folder "/proj_test_0004/not_existing_dir".
    //Expected: the object is moved; there are no errors in the log.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");
    QAbstractItemModel *model = treeView->model();

    const QModelIndex dirItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0004_dir1");
    const QModelIndex dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0004_dir2");
    const QModelIndex objItem = GTUtilsProjectTreeView::findIndex(os, "pt0004_human_T1");
    CHECK_SET_ERR(treeView->isExpanded(dirItem2), "The folder item has not expanded after double click");

    GTUtilsProjectTreeView::dragAndDrop(os, objItem, dirItem1);
    CHECK_SET_ERR(2 == model->rowCount(dirItem1), "Invalid child item count");
    CHECK_SET_ERR(0 == model->rowCount(dirItem2), "Invalid child item count");
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(proj_test_0005) {
    //Objects drag'n'drop
    //1. Connect to the "ugene_gui_test" database.
    //2. Expand the folder "/proj_test_0005/pt0005_dir1/pt0005_dir2".
    //Expected: the folder is expanded; there is one object: human_T1.
    //3. Expand the folder "/proj_test_0005/pt0005_dir1/pt0005_dir3".
    //Expected: the folder is expanded; there is one object: COI.
    //4. Select both objects using CTRL.
    //5. Drag'n'drop the objects to the folder "/proj_test_0005/pt0005_dir1".
    //Expected: the objects are moved; there are no errors in the log.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");
    QAbstractItemModel *model = treeView->model();

    { // drag'n'drop
        const QModelIndex dirItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir1");
        const QModelIndex dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir2");
        const QModelIndex dirItem3 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir3");
        const QModelIndex objItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0005_human_T1");
        const QModelIndex objItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0005_COI");

        CHECK_SET_ERR(treeView->isExpanded(dirItem2), "The folder 2 item has not expanded after double click");
        CHECK_SET_ERR(treeView->isExpanded(dirItem3), "The folder 3 item has not expanded after double click");

        GTUtilsProjectTreeView::dragAndDropSeveralElements(os, QModelIndexList()<<objItem1<<objItem2, dirItem1);
    }

    const QModelIndex dirItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir1");
    const QModelIndex dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir2");
    const QModelIndex dirItem3 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir3");
    const QModelIndex objItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0005_human_T1");
    const QModelIndex objItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0005_COI");
    CHECK_SET_ERR(4 == model->rowCount(dirItem1), "Invalid child item count 1");
    CHECK_SET_ERR(0 == model->rowCount(dirItem2), "Invalid child item count 2");
    CHECK_SET_ERR(0 == model->rowCount(dirItem3), "Invalid child item count 3");
    CHECK_SET_ERR(dirItem1 == dirItem2.parent(), "Invalid parent item 1");
    CHECK_SET_ERR(dirItem1 == dirItem3.parent(), "Invalid parent item 2");
    CHECK_SET_ERR(dirItem1 == objItem1.parent(), "Invalid parent item 3");
    CHECK_SET_ERR(dirItem1 == objItem2.parent(), "Invalid parent item 4");
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(proj_test_0006) {
    //Folder drag'n'drop
    //1. Connect to the "ugene_gui_test" database.
    //2. Expand the folder "/proj_test_0006/pt0006_dir1/pt0006_dir2".
    //Expected: the folder is expanded; there is one object: pt0006_human_T1.
    //3. Drag'n'drop the folder "pt0006_dir2" to the folder "/proj_test_0006/".
    //Expected: the folder is moved; there are no errors in the log; the structure is the following:
    //  proj_test_0006
    //    pt0006_dir1
    //    pt0006_dir2
    //      pt0006_human_T1
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );
    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");
    QAbstractItemModel *model = treeView->model();

    { // drag'n'drop
        const QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "proj_test_0006");
        const QModelIndex dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0006_dir2");
        GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, dirItem2));
        GTMouseDriver::click(os);

        GTUtilsProjectTreeView::dragAndDrop(os, dirItem2, dirItem);
    }

    const QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "proj_test_0006");
    const QModelIndex dirItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0006_dir1");
    const QModelIndex dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0006_dir2");
    const QModelIndex objItem = GTUtilsProjectTreeView::findIndex(os, "pt0006_human_T1");
    CHECK_SET_ERR(2 == model->rowCount(dirItem), "Invalid child item count 1");
    CHECK_SET_ERR(0 == model->rowCount(dirItem1), "Invalid child item count 2");
    CHECK_SET_ERR(1 == model->rowCount(dirItem2), "Invalid child item count 3");
    CHECK_SET_ERR(dirItem == dirItem1.parent(), "Invalid parent item 1");
    CHECK_SET_ERR(dirItem == dirItem2.parent(), "Invalid parent item 2");
    CHECK_SET_ERR(dirItem2 == objItem.parent(), "Invalid parent item 3");

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(proj_test_0007) {
    //Filter
    //1. Connect to the "ugene_gui_test" database.
    //2. Fill the object name filter: "pt0007".
    //Expected: there are only two objects in the project (pt0007_human_T1 and pt0007_COI).
    //3. Clear the filter.
    //Expected: the project has usual structure with folders.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );
    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");
    QAbstractItemModel *model = treeView->model();

    QLineEdit *filterEdit = dynamic_cast<QLineEdit*>(GTWidget::findWidget(os, "nameFilterEdit"));
    CHECK(NULL != filterEdit, );

    GTLineEdit::setText(os, filterEdit, "pt0007");
    int objCount = model->rowCount(QModelIndex());
    CHECK_SET_ERR(2 == objCount, "Invalid filtered objects count");
    QString objName1 = model->index(0, 0, QModelIndex()).data().toString();
    QString objName2 = model->index(1, 0, QModelIndex()).data().toString();
    CHECK_SET_ERR("[m] pt0007_COI" == objName1, "Wrong object name 1");
    CHECK_SET_ERR("[s] pt0007_human_T1" == objName2, "Wrong object name 2");

    GTLineEdit::setText(os, filterEdit, "");
    int docCount = model->rowCount(QModelIndex());
    CHECK_SET_ERR(1 == docCount, "Invalid filtered docs count");
    QString docName = model->index(0, 0, QModelIndex()).data().toString();
    CHECK_SET_ERR(GTDatabaseConfig::database() == docName, "Wrong doc name");
}

GUI_TEST_CLASS_DEFINITION(import_test_0001) {
//    Object drag'n'drop
//    1. Connect to the "ugene_gui_test" database.
//    2. Create the folder "/import_test_0001".
//    3. Open "samples/FASTA/human_T1.fa".
//    4. Drag'n'drop the sequence object to the folder "/import_test_0001".
//    Expected: the object is imported.
//    5. Double click the new object.
//    Expected: the sequence view is opened.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString newFolderName = "import_test_0001";
    const QString newFolderPath = parentFolderPath + U2ObjectDbi::PATH_SEP + newFolderName;
    const QString fileDocName = "human_T1.fa";
    const QString fileObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString importedObjectPath = newFolderPath + U2ObjectDbi::PATH_SEP + fileObjectName;

    const QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, parentFolderPath, newFolderName);
    const QModelIndex newFolderItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, newFolderPath);

    GTFileDialog::openFile(os, dataDir + "/samples/FASTA/", "human_T1.fa");
    const QModelIndex fileDocIndex = GTUtilsProjectTreeView::findIndex(os, fileDocName);
    const QModelIndex objectDocIndex = GTUtilsProjectTreeView::findIndex(os, fileObjectName, fileDocIndex);

    GTUtilsProjectTreeView::dragAndDrop(os, objectDocIndex, newFolderItemIndex);
    GTGlobals::sleep(200);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Wait until project updater shows the item (something about 10 seconds)
    GTGlobals::sleep(20000);

    const QModelIndex importedObjectItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, importedObjectPath);
    CHECK_SET_ERR(importedObjectItemIndex.isValid(), "Can't find the imported object");

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(import_test_0002) {
//    Document drag'n'drop
//    1. Connect to the "ugene_gui_test" database.
//    2. Create the folder "/import_test_0002".
//    3. Open "samples/FASTA/human_T1.fa".
//    4. Drag'n'drop the document to the folder "/import_test_0002".
//    Expected: the document is imported.
//    5. Double click the new object.
//    Expected: the sequence view is opened.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString newFolderName = "import_test_0002";
    const QString newFolderPath = parentFolderPath + U2ObjectDbi::PATH_SEP + newFolderName;
    const QString fileDocName = "human_T1.fa";
    const QString fileObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString importedDocFolderPath = newFolderPath + U2ObjectDbi::PATH_SEP + fileDocName;
    const QString importedObjectPath = importedDocFolderPath + U2ObjectDbi::PATH_SEP + fileObjectName;

    const QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, parentFolderPath, newFolderName);
    const QModelIndex newFolderItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, newFolderPath);

    GTFileDialog::openFile(os, dataDir + "/samples/FASTA/", "human_T1.fa");
    const QModelIndex fileDocIndex = GTUtilsProjectTreeView::findIndex(os, fileDocName);

    GTUtilsProjectTreeView::dragAndDrop(os, fileDocIndex, newFolderItemIndex);
    GTGlobals::sleep(200);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Wait until project updater shows the item (something about 10 seconds)
    GTGlobals::sleep(20000);

    const QModelIndex importedDocFolderItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, importedDocFolderPath);
    CHECK_SET_ERR(importedDocFolderItemIndex.isValid(), "Can't find the imported document's folder");

    const QModelIndex importedObjectItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, importedObjectPath);
    CHECK_SET_ERR(importedObjectItemIndex.isValid(), "Can't find the imported object");

    GTUtilsProjectTreeView::doubleClickItem(os, importedObjectItemIndex);
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(200);

    QWidget* sequenceView = GTWidget::findWidget(os, connectionName + " " + fileObjectName);
    CHECK_SET_ERR(NULL != sequenceView, "Sequence view wasn't opened");

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(import_test_0003) {
//    Annotations drag'n'drop
//    1. Connect to the "ugene_gui_test" database.
//    2. Open "samples/Genbank/murine.gb".
//    3. Double click the sequence object "/imp_test_0003/NC_001363" in the database.
//    4. Drag'n'drop the annotation object from murine.gb document to the folder "/imp_test_0003".
//    Expected: the object is imported.
//    5. Drag'n'drop the new annotation object to the sequence view and connect it with the sequence.
//    Expected: the annotations are added to the sequence view.
//    6. Reconnect to the database.
//    7. Double click the sequence object "/imp_test_0003/NC_001363" in the database.
//    Expected: the sequence view is opened; annotations are shown too.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString folderName = "import_test_0003";
    const QString folderPath = parentFolderPath + U2ObjectDbi::PATH_SEP + folderName;
    const QString fileDocName = "murine.gb";
    const QString sequenceObjectName = "[s] NC_001363";
    const QString annotationObjectName = "NC_001363 features";
    const QString annotationTableName = "NC_001363 features [%1]";
    const QString databaseSequenceObjectPath = folderPath + U2ObjectDbi::PATH_SEP + sequenceObjectName;
    const QString databaseAnnotationObjectPath = folderPath + U2ObjectDbi::PATH_SEP + annotationObjectName;

    QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);
    const QModelIndex folderItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, folderPath);
    QModelIndex databaseSequenceObjectItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, databaseSequenceObjectPath);

    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/", "murine.gb");
    const QModelIndex fileDocIndex = GTUtilsProjectTreeView::findIndex(os, fileDocName);
    const QModelIndex fileAnnotationObjectIndex = GTUtilsProjectTreeView::findIndex(os, annotationObjectName, fileDocIndex);

    GTUtilsProjectTreeView::doubleClickItem(os, databaseSequenceObjectItemIndex);
    GTGlobals::sleep(200);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QWidget* sequenceView = GTWidget::findWidget(os, connectionName + " " + sequenceObjectName);
    CHECK_SET_ERR(NULL != sequenceView, "Sequence view wasn't opened");

    GTUtilsProjectTreeView::dragAndDrop(os, fileAnnotationObjectIndex, folderItemIndex);
    GTGlobals::sleep(200);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Wait until project updater shows the item (something about 10 seconds)
    GTGlobals::sleep(20000);

    const QModelIndex databaseAnnotaionObjectItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, databaseAnnotationObjectPath);
    CHECK_SET_ERR(databaseAnnotaionObjectItemIndex.isValid(), "Can't find the imported annotation object");

    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));

    QTreeWidget *annotationTableWidget = GTUtilsAnnotationsTreeView::getTreeWidget(os);
    CHECK_SET_ERR(NULL != annotationTableWidget, "Annotations tree widget is NULL");
    GTUtilsProjectTreeView::dragAndDrop(os, databaseAnnotaionObjectItemIndex, annotationTableWidget);

    GTUtilsSharedDatabaseDocument::disconnectDatabase(os, databaseDoc);
    connectionName = connectToTestDatabase(os);
    databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);
    databaseSequenceObjectItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, databaseSequenceObjectPath);

    GTUtilsProjectTreeView::doubleClickItem(os, databaseSequenceObjectItemIndex);
    GTGlobals::sleep(200);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    sequenceView = GTWidget::findWidget(os, connectionName + " " + sequenceObjectName);
    CHECK_SET_ERR(NULL != sequenceView, "Sequence view wasn't opened again");

    annotationTableWidget = GTUtilsAnnotationsTreeView::getTreeWidget(os);
    CHECK_SET_ERR(NULL != annotationTableWidget, "Annotations tree widget is NULL again");

    QTreeWidgetItem* annotationTable = GTUtilsAnnotationsTreeView::findItem(os, annotationTableName.arg(connectionName));
    CHECK_SET_ERR(NULL != annotationTable, "Annotation table is NULL");

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(import_test_0004) {
//    Import a file via the import dialog, destination path is changed in the dialog

//    1. Connect to the "ugene_gui_test" database.

//    2. Create a folder in the database: {import_test_0004}.

//    3. Call context menu on the database connection document, select {Add -> Import to the database...} item.
//    Expected state: an import dialog appears.

//    4. Click the "Add files" button, select {data/samples/FASTA/human_T1.fa}.
//    Expected state: the file is added to the orders list, it will be imported into the {/} folder.

//    5. Click twice to the item on the destination folder column and change the folder, set {/import_test_0004}.
//    Expected state: the file is present in the orders list, it will be imported into the {/import_test_0004} folder.

//    6. Click the "Import" button.
//    Expected state: an import task is started, after it finishes a sequence object appears in the {/import_test_0004} folder.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString folderName = "import_test_0004";
    const QString folderPath = U2ObjectDbi::ROOT_FOLDER + folderName;
    const QString filePath = QFileInfo(dataDir + "samples/FASTA/human_T1.fa").absoluteFilePath();
    const QString sequenceObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString databaseSequenceObjectPath = folderPath + U2ObjectDbi::PATH_SEP + sequenceObjectName;


    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addFilesAction;
    addFilesAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, QStringList() << filePath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_FILES, addFilesAction);

    QVariantMap changeDestinationAction;
    changeDestinationAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__ITEM, filePath);
    changeDestinationAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__DESTINATION_FOLDER, folderPath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_DESTINATION_FOLDER, changeDestinationAction);

    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());

    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));


    QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, parentFolderPath, folderName);

    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, parentFolderPath);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(15000);

    const QModelIndex sequenceObjectIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, databaseSequenceObjectPath);
    CHECK_SET_ERR(sequenceObjectIndex.isValid(), "Result item wasn't found");

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(import_test_0005) {
//    Import a file via the import dialog, file is imported to the inner folder

//    1. Connect to the "ugene_gui_test" database.

//    2. Call context menu on the {import_test_0005} folder in the database connection document, select {Add -> Import to the folder...} item.
//    Expected state: an import dialog appears.

//    3. Click the "Add files" button, select {data/samples/FASTA/human_T1.fa}.
//    Expected state: the file is added to the orders list, it will be imported into the {/import_test_0005} folder.

//    4. Click the "Import" button.
//    Expected state: an import task is started, after it finishes a sequence object appears in the {/import_test_0005} folder.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString folderName = "import_test_0005";
    const QString folderPath = U2ObjectDbi::ROOT_FOLDER + folderName;
    const QString filePath = QFileInfo(dataDir + "samples/FASTA/human_T1.fa").absoluteFilePath();
    const QString sequenceObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString databaseSequenceObjectPath = folderPath + U2ObjectDbi::PATH_SEP + sequenceObjectName;


    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addFilesAction;
    addFilesAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, QStringList() << filePath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_FILES, addFilesAction);

    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());

    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));


    QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, parentFolderPath, folderName);

    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, folderPath);

    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTGlobals::sleep(15000);

    const QModelIndex sequenceObjectIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, databaseSequenceObjectPath);
    CHECK_SET_ERR(sequenceObjectIndex.isValid(), "Result item wasn't found");

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(view_test_0001) {
//    View annotated sequence
//    1. Connect to the "ugene_gui_test" database.
//    2. Double click the object "/view_test_0001/NC_001363".
//    Expected: the sequence and its annotations are opened in the sequence view.
//    3. Scroll the sequence view.
//    Expected: the sequence and annotations are correctly viewed.

    GTLogTracer lt;

    const QString folderName = "view_test_0001";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString sequenceVisibleName = "[s] NC_001363";
    const QString annotationVisibleName = "[a] NC_001363 features";
    const QString someFeatureName = "CDS";
    const QString databaseSequenceObjectPath = folderPath + U2ObjectDbi::PATH_SEP + sequenceVisibleName;
    const QString databaseAnnotationObjectPath = folderPath + U2ObjectDbi::PATH_SEP + annotationVisibleName;
    const int position = 2970;

    const QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseSequenceObjectPath);
    QWidget* seqView = GTWidget::findWidget(os, connectionName + " " + sequenceVisibleName);
    CHECK_SET_ERR(NULL != seqView, "View wasn't opened");

    QTreeWidget* annotationTableWidget = GTUtilsAnnotationsTreeView::getTreeWidget(os);
    CHECK_SET_ERR(NULL != annotationTableWidget, "Annotations tree widget is NULL");

    QTreeWidgetItem* annotationTable = GTUtilsAnnotationsTreeView::findItem(os, someFeatureName);
    CHECK_SET_ERR(NULL != annotationTable, "Annotation table is NULL");

    GTUtilsSequenceView::goToPosition(os, position);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(view_test_0002) {
//    View MSA
//    1. Connect to the "ugene_gui_test" database.
//    2. Double click the object "/view_test_0002/[m] COI".
//    Expected: the alignment is opened in the MSA Editor.
//    3. Scroll the alignment.
//    Expected: the alignment is correctly viewed.

    GTLogTracer lt;

    const QString folderName = "view_test_0002";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString malignmentVisibleName = "[m] COI";
    const QString databaseMalignmentObjectPath = folderPath + U2ObjectDbi::PATH_SEP + malignmentVisibleName;
    const QPoint position(300, 6);

    const QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseMalignmentObjectPath);
    QWidget* msaView = GTWidget::findWidget(os, connectionName + " " + malignmentVisibleName);
    CHECK_SET_ERR(NULL != msaView, "View wasn't opened");

    GTUtilsMSAEditorSequenceArea::clickToPosition(os, position);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(view_test_0003) {
//    View assembly
//    1. Connect to the "ugene_gui_test" database.
//    2. Double click the object "/view_test_0003/[as] chrM".
//    Expected: the assembly and its reference is opened in the Assembly browser.
//    3. Scroll the assembly.
//    Expected: the assembly is correctly viewed.

    GTLogTracer lt;

    const QString folderName = "view_test_0003";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString assemblyVisibleName = "[as] chrM";
    const QString databaseAssemblyObjectPath = folderPath + U2ObjectDbi::PATH_SEP + assemblyVisibleName;

    const QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseAssemblyObjectPath);
    QWidget* assemblyView = GTWidget::findWidget(os, connectionName + " " + assemblyVisibleName);
    CHECK_SET_ERR(NULL != assemblyView, "View wasn't opened");

    bool hasReference = GTUtilsAssemblyBrowser::hasReference(os, assemblyView);
    CHECK_SET_ERR(hasReference, "Reference is not found");

    GTUtilsAssemblyBrowser::zoomToMax(os);

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(view_test_0004) {
//    View text
//    1. Connect to the "ugene_gui_test" database.
//    2. Double click the object "/view_test_0004/text".
//    Expected: the text is opened in the text editor.

    GTLogTracer lt;

    const QString folderName = "view_test_0004";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString textVisibleName = "[t] Text";
    const QString databaseTextObjectPath = folderPath + U2ObjectDbi::PATH_SEP + textVisibleName;

    const QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseTextObjectPath);
    QWidget* textView = GTWidget::findWidget(os, connectionName + " " + textVisibleName);
    CHECK_SET_ERR(NULL != textView, "View wasn't opened");

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(view_test_0005) {
//    View chromatogram
//    1. Connect to the "ugene_gui_test" database.
//    2. Double click the object "/view_test_0005/[c] Chromatogram".
//    Expected: the sequence and its chromatogram are opened in the sequence view.

    GTLogTracer lt;

    const QString folderName = "view_test_0005";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString sequenceObjectName = "A1#berezikov";
    const QString sequenceVisibleName = "[s] " + sequenceObjectName;
    const QString chromatogramVisibleName = "[c] Chromatogram";
    const QString databaseChromatogramObjectPath = folderPath + U2ObjectDbi::PATH_SEP + chromatogramVisibleName;

    const QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseChromatogramObjectPath);

    QWidget* seqView = GTWidget::findWidget(os, connectionName + " " + sequenceVisibleName);
    CHECK_SET_ERR(NULL != seqView, "Sequence view wasn't opened");

    QWidget* chromaView = seqView->findChild<QWidget*>("chromatogram_view_" + sequenceObjectName);
    CHECK_SET_ERR(NULL != chromaView, "Chromatogram view wasn't opened");

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(view_test_0006) {
//    View tree
//    1. Connect to the "ugene_gui_test" database.
//    2. Double click the object "/view_test_0006/tree".
//    Expected: the tree is opened in the Tree Viewer.

    GTLogTracer lt;

    const QString folderName = "view_test_0006";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString treeVisibleName = "[tr] COI";
    const QString databaseTreeObjectPath = folderPath + U2ObjectDbi::PATH_SEP + treeVisibleName;

    const QString connectionName = connectToTestDatabase(os);
    Document* databaseDoc = GTUtilsSharedDatabaseDocument::getDatabaseDocumentByName(os, connectionName);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseTreeObjectPath);
    QWidget* treeView = GTWidget::findWidget(os, connectionName + " " + treeVisibleName);
    CHECK_SET_ERR(NULL != treeView, "View wasn't opened");

    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(del_test_0001) {
    //Delete object
    //1. Connect to the "ugene_gui_test" database.
    //2. Double click the object "/del_tests/dt0001_human_T1".
    //Expected: the sequence is opened.
    //3. Right click the object "/del_tests/dt0001_human_T1" -> Remove object from document.
    //Expected: the sequence view is closed; the object is moved into the folder "/Recycle bin".
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[s] dt0001_human_T1"));
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep(3000);
    GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_OP(os, );

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<ACTION_PROJECT__REMOVE_SELECTED));
    GTMouseDriver::click(os, Qt::RightButton);

    GTGlobals::sleep(3000);
    QWidget *seqView = GTWidget::findWidget(os, "ADV_single_sequence_widget_0", NULL, GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == seqView, "Sequence view is not closed");

    const QModelIndex rbItem = GTUtilsProjectTreeView::findIndex(os, "Recycle bin");
    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, rbItem));
    GTMouseDriver::doubleClick(os);
    GTUtilsProjectTreeView::findIndex(os, "dt0001_human_T1", GTGlobals::FindOptions(false));
    const QModelIndex objItem = GTUtilsProjectTreeView::findIndex(os, "dt0001_human_T1");
    CHECK_SET_ERR(rbItem == objItem.parent(), "Object is not in Recycle bin");
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(del_test_0002) {
    //Delete folder
    //1. Connect to the "ugene_gui_test" database.
    //2. Double click the object "/del_tests/dt0002_dir/dt0002_COI".
    //Expected: the alignment is opened.
    //3. Double click the object "/del_tests/dt0002_dir/dt0002_human_T1".
    //Expected: the sequence is opened.
    //4. Right click the folder "/del_tests/dt0002_dir" -> Remove folder.
    //Expected: the views are closed; the folder and its objects are moved into the folder "/Recycle bin".
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");
    QAbstractItemModel *model = treeView->model();

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[s] dt0002_human_T1"));
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep(3000);
    GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_OP(os, );

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[m] dt0002_COI"));
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep(3000);
    GTWidget::findWidget(os, "msa_editor_dt0002_COI");
    CHECK_OP(os, );

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "dt0002_dir"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<ACTION_PROJECT__REMOVE_SELECTED));
    GTMouseDriver::click(os, Qt::RightButton);

    GTGlobals::sleep(3000);
    QWidget *seqView = GTWidget::findWidget(os, "ADV_single_sequence_widget_0", NULL, GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == seqView, "Sequence view is not closed");
    QWidget *msaView = GTWidget::findWidget(os, "msa_editor_dt0002_COI", NULL, GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == msaView, "MSA Editor is not closed");

    const QModelIndex rbItem = GTUtilsProjectTreeView::findIndex(os, "Recycle bin");
    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, rbItem));
    const QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "dt0002_dir");
    CHECK_SET_ERR(rbItem == dirItem.parent(), "Folder is not in Recycle bin");
    CHECK_SET_ERR(0 == model->rowCount(dirItem), "Objects of a folder in recycle bin are shown");
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(del_test_0003) {
    //Clean up recycle bin [user]
    //1. Connect to the "ugene_gui_test" database as a simple user.
    //2. Right click the folder "/Recycle bin" -> Clean up recycle bin.
    //Expected: the folder "/Recycle bin" becomes empty.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");
    QAbstractItemModel *model = treeView->model();

    const QModelIndex rbItem = GTUtilsProjectTreeView::findIndex(os, "Recycle bin");
    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[s] dt0003_human_T1"));
    GTMouseDriver::doubleClick(os);
    GTGlobals::sleep(3000);
    QWidget *seqView = GTWidget::findWidget(os, "ADV_single_sequence_widget_0", NULL, GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == seqView, "Sequence view is opened");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, rbItem));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList()<<"empty_rb"));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep(3000);

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, rbItem));
    CHECK_SET_ERR(0 == model->rowCount(rbItem), "Recycle bin is not empty");
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(export_test_0001) {
    //Export annotated sequence
    //1. Connect to the "ugene_gui_test" database.
    //2. Right click the object "/export_tests/et0001_human_T1" -> Export/Import -> Export sequences
    //3. Set the correct path and the Genbank format and click "Export".
    //Expected: the sequence and its annotations are exported into the file that is opened in the project.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[s] et0001_sequence"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_SEQUENCE));
    GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0001_export.fasta", GTGlobals::UseMouse));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep(3000);

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "et0001_export.fasta"));
    GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(export_test_0002) {
    //Export annotations
    //1. Connect to the "ugene_gui_test" database.
    //2. Right click the object "/export_tests/et0002_features" -> Export/Import -> Export object
    //3. Set the correct path and format and click "OK".
    //Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[a] et0002_features"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << "ep_exportAnnotations2CSV"));
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(testDir + "_common_data/scenarios/sandbox/et0002_features.gb", ExportAnnotationsFiller::genbank, os));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep(5000);

    bool exported = QFileInfo(testDir + "_common_data/scenarios/sandbox/et0002_features.gb").exists();
    CHECK_SET_ERR(exported, "Object is not exported");
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(export_test_0003) {
    //Export MSA
    //1. Connect to the "ugene_gui_test" database.
    //2. Right click the object "/export_tests/et0003_alignment" -> Export/Import -> Export object
    //3. Set the correct path and format and click "OK".
    //Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[m] et0003_alignment"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_OBJECT));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0003_alignment.aln", 
        ExportDocumentDialogFiller::CLUSTALW, false, true));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep(5000);

    GTWidget::findWidget(os, "msa_editor_et0003_alignment");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(export_test_0004) {
    //Export assembly
    //1. Connect to the "ugene_gui_test" database.
    //2. Right click the object "/export_tests/et0004_assemply" -> Export/Import -> Export object
    //3. Set the correct path and format and click "OK".
    //Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[as] et0004_assembly"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_OBJECT));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0004_assembly.bam", 
        ExportDocumentDialogFiller::BAM, false, true));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, testDir + "_common_data/scenarios/sandbox/et0004_assembly.bam.ugenedb"));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep(20000);

    GTWidget::findWidget(os, "et0004_assembly.bam [as] et0004_assembly");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(export_test_0005) {
    //Export variations
    //1. Connect to the "ugene_gui_test" database.
    //2. Right click the object "/export_tests/variations" -> Export/Import -> Export object
    //3. Set the correct path and format and click "OK".
    //Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[v] et0005_variations"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_OBJECT));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0005_variations.vcf", 
        ExportDocumentDialogFiller::VCF, false, true));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep(5000);

    bool exported = QFileInfo(testDir + "_common_data/scenarios/sandbox/et0005_variations.vcf").exists();
    CHECK_SET_ERR(exported, "Object is not exported");
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(export_test_0006) {
    //Export text
    //1. Connect to the "ugene_gui_test" database.
    //2. Right click the object "/export_tests/text" -> Export/Import -> Export object
    //3. Set the correct path and format and click "OK".
    //Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[t] et0006_text"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_OBJECT));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0006_text.txt", 
        ExportDocumentDialogFiller::TEXT, false, true));
    GTUtilsDialog::waitForDialog(os, new SelectDocumentFormatDialogFiller(os));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep(5000);

    GTWidget::findWidget(os, "et0006_text [t] Text");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(export_test_0007) {
    //Export chromatogram
    //1. Connect to the "ugene_gui_test" database.
    //2. Right click the object "/export_tests/et0007_chroma" -> Export/Import -> Export sequences
    //3. Set the correct path and format and click "OK".
    //Expected: the sequence and its chromatogram are exported into the file that is opened in the project.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[c] et0007_chroma"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_CHROMATOGRAM));
    GTUtilsDialog::waitForDialog(os, new ExportChromatogramFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0007_chroma.scf",
        ExportChromatogramFiller::SCF, false, false, true));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep(5000);

    GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

GUI_TEST_CLASS_DEFINITION(export_test_0008) {
    //Export tree
    //1. Connect to the "ugene_gui_test" database.
    //2. Right click the object "/export_tests/et0008_tree" -> Export/Import -> Export object
    //3. Set the correct path and format and click "OK".
    //Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView *treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(NULL != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(os, GTUtilsProjectTreeView::getItemCenter(os, "[tr] et0008_tree"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_OBJECT));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0008_tree.nwk", 
        ExportDocumentDialogFiller::NWK, false, true));
    GTMouseDriver::click(os, Qt::RightButton);
    GTGlobals::sleep(5000);

    GTWidget::findWidget(os, "et0008_tree [tr] Tree");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasError(), "errors in log");
}

} // GUITest_common_scenarios_shared_database
} // U2