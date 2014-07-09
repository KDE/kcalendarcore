/*
  This file is part of the kcalcore library.

  Copyright (c) 2006 Allen Winter <winter@kde.org>

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

#include "testduration.h"
#include "duration.h"

#include <kdatetime.h>

#include <qtest.h>
QTEST_MAIN(DurationTest)

using namespace KCalCore;

void DurationTest::testValidity()
{
    const KDateTime firstDateTime(QDate(2006, 8, 3), QTime(7, 0, 0), KDateTime::UTC);

    Duration d(firstDateTime,
               KDateTime(QDate(2006, 8, 3), QTime(8, 0, 0), KDateTime::UTC));

    QCOMPARE(d.asSeconds(), 1 * 60 * 60);
}

void DurationTest::testCompare()
{
    const KDateTime firstDateTime(QDate(2006, 8, 3), QTime(7, 0, 0), KDateTime::UTC);

    Duration d1(firstDateTime,
                KDateTime(QDate(2006, 8, 3), QTime(8, 0, 0), KDateTime::UTC));
    //d1 has 1hr duration

    Duration d2(2 * 60 * 60);   // 2hr duration

    Duration d1copy(d1);   // test copy constructor
    Duration d1assign = d1; // test operator=

    QVERIFY(d1 < d2);
    QVERIFY(d1 != d2);
    QVERIFY(d1copy == d1);
    QVERIFY(d1assign == d1);

    Duration d3(7, Duration::Days);
    Duration d4(7 * 24 * 60 * 60, Duration::Seconds);
    QVERIFY(d3 != d4);   // cannot compare days durations with seconds durations

    QVERIFY(d3 > d2);
    QVERIFY(-d3 < d2);

    Duration d5 = d1;
    d5 += d2; // should be 3hrs
    QVERIFY(d5 > d2);
    QVERIFY(d2 < d5);
    Duration d6(3 * 60 * 60);
    QVERIFY(d6 == d5);
    QVERIFY((d6-=(2 * 60 * 60)) == d1);
}


void DurationTest::testSerializer_data()
{
    QTest::addColumn<KCalCore::Duration>("duration");

    Duration duration1;
    Duration duration2(7, Duration::Days);
    Duration duration3(7 * 24 * 60 * 60, Duration::Seconds);

    const KDateTime firstDateTime(QDate(2006, 8, 3), QTime(7, 0, 0), KDateTime::UTC);
    Duration duration4(firstDateTime, KDateTime(QDate(2006, 8, 3), QTime(8, 0, 0), KDateTime::UTC));


    QTest::newRow("duration1") << duration1;
    QTest::newRow("duration2") << duration2;
    QTest::newRow("duration3") << duration3;
    QTest::newRow("duration4") << duration4;
}

void DurationTest::testSerializer()
{
    QFETCH(KCalCore::Duration, duration);

    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << duration; // Serialize

    Duration duration2;
    QDataStream stream2(&array, QIODevice::ReadOnly);
    stream2 >> duration2; // deserialize
    QVERIFY(duration == duration2);
}
