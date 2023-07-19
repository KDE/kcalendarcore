/*
  This file is part of the kcalendarcore library.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "testavailability.h"
#include "icalformat.h"

#include <QTest>
QTEST_MAIN(AvailabilityTest)

using namespace KCalendarCore;

const auto TEST_TZ = "UTC";

void AvailabilityTest::prettyPrint(QVector<Availability::Ptr> availabilities)
{
    for (auto availability : availabilities) {
        QVector<QSharedPointer<Available>> availables = availability->getAvailables();
        qDebug() << "Available Count: " << availables.count();
        for (auto available : availables) {
            qDebug() << "uid: " << available->uid();
            qDebug() << "summary: " << available->summary();
            qDebug() << "dtStart: " << available->dtStart();
            qDebug() << "dtEnd: " << available->dtEnd();
            qDebug() << "dtStamp: " << available->dtStamp();
            qDebug() << "mDuration: " << available->duration();
            qDebug() << "location: " << available->location();
            qDebug() << "mCategories: " << available->categories();
            qDebug() << "mComments: " << available->comments();
            qDebug() << "contacts: " << available->contacts();
        }
    }
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
    QVector<Availability::Ptr> avai = format.parseAvailability(avaiString);
    if (avai.count() > 0) {
        prettyPrint(avai);
    } else {
        qDebug() << __FUNCTION__ << "Null";
    }
}

void AvailabilityTest::parseAvailability2()
{
    const QString avaiString = QStringLiteral(
        "BEGIN:VCALENDAR\n"
        "BEGIN:VAVAILABILITY\n"
        "ORGANIZER:mailto:bernard@example.com\n"
        "UID:84D0F948-7FC6-4C1D-BBF3-BA9827B424B5\n"
        "DTSTAMP:20111005T133225Z\n"
        "DTSTART;TZID=America/Montreal:20111002T000000\n"
        "DTEND;TZID=America/Montreal:20111202T000000\n"
        "BEGIN:AVAILABLE\n"
        "UID:7B33093A-7F98-4EED-B381-A5652530F04D\n"
        "SUMMARY:Monday to Thursday from 9:00 to 17:00\n"
        "DTSTART;TZID=America/Montreal:20111002T090000\n"
        "DTEND;TZID=America/Montreal:20111002T170000\n"
        "RRULE:FREQ=WEEKLY;BYDAY=MO,TU,WE,TH\n"
        "LOCATION:Main Office\n"
        "END:AVAILABLE\n"
        "BEGIN:AVAILABLE\n"
        "UID:DF39DC9E-D8C3-492F-9101-0434E8FC1896\n"
        "SUMMARY:Friday from 9:00 to 12:00\n"
        "DTSTART;TZID=America/Montreal:20111006T090000\n"
        "DTEND;TZID=America/Montreal:20111006T120000\n"
        "RRULE:FREQ=WEEKLY\n"
        "LOCATION:Branch Office\n"
        "END:AVAILABLE\n"
        "END:VAVAILABILITY\n"
        "END:VCALENDAR\n");

    ICalFormat format;
    QVector<Availability::Ptr> avai = format.parseAvailability(avaiString);
    if (avai.count() > 0) {
        prettyPrint(avai);
    } else {
        qDebug() << __FUNCTION__ << "Null";
    }
}

void AvailabilityTest::parseAvailability3()
{
    const QString avaiString = QStringLiteral(
        "BEGIN:VCALENDAR\n"
        "BEGIN:VAVAILABILITY\n"
        "ORGANIZER:mailto:bernard@example.com\n"
        "UID:BE082249-7BDD-4FE0-BDBA-DE6598C32FC9\n"
        "DTSTAMP:20111005T133225Z\n"
        "DTSTART;TZID=America/Montreal:20111002T000000\n"
        "DTEND;TZID=America/Montreal:20111023T030000\n"
        "BEGIN:AVAILABLE\n"
        "UID:54602321-CEDB-4620-9099-757583263981\n"
        "SUMMARY:Monday to Friday from 9:00 to 17:00\n"
        "DTSTART;TZID=America/Montreal:20111002T090000\n"
        "DTEND;TZID=America/Montreal:20111002T170000\n"
        "RRULE:FREQ=WEEKLY;BYDAY=MO,TU,WE,TH,FR\n"
        "LOCATION:Montreal\n"
        "END:AVAILABLE\n"
        "END:VAVAILABILITY\n"
        "BEGIN:VAVAILABILITY\n"
        "ORGANIZER:mailto:bernard@example.com\n"
        "UID:A1FF55E3-555C-433A-8548-BF4864B5621E\n"
        "DTSTAMP:20111005T133225Z\n"
        "DTSTART;TZID=America/Denver:20111023T000000\n"
        "DTEND;TZID=America/Denver:20111030T000000\n"
        "BEGIN:AVAILABLE\n"
        "UID:57DD4AAF-3835-46B5-8A39-B3B253157F01\n"
        "SUMMARY:Monday to Friday from 9:00 to 17:00\n"
        "DTSTART;TZID=America/Denver:20111023T090000\n"
        "DTEND;TZID=America/Denver:20111023T170000\n"
        "RRULE:FREQ=WEEKLY;BYDAY=MO,TU,WE,TH,FR\n"
        "LOCATION:Denver\n"
        "END:AVAILABLE\n"
        "END:VAVAILABILITY\n"
        "BEGIN:VAVAILABILITY\n"
        "ORGANIZER:mailto:bernard@example.com\n"
        "UID:1852F9E1-E0AA-4572-B4C4-ED1680A4DA40\n"
        "DTSTAMP:20111005T133225Z\n"
        "DTSTART;TZID=America/Montreal:20111030T030000\n"
        "BEGIN:AVAILABLE\n"
        "UID:D27C421F-16C2-4ECB-8352-C45CA352C72A\n"
        "SUMMARY:Monday to Friday from 9:00 to 17:00\n"
        "DTSTART;TZID=America/Montreal:20111030T090000\n"
        "DTEND;TZID=America/Montreal:20111030T170000\n"
        "RRULE:FREQ=WEEKLY;BYDAY=MO,TU,WE,TH,FR\n"
        "LOCATION:Montreal\n"
        "END:AVAILABLE\n"
        "END:VAVAILABILITY\n"
        "END:VCALENDAR\n");

    ICalFormat format;
    QVector<Availability::Ptr> avai = format.parseAvailability(avaiString);
    if (avai.count() > 0) {
        prettyPrint(avai);
    } else {
        qDebug() << __FUNCTION__ << "Null";
    }
}

void AvailabilityTest::testValidity()
{
    QDate dt = QDate::currentDate();
    Availability availability;
    availability.setOrganizer(QStringLiteral("mailto:bernard@example.com"));

    // Available::Ptr available;
    // available->setUid(QStringLiteral("34EDA59B-6BB1-4E94-A66C-64999089C0AF"));
    // available->setSummary(QStringLiteral("Monday to Friday from 9:00 to 17:00"));

    // availability.addNewAvailable(available);

    // QCOMPARE(availability.organizer(), Person(QStringLiteral(""), QStringLiteral("bernard@example.com")));
}
