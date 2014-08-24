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

#include <QTemporaryFile>

#include <QtCore/QDateTime>
#include <QtCore/QTextStream>

#include <stdlib.h>

#include <qtest.h>
QTEST_MAIN(ICalTimeZonesTest)

extern "C" {
#include <libical/ical.h>
}
#include "icaltimezones.h"
using namespace KCalCore;

static icalcomponent *loadCALENDAR(const char *vcal);
static icalcomponent *loadVTIMEZONE(const char *vtz);

#define QDTUtc(y,mo,d,h,mi,s)  QDateTime(QDate(y,mo,d), QTime(h,mi,s), Qt::UTC)
#define QDTLocal(y,mo,d,h,mi,s)  QDateTime(QDate(y,mo,d), QTime(h,mi,s), Qt::LocalTime)

static QDateTime start(QDate(1967,10,29), QTime(6,0,0), Qt::UTC);
static QDateTime daylight87(QDate(1987,4,5),   QTime(7,0,0), Qt::UTC);
static QDateTime standardOct87(QDate(1987,10,25), QTime(6,0,0), Qt::UTC);
static QDateTime daylight88(QDate(1988,4,3),   QTime(7,0,0), Qt::UTC);
static QDateTime daylight97(QDate(1997,4,6),   QTime(7,0,0), Qt::UTC);
static QDateTime standardOct97(QDate(1997,10,26), QTime(6,0,0), Qt::UTC);
static QDateTime spring98(QDate(1998,5,5),   QTime(7,0,0), Qt::UTC);
static QDateTime standardOct98(QDate(1998,10,25), QTime(6,0,0), Qt::UTC);
static QDateTime daylight99(QDate(1999,4,25),  QTime(7,0,0), Qt::UTC);
static QDateTime standardOct99(QDate(1999,10,31), QTime(6,0,0), Qt::UTC);
static QDateTime daylight00(QDate(2000,4,30),  QTime(7,0,0), Qt::UTC);
static QDateTime spring01(QDate(2001,5,1),   QTime(7,0,0), Qt::UTC);

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

void ICalTimeZonesTest::parse()
{
    // Create the full CALENDAR text and write it to a temporary file
    QByteArray text = calendarHeader;
    text += VTZ_Western;
    text += VTZ_other;
    text += calendarFooter;
    QTemporaryFile tmpFile;
    tmpFile.open();
    QString path = tmpFile.fileName();
    QTextStream ts(&tmpFile);
    ts << text.data();
    ts.flush();

    // Parse the file, the CALENDAR text string and the individual VTIMEZONE strings,
    // and check that ICalTimeZone instances with the same names are created in each case.
    ICalTimeZoneSource src;
    ICalTimeZones timezones1;
    QVERIFY(src.parse(path, timezones1));

    icalcomponent *calendar = loadCALENDAR(text);
    QVERIFY(calendar);
    ICalTimeZones timezones2;
    QVERIFY(src.parse(calendar, timezones2));

    icaltimezone *icaltz = icaltimezone_new();
    for (icalcomponent *ctz = icalcomponent_get_first_component(calendar, ICAL_VTIMEZONE_COMPONENT);
            ctz;  ctz = icalcomponent_get_next_component(calendar, ICAL_VTIMEZONE_COMPONENT)) {
        ICalTimeZone tz = src.parse(ctz);
        QVERIFY(tz.isValid());
        QVERIFY(timezones1.zone(tz.name()).isValid());
        QVERIFY(timezones2.zone(tz.name()).isValid());

        QVERIFY(icaltimezone_set_component(icaltz, ctz));
        ICalTimeZone tz2 = src.parse(icaltz);
        QVERIFY(tz2.isValid());
        QCOMPARE(tz2.name(), tz.name());
    }
    icaltimezone_free(icaltz, 1);
    icalcomponent_free(calendar);
}


/////////////////////
// ICalTimeZone tests
/////////////////////

