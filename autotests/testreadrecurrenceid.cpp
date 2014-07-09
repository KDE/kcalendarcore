/*
 * Copyright (C) 2012  Christian Mollekopf <mollekopf@kolabsys.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "testreadrecurrenceid.h"

#include <qdebug.h>

#include "memorycalendar.h"
#include "icalformat.h"
#include "exceptions.h"

#include <qtest.h>
QTEST_MAIN(TestReadRecurrenceId)

void TestReadRecurrenceId::testReadSingleException()
{
    KCalCore::ICalFormat format;
    QFile file(ICALTESTDATADIR "test_recurrenceid_single.ics");
    QVERIFY(file.open(QIODevice::ReadOnly));
//   qDebug() << file.readAll();

    KCalCore::Incidence::Ptr i = format.fromString(QString::fromUtf8(file.readAll()));
    if (!i) {
        qWarning() << "Failed to parse incidence!";
        if (format.exception()) {
            qWarning() << format.exception()->arguments();
        }
    }
    QVERIFY(i);
    QVERIFY(i->hasRecurrenceId());
}

void TestReadRecurrenceId::testReadSingleExceptionWithThisAndFuture()
{
    KCalCore::ICalFormat format;
    QFile file(ICALTESTDATADIR "test_recurrenceid_thisandfuture.ics");
    QVERIFY(file.open(QIODevice::ReadOnly));
    KCalCore::Incidence::Ptr i = format.fromString(QString::fromUtf8(file.readAll()));
    QVERIFY(i);
    QVERIFY(i->hasRecurrenceId());
    QVERIFY(i->thisAndFuture());
}

void TestReadRecurrenceId::testReadWriteSingleExceptionWithThisAndFuture()
{
    KCalCore::MemoryCalendar::Ptr cal(new KCalCore::MemoryCalendar("UTC"));
    KCalCore::ICalFormat format;
    KCalCore::Incidence::Ptr inc(new KCalCore::Event);
    inc->setDtStart(KDateTime::currentUtcDateTime());
    inc->setRecurrenceId(KDateTime::currentUtcDateTime());
    inc->setThisAndFuture(true);
    cal->addIncidence(inc);
    const QString result = format.toString(cal, QString());
    qDebug() << result;

    KCalCore::Incidence::Ptr i = format.fromString(result);
    QVERIFY(i);
    QVERIFY(i->hasRecurrenceId());
    QVERIFY(i->thisAndFuture());
}

void TestReadRecurrenceId::testReadExceptionWithMainEvent()
{
    KCalCore::MemoryCalendar::Ptr calendar(new KCalCore::MemoryCalendar(KDateTime::UTC));
    KCalCore::ICalFormat format;
    QFile file(ICALTESTDATADIR "test_recurrenceid.ics");
    QVERIFY(file.open(QIODevice::ReadOnly));
    format.fromString(calendar, QString::fromUtf8(file.readAll()));
    QCOMPARE(calendar->rawEvents().size(), 2);
}

