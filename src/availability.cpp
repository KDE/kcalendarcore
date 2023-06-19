#include "availability.h"
#include "incidence_p.h"

#include "kcalendarcore_debug.h"

using namespace KCalendarCore;

void Availability::setUid(const QString &uid)
{
    mUid = uid;
}

QString Availability::uid() const
{
    return mUid;
}

void Availability::setDtStart(const QDateTime &dt)
{
    mDtStart = dt;
}

QDateTime Availability::dtStart() const
{
    return mDtStart;
}