void ICalTimeZonesTest::general()
{
    icalcomponent *vtimezone = loadVTIMEZONE(VTZ_Western);
    QVERIFY(vtimezone);
    ICalTimeZoneSource src;
    ICalTimeZone tz = src.parse(vtimezone);
    QVERIFY(tz.isValid());
    icaltimezone *icaltz = icaltimezone_new();
    QVERIFY(icaltimezone_set_component(icaltz, vtimezone));
    ICalTimeZone itz = src.parse(icaltz);
    QVERIFY(itz.isValid());

    QCOMPARE(tz.name(), QString::fromLatin1("Test-Dummy-Western"));
    QCOMPARE(tz.url(), QByteArray("http://tz.reference.net/dummies/western"));
    QCOMPARE(tz.city(), QString::fromLatin1("Zedland/Tryburgh"));
    QCOMPARE(tz.lastModified(), QDateTime(QDate(1987,1,1), QTime(0,0,0), Qt::UTC));
    QCOMPARE(tz.vtimezone(), QByteArray(VTZ_Western));

    ICalTimeZone tz1(tz);
    QCOMPARE(tz1.name(), tz.name());
    QCOMPARE(tz1.url(), tz.url());
    QCOMPARE(tz1.city(), tz.city());
    QCOMPARE(tz1.lastModified(), tz.lastModified());
    QCOMPARE(tz1.vtimezone(), tz.vtimezone());

    QCOMPARE(itz.name(), tz.name());
    QCOMPARE(itz.url(), tz.url());
    QCOMPARE(itz.city(), tz.city());
    QCOMPARE(itz.lastModified(), tz.lastModified());
    icaltimezone_free(icaltz, 0);

    vtimezone = loadVTIMEZONE(VTZ_other);
    QVERIFY(vtimezone);
    ICalTimeZone tz2 = src.parse(vtimezone);
    QVERIFY(tz2.isValid());
    QVERIFY(icaltimezone_set_component(icaltz, vtimezone));
    ICalTimeZone itz2 = src.parse(icaltz);
    QVERIFY(itz2.isValid());

    QCOMPARE(tz2.name(), QString::fromLatin1("Test-Dummy-Other"));
    QCOMPARE(tz2.url(), QByteArray("http://tz.reference.net/dummies/other"));
    QCOMPARE(tz2.city(), QString::fromLatin1("Wyland/Tryburgh"));
    QVERIFY(tz2.lastModified().isNull());
    QCOMPARE(tz2.vtimezone(), QByteArray(VTZ_other));

    tz1 = tz2;
    QCOMPARE(tz1.name(), tz2.name());
    QCOMPARE(tz1.url(), tz2.url());
    QCOMPARE(tz1.city(), tz2.city());
    QCOMPARE(tz1.lastModified(), tz2.lastModified());
    QCOMPARE(tz1.vtimezone(), tz2.vtimezone());

    QCOMPARE(tz1.name(), itz2.name());
    QCOMPARE(tz1.url(), itz2.url());
    QCOMPARE(tz1.city(), itz2.city());
    QCOMPARE(tz1.lastModified(), itz2.lastModified());

    icaltimezone_free(icaltz, 1);
}

