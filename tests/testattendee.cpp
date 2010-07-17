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

#include "testattendee.h"
#include "testattendee.moc"
#include "../attendee.h"

#include <qtest_kde.h>
QTEST_KDEMAIN( AttendeeTest, NoGUI )

using namespace KCalCore;

void AttendeeTest::testValidity()
{
  Attendee attendee( "fred", "fred@flintstone.com" );
  attendee.setRole( Attendee::Chair );
  QVERIFY( attendee.role() == Attendee::Chair );
}

void AttendeeTest::testCompare()
{
  Attendee attendee1( "fred", "fred@flintstone.com" );
  Attendee attendee2( "wilma", "wilma@flintstone.com" );
  attendee1.setRole( Attendee::ReqParticipant );
  attendee2.setRole( Attendee::Chair );
  QVERIFY( !( attendee1 == attendee2 ) );
  attendee2.setRole( Attendee::ReqParticipant );
  QVERIFY( ! ( attendee1 == attendee2 ) );
  QVERIFY( attendee1.name() == "fred" );
}

void AttendeeTest::testAssign()
{
  Attendee attendee1( "fred", "fred@flintstone.com" );
  Attendee attendee2 = attendee1;
  QVERIFY( attendee1 == attendee2 );

  attendee2.setRole( Attendee::NonParticipant );
  QVERIFY( !( attendee1 == attendee2 ) );

  Attendee attendee3( attendee1 );
  QVERIFY( attendee3 == attendee1 );
}
