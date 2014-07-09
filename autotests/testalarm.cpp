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

#include "event.h"
#include "alarm.h"

#include <qtest.h>
QTEST_MAIN(AlarmTest)

using namespace KCalCore;

void AlarmTest::testValidity()
{
    Event inc;
    Alarm alarm(&inc);
    alarm.setType(Alarm::Email);
    QVERIFY(alarm.type() == Alarm::Email);
}

void AlarmTest::testCompare()
{
    Event inc1, inc2;
    Alarm alarm1(&inc1), alarm2(&inc2);
    alarm1.setType(Alarm::Email);
    alarm2.setType(Alarm::Email);

    alarm1.setMailAddress(Person::Ptr(new Person("name", "email@foo.com")));
    alarm2.setMailAddress(Person::Ptr(new Person("name", "email@foo.com")));

    QVERIFY(alarm1 == alarm2);

    alarm2.setMailAddress(Person::Ptr(new Person("name", "email@foo.pt")));
    QVERIFY(alarm1 != alarm2);

    alarm2.setType(Alarm::Display);
    QVERIFY(alarm1 != alarm2);
}

void AlarmTest::testAssignment()
{
    Alarm alarm1(0);
    alarm1.setType(Alarm::Display);
    Alarm alarm2 = alarm1;
    QVERIFY(alarm1 == alarm2);
    Alarm *alarm3 = new Alarm(alarm1);
    QVERIFY(alarm2 == *alarm3);
}


void AlarmTest::testSerializer_data()
{
    QTest::addColumn<KCalCore::Alarm::Ptr>("alarm");
    Alarm::Ptr a1 = Alarm::Ptr(new Alarm(0));
    Alarm::Ptr a2 = Alarm::Ptr(new Alarm(0));
    Alarm::Ptr a3 = Alarm::Ptr(new Alarm(0));
    Alarm::Ptr a4 = Alarm::Ptr(new Alarm(0));


    a1->setType(Alarm::Email);
    a2->setType(Alarm::Procedure);
    a3->setType(Alarm::Display);
    a4->setType(Alarm::Audio);

    a3->setDisplayAlarm("foo");
    a3->setText("foo bar");
    a4->setAudioFile("file.mp3");
    a2->setProgramFile("/usr/bin/foo");
    a2->setProgramArguments("--play");

    a1->setMailSubject("empty subject");

    Person::List persons;
    persons << Person::Ptr(new Person("a", "a@a.pt")) << Person::Ptr(new Person("b", "b@b.pt"));
    a1->setMailAddresses(persons);
    a1->setMailAttachment("foo attachment");
    a1->setMailText("mail body");

    a1->setTime(KDateTime(QDate(2006, 8, 3), QTime(8, 0, 0), KDateTime::UTC));
    a2->setStartOffset(Duration(7, Duration::Days));
    a3->setEndOffset(Duration(1, Duration::Days));

    a1->setSnoozeTime(Duration(1, Duration::Seconds));
    a1->setRepeatCount(50);
    a1->setEnabled(true);
    a2->setEnabled(true);
    a3->setHasLocationRadius(false);
    a3->setLocationRadius(100);

    QTest::newRow("alarm1") << a1;
    QTest::newRow("alarm2") << a2;
    QTest::newRow("alarm3") << a3;
    QTest::newRow("alarm4") << a4;
}

void AlarmTest::testSerializer()
{
    QFETCH(KCalCore::Alarm::Ptr, alarm);

    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << alarm; // Serialize

    Alarm::Ptr alarm2 = Alarm::Ptr(new Alarm(0));
    //QVERIFY(*alarm != *alarm2);
    QDataStream stream2(&array, QIODevice::ReadOnly);
    stream2 >> alarm2; // deserialize
    QVERIFY(*alarm == *alarm2);
}

