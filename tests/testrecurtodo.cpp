/*
  This file is part of the kcalcore library.

  Copyright (c) 2011 Sérgio Martins <iamsergio@gmail.com>

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
#include "testrecurtodo.h"
#include "../todo.h"
#include <kdebug.h>
#include <qtest_kde.h>
QTEST_KDEMAIN( RecurTodoTest, NoGUI )

using namespace KCalCore;

void RecurTodoTest::testAllDay()
{
  qputenv( "TZ", "GMT" );
  const QDate dueDate( QDate::currentDate().addDays( -3 ) );
  Todo *todo = new Todo();
  todo->setDtStart( KDateTime( dueDate.addDays( -1 ) ) );
  todo->setDtDue( KDateTime( dueDate ) );
  todo->setSummary( QLatin1String( "All day event" ) );
  todo->setAllDay( true );

  Recurrence *recurrence = todo->recurrence();
  recurrence->unsetRecurs();
  recurrence->setDaily( 1 );
  QVERIFY( todo->dtDue() == KDateTime( dueDate ) );
  todo->setCompleted( KDateTime::currentUtcDateTime() );
  QVERIFY( todo->recurs() );
  QVERIFY( todo->percentComplete() == 0 );
  QVERIFY( todo->dtDue().date() == QDate::currentDate() );

  todo->setCompleted( KDateTime::currentUtcDateTime() );
  QVERIFY( todo->dtDue().date() == QDate::currentDate().addDays( 1 ) );
  QVERIFY( todo->dtDue( true /*first ocurrence*/ ).date() == dueDate );
}

void RecurTodoTest::testRecurrenceStart()
{
  qputenv( "TZ", "GMT" );
  const QDateTime currentDateTime = QDateTime::currentDateTime();
  const QDate currentDate = currentDateTime.date();
  const QTime currentTimeWithMS = currentDateTime.time();

  const QDate fourDaysAgo( currentDate.addDays( -4 ) );
  const QDate treeDaysAgo( currentDate.addDays( -3 ) );
  const QTime currentTime( currentTimeWithMS.hour(), currentTimeWithMS.minute(), currentTimeWithMS.second() );

  Todo *todo = new Todo();
  Recurrence *recurrence = todo->recurrence();
  recurrence->unsetRecurs();
  recurrence->setDaily( 1 );
  todo->setDtStart( KDateTime( fourDaysAgo, currentTime ) );
  const KDateTime originalDtDue(treeDaysAgo, currentTime );
  todo->setDtDue( originalDtDue );
  todo->setSummary( QLatin1String( "Not an all day event" ) );
  QVERIFY( !todo->allDay() );
  QVERIFY( recurrence->startDateTime().isValid() );
}

void RecurTodoTest::testNonAllDay()
{
  qputenv( "TZ", "GMT" );
  const QDateTime currentDateTime = QDateTime::currentDateTime();
  const QDate currentDate = currentDateTime.date();
  const QTime currentTimeWithMS = currentDateTime.time();

  const QDate fourDaysAgo( currentDate.addDays( -4 ) );
  const QDate treeDaysAgo( currentDate.addDays( -3 ) );
  const QTime currentTime( currentTimeWithMS.hour(), currentTimeWithMS.minute(), currentTimeWithMS.second() );

  Todo *todo = new Todo();
  todo->setDtStart( KDateTime( fourDaysAgo, currentTime ) );
  const KDateTime originalDtDue(treeDaysAgo, currentTime );
  todo->setDtDue( originalDtDue );
  todo->setSummary( QLatin1String( "Not an all day event" ) );
  QVERIFY( !todo->allDay() );
  Recurrence *recurrence = todo->recurrence();
  recurrence->unsetRecurs();
  recurrence->setDaily( 1 );
  QVERIFY( recurrence->startDateTime().isValid() );
  QVERIFY( todo->dtDue() == originalDtDue );
  todo->setCompleted( KDateTime::currentUtcDateTime() );
  QVERIFY( todo->recurs() );
  QVERIFY( todo->percentComplete() == 0 );

  const bool equal = todo->dtDue() == KDateTime( currentDate,
                                                 currentTime,
                                                 todo->dtDue().timeSpec() ).addDays( 1 );
  if ( !equal ) {
    kDebug() << "Test Failed. dtDue = " << todo->dtDue() << "OriginalDtDue:" << originalDtDue
             <<  "KDateTime:"
             << KDateTime( currentDate, currentTime, todo->dtDue().timeSpec() ).addDays( 1 );
  }

  QVERIFY( equal );

  todo->setCompleted( KDateTime::currentUtcDateTime() );
  QVERIFY( todo->dtDue() == KDateTime( currentDate, currentTime, todo->dtDue().timeSpec() ).addDays( 2 ) );
  QVERIFY( todo->dtDue( true /*first ocurrence*/ ) == KDateTime( treeDaysAgo, currentTime ) );
}

