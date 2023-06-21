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
    mutable QString mUid;

public:
    void setUid(const QString &uid);

    QString uid() const;
};

void Available::Private::setUid(const QString &uid)
{
    mUid = uid;
}

QString Available::Private::uid() const
{
    return mUid;
}

//@endcond
Available::Available()
    : d(new Available::Private)
{
}

Available::~Available() = default;

void Available::setUid(const QString &uid)
{
    d->setUid(uid);
}

QString Available::uid() const
{
    return d->uid();
}
