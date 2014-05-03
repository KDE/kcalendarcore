/*
  This file is part of the kcalcore library.

  Copyright (C) 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
  Author: Sergio Martins <sergio.martins@kdab.com>

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

#include "testicalformat.h"
#include "event.h"
#include "icalformat.h"
#include "memorycalendar.h"

#include <QDebug>
#include <kdatetime.h>

#include <qtest_kde.h>

#include <unistd.h>

QTEST_KDEMAIN(ICalFormatTest, NoGUI)

using namespace KCalCore;

void ICalFormatTest::testCharsets()
{
    ICalFormat format;
    const QDate currentDate = QDate::currentDate();
    Event::Ptr event = Event::Ptr(new Event());
    event->setUid("12345");
    event->setDtStart(KDateTime(currentDate));
    event->setDtEnd(KDateTime(currentDate.addDays(1)));

    // ü
    const QChar latin1_umlaut[] = { 0xFC, '\0' };
    event->setSummary(QString(latin1_umlaut));

    // Test if toString( Incidence ) didn't mess charsets
    const QString serialized = format.toString(event.staticCast<Incidence>());
    const QChar utf_umlaut[] = { 0xC3, 0XBC, '\0' };
    QVERIFY(serialized.toUtf8().contains(QString(utf_umlaut).toLatin1().constData()));
    QVERIFY(!serialized.toUtf8().contains(QString(latin1_umlaut).toLatin1().constData()));
    QVERIFY(serialized.toLatin1().contains(QString(latin1_umlaut).toLatin1().constData()));
    QVERIFY(!serialized.toLatin1().contains(QString(utf_umlaut).toLatin1().constData()));

    // test fromString( QString )
    const QString serializedCalendar =
        "BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\n" +
        serialized +
        "\nEND:VCALENDAR";

    Incidence::Ptr event2 = format.fromString(serializedCalendar);
    QVERIFY(event->summary() == event2->summary());
    QVERIFY(event2->summary().toUtf8() ==
            QByteArray(QString(utf_umlaut).toLatin1().constData()));

    // test save()
    MemoryCalendar::Ptr calendar(new MemoryCalendar("UTC"));
    calendar->addIncidence(event);
    QVERIFY(format.save(calendar, "hommer.ics"));

    // Make sure hommer.ics is in UTF-8
    QFile file("hommer.ics");
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    const QByteArray bytesFromFile = file.readAll();
    QVERIFY(bytesFromFile.contains(QString(utf_umlaut).toLatin1().constData()));
    QVERIFY(!bytesFromFile.contains(QString(latin1_umlaut).toLatin1().constData()));
    file.close();

    // Test load:
    MemoryCalendar::Ptr calendar2(new MemoryCalendar("UTC"));
    QVERIFY(format.load(calendar2, "hommer.ics"));
    QVERIFY(calendar2->incidences().count() == 1);

    // qDebug() << format.toString( event.staticCast<Incidence>() );
    // qDebug() << format.toString( calendar2->incidences().first() );

    Event::Ptr loadedEvent = calendar2->incidences().first().staticCast<Event>();
    QVERIFY(loadedEvent->summary().toUtf8() ==
            QByteArray(QString(utf_umlaut).toLatin1().constData()));
    QVERIFY(*loadedEvent == *event);

    // Test fromRawString()
    MemoryCalendar::Ptr calendar3(new MemoryCalendar("UTC"));
    format.fromRawString(calendar3, bytesFromFile);
    QVERIFY(calendar3->incidences().count() == 1);
    QVERIFY(*calendar3->incidences().first() == *event);

    unlink("hommer.ics");
}

void ICalFormatTest::testVolatileProperties()
{
    // Volatile properties are not written to the serialized data
    ICalFormat format;
    const QDate currentDate = QDate::currentDate();
    Event::Ptr event = Event::Ptr(new Event());
    event->setUid("12345");
    event->setDtStart(KDateTime(currentDate));
    event->setDtEnd(KDateTime(currentDate.addDays(1)));
    event->setCustomProperty("VOLATILE", "FOO", "BAR");
    QString string = format.toICalString(event);
    Incidence::Ptr incidence = format.fromString(string);

    QCOMPARE(incidence->uid(), QStringLiteral("12345"));
    QVERIFY(incidence->customProperties().isEmpty());
}

void ICalFormatTest::testCuType()
{
    ICalFormat format;
    const QDate currentDate = QDate::currentDate();
    Event::Ptr event(new Event());
    event->setUid("12345");
    event->setDtStart(KDateTime(currentDate));
    event->setDtEnd(KDateTime(currentDate.addDays(1)));

    Attendee::Ptr attendee(new Attendee("fred", "fred@flintstone.com"));
    attendee->setCuType(Attendee::Resource);

    event->addAttendee(attendee);

    const QString serialized = format.toString(event.staticCast<Incidence>());

    // test fromString(QString)
    const QString serializedCalendar =
        "BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\n" +
        serialized +
        "\nEND:VCALENDAR";

    Incidence::Ptr event2 = format.fromString(serializedCalendar);
    QVERIFY(event2->attendeeCount() == 1);
    Attendee::Ptr attendee2 = event2->attendees()[0];
    QVERIFY(attendee2->cuType() == attendee->cuType());
    QVERIFY(attendee2->name() == attendee->name());
    QVERIFY(attendee2->email() == attendee->email());
}
