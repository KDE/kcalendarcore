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
    QDateTime mDtStart; // start time
    QDateTime mDtEnd; // end time -> only one of dtEnd/duration allowed, so opting to always saving dtEnd.

    // TODO: This value isn't stored for other iCal elements. Only mentions are in icalformat_p.cpp mostly.
    // Do we need to add it?
    QDateTime mDtStamp;

    mutable QString mUid;
    QString mSummary;

public:
    void setDtStart(const QDateTime &dt);

    QDateTime dtStart() const;

    void setDtEnd(const QDateTime &dt);

    QDateTime dtEnd() const;

    void setDtStamp(const QDateTime &dt);

    QDateTime dtStamp() const;

    void setUid(const QString &uid);

    QString uid() const;

    void setSummary(const QString &summary);

    QString summary() const;
};

void Available::Private::setDtStart(const QDateTime &dt)
{
    mDtStart = dt;
}

QDateTime Available::Private::dtStart() const
{
    return mDtStart;
}

void Available::Private::setDtEnd(const QDateTime &dt)
{
    mDtEnd = dt;
}

QDateTime Available::Private::dtEnd() const
{
    return mDtEnd;
}

void Available::Private::setDtStamp(const QDateTime &dt)
{
    mDtStamp = dt;
}

QDateTime Available::Private::dtStamp() const
{
    return mDtStamp;
}

void Available::Private::setUid(const QString &uid)
{
    mUid = uid;
}

QString Available::Private::uid() const
{
    return mUid;
}

void Available::Private::setSummary(const QString &summary)
{
    mSummary = summary;
}

QString Available::Private::summary() const
{
    return mSummary;
}

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
    d->setUid(uid);
}

QString Available::uid() const
{
    return d->uid();
}

void Available::setDtStart(const QDateTime &dt)
{
    d->setDtStart(dt);
}

QDateTime Available::dtStart() const
{
    return d->dtStart();
}

void Available::setDtEnd(const QDateTime &dt)
{
    d->setDtEnd(dt);
}

QDateTime Available::dtEnd() const
{
    return d->dtEnd();
}

void Available::setDtStamp(const QDateTime &dt)
{
    d->setDtStamp(dt);
}

QDateTime Available::dtStamp() const
{
    return d->dtStamp();
}

void Available::setSummary(const QString &summary)
{
    return d->setSummary(summary);
}

QString Available::summary() const
{
    return d->summary();
}
