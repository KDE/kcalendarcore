/*
  This file is part of the kcalcore library.

  Copyright (c) 2007-2008 Allen Winter <winter@kde.org>

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
#include "testfreebusy.h"
#include "freebusy.h"

#include <qtest.h>
QTEST_MAIN(FreeBusyTest)

using namespace KCalCore;

void FreeBusyTest::testValidity()
{
    const KDateTime firstDateTime(QDate(2007, 7, 23), QTime(7, 0, 0), KDateTime::UTC);

    FreeBusy fb1(
        firstDateTime,
        KDateTime(QDate(2007, 7, 23), QTime(8, 0, 0), KDateTime::UTC));

    QVERIFY(fb1.dtEnd() ==
            KDateTime(QDate(2007, 7, 23), QTime(8, 0, 0), KDateTime::UTC));
}

void FreeBusyTest::testAddSort()
{
    Period::List periods;

    const KDateTime firstq1DateTime(QDate(2007, 7, 23), QTime(7, 0, 0), KDateTime::UTC);
    Period q1(
        firstq1DateTime,
        KDateTime(QDate(2007, 7, 23), QTime(8, 0, 0), KDateTime::UTC));
    periods.append(q1);

    const KDateTime firstq2DateTime(QDate(2007, 8, 23), QTime(7, 0, 0), KDateTime::UTC);
    Period q2(
        firstq2DateTime,
        KDateTime(QDate(2007, 8, 23), QTime(8, 0, 0), KDateTime::UTC));
    periods.append(q2);

    const KDateTime firstq3DateTime(QDate(2007, 9, 23), QTime(7, 0, 0), KDateTime::UTC);
    Period q3(
        firstq3DateTime,
        KDateTime(QDate(2007, 9, 23), QTime(8, 0, 0), KDateTime::UTC));
    periods.append(q3);

    FreeBusy fb1;
    fb1.addPeriods(periods);

    const KDateTime firstfb1DateTime(QDate(2007, 10, 27), QTime(7, 0, 0), KDateTime::UTC);
    fb1.addPeriod(
        firstfb1DateTime,
        KDateTime(QDate(2007, 10, 27), QTime(8, 0, 0), KDateTime::UTC));

    const KDateTime secondfb1DateTime(QDate(2007, 8, 27), QTime(7, 0, 0), KDateTime::UTC);
    fb1.addPeriod(
        secondfb1DateTime,
        KDateTime(QDate(2007, 8, 27), QTime(8, 0, 0), KDateTime::UTC));

    const KDateTime thirdfb1DateTime(QDate(2007, 6, 27), QTime(7, 0, 0), KDateTime::UTC);
    fb1.addPeriod(
        thirdfb1DateTime,
        KDateTime(QDate(2007, 6, 27), QTime(8, 0, 0), KDateTime::UTC));

    QVERIFY(fb1.busyPeriods().last().end() ==
            KDateTime(QDate(2007, 10, 27), QTime(8, 0, 0), KDateTime::UTC));

}

void FreeBusyTest::testAssign()
{
    const KDateTime firstDateTime(QDate(2007, 7, 23), QTime(7, 0, 0), KDateTime::UTC);

    FreeBusy fb1(
        firstDateTime,
        KDateTime(QDate(2007, 7, 23), QTime(8, 0, 0), KDateTime::UTC));

    FreeBusy fb2 = fb1;
    QVERIFY(fb1 == fb2);

    fb1.setDtStart(firstDateTime.addDays(1));
    fb2.setDtStart(firstDateTime.addDays(2));
    QVERIFY(!(fb1 == fb2));

    FreeBusy fb3 = fb2;
    QVERIFY(fb3 == fb2);

    KDateTime dt = fb3.dtEnd();
    fb3.setDtEnd(dt.addDays(1));
    fb2.setDtEnd(dt.addDays(1));
    QVERIFY(fb2 == fb3);
}

void FreeBusyTest::testDataStream()
{
    const KDateTime firstDateTime(QDate(2007, 7, 23), QTime(7, 0, 0), KDateTime::UTC);
    FreeBusy::Ptr fb1(new FreeBusy(firstDateTime,
                                   KDateTime(QDate(2007, 7, 23), QTime(8, 0, 0), KDateTime::UTC)));

    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << fb1;

    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);

    FreeBusy::Ptr fb2;

    in_stream >> fb2;

    QVERIFY(fb1->dtEnd() == fb2->dtEnd());
    QVERIFY(fb1->busyPeriods() == fb2->busyPeriods());
//   QVERIFY( *fb1 == *fb2 );
}
