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

#include "kcalendarcore_debug.h"

#include <QDateTime>

using namespace KCalendarCore;

//@cond PRIVATE
class Q_DECL_HIDDEN KCalendarCore::AvailableBase::AvailableBasePrivate
{
public:
    AvailableBasePrivate() = default;

    AvailableBasePrivate(const AvailableBasePrivate &other)
    {
        init(other);
    }

    void init(const AvailableBasePrivate &other);

    virtual ~AvailableBasePrivate() = default;

    QString mUid; // unique id
    QDateTime mDtStamp;
    QDateTime mDtStart; // start time
    QDateTime mDtEnd; // end time -> only one of dtEnd/duration allowed, so opting to always saving dtEnd.

    QString mSummary;
    bool mSummaryIsRich = false; // summary string is richtext.

    bool mLocationIsRich = false; // location string is richtext.
    QString mLocation; // location string
};

void AvailableBase::AvailableBasePrivate::init(const AvailableBase::AvailableBasePrivate &other)
{
    mUid = other.mUid;
    mDtStamp = other.mDtStamp;
}

//@endcond

AvailableBase::AvailableBase(const AvailableBase &i)
    : CustomProperties(i)
    , d(new KCalendarCore::AvailableBase::AvailableBasePrivate(*i.d))
{
    // TODO    setUid(CalFormat::createUniqueId());
}

AvailableBase::~AvailableBase()
{
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
