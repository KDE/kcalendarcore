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
    mutable Recurrence *mRecurrence = nullptr; // recurrence
};

//@endcond

Available::~Available() = default;

Available::Available(const Available &other)
    : AvailableBase(other)
    , d(new Available::Private(*other.d))
{
}

Recurrence *Available::recurrence() const
{
    //    Q_D(const Available); TODO use of this?
    if (!d->mRecurrence) {
        d->mRecurrence = new Recurrence();

        // TODO below is hardcoded for now.
        d->mRecurrence->setStartDateTime(QDateTime(), true);
        d->mRecurrence->setAllDay(true);
        d->mRecurrence->setRecurReadOnly(false);
        //        d->mRecurrence->addObserver(const_cast<KCalendarCore::Available *>(this)); TODO revisit
    }

    return d->mRecurrence;
}
