/*
  This file is part of the kcalcore library.

  Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2005 Reinhold Kainhofer <reinhold@kainhofe.com>

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
#include "memorycalendar.h"

#include <KConfig>
#include <KConfigGroup>
#include <KSystemTimeZones>

#include <QDebug>
#include <QDate>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QCoreApplication>
#include <QCommandLineParser>

using namespace KCalCore;

int main(int argc, char **argv)
{
    // workaround KSystemTimeZones failing on Linux on the CI without a fully functional KTimeZone daemon
    // this can be removed as soon as we moved entirely to QTimeZone
    QStandardPaths::setTestModeEnabled(true);
    {
        KConfig config(QLatin1String("ktimezonedrc"));
        KConfigGroup group(&config, "TimeZones");
        group.writeEntry("ZoneinfoDir", "/usr/share/zoneinfo");
        group.writeEntry("Zonetab", "/usr/share/zoneinfo/zone.tab");
    }

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("verbose"), QStringLiteral("Verbose output")));
    parser.addPositionalArgument(QStringLiteral("input"), QStringLiteral("Name of input file"));
    parser.addPositionalArgument(QStringLiteral("output"), QStringLiteral("optional name of output file for the recurrence dates"));

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("testrecurson"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1"));
    parser.process(app);

    const QStringList parsedArgs = parser.positionalArguments();

    if (parsedArgs.isEmpty()) {
        parser.showHelp();
    }

    QString input = parsedArgs[0];

    QTextStream *outstream = nullptr;
    QString fn;
    if (parsedArgs.count() > 1) {
        fn = parsedArgs[1];
//     qDebug() << "We have a file name given:" << fn;
    }
    QFile outfile(fn);
    if (!fn.isEmpty() && outfile.open(QIODevice::WriteOnly)) {
//     qDebug() << "Opened output file!!!";
        outstream = new QTextStream(&outfile);
    }

    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));

    FileStorage store(cal, input);
    if (!store.load()) {
        return 1;
    }
    QString tz = cal->nonKDECustomProperty("X-LibKCal-Testsuite-OutTZ");
    const KDateTime::Spec spec = tz.isEmpty() ? cal->timeSpec() : KSystemTimeZones::zone(tz);
    qDebug() << spec.type() << spec.timeZone().name() << tz;

    Incidence::List inc = cal->incidences();

    for (Incidence::List::Iterator it = inc.begin(); it != inc.end(); ++it) {
        Incidence::Ptr incidence = *it;

//     qDebug() << " ->" << incidence->summary() << "<-";

//     incidence->recurrence()->dump();

        QDate dt(1996, 7, 1);
        if (outstream) {
            // Output to file for testing purposes
            int nr = 0;
            while (dt.year() <= 2020 && nr <= 500) {
                if (incidence->recursOn(dt, spec)) {
                    (*outstream) << dt.toString(Qt::ISODate) << endl;
                    nr++;
                }
                dt = dt.addDays(1);
            }
        } else {
            dt = QDate(2005, 1, 1);
            while (dt.year() < 2007) {
                if (incidence->recursOn(dt, spec)) {
                    qDebug() << dt.toString(Qt::ISODate);
                }
                dt = dt.addDays(1);
            }
        }
    }

    delete outstream;
    outfile.close();
    return 0;
}
