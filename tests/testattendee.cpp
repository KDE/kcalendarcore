/*
  This file is part of the kcalcore library.
  Copyright (C) 2006,2008 Allen Winter <winter@kde.org>
  Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>
  Copyright (C) 2009-2010 Klaralvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>

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

#include <kdebug.h>

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

void AttendeeTest::testDataStreamOut()
{
  Attendee::Ptr attendee1( new Attendee( "fred", "fred@flintstone.com" ) );
  attendee1->setRSVP( true );
  attendee1->setRole( Attendee::Chair );
  attendee1->setUid( "Shooby Doo Bop" );
  attendee1->setDelegate( "I AM THE Delegate" );
  attendee1->setDelegator( "AND I AM THE Delegator" );
  attendee1->setCustomProperty("name", "value");
  attendee1->setCustomProperty("foo", "bar");

  QByteArray byteArray;
  QDataStream out_stream( &byteArray, QIODevice::WriteOnly );

  out_stream << attendee1;

  QDataStream in_stream( &byteArray, QIODevice::ReadOnly );

  Person::Ptr person;
  bool rsvp;
  QString name, email, delegate, delegator, uid;
  CustomProperties customProperties;
  Attendee::Role role;
  Attendee::PartStat status;
  uint role_int, status_int;

  in_stream >> person;
  QVERIFY( person->name() ==  attendee1->name() );
  QVERIFY( person->email() ==  attendee1->email() );

  in_stream >> rsvp;
  QVERIFY( rsvp == attendee1->RSVP() );

  in_stream >> role_int;
  role = Attendee::Role( role_int );
  QVERIFY( role == attendee1->role() );

  in_stream >> status_int;
  status = Attendee::PartStat( status_int );
  QVERIFY( status == attendee1->status() );

  in_stream >> uid;
  QVERIFY( uid == attendee1->uid() );

  in_stream >> delegate;
  QVERIFY( delegate == attendee1->delegate() );

  in_stream >> delegator;
  QVERIFY( delegator == attendee1->delegator() );

  in_stream >> customProperties;
  QVERIFY( customProperties == attendee1->customProperties() );
}

void AttendeeTest::testDataStreamIn()
{
  Attendee::Ptr attendee1( new Attendee( "fred", "fred@flintstone.com" ) );
  attendee1->setRSVP( true );
  attendee1->setRole( Attendee::Chair );
  attendee1->setUid( "Shooby Doo Bop" );
  attendee1->setDelegate( "I AM THE Delegate" );
  attendee1->setDelegator( "AND I AM THE Delegator" );
  attendee1->setCustomProperty("name", "value");
  attendee1->setCustomProperty("foo", "bar");

  QByteArray byteArray;
  QDataStream out_stream( &byteArray, QIODevice::WriteOnly );

  out_stream << attendee1;

  Attendee::Ptr attendee2;
  QDataStream in_stream( &byteArray, QIODevice::ReadOnly );

  in_stream >> attendee2;

  QVERIFY( attendee2 );
  QVERIFY( attendee2->uid() == attendee1->uid() );
  QVERIFY( attendee2->RSVP() == attendee1->RSVP() );
  QVERIFY( attendee2->role() == attendee1->role() );
  QVERIFY( attendee2->status() == attendee1->status() );
  QVERIFY( attendee2->delegate() == attendee1->delegate() );
  QVERIFY( attendee2->delegator() == attendee1->delegator() );
  QVERIFY( attendee2->customProperties() == attendee1->customProperties() );
  QVERIFY( *attendee1 == *attendee2 );
}