void ICalTimeZonesTest::offsetAtUtc()
{
    QDateTime local(QDate(2000,6,30), QTime(7,0,0), Qt::LocalTime);

    icalcomponent *vtimezone = loadVTIMEZONE(VTZ_Western);
    QVERIFY(vtimezone);
    ICalTimeZoneSource src;
    ICalTimeZone tz = src.parse(vtimezone);
    QVERIFY(tz.isValid());
    icalcomponent_free(vtimezone);

    QCOMPARE(tz.data(true)->previousUtcOffset(), -4 * 3600);
    QCOMPARE(tz.transitions()[0].time(), start);
    QCOMPARE(tz.offsetAtUtc(start.addSecs(-1)), -4 * 3600);
    QCOMPARE(tz.offsetAtUtc(start), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(daylight87.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(daylight87), -4 * 3600);
    QCOMPARE(tz.offsetAtUtc(standardOct87.addSecs(-1)), -4 * 3600);
    QCOMPARE(tz.offsetAtUtc(standardOct87), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(standardOct87.addDays(1)), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(daylight88.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(daylight88), -4 * 3600);
    QCOMPARE(tz.offsetAtUtc(daylight97.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(daylight97), -4 * 3600);
    QCOMPARE(tz.offsetAtUtc(standardOct97.addSecs(-1)), -4 * 3600);
    QCOMPARE(tz.offsetAtUtc(standardOct97), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(spring98), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(standardOct98.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(standardOct98), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(daylight99.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(daylight99), -4 * 3600);
    QCOMPARE(tz.offsetAtUtc(standardOct99.addSecs(-1)), -4 * 3600);
    QCOMPARE(tz.offsetAtUtc(standardOct99), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(daylight00.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(daylight00), -4 * 3600);
    QCOMPARE(tz.offsetAtUtc(spring01), -5 * 3600);
    QCOMPARE(tz.offsetAtUtc(local), 0);

    // Check that copy constructor copies phases correctly
    ICalTimeZone tz1(tz);
    QCOMPARE(tz1.offsetAtUtc(start.addSecs(-1)), -4 * 3600);
    QCOMPARE(tz1.offsetAtUtc(start), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(daylight87.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(daylight87), -4 * 3600);
    QCOMPARE(tz1.offsetAtUtc(standardOct87.addSecs(-1)), -4 * 3600);
    QCOMPARE(tz1.offsetAtUtc(standardOct87), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(standardOct87.addDays(1)), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(daylight88.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(daylight88), -4 * 3600);
    QCOMPARE(tz1.offsetAtUtc(daylight97.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(daylight97), -4 * 3600);
    QCOMPARE(tz1.offsetAtUtc(standardOct97.addSecs(-1)), -4 * 3600);
    QCOMPARE(tz1.offsetAtUtc(standardOct97), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(spring98), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(standardOct98.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(standardOct98), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(daylight99.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(daylight99), -4 * 3600);
    QCOMPARE(tz1.offsetAtUtc(standardOct99.addSecs(-1)), -4 * 3600);
    QCOMPARE(tz1.offsetAtUtc(standardOct99), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(daylight00.addSecs(-1)), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(daylight00), -4 * 3600);
    QCOMPARE(tz1.offsetAtUtc(spring01), -5 * 3600);
    QCOMPARE(tz1.offsetAtUtc(local), 0);
}

void ICalTimeZonesTest::offset()
{
    icalcomponent *vtimezone = loadVTIMEZONE(VTZ_Western);
    QVERIFY(vtimezone);
    ICalTimeZoneSource src;
    ICalTimeZone tz = src.parse(vtimezone);
    QVERIFY(tz.isValid());
    icalcomponent_free(vtimezone);

    QCOMPARE(tz.offset(KTimeZone::toTime_t(start.addSecs(-1))), -4 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(start)), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(daylight87.addSecs(-1))), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(daylight87)), -4 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(standardOct87.addSecs(-1))), -4 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(standardOct87)), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(standardOct87.addDays(1))), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(daylight88.addSecs(-1))), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(daylight88)), -4 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(daylight97.addSecs(-1))), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(daylight97)), -4 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(standardOct97.addSecs(-1))), -4 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(standardOct97)), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(spring98)), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(standardOct98.addSecs(-1))), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(standardOct98)), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(daylight99.addSecs(-1))), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(daylight99)), -4 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(standardOct99.addSecs(-1))), -4 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(standardOct99)), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(daylight00.addSecs(-1))), -5 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(daylight00)), -4 * 3600);
    QCOMPARE(tz.offset(KTimeZone::toTime_t(spring01)), -5 * 3600);
}

