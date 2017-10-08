/*
  This file is part of the kcalcore library.

  Copyright (c) 2005-2007 David Jarvie <software@astrojar.org.uk>

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

#include "testicaltimezones.h"
#include "icaltimezones_p.h"

#include <QDateTime>

#include <QTest>

QTEST_MAIN(ICalTimeZonesTest)

extern "C" {
#include <libical/ical.h>
}
using namespace KCalCore;

static icalcomponent *loadCALENDAR(const char *vcal);

// First daylight savings time has an end date, takes a break for a year,
// and is then replaced by another
static const char *VTZ_Western =
    "BEGIN:VTIMEZONE\r\n"
    "TZID:Test-Dummy-Western\r\n"
    "LAST-MODIFIED:19870101T000000Z\r\n"
    "TZURL:http://tz.reference.net/dummies/western\r\n"
    "LOCATION:Zedland/Tryburgh\r\n"
    "X-LIC-LOCATION:Wyland/Tryburgh\r\n"
    "BEGIN:STANDARD\r\n"
    "DTSTART:19671029T020000\r\n"
    "RRULE:FREQ=YEARLY;BYDAY=-1SU;BYMONTH=10\r\n"
    "TZOFFSETFROM:-0400\r\n"
    "TZOFFSETTO:-0500\r\n"
    "TZNAME:WST\r\n"
    "END:STANDARD\r\n"
    "BEGIN:DAYLIGHT\r\n"
    "DTSTART:19870405T020000\r\n"
    "RRULE:FREQ=YEARLY;UNTIL=19970406T070000Z;BYDAY=1SU;BYMONTH=4\r\n"
    "TZOFFSETFROM:-0500\r\n"
    "TZOFFSETTO:-0400\r\n"
    "TZNAME:WDT1\r\n"
    "END:DAYLIGHT\r\n"
    "BEGIN:DAYLIGHT\r\n"
    "DTSTART:19990425T020000\r\n"
    "RDATE;VALUE=DATE-TIME:20000430T020000\r\n"
    "TZOFFSETFROM:-0500\r\n"
    "TZOFFSETTO:-0400\r\n"
    "TZNAME:WDT2\r\n"
    "END:DAYLIGHT\r\n"
    "END:VTIMEZONE\r\n";

// Standard time only
static const char *VTZ_other =
    "BEGIN:VTIMEZONE\r\n"
    "TZID:Test-Dummy-Other\r\n"
    "TZURL:http://tz.reference.net/dummies/other\r\n"
    "X-LIC-LOCATION:Wyland/Tryburgh\r\n"
    "BEGIN:STANDARD\r\n"
    "DTSTART:19500101T000000\r\n"
    "RDATE;VALUE=DATE-TIME:19500101T000000\r\n"
    "TZOFFSETFROM:+0000\r\n"
    "TZOFFSETTO:+0300\r\n"
    "TZNAME:OST\r\n"
    "END:STANDARD\r\n"
    "END:VTIMEZONE\r\n";

static const char *VTZ_other_DST =
    "BEGIN:VTIMEZONE\r\n"
    "TZID:Test-Dummy-Other-DST\r\n"
    "BEGIN:STANDARD\r\n"
    "DTSTART:19500101T000000\r\n"
    "RRULE:FREQ=YEARLY;BYDAY=1SU;BYMONTH=11\r\n"
    "TZOFFSETFROM:+0000\r\n"
    "TZOFFSETTO:+0300\r\n"
    "TZNAME:OST\r\n"
    "END:STANDARD\r\n"
    "BEGIN:DAYLIGHT\r\n"
    "DTSTART:19500501T000000\r\n"
    "RRULE:FREQ=YEARLY;BYDAY=3SU;BYMONTH=5\r\n"
    "TZOFFSETFROM:+0200\r\n"
    "TZOFFSETTO:+0500\r\n"
    "TZNAME:DST\r\n"
    "END:DAYLIGHT\r\n"
    "END:VTIMEZONE\r\n";

static const char *VTZ_Prague =
    "BEGIN:VTIMEZONE\r\n"
    "TZID:Europe/Prague\r\n"
    "BEGIN:STANDARD\r\n"
    "TZNAME:CET\r\n"
    "TZOFFSETFROM:+0000\r\n"
    "TZOFFSETTO:+0100\r\n"
    "DTSTART:19781231T230000\r\n"
    "RDATE;VALUE=DATE-TIME:19781231T230000\r\n"
    "END:STANDARD\r\n"
    "BEGIN:DAYLIGHT\r\n"
    "TZNAME:CEST\r\n"
    "TZOFFSETFROM:+0100\r\n"
    "TZOFFSETTO:+0200\r\n"
    "DTSTART:19810329T020000\r\n"
    "RRULE:FREQ=YEARLY;BYDAY=-1SU;BYMONTH=3\r\n"
    "END:DAYLIGHT\r\n"
    "BEGIN:DAYLIGHT\r\n"
    "TZNAME:CEST\r\n"
    "TZOFFSETFROM:+0100\r\n"
    "TZOFFSETTO:+0200\r\n"
    "DTSTART:19790401T020000\r\n"
    "RDATE;VALUE=DATE-TIME:19790401T020000\r\n"
    "RDATE;VALUE=DATE-TIME:19800406T020000\r\n"
    "END:DAYLIGHT\r\n"
    "BEGIN:STANDARD\r\n"
    "TZNAME:CET\r\n"
    "TZOFFSETFROM:+0200\r\n"
    "TZOFFSETTO:+0100\r\n"
    "DTSTART:19971026T030000\r\n"
    "RRULE:FREQ=YEARLY;BYDAY=-1SU;BYMONTH=10\r\n"
    "END:STANDARD\r\n"
    "BEGIN:STANDARD\r\n"
    "TZNAME:CET\r\n"
    "TZOFFSETFROM:+0200\r\n"
    "TZOFFSETTO:+0100\r\n"
    "DTSTART:19790930T030000\r\n"
    "RRULE:FREQ=YEARLY;UNTIL=19961027T030000;COUNT=17;BYDAY=-1SU;BYMONTH=9\r\n"
    "RDATE;VALUE=DATE-TIME:19950924T030000\r\n"
    "END:STANDARD\r\n"
    "END:VTIMEZONE\r\n";

// CALENDAR component header and footer
static const char *calendarHeader =
    "BEGIN:VCALENDAR\r\n"
    "PRODID:-//Libkcal//NONSGML ICalTimeZonesTest//EN\r\n"
    "VERSION:2.0\r\n";
static const char *calendarFooter =
    "END:CALENDAR\r\n";

///////////////////////////
// ICalTimeZoneSource tests
///////////////////////////

void ICalTimeZonesTest::initTestCase()
{
    qputenv("TZ", "Europe/Zurich");
}

void ICalTimeZonesTest::parse_data()
{
    QTest::addColumn<QByteArray>("vtimezone");
    QTest::addColumn<QDateTime>("onDate");
    QTest::addColumn<QByteArray>("origTz");
    QTest::addColumn<QByteArray>("expTz");

    QTest::newRow("dummy-western")
                << QByteArray(VTZ_Western) << QDateTime{}
                << QByteArray("Test-Dummy-Western") << QByteArray("America/Toronto");
    QTest::newRow("dummy-other")
                << QByteArray(VTZ_other) << QDateTime{}
                << QByteArray("Test-Dummy-Other") << QByteArray("UTC+03:00");
    QTest::newRow("dummy-other-dst DST")
                << QByteArray(VTZ_other_DST) << QDateTime({ 2017, 03, 10 }, {})
                << QByteArray("Test-Dummy-Other-DST") << QByteArray("UTC+03:00");
    QTest::newRow("dummy-other-dst STD")
                << QByteArray(VTZ_other_DST) << QDateTime({ 2017, 07, 05 }, {})
                << QByteArray("Test-Dummy-Other-DST") << QByteArray("UTC+05:00");
    QTest::newRow("dummy-other-dst DST after")
                << QByteArray(VTZ_other_DST)
                << QDateTime({ 2017, 12, 24 }, {})
                << QByteArray("Test-Dummy-Other-DST") << QByteArray("UTC+03:00");
    QTest::newRow("iana")
                << QByteArray() << QDateTime({ 2017, 9, 14 }, {})
                << QByteArray("Europe/Zurich") << QByteArray("Europe/Zurich");
}

void ICalTimeZonesTest::parse()
{
    QFETCH(QByteArray, vtimezone);
    QFETCH(QDateTime, onDate);
    QFETCH(QByteArray, origTz);
    QFETCH(QByteArray, expTz);

    QByteArray calText(calendarHeader);
    calText += vtimezone;
    calText += calendarFooter;

    auto vcalendar = loadCALENDAR(calText.constData());

    ICalTimeZoneCache timezones;
    ICalTimeZoneParser parser(&timezones);
    parser.parse(vcalendar);

    icalcomponent_free(vcalendar);

    QCOMPARE(timezones.tzForTime(onDate, origTz).id(), expTz);
}

void ICalTimeZonesTest::write()
{
    auto vtimezone = ICalTimeZoneParser::vcaltimezoneFromQTimeZone(QTimeZone("Europe/Prague"),
                                                                   QDateTime::currentDateTimeUtc().addYears(-200));
#if defined(USE_ICAL_3)
    QCOMPARE(vtimezone, QByteArray(VTZ_Prague).replace(";VALUE=DATE-TIME", "")); //krazy:exclude=doublequote_chars
#else
    QCOMPARE(vtimezone, QByteArray(VTZ_Prague));
#endif
}

icalcomponent *loadCALENDAR(const char *vcal)
{
    icalcomponent *calendar = icalcomponent_new_from_string(const_cast<char *>(vcal));
    if (calendar) {
        if (icalcomponent_isa(calendar) == ICAL_VCALENDAR_COMPONENT) {
            return calendar;
        }
        icalcomponent_free(calendar);
    }
    return nullptr;
}
