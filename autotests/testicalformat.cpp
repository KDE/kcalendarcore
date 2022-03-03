/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
  SPDX-FileContributor: Sergio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testicalformat.h"
#include "event.h"
#include "icalformat.h"
#include "memorycalendar.h"
#include "occurrenceiterator.h"

#include <QTest>
#include <QTimeZone>

QTEST_MAIN(ICalFormatTest)

using namespace KCalendarCore;

void ICalFormatTest::testDeserializeSerialize()
{
    ICalFormat format;

    const QString serializedCalendar = QLatin1String(
        "BEGIN:VCALENDAR\n"
        "PRODID:-//IDN nextcloud.com//Calendar app 2.0.4//EN\n"
        "VERSION:2.0\n"
        "BEGIN:VEVENT\n"
        "CREATED:20201103T161248Z\n"
        "DTSTAMP:20201103T161340Z\n"
        "LAST-MODIFIED:20201103T161340Z\n"
        "SEQUENCE:2\n"
        "UID:bd1d299d-3b03-4514-be69-e680ad2ff884\n"
        "DTSTART;TZID=Europe/Paris:20201103T100000\n"
        "DTEND;TZID=Europe/Paris:20201103T110000\n"
        "SUMMARY:test recur\n"
        "RRULE:FREQ=DAILY;COUNT=4\n"
        "END:VEVENT\n"
        "BEGIN:VEVENT\n"
        "CREATED:20201103T161823Z\n"
        "DTSTAMP:20201103T161823Z\n"
        "LAST-MODIFIED:20201103T161823Z\n"
        "SEQUENCE:1\n"
        "UID:bd1d299d-3b03-4514-be69-e680ad2ff884\n"
        "DTSTART;TZID=Europe/Paris:20201104T111500\n"
        "DTEND;TZID=Europe/Paris:20201104T121500\n"
        "SUMMARY:test recur\n"
        "COLOR:khaki\n"
        "RECURRENCE-ID;TZID=Europe/Paris:20201104T100000\n"
        "END:VEVENT\n"
        "END:VCALENDAR");
    MemoryCalendar::Ptr calendar = MemoryCalendar::Ptr(new MemoryCalendar(QTimeZone::utc()));
    QVERIFY(format.fromString(calendar, serializedCalendar));
    const QString uid = QString::fromLatin1("bd1d299d-3b03-4514-be69-e680ad2ff884");
    Incidence::Ptr parent = calendar->incidence(uid);
    QVERIFY(parent);
    const QDateTime start(QDate(2020, 11, 3), QTime(9, 0), QTimeZone::utc());
    QCOMPARE(parent->dtStart(), start);
    QCOMPARE(parent.staticCast<Event>()->dtEnd(), start.addSecs(3600));
    QCOMPARE(parent->summary(), QString::fromLatin1("test recur"));
    QCOMPARE(parent->revision(), 2);
    Recurrence *recur = parent->recurrence();
    QVERIFY(recur->recurs());
    QCOMPARE(recur->duration(), 4);
    QCOMPARE(recur->recurrenceType(), static_cast<ushort>(Recurrence::rDaily));

    Incidence::Ptr occurrence = calendar->incidence(uid, start.addDays(1));
    QVERIFY(occurrence);
    const QDateTime startOcc(QDate(2020, 11, 4), QTime(10, 15), QTimeZone::utc());
    QCOMPARE(occurrence->dtStart(), startOcc);
    QCOMPARE(occurrence.staticCast<Event>()->dtEnd(), startOcc.addSecs(3600));
    QCOMPARE(occurrence->color(), QString::fromLatin1("khaki"));
    QCOMPARE(occurrence->summary(), QString::fromLatin1("test recur"));
    QCOMPARE(occurrence->revision(), 1);
    QVERIFY(occurrence->hasRecurrenceId());
    QCOMPARE(occurrence->recurrenceId(), start.addDays(1));

    const QString serialization = format.toString(calendar, QString());
    QVERIFY(!serialization.isEmpty());
    MemoryCalendar::Ptr check = MemoryCalendar::Ptr(new MemoryCalendar(QTimeZone::utc()));
    QVERIFY(format.fromString(check, serialization));
    Incidence::Ptr reparent = check->incidence(uid);
    QVERIFY(reparent);
    QCOMPARE(*parent, *reparent);
    Incidence::Ptr reoccurence = check->incidence(uid, start.addDays(1));
    QVERIFY(reoccurence);
    QCOMPARE(*occurrence, *reoccurence);
}

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
    const QChar latin1_umlaut[] = {(QChar)0xFC, QLatin1Char('\0')};
    event->setSummary(QString(latin1_umlaut));

    // Test if toString( Incidence ) didn't mess charsets
    const QString serialized = format.toString(event.staticCast<Incidence>());
    const QChar utf_umlaut[] = {(QChar)0xC3, (QChar)0XBC, QLatin1Char('\0')};
    QVERIFY(serialized.toUtf8().contains(QString(utf_umlaut).toLatin1().constData()));
    QVERIFY(!serialized.toUtf8().contains(QString(latin1_umlaut).toLatin1().constData()));
    QVERIFY(serialized.toLatin1().contains(QString(latin1_umlaut).toLatin1().constData()));
    QVERIFY(!serialized.toLatin1().contains(QString(utf_umlaut).toLatin1().constData()));

    // test fromString( QString )
    const QString serializedCalendar = QLatin1String("BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\n") + serialized
        + QLatin1String("\nEND:VCALENDAR");

    Incidence::Ptr event2 = format.fromString(serializedCalendar);
    QVERIFY(event->summary() == event2->summary());
    QVERIFY(event2->summary().toUtf8() == QByteArray(QString(utf_umlaut).toLatin1().constData()));

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

    Event::Ptr loadedEvent = calendar2->incidences().at(0).staticCast<Event>();
    QVERIFY(loadedEvent->summary().toUtf8() == QByteArray(QString(utf_umlaut).toLatin1().constData()));
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
    const QString serializedCalendar = QLatin1String("BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\n") + serialized
        + QLatin1String("\nEND:VCALENDAR");

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
    event->setDtStart(QDate(2017, 03, 24).startOfDay());
    Alarm::Ptr alarm = event->newAlarm();
    alarm->setType(Alarm::Display);
    alarm->setStartOffset(Duration(0));

    const QString serialized = QLatin1String("BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\n")
        + format.toString(event.staticCast<Incidence>()) + QLatin1String("\nEND:VCALENDAR");

    Incidence::Ptr event2 = format.fromString(serialized);
    Alarm::Ptr alarm2 = event2->alarms()[0];
    QCOMPARE(*alarm, *alarm2);
}

