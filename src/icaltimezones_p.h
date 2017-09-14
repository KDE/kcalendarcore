/*
  This file is part of the kcalcore library.

  Copyright (c) 2005-2007 David Jarvie <djarvie@kde.org>

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
#ifndef KCALCORE_ICALTIMEZONES_H
#define KCALCORE_ICALTIMEZONES_H

#include "kcalcore_export.h"
#include "incidencebase.h"

#include <QTimeZone>
#include <QHash>
#include <QVector>

#ifndef ICALCOMPONENT_H
typedef struct icalcomponent_impl icalcomponent;
#endif
#ifndef ICALTIMEZONE_DEFINED
#define ICALTIMEZONE_DEFINED
typedef struct _icaltimezone  icaltimezone;
#endif

namespace KCalCore
{

class ICalTimeZonePhase;

class ICalTimeZonePhase
{
public:
    void dump();

    QSet<QByteArray> abbrevs;         // abbreviations of the phase
    int utcOffset = 0;                // UTC offset of the phase
    QVector<QDateTime> transitions;   // times on which transition into this phase occurs
};

class ICalTimeZone
{
public:
    void dump();

    QByteArray id;                    // original TZID
    QTimeZone qZone;                  // QTimeZone mapped from TZID
    ICalTimeZonePhase standard;       // standard time
    ICalTimeZonePhase daylight;       // DST time
};


class KCALCORE_EXPORT ICalTimeZoneCache
{
public:
    explicit ICalTimeZoneCache();

    void insert(const QByteArray &id, const ICalTimeZone &tz);

    QTimeZone tzForTime(const QDateTime &dt, const QByteArray &tzid) const;

private:
    QHash<QByteArray, ICalTimeZone> mCache;
};

using TimeZoneEarliestDate = QHash<QTimeZone, QDateTime>;

class KCALCORE_EXPORT ICalTimeZoneParser
{
public:
    explicit ICalTimeZoneParser(ICalTimeZoneCache *cache);

    void parse(icalcomponent *calendar);

    static void updateTzEarliestDate(const IncidenceBase::Ptr &incidence,
                                     TimeZoneEarliestDate *earliestDate);

    static icaltimezone *icaltimezoneFromQTimeZone(const QTimeZone &qtz,
                                                   const QDateTime &earliest);
    static QByteArray vcaltimezoneFromQTimeZone(const QTimeZone &qtz,
                                                const QDateTime &earliest);

private:
    static icalcomponent *icalcomponentFromQTimeZone(const QTimeZone &qtz,
                                                     const QDateTime &earliest);


    ICalTimeZone parseTimeZone(icalcomponent *zone);
    bool parsePhase(icalcomponent *c, ICalTimeZonePhase &phase);
    QTimeZone resolveICalTimeZone(const ICalTimeZone &icalZone);

    ICalTimeZoneCache *mCache;
};

} // namespace KCalCore


inline uint qHash(const QTimeZone &tz)
{
    return qHash(tz.id());
}

#endif
