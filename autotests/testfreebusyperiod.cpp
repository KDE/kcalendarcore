/*
  This file is part of the kcalcore library.

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

#include "testfreebusyperiod.h"

#include "freebusyperiod.h"

#include <qtest.h>

QTEST_MAIN(FreeBusyPeriodTest)
using namespace KCalCore;

void FreeBusyPeriodTest::testValidity()
{
    const KDateTime p1DateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC);
    FreeBusyPeriod p1(p1DateTime, Duration(60));

    QString summary = "I can haz summary?";
    QString location = "The Moon";
    p1.setSummary(summary);
    p1.setLocation(location);

    QVERIFY(p1.hasDuration());
    QCOMPARE(p1.duration().asSeconds(), 60);
    QVERIFY(p1.start() == KDateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC));

    QCOMPARE(p1.summary(), summary);
    QCOMPARE(p1.location(), location);
}

void FreeBusyPeriodTest::testAssign()
{
    const KDateTime p1DateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC);
    FreeBusyPeriod p1(p1DateTime, Duration(60));
    FreeBusyPeriod p2;

    QString summary = "I can haz summary?";
    QString location = "The Moon";
    p1.setSummary(summary);
    p1.setLocation(location);

    p2 = p1;

    QVERIFY(p2.hasDuration());
    QVERIFY(p2.duration().asSeconds() == 60);
    QVERIFY(p2.start() == KDateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC));
    QCOMPARE(p1.summary(), summary);
    QCOMPARE(p1.location(), location);
}

void FreeBusyPeriodTest::testDataStreamOut()
{
    const KDateTime p1DateTime(QDate(2006, 8, 30), QTime(7, 0, 0), KDateTime::UTC);
    FreeBusyPeriod p1(p1DateTime, Duration(60));

    p1.setSummary("I can haz summary?");
    p1.setLocation("The Moon");

    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << p1;

    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);

    Period p2;
    Period periodParent = static_cast<Period>(p1);
    in_stream >> p2;
    QVERIFY(periodParent == p2);

    QString summary;
    in_stream >> summary;
    QCOMPARE(summary, p1.summary());

    QString location;
    in_stream >> location;
    QCOMPARE(location, p1.location());
}

void FreeBusyPeriodTest::testDataStreamIn()
{
    const KDateTime p1DateTime(QDate(2006, 8, 30));
    const Duration duration(24 * 60 * 60) ;
    FreeBusyPeriod p1(p1DateTime, duration);
    p1.setSummary("I can haz summary?");
    p1.setLocation("The Moon");

    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << p1;

    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);

    FreeBusyPeriod p2;
    in_stream >> p2;

    QCOMPARE(p2, p1);
}
