/*
  This file is part of the kcalcore library.

  Copyright (C) 2006,2008 Allen Winter <winter@kde.org>

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
#include "testevent.h"
#include "event.h"

#include <qtest.h>
QTEST_MAIN(EventTest)

Q_DECLARE_METATYPE(KCalCore::Incidence::DateTimeRole)

using namespace KCalCore;

void EventTest::testSetRoles_data()
{
    QTest::addColumn<KDateTime>("originalDtStart");
    QTest::addColumn<KDateTime>("originalDtEnd");

    QTest::addColumn<KCalCore::Incidence::DateTimeRole>("setRole");
    QTest::addColumn<KDateTime>("dateTimeToSet");
    QTest::addColumn<KDateTime>("expectedDtStart");
    QTest::addColumn<KDateTime>("expectedDtEnd");


    const KDateTime todayDate(QDate::currentDate());   // all day event
    const KDateTime todayDateTime = KDateTime::currentUtcDateTime();

    QTest::newRow("dnd 0 duration") << todayDate << todayDate << KCalCore::Incidence::RoleDnD
                                    << todayDateTime << todayDateTime << todayDateTime.addSecs(3600);
}

void EventTest::testSetRoles()
{
    QFETCH(KDateTime, originalDtStart);
    QFETCH(KDateTime, originalDtEnd);
    QFETCH(KCalCore::Incidence::DateTimeRole, setRole);

    QFETCH(KDateTime, dateTimeToSet);
    QFETCH(KDateTime, expectedDtStart);
    QFETCH(KDateTime, expectedDtEnd);

    Event::Ptr event = Event::Ptr(new Event());
    event->setDtStart(originalDtStart);
    event->setDtEnd(originalDtEnd);
    event->setAllDay(originalDtStart.isDateOnly());

    event->setDateTime(dateTimeToSet, setRole);
    QCOMPARE(event->dtStart(), expectedDtStart);
    QCOMPARE(event->dtEnd(), expectedDtEnd);
}

void EventTest::testValidity()
{
    QDate dt = QDate::currentDate();
    Event *event = new Event();
    event->setDtStart(KDateTime(dt));
    event->setDtEnd(KDateTime(dt).addDays(1));
    event->setSummary("Event1 Summary");
    event->setDescription("This is a description of the first event");
    event->setLocation("the place");
    //KDE5: QVERIFY( event->typeStr() == i18n( "event" ) );
    QVERIFY(event->summary() == "Event1 Summary");
    QVERIFY(event->location() == "the place");
    QVERIFY(event->type() == Incidence::TypeEvent);
}

void EventTest::testCompare()
{
    QDate dt = QDate::currentDate();
    Event event1;
    event1.setDtStart(KDateTime(dt));
    event1.setDtEnd(KDateTime(dt).addDays(1));
    event1.setSummary("Event1 Summary");
    event1.setDescription("This is a description of the first event");
    event1.setLocation("the place");

    Event event2;
    event2.setDtStart(KDateTime(dt).addDays(1));
    event2.setDtEnd(KDateTime(dt).addDays(2));
    event2.setSummary("Event2 Summary");
    event2.setDescription("This is a description of the second event");
    event2.setLocation("the other place");

    QVERIFY(!(event1 == event2));
    QVERIFY(event1.dtEnd() == event2.dtStart());
    QVERIFY(event2.summary() == "Event2 Summary");
}

void EventTest::testClone()
{
    QDate dt = QDate::currentDate();
    Event event1;
    event1.setDtStart(KDateTime(dt));
    event1.setDtEnd(KDateTime(dt).addDays(1));
    event1.setSummary("Event1 Summary");
    event1.setDescription("This is a description of the first event");
    event1.setLocation("the place");

    Event *event2 = event1.clone();
    QVERIFY(event1.summary() == event2->summary());
    QVERIFY(event1.dtStart() == event2->dtStart());
    QVERIFY(event1.dtEnd() == event2->dtEnd());
    QVERIFY(event1.description() == event2->description());
    QVERIFY(event1.location() == event2->location());
}

void EventTest::testCopy()
{
    QDate dt = QDate::currentDate();
    Event event1;
    event1.setDtStart(KDateTime(dt));
    event1.setDtEnd(KDateTime(dt).addDays(1));
    event1.setSummary("Event1 Summary");
    event1.setDescription("This is a description of the first event");
    event1.setLocation("the place");
    event1.setTransparency(Event::Transparent);

    Event event2 = event1;
    QVERIFY(event1.summary() == event2.summary());
    QVERIFY(event1.dtStart() == event2.dtStart());
    QVERIFY(event1.dtEnd() == event2.dtEnd());
    QVERIFY(event1.description() == event2.description());
    QVERIFY(event1.location() == event2.location());
}

void EventTest::testAssign()
{
    QDate dt = QDate::currentDate();
    Event event1;
    event1.setDtStart(KDateTime(dt));
    event1.setDtEnd(KDateTime(dt).addDays(1));
    event1.setSummary("Event1 Summary");
    event1.setDescription("This is a description of the first event");
    event1.setLocation("the place");
    event1.setTransparency(Event::Transparent);

    Event event2 = event1;
    QVERIFY(event1 == event2);
}

void EventTest::testSerializer_data()
{
    QTest::addColumn<KCalCore::Event::Ptr>("event");
    KDateTime today = KDateTime::currentUtcDateTime();
    KDateTime yesterday = today.addDays(-1);

    Event::Ptr event1 = Event::Ptr(new Event());
    Attendee::Ptr attendee1(new Attendee("fred", "fred@flintstone.com"));
    event1->addAttendee(attendee1);
    event1->setDtStart(yesterday);
    event1->setDtEnd(today);

    Event::Ptr event2 = Event::Ptr(new Event());
    Attendee::Ptr attendee2(new Attendee("fred", "fred@flintstone.com"));
    event2->addAttendee(attendee2);
    event2->setDtStart(yesterday);
    event2->setDtEnd(today);
    event2->setAllDay(true);

    event2->addComment("comment1");
    event2->setUrl(QUrl("http://someurl"));

    event2->setCustomProperty("app", "key", "value");

    // Remaining properties tested in testtodo.cpp

    QTest::newRow("event") << event1;
    QTest::newRow("event2") << event2;
}

void EventTest::testSerializer()
{
    QFETCH(KCalCore::Event::Ptr, event);
    IncidenceBase::Ptr incidenceBase = event.staticCast<KCalCore::IncidenceBase>();

    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << incidenceBase;

    Event::Ptr event2 = Event::Ptr(new Event());
    IncidenceBase::Ptr incidenceBase2 = event2.staticCast<KCalCore::IncidenceBase>();
    QVERIFY(*event != *event2);
    QDataStream stream2(&array, QIODevice::ReadOnly);
    stream2 >> incidenceBase2;
    QVERIFY(*event == *event2);
}

void EventTest::testDurationDtEnd()
{
    const QDate dt = QDate::currentDate();

    {
        Event event;
        event.setDtStart(KDateTime(dt));
        event.setDtEnd(KDateTime(dt).addDays(1));
        QCOMPARE(event.hasEndDate(), true);
        QCOMPARE(event.hasDuration(), false);
    }
    {
        Event event;
        event.setDtStart(KDateTime(dt));
        event.setDuration(Duration(KDateTime(dt), KDateTime(dt).addDays(1)));
        QCOMPARE(event.hasDuration(), true);
        QCOMPARE(event.hasEndDate(), false);
    }

}
