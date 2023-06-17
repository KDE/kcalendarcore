#include "available.h"
#include "incidence_p.h"

#include "kcalendarcore_debug.h"

using namespace KCalendarCore;

void Available::setUid(const QString &uid)
{
    mUid = uid;
}

QString Available::uid() const
{
    return mUid;
}
