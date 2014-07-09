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

#include "testoccurrenceiterator.h"
#include "occurrenceiterator.h"
#include "memorycalendar.h"
#include "calfilter.h"

#include <qtest.h>
#include <qdebug.h>
QTEST_MAIN(TestOccurrenceIterator)

void TestOccurrenceIterator::testIterationWithExceptions()
{
    KCalCore::MemoryCalendar calendar(KDateTime::UTC);

    KDateTime start(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC);
    KDateTime end(QDate(2013, 03, 10), QTime(11, 0, 0), KDateTime::UTC);

    KDateTime recurrenceId(QDate(2013, 03, 11), QTime(10, 0, 0), KDateTime::UTC);
    KDateTime exceptionStart(QDate(2013, 03, 11), QTime(12, 0, 0), KDateTime::UTC);
    KDateTime exceptionEnd(QDate(2013, 03, 11), QTime(13, 0, 0), KDateTime::UTC);

    KDateTime actualEnd(QDate(2013, 03, 12), QTime(11, 0, 0), KDateTime::UTC);

    KCalCore::Event::Ptr event1(new KCalCore::Event());
    event1->setUid("event1");
    event1->setSummary("event1");
    event1->setDtStart(start);
    event1->setDtEnd(end);
    event1->recurrence()->setDaily(1);
    calendar.addEvent(event1);

    KCalCore::Event::Ptr exception(new KCalCore::Event());
    exception->setUid(event1->uid());
    exception->setSummary("exception");
    exception->setRecurrenceId(recurrenceId);
    exception->setDtStart(exceptionStart);
    exception->setDtEnd(exceptionEnd);
    calendar.addEvent(exception);

    int occurrence = 0;
    KCalCore::OccurrenceIterator rIt(calendar, start, actualEnd);
    while (rIt.hasNext()) {
        rIt.next();
        occurrence++;
        if (occurrence == 1) {
            QCOMPARE(rIt.occurrenceStartDate(), start);
            QCOMPARE(rIt.incidence()->summary(), event1->summary());
        }
        if (occurrence == 2) {
            QCOMPARE(rIt.occurrenceStartDate(), exceptionStart);
            QCOMPARE(rIt.incidence()->summary(), exception->summary());
        }
        if (occurrence == 3) {
            QCOMPARE(rIt.occurrenceStartDate(), start.addDays(2));
            QCOMPARE(rIt.incidence()->summary(), event1->summary());
        }
//     qDebug() << occurrence;
//     qDebug() << "occurrence: " << rIt.occurrenceStartDate().toString();
//     qDebug() << "uid: " << rIt.incidence()->uid();
//     qDebug() << "summary: " << rIt.incidence()->summary();
//     qDebug() << "start: " << rIt.incidence()->dtStart().toString();
//     qDebug();
    }
    QCOMPARE(occurrence, 3);
}

void TestOccurrenceIterator::testEventsAndTodos()
{
    KCalCore::MemoryCalendar calendar(KDateTime::UTC);

    KDateTime start(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC);
    KDateTime end(QDate(2013, 03, 10), QTime(11, 0, 0), KDateTime::UTC);

    KDateTime actualEnd(QDate(2013, 03, 13), QTime(11, 0, 0), KDateTime::UTC);

    KCalCore::Event::Ptr event(new KCalCore::Event());
    event->setUid("event");
    event->setDtStart(start);
    event->recurrence()->setDaily(1);
    event->recurrence()->setDuration(2);
    calendar.addEvent(event);

    KCalCore::Todo::Ptr todo(new KCalCore::Todo());
    todo->setUid("todo");
    todo->setDtStart(start);
    todo->recurrence()->setDaily(1);
    todo->recurrence()->setDuration(2);
    calendar.addTodo(todo);

    KCalCore::OccurrenceIterator rIt(calendar, start, actualEnd);
    QList<KDateTime> expectedTodoOccurrences;
    expectedTodoOccurrences << start << start.addDays(1);
    QList<KDateTime> expectedEventOccurrences;
    expectedEventOccurrences << start << start.addDays(1);
    while (rIt.hasNext()) {
        rIt.next();
        qDebug() << rIt.occurrenceStartDate().toString();
        if (rIt.incidence()->type() == KCalCore::Incidence::TypeTodo) {
            QCOMPARE(expectedTodoOccurrences.removeAll(rIt.occurrenceStartDate()), 1);
        } else {
            QCOMPARE(expectedEventOccurrences.removeAll(rIt.occurrenceStartDate()), 1);
        }
    }
    QCOMPARE(expectedTodoOccurrences.size(), 0);
    QCOMPARE(expectedEventOccurrences.size(), 0);
}

void TestOccurrenceIterator::testFilterCompletedTodos()
{
    KCalCore::MemoryCalendar calendar(KDateTime::UTC);
    calendar.filter()->setCriteria(KCalCore::CalFilter::HideCompletedTodos);

    KDateTime start(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC);
    KDateTime end(QDate(2013, 03, 10), QTime(11, 0, 0), KDateTime::UTC);

    KDateTime actualEnd(QDate(2013, 03, 13), QTime(11, 0, 0), KDateTime::UTC);

    KCalCore::Todo::Ptr todo(new KCalCore::Todo());
    todo->setUid("todo");
    todo->setDtDue(start);
    todo->setDtStart(start);
    todo->recurrence()->setDaily(1);
    todo->recurrence()->setDuration(2);
    //Yes, recurring todos are weird... setting this says that all occurrences until this one have been completed, and thus should be skipped.
    //that's what kontact did, so it's what we test now.
    todo->setDtRecurrence(start.addDays(2));
    calendar.addTodo(todo);

    KCalCore::OccurrenceIterator rIt(calendar, start, actualEnd);
    QVERIFY(!rIt.hasNext());
}

