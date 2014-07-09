/*
  This file is part of the kcalcore library.

  Copyright (C) 2006-2007 Allen Winter <winter@kde.org>

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

#include "testmemorycalendar.h"
#include "filestorage.h"
#include "memorycalendar.h"

#include <qdebug.h>

#include <unistd.h>

#include <qtest.h>
QTEST_MAIN(MemoryCalendarTest)

using namespace KCalCore;

void MemoryCalendarTest::testValidity()
{
    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    cal->setProductId(QStringLiteral("fredware calendar"));
    QVERIFY(cal->productId() == QLatin1String("fredware calendar"));
    QVERIFY(cal->timeZoneId() == QLatin1String("UTC"));
    QVERIFY(cal->timeSpec() == KDateTime::UTC);
    cal->close();
}

void MemoryCalendarTest::testEvents()
{
    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    cal->setProductId(QStringLiteral("fredware calendar"));
    QDate dt = QDate::currentDate();

    Event::Ptr event1 = Event::Ptr(new Event());
    event1->setUid("1");
    event1->setDtStart(KDateTime(dt));
    event1->setDtEnd(KDateTime(dt).addDays(1));
    event1->setSummary("Event1 Summary");
    event1->setDescription("This is a description of the first event");
    event1->setLocation("the place");

    Event::Ptr event2 = Event::Ptr(new Event());
    event2->setUid("2");
    event2->setDtStart(KDateTime(dt).addDays(1));
    event2->setDtEnd(KDateTime(dt).addDays(2));
    event2->setSummary("Event2 Summary");
    event2->setDescription("This is a description of the second event");
    event2->setLocation("the other place");

    QVERIFY(cal->addEvent(event1));
    QVERIFY(cal->addEvent(event2));

    FileStorage store(cal, "foo.ics");
    QVERIFY(store.save());
    cal->close();
    unlink("foo.ics");
}

void MemoryCalendarTest::testIncidences()
{
    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    cal->setProductId(QStringLiteral("fredware calendar"));
    QDate dt = QDate::currentDate();

    Event::Ptr event1 = Event::Ptr(new Event());
    event1->setUid("1");
    event1->setDtStart(KDateTime(dt));
    event1->setDtEnd(KDateTime(dt).addDays(1));
    event1->setSummary("Event1 Summary");
    event1->setDescription("This is a description of the first event");
    event1->setLocation("the place");

    Event::Ptr event2 = Event::Ptr(new Event());
    event2->setUid("2");
    event2->setDtStart(KDateTime(dt).addDays(1));
    event2->setDtEnd(KDateTime(dt).addDays(2));
    event2->setSummary("Event2 Summary");
    event2->setDescription("This is a description of the second event");
    event2->setLocation("the other place");

    QVERIFY(cal->addEvent(event1));
    QVERIFY(cal->addEvent(event2));

    Todo::Ptr todo1 = Todo::Ptr(new Todo());
    todo1->setUid("3");
    todo1->setDtStart(KDateTime(dt).addDays(1));
    todo1->setDtDue(KDateTime(dt).addDays(2));
    todo1->setSummary("Todo1 Summary");
    todo1->setDescription("This is a description of a todo");
    todo1->setLocation("this place");

    Todo::Ptr todo2 = Todo::Ptr(new Todo());
    todo2->setUid("4");
    todo2->setDtStart(KDateTime(dt).addDays(1));
    todo2->setAllDay(true);
    todo2->setSummary("<qt><h1>Todo2 Summary</h1></qt>", true);
    todo2->setDescription("This is a description of a todo");
    todo2->setLocation("<html><a href=\"http://www.fred.com\">this place</a></html>", true);

    QVERIFY(cal->addTodo(todo1));
    QVERIFY(cal->addTodo(todo2));

    FileStorage store(cal, "foo.ics");
    QVERIFY(store.save());
    cal->close();

    QVERIFY(store.load());
    Todo::Ptr todo = cal->incidence("4").staticCast<Todo>();
    QVERIFY(todo->uid() == "4");
    QVERIFY(todo->summaryIsRich());
    QVERIFY(todo->locationIsRich());
    cal->close();
    unlink("foo.ics");
}

void MemoryCalendarTest::testRelationsCrash()
{
    // Before, there was a crash that occurred only when reloading a calendar in which
    // the incidences had special relations.
    // This test tests that scenario, and will crash if it fails.
    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    FileStorage store1(cal, ICALTESTDATADIR "test_relations.ics");
    QVERIFY(store1.load());
    const Todo::List oldTodos = cal->todos();
    qDebug() << "Loaded " << oldTodos.count() << " todos into oldTodos.";

    FileStorage store2(cal, ICALTESTDATADIR "test_relations.ics");
    QVERIFY(store2.load());
    const Todo::List newTodos = cal->todos();
    qDebug() << "Loaded " << newTodos.count() << " into newTodos.";

    // We can saftely access the old deleted todos here, since they are not really deleted
    // and are still kept in a map of deleted items somewhere.
    //
    // Here we make sure that non of the old items have connections to the new items, and
    // the other way around.


    // This doesn't makes sense so i commented it. when you load a calendar the second time
    // it reuses what it can, so oldTodo == newTodo

    /*  foreach ( Todo::Ptr  oldTodo, oldTodos ) {
        foreach ( Todo::Ptr newTodo, newTodos ) {
          QVERIFY( oldTodo != newTodo );

          // Make sure that none of the new todos point to an old, deleted todo
          QVERIFY( newTodo->relatedTo() != oldTodo );
          QVERIFY( !newTodo->relations().contains( oldTodo ) );

          // Make sure that none of the old todos point to a new todo
          QVERIFY( oldTodo->relatedTo() != newTodo );
          QVERIFY( !oldTodo->relations().contains( newTodo ) );
        }
      }
    */
    cal->close();
}

