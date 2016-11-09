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

#ifndef _U2_MULTILINGUAL_HTML_VIEW_H_
#define _U2_MULTILINGUAL_HTML_VIEW_H_

#include <qglobal.h>
#if (QT_VERSION < 0x050400) //Qt 5.7
#include <QtWebKitWidgets/QWebView>
#else
#include <QtWebEngineWidgets/QWebEngineView>
#endif

namespace U2 {

#if (QT_VERSION < 0x050400) //Qt 5.7
class MultilingualHtmlView : public QWebView {
#else

class MultilingualHtmlView : public QWebEngineView {
#endif
    Q_OBJECT
public:
    MultilingualHtmlView(const QString& htmlPath, QWidget* parent = NULL);
    bool isLoaded() const;

protected slots:
    virtual void sl_loaded(bool ok);
    virtual void sl_linkActivated(const QUrl& url);

signals:
    void si_loaded(bool ok);

private:
    void loadPage(const QString& htmlPath);
    bool loaded;
};

} // namespace

#endif // _U2_MULTILINGUAL_HTML_VIEW_H_
