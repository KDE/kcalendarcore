/*
 *  SPDX-FileCopyrightText: 2022 Glen Ditchfield <GJDitchfield@acm.org>
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "incidencebase.h"

#include <QTest>
#include <QTimeZone>

using namespace KCalendarCore;

class TestIdentical: public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void testTrue()
    {
        QVERIFY(
            identical(QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), Qt::LocalTime),
                      QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), Qt::LocalTime)));

        QVERIFY(
            identical(QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), QTimeZone("Atlantic/Azores")),
                      QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), QTimeZone("Atlantic/Azores"))));

        QVERIFY(identical(QDateTime(), QDateTime()));
    }

    void testFalse_data()
    {
        QTest::addColumn<QDateTime>("dt1");
        QTest::addColumn<QDateTime>("dt2");

        QTest::newRow("date")
            << QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), Qt::LocalTime)
            << QDateTime(QDate(2022, 02, 23), QTime(22, 22, 22), Qt::LocalTime);
        QTest::newRow("time")
            << QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), Qt::LocalTime)
            << QDateTime(QDate(2022, 02, 22), QTime(22, 22, 23), Qt::LocalTime);

        // Note:  Qt::LocalTime is used for "floating" date-times; "this time
        // in the current system time zone".
        QTest::newRow("timeSpec")
            << QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), Qt::LocalTime)
            << QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), QTimeZone::systemTimeZone());

        // Both zones are UTC+0.
        QTest::newRow("timeZone")
            << QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), QTimeZone("Africa/Abidjan"))
            << QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), QTimeZone("Africa/Accra"));

        QTest::newRow("invalid timeSpec")
            << QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), QDateTime().timeSpec())
            << QDateTime();
        QTest::newRow("invalid timeZone")
            << QDateTime(QDate(2022, 02, 22), QTime(22, 22, 22), QDateTime().timeZone())
            << QDateTime();
    }

    void testFalse()
    {
        QFETCH(QDateTime, dt1);
        QFETCH(QDateTime, dt2);
        QVERIFY(!identical(dt1, dt2));
    }
};

QTEST_MAIN(TestIdentical)
#include "testidentical.moc"
