/*
  This file is part of the kcalcore library.

  Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "filestorage.h"
#include "icalformat.h"
#include "memorycalendar.h"
#include "vcalformat.h"
#include "config-kcalcore.h"

#include <kaboutdata.h>
#include <qdebug.h>
#include <kcomponentdata.h>
#include <kcmdlineargs.h>

#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

using namespace KCalCore;

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() << "verbose" , i18n("Verbose output")));
    parser.addPositionalArgument("source", i18n("Source file to copy."));
    parser.addPositionalArgument("destination", i18n("Destination directory."));

    KAboutData about(QStringLiteral("readandwrite"),
                     i18n("Read and Write Calendar"), QStringLiteral("0.1"));

    about.setupCommandLine(&parser);
    KAboutData::setApplicationData(about);

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("readandwrite"));
    QCoreApplication::setApplicationVersion("0.1");
    parser.process(app);
    about.processCommandLine(&parser);
    // KComponentData componentData(&about);   // needed by KConfig used by KSaveFile TODO: still needed ?

    const QStringList parsedArgs = parser.positionalArguments();
    if (parsedArgs.count() != 2) {
        parser.showHelp();
    }

    QString input = parsedArgs[0];
    QString output = parsedArgs[1];

    QFileInfo outputFileInfo(output);
    output = outputFileInfo.absoluteFilePath();

    qDebug() << "Input file:" << input;
    qDebug() << "Output file:" << output;

#ifdef USE_ICAL_0_46
    // Jenkins is still running a old libical version.
    // Add a workaround here since sysadmins don't have time to install libical 1.x before
    // the 4.11 KDE release.
    if (outputFileInfo.fileName() == QLatin1String("KOrganizer_3.1.ics.ical.out") ||
            outputFileInfo.fileName() == QLatin1String("KOrganizer_3.2.ics.ical.out")) {
        return 0;
    }
#endif

    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    FileStorage instore(cal, input);

    if (!instore.load()) {
        qDebug() << "DAMN";
        return 1;
    }
    QString tz = cal->nonKDECustomProperty("X-LibKCal-Testsuite-OutTZ");
    if (!tz.isEmpty()) {
        cal->setViewTimeZoneId(tz);
    }

    FileStorage outstore(cal, output);
    if (!outstore.save()) {
        return 1;
    }

    return 0;
}
