/*
  This file is part of the kcalendarcore library.

  SPDX-FileCopyrightText: 2006, 2008 Allen Winter <winter@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "testavailability.h"
#include "availability.h"
#include "icalformat.h"

#include <QTest>
QTEST_MAIN(AvailabilityTest)

using namespace KCalendarCore;

const auto TEST_TZ = "UTC";

void AvailabilityTest::initTestCase()
{
    qputenv("TZ", TEST_TZ);
}

void AvailabilityTest::parseAvailable()
{
    const QString avaiString = QStringLiteral(
        "BEGIN:VCALENDAR\n"
        "BEGIN:AVAILABLE\n"
        "UID:34EDA59B-6BB1-4E94-A66C-64999089C0AF\n"
        "SUMMARY:Monday to Friday from 9:00 to 17:00\n"
        "DTSTART;TZID=America/Montreal:20111002T090000\n"
        "DTEND;TZID=America/Montreal:20111002T170000\n"
        "RRULE:FREQ=WEEKLY;BYDAY=MO,TU,WE,TH,FR\n"
        "END:AVAILABLE\n"
        "END:VCALENDAR\n");

    ICalFormat format;
    Available::Ptr avai = format.parseAvailable(avaiString);
    if (avai) {
        qDebug() << __FUNCTION__ << "Not null";
    } else {
        qDebug() << __FUNCTION__ << "Null";
    }
    //    qDebug() << avai->getAvailables().count() << " " << avai->dtStart().toString();
}

void AvailabilityTest::parseAvailability()
{
    const QString avaiString = QStringLiteral(
        "BEGIN:VCALENDAR\n"
        "BEGIN:VAVAILABILITY\n"
        "ORGANIZER:mailto:bernard@example.com\n"
        "UID:0428C7D2-688E-4D2E-AC52-CD112E2469DF\n"
        "DTSTAMP:20111005T133225Z\n"
        "BEGIN:AVAILABLE\n"
        "UID:34EDA59B-6BB1-4E94-A66C-64999089C0AF\n"
        "SUMMARY:Monday to Friday from 9:00 to 17:00\n"
        "DTSTART;TZID=America/Montreal:20111002T090000\n"
        "DTEND;TZID=America/Montreal:20111002T170000\n"
        "RRULE:FREQ=WEEKLY;BYDAY=MO,TU,WE,TH,FR\n"
        "END:AVAILABLE\n"
        "END:VAVAILABILITY\n"
        "END:VCALENDAR\n");

    ICalFormat format;
    Availability::Ptr avai = format.parseAvailability(avaiString);
    if (avai) {
        qDebug() << __FUNCTION__ << "Not null";
    } else {
        qDebug() << __FUNCTION__ << "Null";
    }
    //    qDebug() << avai->getAvailables().count() << " " << avai->dtStart().toString();
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
