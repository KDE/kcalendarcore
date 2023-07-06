/*
  This file is part of the kcalendarcore library.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "testavailable.h"
#include "available.h"

#include <QTest>
QTEST_MAIN(AvailableTest)

using namespace KCalendarCore;

const auto TEST_TZ = "UTC";

void AvailableTest::initTestCase()
{
    qputenv("TZ", TEST_TZ);
}

void AvailableTest::testAvailable()
{
    QDate dt = QDate::currentDate();
    Available available;
    available.setDtStart(QDateTime(dt, {}));
    available.setSummary(QStringLiteral("To-do1 Summary"));
    available.setDescription(QStringLiteral("This is a description of the first to-do"));
    available.setLocation(QStringLiteral("the place"), false);

    QCOMPARE(available.summary(), QStringLiteral("To-do1 Summary"));
    QCOMPARE(available.location(), QStringLiteral("the place"));
}

void AvailableTest::testCompare()
{
    QDate dt = QDate::currentDate();
    Available available1;
    available1.setDtStart(QDateTime(dt, {}));
    available1.setSummary(QStringLiteral("To-do1 Summary"));
    available1.setDescription(QStringLiteral("This is a description of the first to-do"));
    available1.setLocation(QStringLiteral("the place"), false);

    Available available2;
    available2.setDtStart(QDateTime(dt, {}).addDays(1));
    available2.setSummary(QStringLiteral("To-do2 Summary"));
    available2.setDescription(QStringLiteral("This is a description of the second to-do"));
    available2.setLocation(QStringLiteral("the other place"), false);

    QVERIFY(!(available1 == available2));
    QCOMPARE(available2.summary(), QStringLiteral("To-do2 Summary"));
}
