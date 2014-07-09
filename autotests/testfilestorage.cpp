/*
  This file is part of the kcalcore library.
  Copyright (C) 2007 Allen Winter <winter@kde.org>

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

#include "testfilestorage.h"
#include "filestorage.h"
#include "memorycalendar.h"


#include <unistd.h>

#include <qtest.h>
QTEST_MAIN(FileStorageTest)

using namespace KCalCore;

void FileStorageTest::testValidity()
{
    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    FileStorage fs(cal, QStringLiteral("fred.ics"));
    QCOMPARE(fs.fileName(), QStringLiteral("fred.ics"));
    QCOMPARE(fs.calendar().data(), cal.data());
    cal->close();
}

void FileStorageTest::testSave()
{
    MemoryCalendar::Ptr cal(new MemoryCalendar(QStringLiteral("UTC")));
    FileStorage fs(cal, QStringLiteral("fred.ics"));

    QDate dt = QDate::currentDate();

    Event::Ptr event1 = Event::Ptr(new Event());
    event1->setUid("1");
    event1->setDtStart(KDateTime(dt));
    event1->setDtEnd(KDateTime(dt).addDays(1));
    event1->setSummary("Event1 Summary");
    event1->setDescription("This is a description of the first event");
    event1->setLocation("the place");
    cal->addEvent(event1);

    Event::Ptr event2 = Event::Ptr(new Event());
    event2->setUid("2");
    event2->setDtStart(KDateTime(dt).addDays(1));
    event2->setDtEnd(KDateTime(dt).addDays(2));
    event2->setSummary("Event2 Summary");
    event2->setDescription("This is a description of the second event");
    event2->setLocation("the other place");
    cal->addEvent(event2);

    QVERIFY(fs.open());
    QVERIFY(fs.save());
    QVERIFY(fs.close());
    cal->close();
    unlink("fred.ics");
}

void FileStorageTest::testSaveLoadSave()
{
    MemoryCalendar::Ptr cal(new MemoryCalendar(QStringLiteral("UTC")));
    FileStorage fs(cal, QStringLiteral("fred.ics"));

    QDate dt = QDate::currentDate();

    Event::Ptr event1 = Event::Ptr(new Event());
    event1->setUid("1");
    event1->setDtStart(KDateTime(dt));
    event1->setDtEnd(KDateTime(dt).addDays(1));
    event1->setSummary("Event1 Summary");
    event1->setDescription("This is a description of the first event");
    event1->setLocation("the place");
    cal->addEvent(event1);

    Event::Ptr event2 = Event::Ptr(new Event());
    event2->setUid("2");
    event2->setDtStart(KDateTime(dt).addDays(1));
    event2->setDtEnd(KDateTime(dt).addDays(2));
    event2->setSummary("Event2 Summary");
    event2->setDescription("This is a description of the second event");
    event2->setLocation("the other place");
    cal->addEvent(event2);

    QVERIFY(fs.open());
    QVERIFY(fs.save());
    QVERIFY(fs.close());
    QVERIFY(fs.open());
    QVERIFY(fs.load());
    Event::Ptr e = fs.calendar()->incidence("1").staticCast<Event>();
    QVERIFY(e != 0);
    QVERIFY(fs.close());
    unlink("fred.ics");

    QVERIFY(fs.open());
    QVERIFY(fs.save());
    QVERIFY(fs.close());
    unlink("fred.ics");
}

void FileStorageTest::testSpecialChars()
{
    const QDate currentDate = QDate::currentDate();
    const QString uid("12345");

    Event::Ptr event = Event::Ptr(new Event());
    event->setUid(uid);
    event->setDtStart(KDateTime(currentDate));
    event->setDtEnd(KDateTime(currentDate.addDays(1)));

    const QChar latin1_umlaut[] = { 0xFC, '\0' };

    event->setSummary(QString(latin1_umlaut));

    // Save to file:
    MemoryCalendar::Ptr cal(new MemoryCalendar(QStringLiteral("UTC")));
    FileStorage fs(cal, QStringLiteral("bart.ics"));
    cal->addEvent(event);

    QVERIFY(fs.open());
    QVERIFY(fs.save());
    QVERIFY(fs.close());

    // Load again:
    MemoryCalendar::Ptr otherCalendar(new MemoryCalendar(QStringLiteral("UTC")));
    FileStorage otherFs(otherCalendar, QStringLiteral("bart.ics"));
    QVERIFY(otherFs.open());
    QVERIFY(otherFs.load());

    Event::Ptr otherEvent = otherCalendar->incidence(uid).staticCast<Event>();
    QVERIFY(otherFs.close());

    QVERIFY(otherEvent);

    // Make sure the retrieved incidence is equal to the original one
    QVERIFY(otherEvent->summary() == event->summary());
    QVERIFY(otherEvent->summary().toLatin1().count() == 1 &&
            strcmp(otherEvent->summary().toLatin1().constData(),
                   QString(latin1_umlaut).toLatin1().constData()) == 0);

    // Make sure bart.ics is in UTF-8
    QFile file(QStringLiteral("bart.ics"));
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

    const QByteArray bytesFromFile = file.readAll();
    const QChar utf_umlaut[] = { 0xC3, 0XBC, '\0' };

    QVERIFY(bytesFromFile.contains(QString(utf_umlaut).toLatin1().constData()));
    QVERIFY(!bytesFromFile.contains(QString(latin1_umlaut).toLatin1().constData()));

    file.close();

    unlink("bart.ics");
}
