/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2006, 2008 Allen Winter <winter@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "testavailability.h"
#include "availability.h"

#include <QTest>
QTEST_MAIN(AvailabilityTest)

using namespace KCalendarCore;

const auto TEST_TZ = "UTC";

void AvailabilityTest::initTestCase()
{
    qputenv("TZ", TEST_TZ);
}

void AvailabilityTest::testValidity()
{
    // QDate dt = QDate::currentDate();
    // Availability availability;
    // availability.setDtStart(QDateTime(dt, {}));
    ////availability.setDtDue(QDateTime(dt, {}).addDays(1));
    // availability.setSummary(QStringLiteral("To-do1 Summary"));
    // availability.setDescription(QStringLiteral("This is a description of the first to-do"));
    // availability.setLocation(QStringLiteral("the place"));

    // QCOMPARE(availability.summary(), QStringLiteral("To-do1 Summary"));
    // QCOMPARE(availability.location(), QStringLiteral("the place"));
}
