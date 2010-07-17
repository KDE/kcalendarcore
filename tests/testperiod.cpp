/*
  This file is part of the kcalcore library.

  Copyright (c) 2006 Narayan Newton <narayannewton@gmail.com>

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

#include "testperiod.h"
#include "testperiod.moc"
#include "../period.h"

#include <qtest_kde.h>
QTEST_KDEMAIN( PeriodTest, NoGUI )

using namespace KCalCore;

void PeriodTest::testValidity()
{
  const KDateTime p1DateTime( QDate( 2006, 8, 30 ), QTime( 7, 0, 0 ), KDateTime::UTC );
  Period p1( p1DateTime,
             Duration( 60 ) );
  Period p2;

  QVERIFY( p1.hasDuration() );
  QCOMPARE( p1.duration().asSeconds(), 60 );
  QVERIFY( p1.start() == KDateTime( QDate( 2006, 8, 30 ), QTime( 7, 0, 0 ), KDateTime::UTC ) );

  p2 = p1;

  QVERIFY( p2.hasDuration() );
  QVERIFY( p2.duration().asSeconds() == 60 );
  QVERIFY( p2.start() == KDateTime( QDate( 2006, 8, 30 ), QTime( 7, 0, 0 ), KDateTime::UTC ) );
}

void PeriodTest::testCompare()
{
  const KDateTime p1DateTime( QDate( 2006, 8, 30 ) );
  Period p1( p1DateTime, Duration( 24 * 60 * 60 ) );
  const KDateTime p2DateTime( QDate( 2006, 8, 29 ) );
  Period p2( p2DateTime, Duration( 23 * 60 * 60 ) );
  Period p1copy( p1 ); // test copy constructor
  Period p1assign = p1; // test operator=

  QVERIFY( p2 < p1 );
  QVERIFY( !( p1 == p2 ) );
  QVERIFY( p1copy == p1 );
  QVERIFY( p1assign == p1 );

}