void ICalTimeZonesTest::offsetAtZoneTime()
{
    int offset2;

    icalcomponent *vtimezone = loadVTIMEZONE(VTZ_Western);
    QVERIFY(vtimezone);
    ICalTimeZoneSource src;
    icaltimezone *icaltz = icaltimezone_new();
    QVERIFY(icaltimezone_set_component(icaltz, vtimezone));
    ICalTimeZone tz = src.parse(icaltz);
    QVERIFY(tz.isValid());

    // Standard time: start of definitions at 2:00:00 local time
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1967,10,29, 0,59,59), &offset2), -4 * 3600);
    QCOMPARE(offset2, -4 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1967,10,29, 1,0,0), &offset2), -4 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1967,10,29, 1,59,59), &offset2), -4 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1967,10,29, 2,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1967,10,29, 2,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1967,10,29, 3,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);

    // Change to daylight savings time at 2:00:00 local time
    // Local times 2:00:00 to 2:59:59 don't exist.
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1987,4,5, 1,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1987,4,5, 2,0,0), &offset2), KTimeZone::InvalidOffset);
    QCOMPARE(offset2, KTimeZone::InvalidOffset);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1987,4,5, 2,59,59), &offset2), KTimeZone::InvalidOffset);
    QCOMPARE(offset2, KTimeZone::InvalidOffset);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1987,4,5, 3,0,0), &offset2), -4 * 3600);
    QCOMPARE(offset2, -4 * 3600);

    // Change to standard time at 2:00:00 local time
    // Local times 2:00:00 to 2:59:59 occur twice.
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1987,10,25, 0,59,59), &offset2), -4 * 3600);
    QCOMPARE(offset2, -4 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1987,10,25, 1,0,0), &offset2), -4 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1987,10,25, 1,59,59), &offset2), -4 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1987,10,25, 2,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1987,10,25, 2,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1987,10,25, 3,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);

    // Change to daylight savings time at 2:00:00 local time
    // Local times 2:00:00 to 2:59:59 don't exist.
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1988,4,3, 1,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1988,4,3, 2,0,0), &offset2), KTimeZone::InvalidOffset);
    QCOMPARE(offset2, KTimeZone::InvalidOffset);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1988,4,3, 2,59,59), &offset2), KTimeZone::InvalidOffset);
    QCOMPARE(offset2, KTimeZone::InvalidOffset);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1988,4,3, 3,0,0), &offset2), -4 * 3600);
    QCOMPARE(offset2, -4 * 3600);

    // Change to daylight savings time at 2:00:00 local time
    // Local times 2:00:00 to 2:59:59 don't exist.
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1997,4,6, 1,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1997,4,6, 2,0,0), &offset2), KTimeZone::InvalidOffset);
    QCOMPARE(offset2, KTimeZone::InvalidOffset);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1997,4,6, 2,59,59), &offset2), KTimeZone::InvalidOffset);
    QCOMPARE(offset2, KTimeZone::InvalidOffset);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1997,4,6, 3,0,0), &offset2), -4 * 3600);
    QCOMPARE(offset2, -4 * 3600);

    // Change to standard time at 2:00:00 local time
    // Local times 2:00:00 to 2:59:59 occur twice.
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1997,10,26, 0,59,59), &offset2), -4 * 3600);
    QCOMPARE(offset2, -4 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1997,10,26, 1,0,0), &offset2), -4 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1997,10,26, 1,59,59), &offset2), -4 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1997,10,26, 2,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1997,10,26, 2,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1997,10,26, 3,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);

    // In standard time (no daylight savings this year)
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1998,5,5, 2,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);

    // Remain in standard time (no daylight savings this year)
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1998,10,25, 0,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1998,10,25, 1,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1998,10,25, 2,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1998,10,25, 2,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1998,10,25, 3,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);

    // Change to daylight savings time at 2:00:00 local time
    // Local times 2:00:00 to 2:59:59 don't exist.
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1999,4,25, 1,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1999,4,25, 2,0,0), &offset2), KTimeZone::InvalidOffset);
    QCOMPARE(offset2, KTimeZone::InvalidOffset);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1999,4,25, 2,59,59), &offset2), KTimeZone::InvalidOffset);
    QCOMPARE(offset2, KTimeZone::InvalidOffset);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1999,4,25, 3,0,0), &offset2), -4 * 3600);
    QCOMPARE(offset2, -4 * 3600);

    // Change to standard time at 2:00:00 local time
    // Local times 2:00:00 to 2:59:59 occur twice.
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1999,10,31, 0,59,59), &offset2), -4 * 3600);
    QCOMPARE(offset2, -4 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1999,10,31, 1,0,0), &offset2), -4 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1999,10,31, 1,59,59), &offset2), -4 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1999,10,31, 2,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1999,10,31, 2,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(1999,10,31, 3,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);

    // Change to daylight savings time at 2:00:00 local time
    // Local times 2:00:00 to 2:59:59 don't exist.
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(2000,4,30, 1,59,59), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(2000,4,30, 2,0,0), &offset2), KTimeZone::InvalidOffset);
    QCOMPARE(offset2, KTimeZone::InvalidOffset);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(2000,4,30, 2,59,59), &offset2), KTimeZone::InvalidOffset);
    QCOMPARE(offset2, KTimeZone::InvalidOffset);
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(2000,4,30, 3,0,0), &offset2), -4 * 3600);
    QCOMPARE(offset2, -4 * 3600);

    // In standard time (no daylight savings this year)
    QCOMPARE(tz.offsetAtZoneTime(QDTLocal(2001,5,1, 2,0,0), &offset2), -5 * 3600);
    QCOMPARE(offset2, -5 * 3600);

    // UTC time
    QCOMPARE(tz.offsetAtZoneTime(daylight99.addSecs(-1), &offset2), 0);
    QCOMPARE(offset2, 0);

    icaltimezone_free(icaltz, 1);
}

