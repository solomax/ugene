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

#include <U2Core/AppContext.h>

#include "Hmmer3Support.h"

namespace U2 {

const QString Hmmer3Support::BUILD_TOOL = "HMMER 3 build";
const QString Hmmer3Support::SEARCH_TOOL = "HMMER 3 search";

Hmmer3Support::Hmmer3Support(const QString &name)
: ExternalTool(name)
{
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    toolKitName = "HMMER 3";
    versionRegExp = QRegExp("HMMER (\\d+.\\d+.\\d+\\w?)");

    // HMM3-build
    if (name == BUILD_TOOL) {
#ifdef Q_OS_WIN
        executableFileName = "hmmbuild.exe";
#elif defined(Q_OS_UNIX)
        executableFileName = "hmmbuild";
#endif

        validationArguments << "-h";
        validMessage = "hmmbuild";
        description = tr("<i>HMMER 3 build</i> constructs HMM profiles from multiple sequence alignments.");
    }

    // HMM3-search
    if (name == SEARCH_TOOL) {
#ifdef Q_OS_WIN
        executableFileName = "hmmsearch.exe";
#elif defined(Q_OS_UNIX)
        executableFileName = "hmmsearch";
#endif

        validationArguments << "-h";
        validMessage = "hmmsearch";
        description = tr("<i>HMMER 3 search</i> searches profile(s) against a sequence database.");
    }
}

} // U2
