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

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <qdebug.h>

#include <QDate>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

using namespace KCalCore;

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() << "verbose" , i18n("Verbose output")));
    parser.addPositionalArgument("input", i18n("Name of input file"));
    parser.addPositionalArgument("output", i18n("optional name of output file for the recurrence dates"));

    KAboutData about(QStringLiteral("testrecurson"), 
                     i18n("Tests all dates from 2002 to 2010 to test if the event recurs on each individual date. "
                          "This is meant to test the Recurrence::recursOn method for errors."),
                     QStringLiteral("0.1"));

    about.setupCommandLine(&parser);
    KAboutData::setApplicationData(about);

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("testrecurson"));
    QCoreApplication::setApplicationVersion("0.1");
    parser.process(app);
    about.processCommandLine(&parser);

    const QStringList parsedArgs = parser.positionalArguments();

    if (parsedArgs.isEmpty()) {
        parser.showHelp();
    }

    QString input = parsedArgs[0];

    QTextStream *outstream = 0;
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
    if (!store.load()) return 1;
    QString tz = cal->nonKDECustomProperty("X-LibKCal-Testsuite-OutTZ");
    if (!tz.isEmpty()) {
        cal->setViewTimeZoneId(tz);
    }

    Incidence::List inc = cal->incidences();

    for (Incidence::List::Iterator it = inc.begin(); it != inc.end(); ++it) {
        Incidence::Ptr incidence = *it;

//     qDebug() << " ->" << incidence->summary() << "<-";

//     incidence->recurrence()->dump();

        QDate dt(1996, 7, 1);
        if (outstream) {
            // Output to file for testing purposes
            int nr = 0;
            while (dt.year() <= 2020 && nr<=500) {
                if (incidence->recursOn(dt, cal->viewTimeSpec())) {
                    (*outstream) << dt.toString(Qt::ISODate) << endl;
                    nr++;
                }
                dt = dt.addDays(1);
            }
        } else {
            dt = QDate(2005, 1, 1);
            while (dt.year() < 2007) {
                if (incidence->recursOn(dt, cal->viewTimeSpec())) {
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
