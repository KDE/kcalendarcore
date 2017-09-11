/*
  This file is part of the kcalcore library.

  Copyright (c) 2017  Daniel Vrátil <dvratil@kde.org>

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

#include "utils.h"

#include <QTimeZone>
#include <KTimeZone>

#include <QDebug>

QDateTime KCalCore::applySpec(const QDateTime &dt, const KDateTime::Spec &spec, bool isAllDay)
{
    if (isAllDay) {
        QDateTime out;
        switch (spec.type()) {
        case KDateTime::UTC:
            return QDateTime(dt.date(), QTime(), Qt::UTC);
        case KDateTime::OffsetFromUTC:
            return QDateTime(dt.date(), QTime(), Qt::OffsetFromUTC, spec.utcOffset());
        case KDateTime::TimeZone:
            return QDateTime(dt.date(), QTime(), QTimeZone(spec.timeZone().name().toUtf8()));
        case KDateTime::LocalZone:
        case KDateTime::ClockTime:
            return QDateTime(dt.date(), QTime(), Qt::LocalTime);
        case KDateTime::Invalid:
            return dt;
        }
        out.setTime({});
        return out;
    } else {
        switch (spec.type()) {
        case KDateTime::UTC:
            return dt.toTimeSpec(Qt::UTC);
        case KDateTime::OffsetFromUTC:
            return dt.toOffsetFromUtc(spec.utcOffset());
        case KDateTime::TimeZone:
            return dt.toTimeZone(QTimeZone(spec.timeZone().name().toUtf8()));
        case KDateTime::LocalZone:
        case KDateTime::ClockTime:
            return dt.toLocalTime();
        case KDateTime::Invalid:
            return dt;
        }
    }

    Q_UNREACHABLE();
    return {};
}
