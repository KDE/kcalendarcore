/*
 * Copyright (C) 2012  Christian Mollekopf <mollekopf@kolabsys.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "testcreateddatecompat.h"
#include "icalformat.h"
#include "memorycalendar.h"

#include <iostream>

#include <QTest>
#include <QTimeZone>

//"X-KDE-ICAL-IMPLEMENTATION-VERSION:1.0\n"

const char *const icalFile32
    = "BEGIN:VCALENDAR\n"
      "PRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\n"
      "VERSION:2.0\n"
      "BEGIN:VEVENT\n"
      "DTSTAMP:20031213T204753Z\n"
      "ORGANIZER:MAILTO:nobody@nowhere\n"
      "CREATED:20031213T204152Z\n"
      "UID:uid\n"
      "SEQUENCE:0\n"
      "LAST-MODIFIED:20031213T204152Z\n"
      "SUMMARY:Holladiho\n"
      "DTSTART:20031213T071500Z\n"
      "END:VEVENT\n"
      "END:VCALENDAR\n";

const char *const icalFile33
    = "BEGIN:VCALENDAR\n"
      "PRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\n"
      "VERSION:2.0\n"
      "X-KDE-ICAL-IMPLEMENTATION-VERSION:1.0\n"
      "BEGIN:VEVENT\n"
      "DTSTAMP:20031213T204753Z\n"
      "ORGANIZER:MAILTO:nobody@nowhere\n"
      "CREATED:20031213T204152Z\n"
      "UID:uid\n"
      "SEQUENCE:0\n"
      "LAST-MODIFIED:20031213T204152Z\n"
      "SUMMARY:Holladiho\n"
      "DTSTART:20031213T071500Z\n"
      "END:VEVENT\n"
      "END:VCALENDAR\n";

void CreatedDateCompatTest::testCompat32()
{
    KCalendarCore::MemoryCalendar::Ptr cal(new KCalendarCore::MemoryCalendar(QTimeZone::utc()));
    KCalendarCore::ICalFormat format;
    QVERIFY(format.fromRawString(cal, QByteArray(icalFile32)));
    KCalendarCore::Event::Ptr event = cal->event(QStringLiteral("uid"));
    QVERIFY(event);
    QCOMPARE(event->created(),
             QDateTime(QDate(2003, 12, 13), QTime(20, 47, 53), Qt::UTC));
}

void CreatedDateCompatTest::testCompat33()
{
    KCalendarCore::MemoryCalendar::Ptr cal(new KCalendarCore::MemoryCalendar(QTimeZone::utc()));
    KCalendarCore::ICalFormat format;
    QVERIFY(format.fromRawString(cal, QByteArray(icalFile33)));
    KCalendarCore::Event::Ptr event = cal->event(QStringLiteral("uid"));
    QVERIFY(event);
    QCOMPARE(event->created(),
             QDateTime(QDate(2003, 12, 13), QTime(20, 41, 52), Qt::UTC));
    QVERIFY(!event->customProperties().contains("X-KDE-ICAL-IMPLEMENTATION-VERSION"));
}

QTEST_MAIN(CreatedDateCompatTest)
