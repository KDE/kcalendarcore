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

KDateTime::Spec KCalCore::zoneToSpec(const QTimeZone& zone)
{
    if (!zone.isValid())
        return KDateTime::Invalid;
    if (zone == QTimeZone::utc())
        return KDateTime::UTC;
    if (zone == QTimeZone::systemTimeZone())
        return KDateTime::LocalZone;
    if (zone.id().startsWith("UTC")) {
        return KDateTime::Spec(KDateTime::OffsetFromUTC, zone.offsetFromUtc(QDateTime::currentDateTimeUtc()));
    } else {
        return KSystemTimeZones::zone(QString::fromLatin1(zone.id()));
    }
}

namespace {

QTimeZone resolveCustomTZ(const KTimeZone &ktz)
{
    // First, let's try Microsoft
    const auto msIana = QTimeZone::windowsIdToDefaultIanaId(ktz.name().toUtf8());
    if (!msIana.isEmpty()) {
        return QTimeZone(msIana);
    }

    int standardUtcOffset = 0;
    bool matched = false;
    const auto phases = ktz.phases();
    for (const auto &phase : phases) {
        if (!phase.isDst()) {
            standardUtcOffset = phase.utcOffset();
            matched = true;
            break;
        }
    }
    if (!matched) {
        standardUtcOffset = ktz.currentOffset(Qt::UTC);
    }

    const auto candidates = QTimeZone::availableTimeZoneIds(standardUtcOffset);
    QMap<int, QTimeZone> matchedCandidates;
    for (const auto &tzid : candidates) {
        const QTimeZone candidate(tzid);
        // This would be a fallback
        if (candidate.hasTransitions() != ktz.hasTransitions()) {
            matchedCandidates.insert(0, candidate);
            continue;
        }

        // Without transitions, we can't do any more precise matching, so just
        // accept this candidate and be done with it
        if (!candidate.hasTransitions() && !ktz.hasTransitions()) {
            return candidate;
        }

        // Calculate how many transitions this candidate shares with the ktz.
        // The candidate with the most matching transitions will win.
        const auto transitions = ktz.transitions(QDateTime(), QDateTime::currentDateTimeUtc());
        int matchedTransitions = 0;
        for (auto it = transitions.rbegin(), end = transitions.rend(); it != end; ++it) {
            const auto &transition = *it;
            const QTimeZone::OffsetDataList candidateTransitions = candidate.transitions(transition.time(), transition.time());
            if (candidateTransitions.isEmpty()) {
                continue;
            }
            ++matchedTransitions; // 1 point for a matching transition
            const auto candidateTransition = candidateTransitions[0];
            const auto abvs = transition.phase().abbreviations();
            for (const auto &abv : abvs) {
                if (candidateTransition.abbreviation == QString::fromUtf8(abv)) {
                    matchedTransitions += 1024; // lots of points for a transition with a matching abbreviation
                    break;
                }
            }
        }
        matchedCandidates.insert(matchedTransitions, candidate); 
    }

    if (!matchedCandidates.isEmpty()) {
        return matchedCandidates.value(matchedCandidates.lastKey());
    }

    return {};
}

}

QTimeZone KCalCore::specToZone(const KDateTime::Spec &spec)
{
    switch (spec.type()) {
        case KDateTime::Invalid:
            return QTimeZone();
        case KDateTime::LocalZone:
        case KDateTime::ClockTime:
            return QTimeZone::systemTimeZone();
        case KDateTime::UTC:
            return QTimeZone::utc();
        default: {
            auto tz = QTimeZone(spec.timeZone().name().toUtf8());
            if (!tz.isValid()) {
                tz = resolveCustomTZ(spec.timeZone());
                qDebug() << "Resolved" << spec.timeZone().name() << "to" << tz.id();
            }
            return tz;
        }
    }

    return QTimeZone::systemTimeZone();
}

QDateTime KCalCore::k2q(const KDateTime &kdt)
{
    if (kdt.isValid()) {
        return QDateTime(kdt.date(), kdt.time(), specToZone(kdt.timeSpec()));
    } else {
        return QDateTime();
    }
}

KDateTime KCalCore::q2k(const QDateTime &qdt, bool allDay)
{
    if (qdt.isValid()) {
        KDateTime kdt(qdt.date(), qdt.time(), zoneToSpec(qdt.timeZone()));
        kdt.setDateOnly(allDay && qdt.time() == QTime(0, 0, 0));
        return kdt;
    } else {
        return KDateTime();
    }
}

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
