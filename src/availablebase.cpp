/*
  This file is part of the kcalcore library.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the AvailableBase class.

*/

#include "availablebase.h"
#include "availablebase_p.h"
#include "customproperties.h"
#include "kcalendarcore_debug.h"

#include <QDateTime>

using namespace KCalendarCore;

//@cond PRIVATE
void AvailableBase::AvailableBasePrivate::init(const AvailableBase::AvailableBasePrivate &other)
{
    mUid = other.mUid;
    mDtStamp = other.mDtStamp;
}

//@endcond

AvailableBase::AvailableBase(AvailableBasePrivate *p)
    : d(p)
{
    // TODO setUid call here
}

AvailableBase::AvailableBase(const AvailableBase &i, AvailableBasePrivate *p)
    : CustomProperties(i)
    , d(p)
{
}

AvailableBase::~AvailableBase()
{
}

AvailableBase &AvailableBase::operator=(const AvailableBase &other)
{
    Q_ASSUME(type() == other.type());

    // this will call derived class's assign
    AvailableBase &ret = assign(other);

    return ret;
}

AvailableBase &AvailableBase::assign(const AvailableBase &other)
{
    CustomProperties::operator=(other);
    d->init(*other.d);
    return *this;
}

bool AvailableBase::operator==(const AvailableBase &i2) const
{
    if (i2.type() != type()) {
        return false;
    } else {
        // equals is called from derived class
        return equals(i2);
    }
}

// TODO understand equals and ==
bool AvailableBase::equals(const AvailableBase &availableBase) const
{
    if (availableBase.type() != type()) {
        return false;
    }
    return true;
}

// AvailableBase::AvailableBase()
//{
// }
//
// AvailableBase::~AvailableBase() = default;

void AvailableBase::setUid(const QString &uid)
//@endcond
{
    if (d->mUid != uid) {
        d->mUid = uid;
    }
}

QString AvailableBase::uid() const
{
    return d->mUid;
}

void AvailableBase::setDtStamp(const QDateTime &dt)
{
    d->mDtStamp = dt;
}

QDateTime AvailableBase::dtStamp() const
{
    return d->mDtStamp;
}

void AvailableBase::setDtStart(const QDateTime &dt)
{
    d->mDtStart = dt;
}

QDateTime AvailableBase::dtStart() const
{
    return d->mDtStart;
}

void AvailableBase::setDtEnd(const QDateTime &dt)
{
    d->mDtEnd = dt;
}

QDateTime AvailableBase::dtEnd() const
{
    return d->mDtEnd;
}

void AvailableBase::setSummary(const QString &summary)
{
    d->mSummary = summary;
}

QString AvailableBase::summary() const
{
    return d->mSummary;
}

bool AvailableBase::summaryIsRich() const
{
    return d->mSummaryIsRich; // TODO
}

void AvailableBase::setLocation(const QString &location, bool isRich)
{
    if (d->mLocation != location || d->mLocationIsRich != isRich) {
        d->mLocation = location;
        d->mLocationIsRich = isRich;
    }
}

QString AvailableBase::location() const
{
    return d->mLocation;
}