void ICalTimeZonesTest::abbreviation()
{
    QDateTime local(QDate(2000,6,30), QTime(7,0,0), Qt::LocalTime);

    icalcomponent *vtimezone = loadVTIMEZONE(VTZ_Western);
    QVERIFY(vtimezone);
    ICalTimeZoneSource src;
    ICalTimeZone tz = src.parse(vtimezone);
    QVERIFY(tz.isValid());
    icalcomponent_free(vtimezone);

    QCOMPARE(tz.abbreviation(start), QByteArray("WST"));
    QCOMPARE(tz.abbreviation(daylight87), QByteArray("WDT1"));
    QCOMPARE(tz.abbreviation(spring98), QByteArray("WST"));
    QCOMPARE(tz.abbreviation(daylight99), QByteArray("WDT2"));
    QCOMPARE(tz.abbreviation(standardOct99), QByteArray("WST"));
    QCOMPARE(tz.abbreviation(spring01), QByteArray("WST"));
    QVERIFY(tz.abbreviation(local).isEmpty());

    QList<QByteArray> abbrs = tz.abbreviations();
    QCOMPARE(abbrs.count(), 3);
    QVERIFY(abbrs.indexOf(QByteArray("WST")) >= 0);
    QVERIFY(abbrs.indexOf(QByteArray("WDT1")) >= 0);
    QVERIFY(abbrs.indexOf(QByteArray("WDT2")) >= 0);
}

void ICalTimeZonesTest::isDstAtUtc()
{
    QDateTime local(QDate(2000,6,30), QTime(7,0,0), Qt::LocalTime);

    icalcomponent *vtimezone = loadVTIMEZONE(VTZ_Western);
    QVERIFY(vtimezone);
    ICalTimeZoneSource src;
    ICalTimeZone tz = src.parse(vtimezone);
    QVERIFY(tz.isValid());
    icalcomponent_free(vtimezone);

    QVERIFY(!tz.isDstAtUtc(start.addSecs(-1)));
    QVERIFY(!tz.isDstAtUtc(start));
    QVERIFY(!tz.isDstAtUtc(daylight87.addSecs(-1)));
    QVERIFY(tz.isDstAtUtc(daylight87));
    QVERIFY(tz.isDstAtUtc(standardOct87.addSecs(-1)));
    QVERIFY(!tz.isDstAtUtc(standardOct87));
    QVERIFY(!tz.isDstAtUtc(standardOct87.addDays(1)));
    QVERIFY(!tz.isDstAtUtc(daylight88.addSecs(-1)));
    QVERIFY(tz.isDstAtUtc(daylight88));
    QVERIFY(!tz.isDstAtUtc(daylight97.addSecs(-1)));
    QVERIFY(tz.isDstAtUtc(daylight97));
    QVERIFY(tz.isDstAtUtc(standardOct97.addSecs(-1)));
    QVERIFY(!tz.isDstAtUtc(standardOct97));
    QVERIFY(!tz.isDstAtUtc(spring98));
    QVERIFY(!tz.isDstAtUtc(standardOct98.addSecs(-1)));
    QVERIFY(!tz.isDstAtUtc(standardOct98));
    QVERIFY(!tz.isDstAtUtc(daylight99.addSecs(-1)));
    QVERIFY(tz.isDstAtUtc(daylight99));
    QVERIFY(tz.isDstAtUtc(standardOct99.addSecs(-1)));
    QVERIFY(!tz.isDstAtUtc(standardOct99));
    QVERIFY(!tz.isDstAtUtc(daylight00.addSecs(-1)));
    QVERIFY(tz.isDstAtUtc(daylight00));
    QVERIFY(!tz.isDstAtUtc(spring01));
    QVERIFY(!tz.isDstAtUtc(local));
}

