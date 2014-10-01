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
  defines the FreeBusyPeriod class.

  @brief
  Represents a period of time.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/

#include "freebusyperiod.h"

using namespace KCalCore;

//@cond PRIVATE
class KCalCore::FreeBusyPeriod::Private
{
public:
    Private():
       mType(Unknown)
    {}

    QString mSummary;
    QString mLocation;
    FreeBusyType mType;
};
//@endcond

FreeBusyPeriod::FreeBusyPeriod() : Period(), d(new KCalCore::FreeBusyPeriod::Private())
{
}

FreeBusyPeriod::FreeBusyPeriod(const KDateTime &start, const KDateTime &end)
    : Period(start, end), d(new KCalCore::FreeBusyPeriod::Private())
{
}

FreeBusyPeriod::FreeBusyPeriod(const KDateTime &start, const Duration &duration)
    : Period(start, duration), d(new KCalCore::FreeBusyPeriod::Private())
{
}

FreeBusyPeriod::FreeBusyPeriod(const FreeBusyPeriod &period)
    : Period(period), d(new KCalCore::FreeBusyPeriod::Private(*period.d))
{
}

FreeBusyPeriod::FreeBusyPeriod(const Period &period)
    : Period(period), d(new KCalCore::FreeBusyPeriod::Private())
{
}

FreeBusyPeriod::~FreeBusyPeriod()
{
    delete d;
}

FreeBusyPeriod &FreeBusyPeriod::operator=(const FreeBusyPeriod &other)
{
    // check for self assignment
    if (&other == this) {
        return *this;
    }

    Period::operator=(other);
    *d = *other.d;
    return *this;
}

QString FreeBusyPeriod::summary() const
{
    return d->mSummary;
}

void FreeBusyPeriod::setSummary(const QString &summary)
{
    d->mSummary = summary;
}

QString FreeBusyPeriod::location() const
{
    return d->mLocation;
}

void FreeBusyPeriod::setLocation(const QString &location)
{
    d->mLocation = location;
}

FreeBusyPeriod::FreeBusyType FreeBusyPeriod::type() const
{
    return d->mType;
}

void FreeBusyPeriod::setType(FreeBusyPeriod::FreeBusyType type)
{
    d->mType = type;
}


QDataStream &KCalCore::operator<<(QDataStream &stream, const KCalCore::FreeBusyPeriod &period)
{
    KCalCore::Period periodParent = static_cast<KCalCore::Period>(period);
    stream << periodParent;
    stream << period.summary() << period.location() << static_cast<int>(period.type());
    return stream;
}

QDataStream &KCalCore::operator>>(QDataStream &stream, FreeBusyPeriod &period)
{
    KCalCore::Period periodParent;
    QString summary, location;
    int type;

    stream >> periodParent >> summary >> location >> type;

    period = periodParent;
    period.setLocation(location);
    period.setSummary(summary);
    period.setType(static_cast<FreeBusyPeriod::FreeBusyType>(type));
    return stream;
}

