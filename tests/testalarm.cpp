/*
  This file is part of the kcalcore library.
  Copyright (C) 2006 Allen Winter <winter@kde.org>

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

#include "testalarm.h"
#include "testalarm.moc"

#include "../event.h"
#include "../alarm.h"

#include <qtest_kde.h>
QTEST_KDEMAIN( AlarmTest, NoGUI )

using namespace KCalCore;

void AlarmTest::testValidity()
{
  Event inc;
  Alarm alarm( &inc );
  alarm.setType( Alarm::Email );
  QVERIFY( alarm.type() == Alarm::Email );
}

void AlarmTest::testCompare()
{
  Event inc1, inc2;
  Alarm alarm1( &inc1 ), alarm2( &inc2 );
  alarm1.setType( Alarm::Email );
  alarm2.setType( Alarm::Email );
  QVERIFY( alarm1 == alarm2 );
  alarm2.setType( Alarm::Display );
  QVERIFY( alarm1 != alarm2 );
}

void AlarmTest::testAssignment()
{
  Alarm alarm1( 0 );
  alarm1.setType( Alarm::Display );
  Alarm alarm2 = alarm1;
  QVERIFY( alarm1 == alarm2 );
  Alarm *alarm3 = new Alarm( alarm1 );
  QVERIFY( alarm2 == *alarm3 );
}