void ICalTimeZonesTest::isDst()
{
    icalcomponent *vtimezone = loadVTIMEZONE(VTZ_Western);
    QVERIFY(vtimezone);
    ICalTimeZoneSource src;
    ICalTimeZone tz = src.parse(vtimezone);
    QVERIFY(tz.isValid());
    icalcomponent_free(vtimezone);

    QVERIFY(!tz.isDst((time_t)start.addSecs(-1).toTime_t()));
    QVERIFY(!tz.isDst((time_t)start.toTime_t()));
    QVERIFY(!tz.isDst((time_t)daylight87.addSecs(-1).toTime_t()));
    QVERIFY(tz.isDst((time_t)daylight87.toTime_t()));
    QVERIFY(tz.isDst((time_t)standardOct87.addSecs(-1).toTime_t()));
    QVERIFY(!tz.isDst((time_t)standardOct87.toTime_t()));
    QVERIFY(!tz.isDst((time_t)standardOct87.addDays(1).toTime_t()));
    QVERIFY(!tz.isDst((time_t)daylight88.addSecs(-1).toTime_t()));
    QVERIFY(tz.isDst((time_t)daylight88.toTime_t()));
    QVERIFY(!tz.isDst((time_t)daylight97.addSecs(-1).toTime_t()));
    QVERIFY(tz.isDst((time_t)daylight97.toTime_t()));
    QVERIFY(tz.isDst((time_t)standardOct97.addSecs(-1).toTime_t()));
    QVERIFY(!tz.isDst((time_t)standardOct97.toTime_t()));
    QVERIFY(!tz.isDst((time_t)spring98.toTime_t()));
    QVERIFY(!tz.isDst((time_t)standardOct98.addSecs(-1).toTime_t()));
    QVERIFY(!tz.isDst((time_t)standardOct98.toTime_t()));
    QVERIFY(!tz.isDst((time_t)daylight99.addSecs(-1).toTime_t()));
    QVERIFY(tz.isDst((time_t)daylight99.toTime_t()));
    QVERIFY(tz.isDst((time_t)standardOct99.addSecs(-1).toTime_t()));
    QVERIFY(!tz.isDst((time_t)standardOct99.toTime_t()));
    QVERIFY(!tz.isDst((time_t)daylight00.addSecs(-1).toTime_t()));
    QVERIFY(tz.isDst((time_t)daylight00.toTime_t()));
    QVERIFY(!tz.isDst((time_t)spring01.toTime_t()));
}

void ICalTimeZonesTest::utcOffsets()
{
    icalcomponent *vtimezone = loadVTIMEZONE(VTZ_Western);
    QVERIFY(vtimezone);
    ICalTimeZoneSource src;
    ICalTimeZone tz = src.parse(vtimezone);
    QVERIFY(tz.isValid());
    icalcomponent_free(vtimezone);

    vtimezone = loadVTIMEZONE(VTZ_other);
    QVERIFY(vtimezone);
    ICalTimeZone tz2 = src.parse(vtimezone);
    QVERIFY(tz2.isValid());
    icalcomponent_free(vtimezone);

    QList<int> offsets = tz.utcOffsets();
    QCOMPARE(offsets.count(), 2);
    QCOMPARE(offsets[0], -5 * 3600);
    QCOMPARE(offsets[1], -4 * 3600);

    offsets = tz2.utcOffsets();
    QCOMPARE(offsets.count(), 1);
    QCOMPARE(offsets[0], 3 * 3600);
}

icalcomponent *loadCALENDAR(const char *vcal)
{
    icalcomponent *calendar = icalcomponent_new_from_string(const_cast<char*>(vcal));
    if (calendar) {
        if (icalcomponent_isa(calendar) == ICAL_VCALENDAR_COMPONENT) {
            return calendar;
        }
        icalcomponent_free(calendar);
    }
    return 0;
}

icalcomponent *loadVTIMEZONE(const char *vtz)
{
    icalcomponent *vtimezone = icalcomponent_new_from_string(const_cast<char*>(vtz));
    if (vtimezone) {
        if (icalcomponent_isa(vtimezone) == ICAL_VTIMEZONE_COMPONENT) {
            return vtimezone;
        }
        icalcomponent_free(vtimezone);
    }
    return 0;
}
