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
#include <QTest>
#include <QTimeZone>

QTEST_MAIN(ICalFormatTest)

using namespace KCalendarCore;

void ICalFormatTest::testCharsets()
{
    ICalFormat format;
    const QDate currentDate = QDate::currentDate();
    Event::Ptr event = Event::Ptr(new Event());
    event->setUid(QStringLiteral("12345"));
    event->setDtStart(QDateTime(currentDate, {}));
    event->setDtEnd(QDateTime(currentDate.addDays(1), {}));
    event->setAllDay(true);

    // ü
    const QChar latin1_umlaut[] = { 0xFC, QLatin1Char('\0') };
    event->setSummary(QString(latin1_umlaut));

    // Test if toString( Incidence ) didn't mess charsets
    const QString serialized = format.toString(event.staticCast<Incidence>());
    const QChar utf_umlaut[] = { 0xC3, 0XBC, QLatin1Char('\0') };
    QVERIFY(serialized.toUtf8().contains(QString(utf_umlaut).toLatin1().constData()));
    QVERIFY(!serialized.toUtf8().contains(QString(latin1_umlaut).toLatin1().constData()));
    QVERIFY(serialized.toLatin1().contains(QString(latin1_umlaut).toLatin1().constData()));
    QVERIFY(!serialized.toLatin1().contains(QString(utf_umlaut).toLatin1().constData()));

    // test fromString( QString )
    const QString serializedCalendar
        = QLatin1String("BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\n")
          +serialized
          +QLatin1String("\nEND:VCALENDAR");

    Incidence::Ptr event2 = format.fromString(serializedCalendar);
    QVERIFY(event->summary() == event2->summary());
    QVERIFY(event2->summary().toUtf8()
            == QByteArray(QString(utf_umlaut).toLatin1().constData()));

    // test save()
    MemoryCalendar::Ptr calendar(new MemoryCalendar(QTimeZone::utc()));
    calendar->addIncidence(event);
    QVERIFY(format.save(calendar, QLatin1String("hommer.ics")));

    // Make sure hommer.ics is in UTF-8
    QFile file(QStringLiteral("hommer.ics"));
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    const QByteArray bytesFromFile = file.readAll();
    QVERIFY(bytesFromFile.contains(QString(utf_umlaut).toLatin1().constData()));
    QVERIFY(!bytesFromFile.contains(QString(latin1_umlaut).toLatin1().constData()));
    file.close();

    // Test load:
    MemoryCalendar::Ptr calendar2(new MemoryCalendar(QTimeZone::utc()));
    QVERIFY(format.load(calendar2, QLatin1String("hommer.ics")));
    QVERIFY(calendar2->incidences().count() == 1);

    // qDebug() << format.toString( event.staticCast<Incidence>() );
    // qDebug() << format.toString( calendar2->incidences().at(0) );

    Event::Ptr loadedEvent = calendar2->incidences().at(0).staticCast<Event>();
    QVERIFY(loadedEvent->summary().toUtf8()
            == QByteArray(QString(utf_umlaut).toLatin1().constData()));
    QVERIFY(*loadedEvent == *event);

    // Test fromRawString()
    MemoryCalendar::Ptr calendar3(new MemoryCalendar(QTimeZone::utc()));
    QVERIFY(format.fromRawString(calendar3, bytesFromFile));
    QVERIFY(calendar3->incidences().count() == 1);
    QVERIFY(*calendar3->incidences().at(0) == *event);

    QFile::remove(QStringLiteral("hommer.ics"));
}

void ICalFormatTest::testVolatileProperties()
{
    // Volatile properties are not written to the serialized data
    ICalFormat format;
    const QDate currentDate = QDate::currentDate();
    Event::Ptr event = Event::Ptr(new Event());
    event->setUid(QStringLiteral("12345"));
    event->setDtStart(QDateTime(currentDate, {}));
    event->setDtEnd(QDateTime(currentDate.addDays(1), {}));
    event->setAllDay(true);
    event->setCustomProperty("VOLATILE", "FOO", QStringLiteral("BAR"));
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
    event->setUid(QStringLiteral("12345"));
    event->setDtStart(QDateTime(currentDate, {}));
    event->setDtEnd(QDateTime(currentDate.addDays(1), {}));
    event->setAllDay(true);

    Attendee attendee(QStringLiteral("fred"), QStringLiteral("fred@flintstone.com"));
    attendee.setCuType(Attendee::Resource);

    event->addAttendee(attendee);

    const QString serialized = format.toString(event.staticCast<Incidence>());

    // test fromString(QString)
    const QString serializedCalendar
        = QLatin1String("BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\n")
          +serialized
          +QLatin1String("\nEND:VCALENDAR");

    Incidence::Ptr event2 = format.fromString(serializedCalendar);
    QVERIFY(event2->attendeeCount() == 1);
    Attendee attendee2 = event2->attendees()[0];
    QVERIFY(attendee2.cuType() == attendee.cuType());
    QVERIFY(attendee2.name() == attendee.name());
    QVERIFY(attendee2.email() == attendee.email());
}

void ICalFormatTest::testAlarm()
{
    ICalFormat format;

    Event::Ptr event(new Event);
    event->setDtStart(QDateTime(QDate(2017, 03, 24)));
    Alarm::Ptr alarm = event->newAlarm();
    alarm->setType(Alarm::Display);
    alarm->setStartOffset(Duration(0));

    const QString serialized
        = QLatin1String("BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\n")
        + format.toString(event.staticCast<Incidence>())
        + QLatin1String("\nEND:VCALENDAR");

    Incidence::Ptr event2 = format.fromString(serialized);
    Alarm::Ptr alarm2 = event2->alarms()[0];
    QCOMPARE(*alarm, *alarm2);
}