void TestOccurrenceIterator::testAllDayEvents()
{
    KCalCore::MemoryCalendar calendar(KDateTime::UTC);

    KDateTime start(QDate(2013, 03, 10), KDateTime::UTC);
    KDateTime actualEnd(QDate(2013, 03, 13), QTime(11, 0, 0), KDateTime::UTC);

    KCalCore::Event::Ptr event(new KCalCore::Event());
    event->setUid("event");
    event->setDtStart(start);
    event->recurrence()->setDaily(1);
    event->recurrence()->setDuration(2);
    calendar.addEvent(event);

    KCalCore::OccurrenceIterator rIt(calendar, start, actualEnd);
    QList<KDateTime> expectedEventOccurrences;
    expectedEventOccurrences << start << start.addDays(1);
    while (rIt.hasNext()) {
        rIt.next();
        qDebug() << rIt.occurrenceStartDate().toString();
        QCOMPARE(expectedEventOccurrences.removeAll(rIt.occurrenceStartDate()), 1);
    }
    QCOMPARE(expectedEventOccurrences.size(), 0);
}

void TestOccurrenceIterator::testWithExceptionThisAndFuture()
{
    KCalCore::MemoryCalendar calendar(KDateTime::UTC);

    KDateTime start(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC);
    KDateTime end(QDate(2013, 03, 10), QTime(11, 0, 0), KDateTime::UTC);

    KDateTime recurrenceId(QDate(2013, 03, 11), QTime(10, 0, 0), KDateTime::UTC);
    KDateTime exceptionStart(QDate(2013, 03, 11), QTime(12, 0, 0), KDateTime::UTC);
    KDateTime exceptionEnd(QDate(2013, 03, 11), QTime(13, 0, 0), KDateTime::UTC);

    KDateTime actualEnd(QDate(2013, 03, 12), QTime(11, 0, 0), KDateTime::UTC);

    KCalCore::Event::Ptr event1(new KCalCore::Event());
    event1->setUid("event1");
    event1->setSummary("event1");
    event1->setDtStart(start);
    event1->setDtEnd(end);
    event1->recurrence()->setDaily(1);
    calendar.addEvent(event1);

    KCalCore::Event::Ptr exception(new KCalCore::Event());
    exception->setUid(event1->uid());
    exception->setSummary("exception");
    exception->setRecurrenceId(recurrenceId);
    exception->setThisAndFuture(true);
    exception->setDtStart(exceptionStart);
    exception->setDtEnd(exceptionEnd);
    calendar.addEvent(exception);

    int occurrence = 0;
    KCalCore::OccurrenceIterator rIt(calendar, start, actualEnd);
    while (rIt.hasNext()) {
        rIt.next();
        occurrence++;
//     qDebug() << occurrence;
//     qDebug() << "occurrence: " << rIt.occurrenceStartDate().toString();
//     qDebug() << "uid: " << rIt.incidence()->uid();
//     qDebug() << "summary: " << rIt.incidence()->summary();
//     qDebug() << "start: " << rIt.incidence()->dtStart().toString();
//     qDebug();
        if (occurrence == 1) {
            QCOMPARE(rIt.occurrenceStartDate(), start);
            QCOMPARE(rIt.incidence()->summary(), event1->summary());
        }
        if (occurrence == 2) {
            QCOMPARE(rIt.occurrenceStartDate(), exceptionStart);
            QCOMPARE(rIt.incidence()->summary(), exception->summary());
        }
        if (occurrence == 3) {
            QCOMPARE(rIt.occurrenceStartDate(), exceptionStart.addDays(1));
            QCOMPARE(rIt.incidence()->summary(), exception->summary());
        }
    }
    QCOMPARE(occurrence, 3);
}

void TestOccurrenceIterator::testSubDailyRecurrences()
{
    KCalCore::MemoryCalendar calendar(KDateTime::UTC);

    KDateTime start(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC);
    KDateTime actualEnd(QDate(2013, 03, 10), QTime(13, 0, 0), KDateTime::UTC);

    KCalCore::Event::Ptr event(new KCalCore::Event());
    event->setUid("event");
    event->setDtStart(start);
    event->recurrence()->setHourly(1);
    event->recurrence()->setDuration(2);
    calendar.addEvent(event);

    KCalCore::OccurrenceIterator rIt(calendar, start, actualEnd);
    QList<KDateTime> expectedEventOccurrences;
    expectedEventOccurrences << start << start.addSecs(60*60);
    while (rIt.hasNext()) {
        rIt.next();
        qDebug() << rIt.occurrenceStartDate().toString();
        QCOMPARE(expectedEventOccurrences.removeAll(rIt.occurrenceStartDate()), 1);
    }
    QCOMPARE(expectedEventOccurrences.size(), 0);

}

void TestOccurrenceIterator::testJournals()
{
    KCalCore::MemoryCalendar calendar(KDateTime::UTC);

    const KDateTime today = KDateTime::currentDateTime(KDateTime::UTC);
    const KDateTime yesterday = today.addDays(-1);
    const KDateTime tomorrow = today.addDays(1);

    KCalCore::Journal::Ptr journal(new KCalCore::Journal());
    journal->setUid("journal");
    journal->setDtStart(today);
    calendar.addJournal(journal);

    KCalCore::OccurrenceIterator rIt(calendar, yesterday, tomorrow);
    QVERIFY(rIt.hasNext());
    rIt.next();
    QCOMPARE(rIt.occurrenceStartDate(), today);
    QVERIFY(!rIt.hasNext());

    KCalCore::OccurrenceIterator rIt2(calendar, tomorrow, tomorrow.addDays(1));
    QVERIFY(!rIt2.hasNext());
}
