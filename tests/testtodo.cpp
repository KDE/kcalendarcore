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
#include "testtodo.h"
#include "testtodo.moc"
#include "../todo.h"

#include <qtest_kde.h>
QTEST_KDEMAIN( TodoTest, NoGUI )

using namespace KCalCore;

void TodoTest::testValidity()
{
  QDate dt = QDate::currentDate();
  Todo *todo = new Todo();
  todo->setDtStart( KDateTime( dt ) );
  todo->setDtDue( KDateTime( dt ).addDays( 1 ) );
  todo->setSummary( "To-do1 Summary" );
  todo->setDescription( "This is a description of the first to-do" );
  todo->setLocation( "the place" );
  todo->setPercentComplete( 5 );
  //KDE5: QVERIFY( todo->typeStr() == i18n( "to-do" ) );
  QVERIFY( todo->summary() == "To-do1 Summary" );
  QVERIFY( todo->location() == "the place" );
  QVERIFY( todo->percentComplete() == 5 );
}

void TodoTest::testCompare()
{
  QDate dt = QDate::currentDate();
  Todo todo1;
  todo1.setDtStart( KDateTime( dt ) );
  todo1.setDtDue( KDateTime( dt ).addDays( 1 ) );
  todo1.setSummary( "To-do1 Summary" );
  todo1.setDescription( "This is a description of the first to-do" );
  todo1.setLocation( "the place" );
  todo1.setCompleted( true );

  Todo todo2;
  todo2.setDtStart( KDateTime( dt ).addDays( 1 ) );
  todo2.setDtDue( KDateTime( dt ).addDays( 2 ) );
  todo2.setSummary( "To-do2 Summary" );
  todo2.setDescription( "This is a description of the second to-do" );
  todo2.setLocation( "the other place" );
  todo2.setCompleted( false );

  QVERIFY( !( todo1 == todo2 ) );
  QVERIFY( todo1.dtDue() == todo2.dtStart() );
  QVERIFY( todo2.summary() == "To-do2 Summary" );
  QVERIFY( !( todo1.isCompleted() == todo2.isCompleted() ) );
}

void TodoTest::testClone()
{
  QDate dt = QDate::currentDate();
  Todo todo1;
  todo1.setDtStart( KDateTime( dt ) );
  todo1.setDtDue( KDateTime( dt ).addDays( 1 ) );
  todo1.setSummary( "Todo1 Summary" );
  todo1.setDescription( "This is a description of the first todo" );
  todo1.setLocation( "the place" );

  Todo *todo2 = todo1.clone();
  QVERIFY( todo1.summary() == todo2->summary() );
  QVERIFY( todo1.dtStart() == todo2->dtStart() );
  QVERIFY( todo1.dtDue() == todo2->dtDue() );
  QVERIFY( todo1.description() == todo2->description() );
  QVERIFY( todo1.location( ) == todo2->location() );
  QVERIFY( todo1.isCompleted() == todo2->isCompleted() );
}

void TodoTest::testAssign()
{
  QDate dt = QDate::currentDate();
  Todo todo1;
  todo1.setDtStart( KDateTime( dt ) );
  todo1.setDtDue( KDateTime( dt ).addDays( 1 ) );
  todo1.setSummary( "Todo1 Summary" );
  todo1.setDescription( "This is a description of the first todo" );
  todo1.setLocation( "the place" );

  Todo todo2 = todo1;
  QVERIFY( todo1 == todo2 );
}

void TodoTest::testSetCompleted() {

  Todo todo1, todo2;
  todo1.setSummary( "Todo Summary" );
  todo2.setSummary( "Todo Summary" );
  KDateTime today = KDateTime::currentUtcDateTime();

  // due yesterday
  KDateTime originalDueDate = today.addDays( -1 );

  todo1.setDtDue( originalDueDate );
  todo1.recurrence()->setDaily( 1 );
  todo1.setCompleted( today );

  todo2.setCompleted( true );

  QVERIFY( originalDueDate != todo1.dtDue() );
  QVERIFY( !todo1.isCompleted() );
  QVERIFY( todo2.isCompleted() );
}

void TodoTest::testStatus() {
  KDateTime today = KDateTime::currentUtcDateTime();
  KDateTime yesterday = today.addDays( -1 );

  Todo todo1;
  todo1.setDtStart( yesterday );
  todo1.setDtDue( today );
  todo1.setPercentComplete( 50 );
  QVERIFY( todo1.isInProgress( false ) );
  QVERIFY( !todo1.isNotStarted( false ) );
  QVERIFY( !todo1.isOverdue() );
  todo1.setPercentComplete( 100 );
  QVERIFY( todo1.isCompleted() );

  Todo todo2 = todo1;
  todo2.setPercentComplete( 33 );
  todo2.setHasDueDate( false );
  QVERIFY( todo2.isOpenEnded() );
}
