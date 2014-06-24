/*
  This file is part of the kcalcore library.

  Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>
  Copyright (c) 2007 David Jarvie <software@astrojar.org.uk>

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
/**
  @file
  This file is part of the API for handling calendar data and
  defines the Period class.

  @brief
  Represents a period of time.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/

#include "period.h"

#include <KDateTime>

#include <QtCore/QHash>

using namespace KCalCore;

//@cond PRIVATE
class KCalCore::Period::Private
{
public:
    Private() : mHasDuration(false), mDailyDuration(false)  {}
    Private(const KDateTime &start, const KDateTime &end, bool hasDuration)
        : mStart(start),
          mEnd(end),
          mHasDuration(hasDuration),
          mDailyDuration(false)
    {}
    KDateTime mStart;    // period starting date/time
    KDateTime mEnd;      // period ending date/time
    bool mHasDuration;   // does period have a duration?
    bool mDailyDuration; // duration is defined as number of days, not seconds
};
//@endcond

Period::Period() : d(new KCalCore::Period::Private())
{
}

Period::Period(const KDateTime &start, const KDateTime &end)
    : d(new KCalCore::Period::Private(start, end, false))
{
}

Period::Period(const KDateTime &start, const Duration &duration)
    : d(new KCalCore::Period::Private(start, duration.end(start), true))
{
    d->mDailyDuration = duration.isDaily();
}

Period::Period(const Period &period)
    : d(new KCalCore::Period::Private(*period.d))
{
}

Period::~Period()
{
    delete d;
}

bool Period::operator<(const Period &other) const
{
    return d->mStart < other.d->mStart;
}

bool Period::operator==(const Period &other) const
{
    return
        ((d->mStart == other.d->mStart) ||
         (!d->mStart.isValid() && !other.d->mStart.isValid())) &&
        ((d->mEnd == other.d->mEnd) ||
         (!d->mEnd.isValid() && !other.d->mEnd.isValid())) &&
        d->mHasDuration == other.d->mHasDuration;
}

Period &Period::operator=(const Period &other)
{
    // check for self assignment
    if (&other == this) {
        return *this;
    }

    *d = *other.d;
    return *this;
}

KDateTime Period::start() const
{
    return d->mStart;
}

KDateTime Period::end() const
{
    return d->mEnd;
}

Duration Period::duration() const
{
    if (d->mHasDuration) {
        return Duration(d->mStart, d->mEnd,
                        d->mDailyDuration ? Duration::Days : Duration::Seconds);
    } else {
        return Duration(d->mStart, d->mEnd);
    }
}

Duration Period::duration(Duration::Type type) const
{
    return Duration(d->mStart, d->mEnd, type);
}

bool Period::hasDuration() const
{
    return d->mHasDuration;
}

void Period::shiftTimes(const KDateTime::Spec &oldSpec,
                        const KDateTime::Spec &newSpec)
{
    if (oldSpec.isValid() && newSpec.isValid() && oldSpec != newSpec) {
        d->mStart = d->mStart.toTimeSpec(oldSpec);
        d->mStart.setTimeSpec(newSpec);
        d->mEnd = d->mEnd.toTimeSpec(oldSpec);
        d->mEnd.setTimeSpec(newSpec);
    }
}

QDataStream &KCalCore::operator<<(QDataStream &stream, const KCalCore::Period &period)
{
    return stream << period.d->mStart
           << period.d->mEnd
           << period.d->mDailyDuration
           << period.d->mHasDuration;
}

QDataStream &KCalCore::operator>>(QDataStream &stream, KCalCore::Period &period)
{
    stream >> period.d->mStart
           >> period.d->mEnd
           >> period.d->mDailyDuration
           >> period.d->mHasDuration;
    return stream;
}

uint qHash(const KCalCore::Period &key)
{
    QString strToHash = key.start().toString();
    if (key.hasDuration()) {
        strToHash += key.duration();
    } else {
        strToHash += key.end().toString();
    }
    return qHash(strToHash);
}

