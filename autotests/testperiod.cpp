/*
  This file is part of the kcalcore library.

  Copyright (c) 2006 Narayan Newton <narayannewton@gmail.com>
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

#include "testperiod.h"
#include "period.h"

#include <ksystemtimezone.h>

#include <qtest.h>
QTEST_MAIN(PeriodTest)

using namespace KCalCore;

void PeriodTest::testValidity()
{
    const KDateTime p1DateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC);
    Period p1(p1DateTime,
              Duration(60));
    Period p2;

    QVERIFY(p1.hasDuration());
    QCOMPARE(p1.duration().asSeconds(), 60);
    QVERIFY(p1.start() == KDateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC));

    p2 = p1;

    QVERIFY(p2.hasDuration());
    QVERIFY(p2.duration().asSeconds() == 60);
    QVERIFY(p2.start() == KDateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC));

    const KDateTime p3DateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC);
    Period p3(p3DateTime, Duration(24 * 60 * 60));

    QVERIFY(p3.hasDuration());
    QVERIFY(p3.duration().asSeconds() == 24 * 60 * 60);
    QVERIFY(p3.start() == KDateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC));
}

void PeriodTest::testCompare()
{
    const KDateTime p1DateTime(QDate(2006, 8, 30));
    Period p1(p1DateTime, Duration(24 * 60 * 60));
    const KDateTime p2DateTime(QDate(2006, 8, 29));
    Period p2(p2DateTime, Duration(23 * 60 * 60));
    const KDateTime p3DateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC);
    Period p3(p3DateTime, Duration(24 * 60 * 60));
    Period p1copy(p1);   // test copy constructor
    Period p1assign = p1; // test operator=
    Period p3copy(p3);
    Period p3assign = p3;

    QVERIFY(p2 < p1);
    QVERIFY(!(p1 == p2));
    QVERIFY(p1copy == p1);
    QVERIFY(p1assign == p1);
    QVERIFY(p3copy == p3);
    QVERIFY(p3assign == p3);

}

void PeriodTest::testDataStreamOut()
{
    const KDateTime p1DateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC);
    const Duration duration(24 * 60 * 60);
    Period p1(p1DateTime, duration);

    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << p1;

    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);

    KDateTime begin;
    in_stream >> begin;
    // There is no way to serialize KDateTime as of KDE4.5
    // and the to/fromString methods do not perform a perfect reconstruction
    // of a datetime
    QVERIFY(begin.compare(p1.start()) == KDateTime::Equal);

    KDateTime end;
    in_stream >> end;
    QVERIFY(end.compare(p1.end()) == KDateTime::Equal);

    bool dailyduration;
    in_stream >> dailyduration;
    QVERIFY(dailyduration == duration.isDaily());

    bool hasduration;
    in_stream >> hasduration;
    QVERIFY(hasduration == p1.hasDuration());
}

void PeriodTest::testDataStreamIn()
{
    const KDateTime p1DateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC);
    const Duration duration(24 * 60 * 60) ;
    Period p1(p1DateTime, duration);

    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << p1;

    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);

    Period p2;

    in_stream >> p2;

    QVERIFY(p1 == p2);
}

