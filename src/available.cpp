#include "available.h"
#include "incidence_p.h"

#include "kcalendarcore_debug.h"

using namespace KCalendarCore;

/**
  Private class that helps to provide binary compatibility between releases.
  u@internal
*/
//@cond PRIVATE
class Q_DECL_HIDDEN KCalendarCore::Available::Private
{
public:
    QDateTime mDtStart; // start time
    QDateTime mDtEnd; // end time -> only one of dtEnd/duration allowed, so opting to always saving dtEnd.

    // TODO: This value isn't stored for other iCal elements. Only mentions are in icalformat_p.cpp mostly.
    // Do we need to add it?
    QDateTime mDtStamp;

    mutable QString mUid;
    QString mSummary;
};

//@endcond
Available::Available()
    : d(new Available::Private)
{
}

Available::~Available() = default;

Available::Available(const Available &other)
    : CustomProperties(other)
    , d(new Available::Private(*other.d))
{
}

void Available::setUid(const QString &uid)
{
    d->mUid = uid;
}

QString Available::uid() const
{
    return d->mUid;
}

void Available::setDtStart(const QDateTime &dt)
{
    d->mDtStart = dt;
}

QDateTime Available::dtStart() const
{
    return d->mDtStart;
}

void Available::setDtEnd(const QDateTime &dt)
{
    d->mDtEnd = dt;
}

QDateTime Available::dtEnd() const
{
    return d->mDtEnd;
}

void Available::setDtStamp(const QDateTime &dt)
{
    d->mDtStamp = dt;
}

QDateTime Available::dtStamp() const
{
    return d->mDtStamp;
}

void Available::setSummary(const QString &summary)
{
    d->mSummary = summary;
}

QString Available::summary() const
{
    return d->mSummary;
}