void RecurTodoTest::testIsAllDay()
{
  ;
  KCalCore::Todo::Ptr todo(new KCalCore::Todo());
  todo->setUid("todo");
  todo->setDtStart(KDateTime(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC));
  todo->setDtDue(KDateTime(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC));
  todo->recurrence()->setDaily(1);
  todo->recurrence()->setDuration(2);
  QCOMPARE(todo->allDay(), false);
  QCOMPARE(todo->recurrence()->allDay(), false);

  KCalCore::Todo::Ptr allDay(new KCalCore::Todo());
  allDay->setUid("todo");
  allDay->setDtStart(KDateTime(QDate(2013, 03, 10), KDateTime::UTC));
  allDay->setDtDue(KDateTime(QDate(2013, 03, 10), KDateTime::UTC));
  allDay->recurrence()->setDaily(1);
  allDay->recurrence()->setDuration(2);
  QCOMPARE(allDay->allDay(), true);
  QCOMPARE(allDay->recurrence()->allDay(), true);
}

void RecurTodoTest::testHasDueDate()
{
  KCalCore::Todo::Ptr todo(new KCalCore::Todo());
  todo->setUid("todo");
  todo->setDtStart(KDateTime(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC));
  todo->recurrence()->setDaily(1);
  todo->recurrence()->setDuration(2);
  QVERIFY(!todo->hasDueDate());
}

void RecurTodoTest::testDtStart()
{
  KDateTime start(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC);
  KCalCore::Todo::Ptr todo(new KCalCore::Todo());
  todo->setUid("todo");
  todo->setDtStart(start);
  todo->recurrence()->setDaily(1);
  todo->recurrence()->setDuration(2);
  QCOMPARE(todo->dtStart(), start);

  KCalCore::Todo::Ptr todoWithDue(new KCalCore::Todo());
  todoWithDue->setUid("todoWithDue");
  todoWithDue->setDtStart(start);
  todoWithDue->setDtDue(KDateTime(start).addSecs(60));
  todoWithDue->recurrence()->setDaily(1);
  todoWithDue->recurrence()->setDuration(2);
  QCOMPARE(todoWithDue->dtStart(), start);
}

void RecurTodoTest::testRecurrenceBasedOnDtStart()
{
  const KDateTime dtstart(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC);
  const KDateTime dtdue(QDate(2013, 03, 10), QTime(11, 0, 0), KDateTime::UTC);

  KCalCore::Todo::Ptr todo(new KCalCore::Todo());
  todo->setUid("todo");
  todo->setDtStart(dtstart);
  todo->setDtDue(dtdue);
  todo->recurrence()->setDaily(1);
  todo->recurrence()->setDuration(3);

  QCOMPARE(todo->recurrence()->getNextDateTime(dtstart), KDateTime(dtstart).addDays(1));
  QCOMPARE(todo->recurrence()->getNextDateTime(KDateTime(dtstart).addDays(1)), KDateTime(dtstart).addDays(2));
  QCOMPARE(todo->recurrence()->getNextDateTime(KDateTime(dtstart).addDays(2)), KDateTime());
}

//For backwards compatibility only
void RecurTodoTest::testRecurrenceBasedOnDue()
{
  const KDateTime dtdue(QDate(2013, 03, 10), QTime(11, 0, 0), KDateTime::UTC);

  KCalCore::Todo::Ptr todo(new KCalCore::Todo());
  todo->setUid("todo");
  todo->setDtDue(dtdue);
  todo->recurrence()->setDaily(1);
  todo->recurrence()->setDuration(3);

  QCOMPARE(todo->recurrence()->getNextDateTime(dtdue), KDateTime(dtdue).addDays(1));
  QCOMPARE(todo->recurrence()->getNextDateTime(KDateTime(dtdue).addDays(1)), KDateTime(dtdue).addDays(2));
  QCOMPARE(todo->recurrence()->getNextDateTime(KDateTime(dtdue).addDays(2)), KDateTime());
}
