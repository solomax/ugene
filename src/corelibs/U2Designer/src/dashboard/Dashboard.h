/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2016 UniPro <ugene@unipro.ru>
 * http://ugene.net
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

#ifndef _U2_DASHBOARD_H_
#define _U2_DASHBOARD_H_

#include <qglobal.h>

#if (QT_VERSION < 0x050400) //Qt 5.7
#include <QWebElement>
#include <QWebView>
#else
#include <QWebEngineView>
#endif

#include <U2Core/U2SafePoints.h>

#include <U2Lang/WorkflowMonitor.h>

class QWebSocketServer;
class WebSocketClientWrapper;

namespace U2 {
using namespace Workflow;

class ExternalToolsWidgetController;

#if (QT_VERSION < 0x050400) //Qt 5.7
class U2DESIGNER_EXPORT Dashboard : public QWebView {
#else
class U2DESIGNER_EXPORT Dashboard : public QWebEngineView {
#endif
    Q_OBJECT
    Q_DISABLE_COPY(Dashboard)
public:
    Dashboard(const WorkflowMonitor *monitor, const QString &name, QWidget *parent);
    Dashboard(const QString &dirPath, QWidget *parent);
    ~Dashboard();

    void onShow();

    const WorkflowMonitor * monitor();
#if (QT_VERSION < 0x050400) //Qt 5.7
    QWebElement getDocument();
#endif
    
    void setClosed();
    QString directory() const;

    QString getName() const;
    void setName(const QString &value);

    void loadSchema();

    /** Modifies the application settings and emits signal for all dashboards */
    void initiateHideLoadButtonHint();

    bool isWorkflowInProgress();

signals:
    void si_loadSchema(const QString &url);
    void si_hideLoadBtnHint();
    void si_workflowStateChanged(bool isRunning);
#if (QT_VERSION >= 0x050400) //Qt 5.7
    void si_serializeContent(const QString& content);
#endif

public slots:
    /** Hides the hint on the current dashboard instance */
    void sl_hideLoadBtnHint();
#if (QT_VERSION >= 0x050400) //Qt 5.7
    void sl_onJsError(const QString& errorMessage);
#endif

private slots:
    void sl_runStateChanged(bool paused);
    void sl_loaded(bool ok);
    void sl_addProblemsWidget();
    void sl_serialize();
    void sl_setDirectory(const QString &dir);
    void sl_workflowStateChanged(Monitor::TaskState state);
#if (QT_VERSION >= 0x050400) //Qt 5.7
    void sl_serializeContent(const QString& content);
#endif

private:
    bool loaded;
    QString loadUrl;
    QString name;
    QString dir;
    bool opened;
    const WorkflowMonitor *_monitor;
#if (QT_VERSION < 0x050400) //Qt 5.7
    QWebElement doc;
#endif
    bool initialized;
    bool workflowInProgress;
    ExternalToolsWidgetController* etWidgetController;

    enum DashboardTab {OverviewDashTab, InputDashTab, OutputDashTab, ExternalToolsTab};

    static const QString EXT_TOOLS_TAB_ID;
    static const QString OVERVIEW_TAB_ID;
    static const QString INPUT_TAB_ID;
//    static const QString OUTPUT_TAB_ID;

private:
    void loadDocument();
    /** Returns the content area of the widget */
#if (QT_VERSION < 0x050400) //Qt 5.7
    QWebElement addWidget(const QString &title, DashboardTab dashTab, int cntNum = -1);

    /** Returns size of the QWebElement "name", it is searched inside "insideElt" only*/
    int containerSize(const QWebElement &insideElt, const QString &name);
#else
    QString addWidget(const QString &title, DashboardTab dashTab, int cntNum = -1);
#endif
    void serialize(U2OpStatus &os);
    void saveSettings();
    void loadSettings();

    void createExternalToolTab();

#if (QT_VERSION >= 0x050400) //Qt 5.7
    QWebSocketServer *server;
    WebSocketClientWrapper *clientWrapper;
    QWebChannel *channel;
#endif
};

class DashboardWidget : public QObject {
    Q_OBJECT
public:
#if (QT_VERSION < 0x050400) //Qt 5.7
    DashboardWidget(const QWebElement &container, Dashboard *parent);
#else
    DashboardWidget(const QString &container, Dashboard *parent);
#endif
protected:
    Dashboard *dashboard;
#if (QT_VERSION < 0x050400) //Qt 5.7
    QWebElement container;
#else
    QString container;
#endif
};

class JavascriptAgent : public QObject {
    Q_OBJECT
public:
    JavascriptAgent(Dashboard *dashboard);

public slots:
    void openUrl(const QString &url);
    void openByOS(const QString &url);
    QString absolute(const QString &url);
    void loadSchema();
    void hideLoadButtonHint();
    void setClipboardText(const QString &text);

private:
    Dashboard *dashboard;
};

class U2DESIGNER_EXPORT DashboardInfo {
public:
    QString path;
    QString dirName;
    QString name;
    bool opened;

public:
    DashboardInfo();
    DashboardInfo(const QString &dirPath, bool opened = true);
    bool operator==(const DashboardInfo &other) const;
};

class U2DESIGNER_EXPORT ScanDashboardsDirTask : public Task {
    Q_OBJECT
public:
    ScanDashboardsDirTask();
    void run();

    QStringList getOpenedDashboards() const;
    QList<DashboardInfo> getResult() const;

private:
    bool isDashboardDir(const QString &dirPath, DashboardInfo &info);

private:
    QStringList openedDashboards;
    QList<DashboardInfo> result;
};

class U2DESIGNER_EXPORT RemoveDashboardsTask : public Task {
    Q_OBJECT
public:
    RemoveDashboardsTask(const QList<DashboardInfo> &dashboards);
    void run();

private:
    QList<DashboardInfo> dashboards;
};

} // U2

Q_DECLARE_METATYPE(U2::DashboardInfo);

#endif // _U2_DASHBOARD_H_
