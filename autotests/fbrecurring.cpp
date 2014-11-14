/*
  This file is part of the kcalcore library.

  Copyright (c) 2007 Allen Winter <winter@kde.org>

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

#include "icalformat.h"
#include "event.h"
#include "memorycalendar.h"
#include "freebusy.h"

#include <iostream>

#include <QDate>

using namespace KCalCore;
using namespace std;

int main()
{
    ICalFormat f;

    MemoryCalendar::Ptr cal(new MemoryCalendar(QStringLiteral("UTC")));

    Event::Ptr event1 = Event::Ptr(new Event);
    event1->setSummary("A");
    event1->setDtStart(KDateTime(QDate(2006, 1, 1), QTime(12, 0, 0)));
    //event1->setDuration( 60 * 60 );
    event1->setDtEnd(KDateTime(QDate(2006, 1, 1), QTime(13, 0, 0)));
    event1->setAllDay(false);
    event1->recurrence()->setDaily(1);
    //event1->recurrence()->setDuration( 2 );
    event1->recurrence()->setEndDateTime(KDateTime(QDate(2006, 1, 3), QTime(13, 0, 0)));
    cout << f.toICalString(event1).toLocal8Bit().data() << endl;
    cal->addEvent(event1);

    Event::Ptr event2 = Event::Ptr(new Event);
    event2->setSummary("B");
    event2->setDtStart(KDateTime(QDate(2006, 1, 1), QTime(13, 0, 0)));
    //event2->setDuration( 60 * 60 );
    event2->setDtEnd(KDateTime(QDate(2006, 1, 1), QTime(14, 0, 0)));
    event2->setAllDay(false);
    event2->recurrence()->setDaily(1);
    //event2->recurrence()->setDuration( 3 );
    event2->recurrence()->setEndDateTime(KDateTime(QDate(2006, 1, 4), QTime(13, 0, 0)));
    cout << f.toICalString(event2).toLocal8Bit().data() << endl;
    cal->addEvent(event2);

    KDateTime start = KDateTime(QDate(2006, 1, 2), QTime(0, 0, 0));
    KDateTime end = KDateTime(QDate(2006, 1, 3), QTime(0, 0, 0));

    FreeBusy::Ptr freebusy = FreeBusy::Ptr(new FreeBusy(cal->rawEvents(start.date(), end.date()), start, end)) ;
    QString result = f.createScheduleMessage(freebusy, iTIPPublish);
    cout << result.toLocal8Bit().data() << endl;

    return 0;
}
