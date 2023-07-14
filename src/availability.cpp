#include "availability.h"
#include "availablebase.h"
#include "availablebase_p.h"
#include "incidence_p.h"

#include "kcalendarcore_debug.h"
#include <qvector.h>

using namespace KCalendarCore;

/**
  Private class that helps to provide binary compatibility between releases.
  u@internal
*/
//@cond PRIVATE
class Q_DECL_HIDDEN KCalendarCore::Availability::Private : public AvailableBasePrivate
{
public:
    QVector<Available> availables;
    Person mOrganizer; // person (owner)
    //    FreeBusyPeriod::FreeBusyType mType; TODO

    // TODO add init
};

//@endcond
Availability::Availability()
    : AvailableBase(new Private())
{
    d = std::make_unique<Private>();
}

Availability::~Availability() = default;

Availability::Availability(const Availability &other)
    : AvailableBase(other.d.get())
{
}

bool Availability::equals(const AvailableBase &availability) const
{
    // If not same type, this returns false.
    if (!AvailableBase::equals(availability)) {
        return false;
    } else {
        const Availability *t = static_cast<const Availability *>(&availability);
        return identical(dtStart(), t->dtStart()) /* TODO fill rest here */
            && identical(dtEnd(), t->dtEnd()) && identical(dtStamp(), t->dtStamp()) && duration() == t->duration()
            && identical(lastModified(), t->lastModified()) && summary() == t->summary() && location() == t->location() && description() == t->description();
    }
}

AvailableBase &Availability::assign(const AvailableBase &other)
{
    if (&other != this) {
        AvailableBase::assign(other);
        //        const auto o = static_cast<const Available*>(&other)->d();
        //        d->mDtEnd = o->mDtEnd; // TODO HERE
    }
    return *this;
}

AvailableBase::AvailableType Availability::type() const
{
    return TypeVAvailability;
}

void Availability::setOrganizer(const Person &organizer)
{
    d->mOrganizer = organizer;
}

void Availability::setOrganizer(const QString &o)
{
    QString mail(o);
    if (mail.startsWith(QLatin1String("MAILTO:"), Qt::CaseInsensitive)) {
        mail.remove(0, 7);
    }

    // split the string into full name plus email.
    const Person organizer = Person::fromFullName(mail);
    setOrganizer(organizer);
}

Person Availability::organizer() const
{
    return d->mOrganizer;
}

// void Availability::setBusyType(const FreeBusyPeriod::FreeBusyType &type)
//{
//     d->setBusyType(type);
// }
//
// FreeBusyPeriod::FreeBusyType Availability::busyType() const
//{
//     return d->busyType();
// }

void Availability::addNewAvailable(const Available &available)
{
    // TODO processing?
    // d->availables.push_back(available); //TODO facing error becaues = operator is private now.
}

QVector<Available> Availability::getAvailables() const
{
    return d->availables;
}
