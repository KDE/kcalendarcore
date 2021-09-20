/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "event.h"
#include "freebusy.h"
#include "icalformat.h"

#include <QDebug>

#include <iostream>

using namespace KCalendarCore;

int main(int, char **)
{
    const QString fbString = QStringLiteral(
        "BEGIN:VCALENDAR\n"
        "PRODID:-//proko2//freebusy 1.0//EN\n"
        "METHOD:PUBLISH\n"
        "VERSION:2.0\n"
        "BEGIN:VFREEBUSY\n"
        "ORGANIZER:MAILTO:test3@kdab.net\n"
        "X-KDE-Foo:bla\n"
        "DTSTAMP:20071202T152453Z\n"
        "URL:http://mail.kdab.net/freebusy/test3%40kdab.net.ifb\n"
        "DTSTART:19700101T000000Z\n"
        "DTEND:200700101T000000Z\n"
        "COMMENT:This is a dummy vfreebusy that indicates an empty calendar\n"
        "FREEBUSY:19700101T000000Z/19700101T000000Z\n"
        "FREEBUSY;X-UID=bGlia2NhbC0xODk4MjgxNTcuMTAxMA==;X-\n"
        " SUMMARY=RW1wbG95ZWUgbWVldGluZw==;X-LOCATION=Um9vb\n"
        " SAyMTM=:20080131T170000Z/20080131T174500Z\n"
        "END:VFREEBUSY\n"
        "END:VCALENDAR\n");

    ICalFormat format;
    FreeBusy::Ptr fb = format.parseFreeBusy(fbString);
    qDebug() << fb->fullBusyPeriods().count() << " " << fb->dtStart().toString();
    const FreeBusyPeriod::List lst = fb->fullBusyPeriods();
    for (const auto &freebusy : lst) {
        qDebug() << freebusy.start().toString() << " " << freebusy.end().toString() << "+ " << freebusy.summary() << ":" << freebusy.location();
    }

    const QMap<QByteArray, QString> props = fb->customProperties();
    for (auto it = props.cbegin(); it != props.cend(); ++it) {
        qDebug() << it.key() << ": " << it.value();
    }
}
