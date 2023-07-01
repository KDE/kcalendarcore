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
