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
#include <KSystemTimeZones>

#include <QDataStream>

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

KDateTime::Spec KCalCore::zoneToSpec(const QTimeZone& zone)
{
    if (zone == QTimeZone::utc())
        return KDateTime::UTC;
    if (zone == QTimeZone::systemTimeZone())
        return KDateTime::LocalZone;
    auto tz = KSystemTimeZones::zone(QString::fromLatin1(zone.id()));
    return tz;
}

QTimeZone KCalCore::specToZone(const KDateTime::Spec &spec)
{
    switch (spec.type()) {
        case KDateTime::LocalZone:
        case KDateTime::ClockTime:
            return QTimeZone::systemTimeZone();
        case KDateTime::UTC:
            return QTimeZone::utc();
        default:
            return QTimeZone(spec.timeZone().name().toUtf8());
    }

    return QTimeZone::systemTimeZone();
}

QDateTime KCalCore::k2q(const KDateTime &kdt)
{
    return QDateTime(kdt.date(), kdt.time(), specToZone(kdt.timeSpec()));
}

KDateTime KCalCore::q2k(const QDateTime &qdt)
{
    return KDateTime(qdt.date(), qdt.time(), zoneToSpec(qdt.timeZone()));

// To remain backwards compatible we need to (de)serialize QDateTime the way KDateTime
// was (de)serialized
void KCalCore::serializeQDateTimeAsKDateTime(QDataStream &out, const QDateTime &dt)
{
    out << dt.date() << dt.time();
    switch (dt.timeSpec()) {
    case Qt::UTC:
        out << static_cast<quint8>('u');
        break;
    case Qt::OffsetFromUTC:
        out << static_cast<quint8>('o') << dt.offsetFromUtc();
        break;
    case Qt::TimeZone:
        serializeQTimeZoneAsSpec(out, dt.timeZone());
        break;
    case Qt::LocalTime:
        out << static_cast<quint8>('c');
        break;
    }
    const bool isDateOnly = dt.date().isValid() && !dt.time().isValid();
    out << quint8(isDateOnly ? 0x01 : 0x00);
}

void KCalCore::deserializeKDateTimeAsQDateTime(QDataStream &in, QDateTime &dt)
{
    QDate date;
    QTime time;
    quint8 ts, flags;

    in >> date >> time >> ts;
    switch (static_cast<uchar>(ts)) {
    case 'u':
        dt = QDateTime(date, time, Qt::UTC);
        break;
    case 'o': {
        int offset;
        in >> offset;
        dt = QDateTime(date, time, Qt::OffsetFromUTC, offset);
        break;
    }
    case 'z': {
        QString tzid;
        in >> tzid;
        dt = QDateTime(date, time, QTimeZone(tzid.toUtf8()));
        break;
    }
    case 'c':
        dt = QDateTime(date, time, Qt::LocalTime);
        break;
    }

    // unused, we don't have a special handling for date-only QDateTime
    in >> flags;
}

void KCalCore::serializeQTimeZoneAsSpec(QDataStream &out, const QTimeZone& tz)
{
    out << static_cast<quint8>('z') << (tz.isValid() ? QString::fromUtf8(tz.id()) : QString());
}

void KCalCore::deserializeSpecAsQTimeZone(QDataStream &in, QTimeZone &tz)
{
    quint8 ts;
    in >> ts;
    switch (static_cast<uchar>(ts)) {
    case 'u':
        tz = QTimeZone::utc();
        return;
    case 'o': {
        int offset;
        in >> offset;
        tz = QTimeZone(offset);
        return;
    }
    case 'z': {
        QString tzid;
        in >> tzid;
        tz = QTimeZone(tzid.toUtf8());
        return;
    }
    case 'c':
        tz = QTimeZone::systemTimeZone();
        break;
    }

}

void KCalCore::serializeQDateTimeSortableList(QDataStream &out, const SortableList<QDateTime> &list)
{
    out << list.size();
    for (const auto &i : list) {
        serializeQDateTimeAsKDateTime(out, i);
    }
}

void KCalCore::deserializeQDateTimeSortableList(QDataStream& in, SortableList<QDateTime>& list)
{
    int size;
    in >> size;
    list.clear();
    list.reserve(size);
    for (int i = 0; i < size; ++i) {
        QDateTime dt;
        deserializeKDateTimeAsQDateTime(in, dt);
        list << dt;
    }
}
