/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "event.h"
#include "icalformat.h"

#include <QDebug>

#include <iostream>

using namespace KCalendarCore;

int main(int, char **)
{
    // std::cout << "Hello World!" << std::endl;
    Event::Ptr ev = Event::Ptr(new Event);
    ev->setSummary(QStringLiteral("Griazi"));
    ICalFormat iformat;
    QString icalstr = iformat.toICalString(ev);
    qDebug() << icalstr;
    Incidence::Ptr ev2 = iformat.fromString(icalstr);
    qDebug() << "Event reread!";

    if (ev2) {
        qDebug() << iformat.toICalString(ev2);
    } else {
        qDebug() << "Could not read incidence";
    }
}
