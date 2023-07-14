#include "available.h"
#include "availablebase.h"
#include "availablebase_p.h"
#include "incidencebase.h" // for identical()

#include "kcalendarcore_debug.h"

using namespace KCalendarCore;

/**
  Private class that helps to provide binary compatibility between releases.
  u@internal
*/
//@cond PRIVATE
class Q_DECL_HIDDEN KCalendarCore::Available::Private : public AvailableBasePrivate
{
public:
    mutable Recurrence *mRecurrence = nullptr; // recurrence

    void init(const Private &other);
};

void Available::Private::init(const Available::Private &other)
{
    mRecurrence = other.mRecurrence;
}

//@endcond

Available::Available()
    : AvailableBase(new Private())
{
    d = std::make_unique<Private>();
}

Available::~Available() = default;

Available::Available(const Available &other)
    : AvailableBase(other.d.get())
{
}

bool Available::equals(const AvailableBase &available) const
{
    // If not same type, this returns false.
    if (!AvailableBase::equals(available)) {
        return false;
    } else {
        const Available *t = static_cast<const Available *>(&available);
        return identical(dtStart(), t->dtStart()) /* TODO fill rest here */
            && identical(dtEnd(), t->dtEnd()) && identical(dtStamp(), t->dtStamp()) && duration() == t->duration()
            && identical(lastModified(), t->lastModified()) && summary() == t->summary() && location() == t->location() && description() == t->description();
    }
}

AvailableBase &Available::assign(const AvailableBase &other)
{
    if (&other != this) {
        // runs init of AvailableBase and copies Base class members.
        AvailableBase::assign(other);

        // runs init of Derived class and copies the rest of the members.
        // TODO fix below. We cannot get d pointer from Available object to call setRecurrence method instead of passing private pointer?
        // const Available* t = static_cast<const Available*>(&other);
        // d->init(*(t->d_func()));
    }
    return *this;
}

AvailableBase::AvailableType Available::type() const
{
    return TypeAvailable;
}

Recurrence *Available::recurrence() const
{
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