void MemoryCalendarTest::testRecurrenceExceptions()
{
    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    cal->setProductId(QStringLiteral("fredware calendar"));
    QDate dt = QDate::currentDate();
    KDateTime start(dt);

    Event::Ptr event1 = Event::Ptr(new Event());
    event1->setUid("1");
    event1->setDtStart(start);
    event1->setDtEnd(start.addDays(1));
    event1->setSummary("Event1 Summary");
    event1->recurrence()->setDaily(1);
    event1->recurrence()->setDuration(3);
    QVERIFY(cal->addEvent(event1));

    const KDateTime recurrenceId = event1->dtStart().addDays(1);
    Event::Ptr exception1 = cal->createException(event1, recurrenceId).staticCast<Event>();
    QCOMPARE(exception1->recurrenceId(), recurrenceId);
    QCOMPARE(exception1->uid(), event1->uid());
    exception1->setSummary("exception");

    QVERIFY(exception1);
    QVERIFY(cal->addEvent(exception1));

    QCOMPARE(cal->event(event1->uid()), event1);
    QCOMPARE(cal->event(event1->uid(), recurrenceId), exception1);

    const Event::List incidences = cal->rawEvents(start.date(), start.addDays(3).date(), start.timeSpec());
    //Contains incidence and exception
    QCOMPARE(incidences.size(), 2);

    //Returns only exceptions for an event
    const Event::List exceptions = cal->eventInstances(event1);
    QCOMPARE(exceptions.size(), 1);
    QCOMPARE(exceptions.first()->uid(), event1->uid());
    QCOMPARE(exceptions.first()->summary(), exception1->summary());
}

void MemoryCalendarTest::testChangeRecurId()
{
    // When we change the recurring id, internal hashtables should be updated.

    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    KDateTime start(QDate::currentDate());

    // Add main event
    Event::Ptr event1 = Event::Ptr(new Event());
    const QString uid = "1";
    event1->setUid(uid);
    event1->setDtStart(start);
    event1->setDtEnd(start.addDays(1));
    event1->setSummary("Event1 Summary");
    event1->recurrence()->setDaily(1);
    event1->recurrence()->setDuration(3);
    QVERIFY(cal->addEvent(event1));


    // Add exception event:
    const KDateTime recurrenceId = event1->dtStart().addDays(1);
    Event::Ptr exception1 = cal->createException(event1, recurrenceId).staticCast<Event>();
    QCOMPARE(exception1->recurrenceId(), recurrenceId);
    QCOMPARE(exception1->uid(), event1->uid());
    exception1->setSummary("exception");
    QVERIFY(exception1);
    QVERIFY(cal->addEvent(exception1));

    const QString oldIdentifier = exception1->instanceIdentifier();
    Incidence::Ptr foo = cal->instance(oldIdentifier);
    QVERIFY(foo && foo->hasRecurrenceId());
    // Now change the recurring id!
    exception1->setRecurrenceId(start.addDays(2));
    const QString newIdentifier = exception1->instanceIdentifier();
    QVERIFY(oldIdentifier != newIdentifier);

    foo = cal->instance(oldIdentifier);
    QVERIFY(!foo);

    foo = cal->instance(newIdentifier);
    QVERIFY(foo);

    // Test hashing
    Incidence::List incidences = cal->incidences();
    QVERIFY(incidences.count() == 2);

    KDateTime newRecId = start.addDays(2);
    Incidence::Ptr main      = cal->incidence(uid);
    Incidence::Ptr exception = cal->incidence(uid, newRecId);
    Incidence::Ptr noException = cal->incidence(uid, recurrenceId);
    QVERIFY(!noException);
    QVERIFY(main);
    QVERIFY(exception);
    QVERIFY(exception->recurrenceId() == newRecId);
    QVERIFY(exception->summary() == "exception");
    QVERIFY(main->summary() == event1->summary());
}

