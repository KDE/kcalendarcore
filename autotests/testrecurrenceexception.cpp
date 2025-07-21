/*
 * SPDX-FileCopyrightText: 2013 Christian Mollekopf <mollekopf@kolabsys.com>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "testrecurrenceexception.h"
#include "memorycalendar.h"

#include <QTest>
QTEST_MAIN(TestRecurrenceException)

void TestRecurrenceException::testCreateTodoException()
{
    const QDateTime dtstart(QDate(2013, 03, 10), QTime(10, 0, 0), QTimeZone::UTC);
    const QDateTime dtdue(QDate(2013, 03, 10), QTime(11, 0, 0), QTimeZone::UTC);
    const QDateTime recurrenceId(QDateTime(dtstart).addDays(1));

    KCalendarCore::Todo::Ptr todo(new KCalendarCore::Todo());
    todo->setUid(QStringLiteral("todo"));
    todo->setDtStart(dtstart);
    todo->setDtDue(dtdue);
    todo->recurrence()->setDaily(1);
    todo->recurrence()->setDuration(3);
    todo->setCreated(dtstart);
    todo->setLastModified(dtstart);

    const KCalendarCore::Todo::Ptr exception = KCalendarCore::MemoryCalendar::createException(todo, recurrenceId, false).staticCast<KCalendarCore::Todo>();
    QCOMPARE(exception->dtStart(), recurrenceId);
    QCOMPARE(exception->dtDue(), QDateTime(dtdue).addDays(1));
    QVERIFY(exception->created() >= todo->created());
    QVERIFY(exception->lastModified() >= exception->created());
    // FIXME should be done on clearing the recurrence, but we can't due to BC.
    // Probably not that important as long as dtRecurrence is ignored if the todo is not recurring
    // QCOMPARE(exception->dtRecurrence(), QDateTime());
    // TODO dtCompleted
}

void TestRecurrenceException::testUpdateDtStart()
{
    const QDateTime dtstart(QDate(2025, 07, 22), QTime(13, 55, 0), QTimeZone::UTC);

    KCalendarCore::Event::Ptr event(new KCalendarCore::Event);
    event->setDtStart(dtstart);
    event->setDtEnd(dtstart.addSecs(300));
    event->recurrence()->setDaily(1);
    event->recurrence()->setDuration(2);

    KCalendarCore::MemoryCalendar::Ptr calendar(new KCalendarCore::MemoryCalendar(QTimeZone::utc()));
    QVERIFY(calendar->addEvent(event));

    KCalendarCore::Incidence::Ptr exception
        = calendar->createException(event, dtstart.addDays(1));
    QCOMPARE(exception->recurrenceId(), dtstart.addDays(1));
    exception->setDtStart(exception->recurrenceId().addSecs(-3600));
    QVERIFY(calendar->addIncidence(exception));

    // Recurrence ids of exception should still match recurring
    // event occurrences after a dtstart update.
    event->setDtStart(dtstart.addSecs(300));
    QVERIFY(event->recursAt(event->dtStart().addDays(1)));
    QCOMPARE(exception->recurrenceId(), event->dtStart().addDays(1));
}

#include "moc_testrecurrenceexception.cpp"
