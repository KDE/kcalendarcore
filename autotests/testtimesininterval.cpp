/*
  This file is part of the kcalcore library.

  Copyright (C) 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
  Author: Sergio Martins <sergio.martins@kdab.com>

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
#include "testtimesininterval.h"
#include "event.h"
#include "utils.h"

#include <QDebug>

#include <QTest>
QTEST_MAIN(TimesInIntervalTest)

using namespace KCalendarCore;

void TimesInIntervalTest::test()
{
    const QDateTime currentDate(QDate::currentDate(), {});
    Event *event = new Event();
    event->setDtStart(currentDate);
    event->setDtEnd(currentDate.addDays(1));
    event->setAllDay(true);
    event->setSummary(QStringLiteral("Event1 Summary"));

    event->recurrence()->setDaily(1);

    //------------------------------------------------------------------------------------------------
    // Just to warm up
    QVERIFY(event->recurs());
    QVERIFY(event->recursAt(currentDate));

    //------------------------------------------------------------------------------------------------
    // Daily recurrence that never stops.
    // Should return numDaysInInterval+1 occurrences
    const int numDaysInInterval = 7;
    QDateTime start(currentDate);
    QDateTime end(start.addDays(numDaysInInterval));

    start.setTime(QTime(0, 0, 0));
    end.setTime(QTime(23, 59, 59));
    auto dateList = event->recurrence()->timesInInterval(start, end);
    QVERIFY(dateList.count() == numDaysInInterval + 1);

    //------------------------------------------------------------------------------------------------
    // start == end == first day of the recurrence, should only return 1 occurrence
    end = start;
    end.setTime(QTime(23, 59, 59));
    dateList = event->recurrence()->timesInInterval(start, end);
    QVERIFY(dateList.count() == 1);

    //------------------------------------------------------------------------------------------------
    // Test daily recurrence that only lasts X days
    const int recurrenceDuration = 3;
    event->recurrence()->setDuration(recurrenceDuration);
    end = start.addDays(100);
    dateList = event->recurrence()->timesInInterval(start, end);
    QVERIFY(dateList.count() == recurrenceDuration);
    //------------------------------------------------------------------------------------------------
    // Test daily recurrence that only lasts X days, and give start == end == last day of
    // recurrence. Previous versions of kcal had a bug and didn't return an occurrence
    start = start.addDays(recurrenceDuration - 1);
    end = start;
    start.setTime(QTime(0, 0, 0));
    end.setTime(QTime(23, 59, 59));

    dateList = event->recurrence()->timesInInterval(start, end);
    QVERIFY(dateList.count() == 1);

    //------------------------------------------------------------------------------------------------
}

//Test that interval start and end are inclusive
void TimesInIntervalTest::testSubDailyRecurrenceIntervalInclusive()
{
    const QDateTime start(QDate(2013, 03, 10), QTime(10, 0, 0), Qt::UTC);
    const QDateTime end(QDate(2013, 03, 10), QTime(11, 0, 0), Qt::UTC);

    KCalendarCore::Event::Ptr event(new KCalendarCore::Event());
    event->setUid(QStringLiteral("event"));
    event->setDtStart(start);
    event->recurrence()->setHourly(1);
    event->recurrence()->setDuration(2);

    QList<QDateTime> expectedEventOccurrences;
    expectedEventOccurrences << start << start.addSecs(60 * 60);

    const auto timesInInterval = event->recurrence()->timesInInterval(start, end);
//   qDebug() << "timesInInterval " << timesInInterval;
    for (const auto &dt : timesInInterval) {
//     qDebug() << dt;
        QCOMPARE(expectedEventOccurrences.removeAll(dt), 1);
    }
    QCOMPARE(expectedEventOccurrences.size(), 0);
}

//Test that the recurrence dtStart is used for calculation and not the interval start date
void TimesInIntervalTest::testSubDailyRecurrence2()
{
    const QDateTime start(QDate(2013, 03, 10), QTime(10, 2, 3), Qt::UTC);
    const QDateTime end(QDate(2013, 03, 10), QTime(13, 4, 5), Qt::UTC);

    KCalendarCore::Event::Ptr event(new KCalendarCore::Event());
    event->setUid(QStringLiteral("event"));
    event->setDtStart(start);
    event->recurrence()->setHourly(1);
    event->recurrence()->setDuration(2);

    QList<QDateTime> expectedEventOccurrences;
    expectedEventOccurrences << start << start.addSecs(60 * 60);

    const auto timesInInterval = event->recurrence()->timesInInterval(start.addSecs(-20), end.addSecs(20));
//   qDebug() << "timesInInterval " << timesInInterval;
    for (const auto &dt : timesInInterval) {
//     qDebug() << dt;
        QCOMPARE(expectedEventOccurrences.removeAll(dt), 1);
    }
    QCOMPARE(expectedEventOccurrences.size(), 0);
}

void TimesInIntervalTest::testSubDailyRecurrenceIntervalLimits()
{
    const QDateTime start(QDate(2013, 03, 10), QTime(10, 2, 3), Qt::UTC);
    const QDateTime end(QDate(2013, 03, 10), QTime(12, 2, 3), Qt::UTC);

    KCalendarCore::Event::Ptr event(new KCalendarCore::Event());
    event->setUid(QStringLiteral("event"));
    event->setDtStart(start);
    event->recurrence()->setHourly(1);
    event->recurrence()->setDuration(3);

    QList<QDateTime> expectedEventOccurrences;
    expectedEventOccurrences << start.addSecs(60 * 60);

    const auto timesInInterval = event->recurrence()->timesInInterval(start.addSecs(1), end.addSecs(-1));
//   qDebug() << "timesInInterval " << timesInInterval;
    for (const auto &dt : timesInInterval) {
//     qDebug() << dt;
        QCOMPARE(expectedEventOccurrences.removeAll(dt), 1);
    }
    QCOMPARE(expectedEventOccurrences.size(), 0);
}
