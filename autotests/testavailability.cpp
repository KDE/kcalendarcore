/*
  This file is part of the kcalendarcore library.

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
    QDate dt = QDate::currentDate();
    Availability availability;
    availability.setOrganizer(QStringLiteral("mailto:bernard@example.com"));

    // available item
    Available available;
    available.setUid(QStringLiteral("34EDA59B-6BB1-4E94-A66C-64999089C0AF"));
    available.setSummary(QStringLiteral("Monday to Friday from 9:00 to 17:00"));

    availability.addNewAvailable(available);

    QCOMPARE(availability.organizer(), Person(QStringLiteral(""), QStringLiteral("bernard@example.com")));
    QCOMPARE(available.summary(), QStringLiteral("Monday to Friday from 9:00 to 17:00"));
}
