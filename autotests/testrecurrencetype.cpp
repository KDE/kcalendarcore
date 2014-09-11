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
#include <ksystemtimezone.h>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

using namespace KCalCore;

static QString dumpTime(const KDateTime &dt, const KDateTime::Spec &viewSpec);

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() << "verbose" , i18n("Verbose output")));
    parser.addPositionalArgument("input", i18n("Name of input file"));
    parser.addPositionalArgument("output", i18n("optional name of output file for the recurrence dates"));

    KAboutData about(QStringLiteral("testrecurrencenew"), 
                     i18n("Load recurrence rules with the new class and print out debug messages"),
                     QStringLiteral("0.1"));

    about.setupCommandLine(&parser);
    KAboutData::setApplicationData(about);

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("testrecurrencenew"));
    QCoreApplication::setApplicationVersion("0.1");
    parser.process(app);
    about.processCommandLine(&parser);

    const QStringList parsedArgs = parser.positionalArguments();

    if (parsedArgs.isEmpty()) {
        parser.showHelp();
    }

    QString input = parsedArgs[0];
    qDebug() << "Input file:" << input;

    QTextStream *outstream = 0;
    QString fn;
    if (parsedArgs.count() > 1) {
        fn = parsedArgs[1];
        qDebug() << "We have a file name given:" << fn;
    }
    QFile outfile(fn);
    if (!fn.isEmpty() && outfile.open(QIODevice::WriteOnly)) {
        qDebug() << "Opened output file!!!";
        outstream = new QTextStream(&outfile);
    }

    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));

    KDateTime::Spec viewSpec;
    FileStorage store(cal, input);
    if (!store.load()) return 1;
    QString tz = cal->nonKDECustomProperty("X-LibKCal-Testsuite-OutTZ");
    if (!tz.isEmpty()) {
        viewSpec = KDateTime::Spec(KSystemTimeZones::zone(tz));
    }

    Incidence::List inc = cal->incidences();

    for (Incidence::List::Iterator it = inc.begin(); it != inc.end(); ++it) {
        Incidence::Ptr incidence = *it;
        qDebug() << "*+*+*+*+*+*+*+*+*+*";
        qDebug() << " ->" << incidence->summary() << "<-";

        incidence->recurrence()->dump();

        KDateTime dt;
        if (incidence->allDay()) {
            dt = incidence->dtStart().addDays(-1);
        } else {
            dt = incidence->dtStart().addSecs(-1);
        }
        int i = 0;
        if (outstream) {
            // Output to file for testing purposes
            while (dt.isValid() && i < 500) {
                ++i;
                dt = incidence->recurrence()->getNextDateTime(dt);
                if (dt.isValid()) {
                    (*outstream) << dumpTime(dt, viewSpec) << endl;
                }
            }
        } else {
            incidence->recurrence()->dump();
            // Output to konsole
            while (dt.isValid() && i<10) {
                ++i;
                qDebug() << "-------------------------------------------";
                dt = incidence->recurrence()->getNextDateTime(dt);
                if (dt.isValid()) {
                    qDebug() << " *~*~*~*~ Next date is:" << dumpTime(dt, viewSpec);
                }
            }
        }
    }

    delete outstream;
    outfile.close();
    return 0;
}

QString dumpTime(const KDateTime &dt, const KDateTime::Spec &viewSpec)
{
    if (!dt.isValid()) {
        return QString();
    }
    KDateTime vdt = viewSpec.isValid() ? dt.toTimeSpec(viewSpec) : dt;
    QString format;
#ifdef FLOAT_IS_DATE_ONLY
    if (vdt.isDateOnly()) {
        format = QStringLiteral("%Y-%m-%d");
    } else
#endif
        format = QStringLiteral("%Y-%m-%dT%H:%M:%S");
    if (vdt.isSecondOccurrence()) {
        format += QStringLiteral(" %Z");
    }
    if (vdt.timeSpec() != KDateTime::ClockTime) {
        format += QStringLiteral(" %:Z");
    }
    return vdt.toString(format);
}
