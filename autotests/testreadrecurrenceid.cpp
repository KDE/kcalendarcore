/*
 * SPDX-FileCopyrightText: 2012 Christian Mollekopf <mollekopf@kolabsys.com>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "testreadrecurrenceid.h"
#include "exceptions.h"
#include "icalformat.h"
#include "memorycalendar.h"

#include <QDebug>
#include <QTest>
#include <QTimeZone>

QTEST_MAIN(TestReadRecurrenceId)

void TestReadRecurrenceId::testReadSingleException()
{
    KCalendarCore::ICalFormat format;
    QFile file(QLatin1String(ICALTESTDATADIR) + QLatin1String("test_recurrenceid_single.ics"));
    QVERIFY(file.open(QIODevice::ReadOnly));
    //   qDebug() << file.readAll();

    auto i = format.fromString(QString::fromUtf8(file.readAll()));
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
    KCalendarCore::ICalFormat format;
    QFile file(QLatin1String(ICALTESTDATADIR) + QLatin1String("test_recurrenceid_thisandfuture.ics"));
    QVERIFY(file.open(QIODevice::ReadOnly));
    auto i = format.fromString(QString::fromUtf8(file.readAll()));
    QVERIFY(i);
    QVERIFY(i->hasRecurrenceId());
    QVERIFY(i->thisAndFuture());
}

void TestReadRecurrenceId::testReadWriteSingleExceptionWithThisAndFuture()
{
    QSharedPointer<KCalendarCore::MemoryCalendar> cal(new KCalendarCore::MemoryCalendar(QTimeZone::utc()));
    KCalendarCore::ICalFormat format;
    QSharedPointer<KCalendarCore::Incidence> inc(new KCalendarCore::Event);
    QTimeZone tz("Europe/Berlin");
    QDateTime startDate(QDate(2015, 1, 2), QTime(3, 4, 5), tz);
    inc->setDtStart(startDate);
    inc->setRecurrenceId(startDate);
    inc->setThisAndFuture(true);
    cal->addIncidence(inc);
    const QString result = format.toString(cal);
    qDebug() << result;

    auto i = format.fromString(result);
    QVERIFY(i);
    QVERIFY(i->hasRecurrenceId());
    QVERIFY(i->thisAndFuture());
    QCOMPARE(i->recurrenceId(), startDate);
}

void TestReadRecurrenceId::testReadExceptionWithMainEvent()
{
    QSharedPointer<KCalendarCore::MemoryCalendar> calendar(new KCalendarCore::MemoryCalendar(QTimeZone::utc()));
    KCalendarCore::ICalFormat format;
    QFile file(QLatin1String(ICALTESTDATADIR) + QLatin1String("test_recurrenceid.ics"));
    QVERIFY(file.open(QIODevice::ReadOnly));
    format.fromString(calendar, QString::fromUtf8(file.readAll()));
    QCOMPARE(calendar->rawEvents().size(), 2);
}

#include "moc_testreadrecurrenceid.cpp"