void ICalFormatTest::testDateTimeSerialization_data()
{
    QTest::addColumn<QDateTime>("dtStart");
    QTest::addColumn<QByteArray>("dtStartData");

    QTest::newRow("UTC time spec")
        << QDateTime(QDate(2021, 4, 9), QTime(12, 00), Qt::UTC)
        << QByteArray("DTSTART:20210409T120000Z");
    QTest::newRow("UTC time zone")
        << QDateTime(QDate(2021, 4, 9), QTime(12, 00), QTimeZone::utc())
        << QByteArray("DTSTART:20210409T120000Z");
    QTest::newRow("named time zone")
        << QDateTime(QDate(2021, 4, 9), QTime(14, 00), QTimeZone("Europe/Paris"))
        << QByteArray("DTSTART;TZID=Europe/Paris:20210409T140000");
}

void ICalFormatTest::testDateTimeSerialization()
{
    QFETCH(QDateTime, dtStart);
    QFETCH(QByteArray, dtStartData);

    Incidence::Ptr event(new Event);
    QVERIFY(event);
    event->setDtStart(dtStart);
    QCOMPARE(event->dtStart(), dtStart);

    ICalFormat format;
    const QByteArray output = format.toRawString(event);
    const QList<QByteArray> lines = output.split('\n');
    for (const QByteArray &line: lines) {
        if (line.startsWith(QByteArray("DTSTART"))) {
            QCOMPARE(line.chopped(1), dtStartData);
            break;
        }
    }
}

