/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2007-2008 Allen Winter <winter@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "testfreebusy.h"
#include "freebusy.h"

#include <QTest>
QTEST_MAIN(FreeBusyTest)

using namespace KCalendarCore;

void FreeBusyTest::testValidity()
{
    const QDateTime firstDateTime(QDate(2007, 7, 23), QTime(7, 0, 0), Qt::UTC);

    FreeBusy fb1(firstDateTime, QDateTime(QDate(2007, 7, 23), QTime(8, 0, 0), Qt::UTC));

    QCOMPARE(fb1.dtEnd(), QDateTime(QDate(2007, 7, 23), QTime(8, 0, 0), Qt::UTC));
}

void FreeBusyTest::testAddSort()
{
    Period::List periods;

    const QDateTime firstq1DateTime(QDate(2007, 7, 23), QTime(7, 0, 0), Qt::UTC);
    Period q1(firstq1DateTime, QDateTime(QDate(2007, 7, 23), QTime(8, 0, 0), Qt::UTC));
    periods.append(q1);

    const QDateTime firstq2DateTime(QDate(2007, 8, 23), QTime(7, 0, 0), Qt::UTC);
    Period q2(firstq2DateTime, QDateTime(QDate(2007, 8, 23), QTime(8, 0, 0), Qt::UTC));
    periods.append(q2);

    const QDateTime firstq3DateTime(QDate(2007, 9, 23), QTime(7, 0, 0), Qt::UTC);
    Period q3(firstq3DateTime, QDateTime(QDate(2007, 9, 23), QTime(8, 0, 0), Qt::UTC));
    periods.append(q3);

    FreeBusy fb1;
    fb1.addPeriods(periods);

    const QDateTime firstfb1DateTime(QDate(2007, 10, 27), QTime(7, 0, 0), Qt::UTC);
    fb1.addPeriod(firstfb1DateTime, QDateTime(QDate(2007, 10, 27), QTime(8, 0, 0), Qt::UTC));

    const QDateTime secondfb1DateTime(QDate(2007, 8, 27), QTime(7, 0, 0), Qt::UTC);
    fb1.addPeriod(secondfb1DateTime, QDateTime(QDate(2007, 8, 27), QTime(8, 0, 0), Qt::UTC));

    const QDateTime thirdfb1DateTime(QDate(2007, 6, 27), QTime(7, 0, 0), Qt::UTC);
    fb1.addPeriod(thirdfb1DateTime, QDateTime(QDate(2007, 6, 27), QTime(8, 0, 0), Qt::UTC));

    Period::List busyPeriods = fb1.busyPeriods();
    QVERIFY(!busyPeriods.isEmpty());
    QCOMPARE(busyPeriods.last().end(), QDateTime(QDate(2007, 10, 27), QTime(8, 0, 0), Qt::UTC));
}

void FreeBusyTest::testAssign()
{
    const QDateTime firstDateTime(QDate(2007, 7, 23), QTime(7, 0, 0), Qt::UTC);

    FreeBusy fb1(firstDateTime, QDateTime(QDate(2007, 7, 23), QTime(8, 0, 0), Qt::UTC));

    FreeBusy fb2 = fb1;
    QCOMPARE(fb1, fb2);

    fb1.setDtStart(firstDateTime.addDays(1));
    fb2.setDtStart(firstDateTime.addDays(2));
    QVERIFY(!(fb1 == fb2));

    FreeBusy fb3 = fb2;
    QCOMPARE(fb3, fb2);

    QDateTime dt = fb3.dtEnd();
    fb3.setDtEnd(dt.addDays(1));
    fb2.setDtEnd(dt.addDays(1));
    QCOMPARE(fb2, fb3);
}

void FreeBusyTest::testDataStream()
{
    const QDateTime firstDateTime(QDate(2007, 7, 23), QTime(7, 0, 0), Qt::UTC);
    FreeBusy::Ptr fb1(new FreeBusy(firstDateTime, QDateTime(QDate(2007, 7, 23), QTime(8, 0, 0), Qt::UTC)));

    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << fb1;

    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);

    FreeBusy::Ptr fb2;

    in_stream >> fb2;

    QCOMPARE(fb1->dtEnd(), fb2->dtEnd());
    QCOMPARE(fb1->busyPeriods(), fb2->busyPeriods());
    //   QVERIFY( *fb1 == *fb2 );
}
