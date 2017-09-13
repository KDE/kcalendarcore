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
#include "setuptzinfo.h"
#include "utils.h"

#include <QTimeZone>

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTimeZone>

using namespace KCalCore;

static QString dumpTime(const QDateTime &dt, const QTimeZone &viewZone);

int main(int argc, char **argv)
{
    qputenv("TZ", "GMT");

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("verbose"), QStringLiteral("Verbose output")));
    parser.addPositionalArgument(QStringLiteral("input"), QStringLiteral("Name of input file"));
    parser.addPositionalArgument(QStringLiteral("output"), QStringLiteral("optional name of output file for the recurrence dates"));

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("testrecurrencenew"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1"));
    parser.process(app);

    QStringList parsedArgs = parser.positionalArguments();

    if (parsedArgs.isEmpty()) {
        parser.showHelp();
    }
    const SetupTzinfo setup;
    QString input = parsedArgs[0];
    qDebug() << "Input file:" << input;

    QTextStream *outstream = nullptr;
    outstream = nullptr;
    QString fn(QLatin1String(""));
    if (parsedArgs.count() > 1) {
        fn = parsedArgs[1];
        qDebug() << "We have a file name given:" << fn;
    }
    QFile outfile(fn);
    if (!fn.isEmpty() && outfile.open(QIODevice::WriteOnly)) {
        qDebug() << "Opened output file!!!";
        outstream = new QTextStream(&outfile);
    }

    MemoryCalendar::Ptr cal(new MemoryCalendar(QTimeZone::utc()));

    QTimeZone viewZone;
    FileStorage store(cal, input);
    if (!store.load()) {
        return 1;
    }
    QString tz = cal->nonKDECustomProperty("X-LibKCal-Testsuite-OutTZ");
    if (!tz.isEmpty()) {
        viewZone = QTimeZone(tz.toUtf8());
    }
    qDebug() << viewZone.id();
    Incidence::List inc = cal->incidences();

    for (Incidence::List::Iterator it = inc.begin(); it != inc.end(); ++it) {
        Incidence::Ptr incidence = *it;
        qDebug() << "*+*+*+*+*+*+*+*+*+*";
        qDebug() << " ->" << incidence->summary() << "<-";

        incidence->recurrence()->dump();

        QDateTime dt = incidence->recurrence()->endDateTime();
        int i = 0;
        if (outstream) {
            if (!dt.isValid()) {
                if (viewZone.isValid()) {
                    dt = QDateTime(QDate(2011, 1, 1), QTime(0, 0, 1), viewZone);
                } else {
                    dt = QDateTime(QDate(2011, 1, 1), QTime(0, 0, 1));
                }
            } else {
                dt = dt.addYears(2);
            }
            qDebug() << "-------------------------------------------";
            qDebug() << " *~*~*~*~ Starting with date:" << dumpTime(dt, viewZone);
            // Output to file for testing purposes
            while (dt.isValid() && i < 500) {
                ++i;
                dt = incidence->recurrence()->getPreviousDateTime(dt);
                if (dt.isValid()) {
                    (*outstream) << dumpTime(dt, viewZone) << endl;
                }
            }
        } else {
            if (!dt.isValid()) {
                dt = QDateTime(QDate(2005, 7, 31), QTime(23, 59, 59), Qt::UTC);
            } else {
                dt = dt.addYears(2);
            }
            incidence->recurrence()->dump();
            qDebug() << "-------------------------------------------";
            qDebug() << " *~*~*~*~ Starting with date:" << dumpTime(dt, viewZone);
            // Output to konsole
            while (dt.isValid() && i < 50) {
                ++i;
                qDebug() << "-------------------------------------------";
                dt = incidence->recurrence()->getPreviousDateTime(dt);
                if (dt.isValid()) {
                    qDebug() << " *~*~*~*~ Previous date is:" << dumpTime(dt, viewZone);
                }
            }
        }
    }

    delete outstream;
    outfile.close();
    return 0;
}

QString dumpTime(const QDateTime &dt, const QTimeZone &viewZone)
{
    if (!dt.isValid()) {
        return QString();
    }
    const QDateTime vdt = viewZone.isValid() ? dt.toTimeZone(viewZone) : dt;
    QString format = QStringLiteral("yyyy-MM-ddThh:mm:ss t");
    if (viewZone.isValid()) {
        format += QStringLiteral(" '%1'").arg(QString::fromUtf8(viewZone.id()));
    }
    return vdt.toString(format);
}
