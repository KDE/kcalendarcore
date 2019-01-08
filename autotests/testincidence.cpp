/*
  This file is part of the kcalcore library.

  Copyright (C) 2015 Sandro Knauß <knauss@kolabsys.com>

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
#include "testincidence.h"
#include "event.h"
#include "utils.h"

#include <QTest>

QTEST_MAIN(IncidenceTest)

Q_DECLARE_METATYPE(KCalCore::Incidence::DateTimeRole)

using namespace KCalCore;

void IncidenceTest::testDtStartChange()
{
    QDate dt = QDate::currentDate();
    QTime t = QTime::currentTime();
    Event inc;
    inc.setDtStart(QDateTime(dt, {}));
    inc.setAllDay(true);
    inc.recurrence()->setDaily(1);
    inc.resetDirtyFields();

    inc.setDtStart(QDateTime(dt, {}));
    QVERIFY(inc.dirtyFields().empty());

    inc.setDtStart(QDateTime(dt, t));
    QCOMPARE(inc.dirtyFields(),
             QSet<IncidenceBase::Field>()
             << IncidenceBase::FieldDtStart << IncidenceBase::FieldRecurrence);
    QCOMPARE(inc.recurrence()->startDateTime().time(), t);
    inc.resetDirtyFields();

    inc.setDtStart(QDateTime(dt, {}).addDays(1));
    QCOMPARE(inc.dirtyFields(),
             QSet<IncidenceBase::Field>()
             << IncidenceBase::FieldDtStart << IncidenceBase::FieldRecurrence);
    QCOMPARE(inc.recurrence()->startDateTime(), QDateTime(dt, {}).addDays(1));
    inc.resetDirtyFields();

    inc.setDtStart(QDateTime());
    QCOMPARE(inc.dirtyFields(),
             QSet<IncidenceBase::Field>()
             << IncidenceBase::FieldDtStart << IncidenceBase::FieldRecurrence);
    QCOMPARE(inc.recurrence()->startDateTime(), QDateTime());
    inc.resetDirtyFields();

    inc.setDtStart(QDateTime(dt, {}).addDays(1));
    QCOMPARE(inc.dirtyFields(),
             QSet<IncidenceBase::Field>()
             << IncidenceBase::FieldDtStart << IncidenceBase::FieldRecurrence);
    QCOMPARE(inc.recurrence()->startDateTime(), QDateTime(dt, {}).addDays(1));
}

void IncidenceTest::testSummaryChange()
{
    Event inc;
    inc.setSummary(QStringLiteral("bla"), false);
    inc.resetDirtyFields();

    inc.setSummary(QStringLiteral("bla"), false);
    QVERIFY(inc.dirtyFields().empty());

    inc.setSummary(QStringLiteral("bla2"), false);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldSummary);
    inc.resetDirtyFields();

    inc.setSummary(QStringLiteral("bla2"), true);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldSummary);
}

void IncidenceTest::testLocationChange()
{
    Event inc;
    inc.setLocation(QStringLiteral("here"), false);
    inc.resetDirtyFields();

    inc.setLocation(QStringLiteral("here"), false);
    QVERIFY(inc.dirtyFields().empty());

    inc.setLocation(QStringLiteral("there"), false);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldLocation);
    inc.resetDirtyFields();

    inc.setLocation(QStringLiteral("there"), true);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldLocation);
}

void IncidenceTest::testRecurrenceTypeChange()
{
    QDate dt = QDate::currentDate();
    Event inc;
    inc.setDtStart(QDateTime(dt, {}));
    KCalCore::Recurrence *r = inc.recurrence();
    r->setDaily(1);
    inc.resetDirtyFields();

    r->setDaily(1);
    QVERIFY(inc.dirtyFields().empty());

    r->setDaily(2);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldRecurrence);
    inc.resetDirtyFields();

    r->setMonthly(2);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldRecurrence);
}

void IncidenceTest::testRecurrenceEndTimeChange()
{
    QDate dt = QDate::currentDate();
    Event inc;
    inc.setDtStart(QDateTime(dt, {}));
    KCalCore::Recurrence *r = inc.recurrence();
    r->setDaily(1);
    r->setEndDateTime(QDateTime(dt, {}).addDays(1));
    inc.resetDirtyFields();

    r->setEndDateTime(QDateTime(dt, {}).addDays(1));
    QVERIFY(inc.dirtyFields().empty());

    r->setEndDateTime(QDateTime(dt, {}).addDays(2));
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldRecurrence);
}

void IncidenceTest::testRecurrenceEndTimeDurationChange()
{
    QDate dt = QDate::currentDate();
    Event inc;
    inc.setDtStart(QDateTime(dt, {}));
    KCalCore::Recurrence *r = inc.recurrence();
    r->setDaily(1);
    inc.resetDirtyFields();
    QCOMPARE(r->duration(), -1);
    QVERIFY(!r->endDateTime().isValid());

    r->setDuration(5);
    QVERIFY(r->endDateTime().isValid());
    inc.resetDirtyFields();

    // duration is set and set enddate to inValid
    r->setDuration(5);
    r->setEndDateTime(QDateTime());
    QVERIFY(inc.dirtyFields().empty());

    // now set valid enddate -> set duration to 0 by sideeffect
    r->setEndDateTime(QDateTime(dt, {}).addDays(1));
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldRecurrence);
    QCOMPARE(r->duration(), 0);
    QCOMPARE(r->endDateTime(), QDateTime(dt, {}).addDays(1));

    // with valid endDate, now setDuration and aftward set invalid endDate
    r->setEndDateTime(QDateTime(dt, {}).addDays(1));
    r->setDuration(5);
    inc.resetDirtyFields();

    r->setEndDateTime(QDateTime());
    QVERIFY(inc.dirtyFields().empty());
    QCOMPARE(r->endDate(), dt.addDays(4));
    QCOMPARE(r->duration(), 5);
}

void IncidenceTest::testRecurrenceDurationChange()
{
    QDate dt = QDate::currentDate();
    Event inc;
    inc.setDtStart(QDateTime(dt, {}));
    KCalCore::Recurrence *r = inc.recurrence();
    r->setDuration(1);
    inc.resetDirtyFields();

    r->setDuration(1);
    QVERIFY(inc.dirtyFields().empty());

    r->setDuration(2);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldRecurrence);
}

void IncidenceTest::testRecurrenceExDatesChange()
{
    QDate dt = QDate::currentDate();
    Event inc;
    inc.setDtStart(QDateTime(dt, {}));
    KCalCore::Recurrence *r = inc.recurrence();
    r->setExDates(DateList() << dt.addDays(1) << dt.addDays(2));
    inc.resetDirtyFields();

    r->setExDates(DateList() << dt.addDays(2) << dt.addDays(1));
    QVERIFY(inc.dirtyFields().empty());

    r->setExDates(DateList() << dt.addDays(1));
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldRecurrence);
}

void IncidenceTest::testRecurrenceMonthlyDate()
{
    QDate dt = QDate::currentDate();
    Event inc;
    inc.setDtStart(QDateTime(dt, {}));
    KCalCore::Recurrence *r = inc.recurrence();
    r->setMonthly(1);
    r->setMonthlyDate(QList<int>() << 1 << 2 << 3);
    inc.resetDirtyFields();

    r->setMonthlyDate(QList<int>() << 3 << 1 << 2);
    QVERIFY(inc.dirtyFields().empty());

    r->setMonthlyDate(QList<int>() << 3 << 1);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldRecurrence);
}

void IncidenceTest::testRecurrenceMonthlyPos()
{
    QDate dt = QDate::currentDate();
    RecurrenceRule::WDayPos pos1(1, 2);
    RecurrenceRule::WDayPos pos2(3, 4);
    RecurrenceRule::WDayPos pos3(1, 2);
    Event inc;
    inc.setDtStart(QDateTime(dt, {}));
    KCalCore::Recurrence *r = inc.recurrence();
    r->setYearly(1);
    r->setMonthlyPos(QList<RecurrenceRule::WDayPos>() << pos1 << pos2);
    inc.resetDirtyFields();

    //TODO: test sorting
    r->setMonthlyPos(QList<RecurrenceRule::WDayPos>() << pos1 << pos2);
    QVERIFY(inc.dirtyFields().empty());

    r->setMonthlyPos(QList<RecurrenceRule::WDayPos>() << pos3);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldRecurrence);
}

void IncidenceTest::testRecurrenceYearlyDay()
{
    QDate dt = QDate::currentDate();
    Event inc;
    inc.setDtStart(QDateTime(dt, {}));
    KCalCore::Recurrence *r = inc.recurrence();
    r->setYearly(1);
    r->setYearlyDay(QList<int>() << 1 << 2 << 3);
    inc.resetDirtyFields();

    r->setYearlyDay(QList<int>() << 3 << 1 << 2);
    QVERIFY(inc.dirtyFields().empty());

    r->setYearlyDay(QList<int>() << 3 << 1);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldRecurrence);
}

void IncidenceTest::testRecurrenceYearlyMonth()
{
    QDate dt = QDate::currentDate();
    Event inc;
    inc.setDtStart(QDateTime(dt, {}));
    KCalCore::Recurrence *r = inc.recurrence();
    r->setYearly(1);
    r->setYearlyMonth(QList<int>() << 1 << 2 << 3);
    inc.resetDirtyFields();

    r->setYearlyMonth(QList<int>() << 3 << 1 << 2);
    QVERIFY(inc.dirtyFields().empty());

    r->setYearlyMonth(QList<int>() << 3 << 1);
    QCOMPARE(inc.dirtyFields(), QSet<IncidenceBase::Field>() << IncidenceBase::FieldRecurrence);
}
