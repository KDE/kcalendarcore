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
#include "testevent.moc"
#include "../event.h"

#include <qtest_kde.h>
QTEST_KDEMAIN( EventTest, NoGUI )

using namespace KCalCore;

void EventTest::testValidity()
{
  QDate dt = QDate::currentDate();
  Event *event = new Event();
  event->setDtStart( KDateTime( dt ) );
  event->setDtEnd( KDateTime( dt ).addDays( 1 ) );
  event->setSummary( "Event1 Summary" );
  event->setDescription( "This is a description of the first event" );
  event->setLocation( "the place" );
  //KDE5: QVERIFY( event->typeStr() == i18n( "event" ) );
  QVERIFY( event->summary() == "Event1 Summary" );
  QVERIFY( event->location() == "the place" );
  QVERIFY( event->type() == Incidence::TypeEvent );
}

void EventTest::testCompare()
{
  QDate dt = QDate::currentDate();
  Event event1;
  event1.setDtStart( KDateTime( dt ) );
  event1.setDtEnd( KDateTime( dt ).addDays( 1 ) );
  event1.setSummary( "Event1 Summary" );
  event1.setDescription( "This is a description of the first event" );
  event1.setLocation( "the place" );

  Event event2;
  event2.setDtStart( KDateTime( dt ).addDays( 1 ) );
  event2.setDtEnd( KDateTime( dt ).addDays( 2 ) );
  event2.setSummary( "Event2 Summary" );
  event2.setDescription( "This is a description of the second event" );
  event2.setLocation( "the other place" );

  QVERIFY( !( event1 == event2 ) );
  QVERIFY( event1.dtEnd() == event2.dtStart() );
  QVERIFY( event2.summary() == "Event2 Summary" );
}

void EventTest::testClone()
{
  QDate dt = QDate::currentDate();
  Event event1;
  event1.setDtStart( KDateTime( dt ) );
  event1.setDtEnd( KDateTime( dt ).addDays( 1 ) );
  event1.setSummary( "Event1 Summary" );
  event1.setDescription( "This is a description of the first event" );
  event1.setLocation( "the place" );

  Event *event2 = event1.clone();
  QVERIFY( event1.summary() == event2->summary() );
  QVERIFY( event1.dtStart() == event2->dtStart() );
  QVERIFY( event1.dtEnd() == event2->dtEnd() );
  QVERIFY( event1.description() == event2->description() );
  QVERIFY( event1.location( ) == event2->location() );
}

void EventTest::testCopy()
{
  QDate dt = QDate::currentDate();
  Event event1;
  event1.setDtStart( KDateTime( dt ) );
  event1.setDtEnd( KDateTime( dt ).addDays( 1 ) );
  event1.setSummary( "Event1 Summary" );
  event1.setDescription( "This is a description of the first event" );
  event1.setLocation( "the place" );
  event1.setTransparency( Event::Transparent );

  Event event2 = event1;
  QVERIFY( event1.summary() == event2.summary() );
  QVERIFY( event1.dtStart() == event2.dtStart() );
  QVERIFY( event1.dtEnd() == event2.dtEnd() );
  QVERIFY( event1.description() == event2.description() );
  QVERIFY( event1.location( ) == event2.location() );
}

void EventTest::testAssign()
{
  QDate dt = QDate::currentDate();
  Event event1;
  event1.setDtStart( KDateTime( dt ) );
  event1.setDtEnd( KDateTime( dt ).addDays( 1 ) );
  event1.setSummary( "Event1 Summary" );
  event1.setDescription( "This is a description of the first event" );
  event1.setLocation( "the place" );
  event1.setTransparency( Event::Transparent );

  Event event2 = event1;
  QVERIFY( event1 == event2 );
}