void ICalFormatTest::testRDate()
{
    ICalFormat format;

    const QString serializedCalendar = QLatin1String(
        "BEGIN:VCALENDAR\n"
        "VERSION:2.0\n"
        "PRODID:-//Lotus Development Corporation//NONSGML Notes 9.0.1//EN_C\n"
        "METHOD:PUBLISH\n"
        "BEGIN:VEVENT\n"
        "DTSTART:20210630T100000Z\n"
        "DTEND:20210630T110000Z\n"
        "TRANSP:OPAQUE\n"
        "RDATE;VALUE=PERIOD:20210630T100000Z/20210630T110000Z\n"
        " ,20210825T100000Z/20210825T110000Z,20211027T100000Z/20211027T110000Z\n"
        " ,20211215T110000Z/PT2H\n"
        "LAST-MODIFIED:20210601T094627Z\n"
        "DTSTAMP:20210601T092939Z\n"
        "UID:5FC21473F5CC80CCC12586E70033ED9C-Lotus_Notes_Generated\n"
        "END:VEVENT\n"
        "END:VCALENDAR\n");
    MemoryCalendar::Ptr calendar(new MemoryCalendar(QTimeZone::utc()));
    QVERIFY(format.fromString(calendar, serializedCalendar));
    const QString uid = QString::fromLatin1("5FC21473F5CC80CCC12586E70033ED9C-Lotus_Notes_Generated");
    Incidence::Ptr event = calendar->incidence(uid);
    QVERIFY(event);
    QVERIFY(event->recurs());

    const QDateTime ev1(QDate(2021, 6, 30), QTime(10, 0), Qt::UTC);
    const QDateTime ev2(QDate(2021, 8, 25), QTime(10, 0), Qt::UTC);
    const QDateTime ev3(QDate(2021, 10, 27), QTime(10, 0), Qt::UTC);
    const QDateTime ev4(QDate(2021, 12, 15), QTime(11, 0), Qt::UTC);
    QCOMPARE(event->recurrence()->rDateTimes(),
             QList<QDateTime>() << ev1 << ev2 << ev3 << ev4);

    OccurrenceIterator it(*calendar, QDateTime(QDate(2021, 6, 1), QTime(0, 0)),
                          QDateTime(QDate(2021, 12, 31), QTime(0, 0)));
    QVERIFY(it.hasNext());
    it.next();
    QCOMPARE(it.occurrenceStartDate(), ev1);
    QCOMPARE(it.occurrenceEndDate(), ev1.addSecs(3600));
    QVERIFY(it.hasNext());
    it.next();
    QCOMPARE(it.occurrenceStartDate(), ev2);
    QCOMPARE(it.occurrenceEndDate(), ev2.addSecs(3600));
    QVERIFY(it.hasNext());
    it.next();
    QCOMPARE(it.occurrenceStartDate(), ev3);
    QCOMPARE(it.occurrenceEndDate(), ev3.addSecs(3600));
    QVERIFY(it.hasNext());
    it.next();
    QCOMPARE(it.occurrenceStartDate(), ev4);
    QCOMPARE(it.occurrenceEndDate(), ev4.addSecs(7200));

    const QStringList output = format.toString(calendar, QString()).split(QString::fromLatin1("\r\n"));
    QVERIFY(output.contains(QString::fromLatin1("RDATE;VALUE=PERIOD:20210630T100000Z/20210630T110000Z")));
    QVERIFY(output.contains(QString::fromLatin1("RDATE;VALUE=PERIOD:20210825T100000Z/20210825T110000Z")));
    QVERIFY(output.contains(QString::fromLatin1("RDATE;VALUE=PERIOD:20211027T100000Z/20211027T110000Z")));
    QVERIFY(output.contains(QString::fromLatin1("RDATE;VALUE=PERIOD:20211215T110000Z/PT2H")));
}

void ICalFormatTest::testDateTime_data()
{
    QTest::addColumn<QByteArray>("dtStartData");
    QTest::addColumn<QDateTime>("dtStart");

    QTest::newRow("clock time")
        << QByteArray("DTSTART:20191113T130000")
        << QDateTime(QDate(2019, 11, 13), QTime(13, 00), Qt::LocalTime);
    QTest::newRow("date")
        << QByteArray("DTSTART;VALUE=DATE:20191113")
        << QDate(2019, 11, 13).startOfDay();
    QTest::newRow("UTC time")
        << QByteArray("DTSTART:20191113T130000Z")
        << QDateTime(QDate(2019, 11, 13), QTime(13, 00), Qt::UTC);
    QTest::newRow("time zone time")
        << QByteArray("DTSTART;TZID=Europe/Paris:20191113T130000")
        << QDateTime(QDate(2019, 11, 13), QTime(13, 00), QTimeZone("Europe/Paris"));
}

void ICalFormatTest::testDateTime()
{
    QFETCH(QByteArray, dtStartData);
    QFETCH(QDateTime, dtStart);

    // test fromString(QString)
    const QByteArray serializedCalendar
        = "BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\nBEGIN:VEVENT\nUID:12345\n"
          + dtStartData
          + "\nEND:VEVENT\nEND:VCALENDAR";

    ICalFormat format;
    Incidence::Ptr event = format.fromString(QString::fromUtf8(serializedCalendar));
    QVERIFY(event);
    QCOMPARE(dtStart, event->dtStart());
}
