#include "availability.h"
#include "incidence_p.h"

#include "kcalendarcore_debug.h"

using namespace KCalendarCore;

/**
  Private class that helps to provide binary compatibility between releases.
  u@internal
*/
//@cond PRIVATE
class Q_DECL_HIDDEN KCalendarCore::Availability::Private
{
public:
    QVector<Available> availables;
    QDateTime mDtStart; // start time
    QDateTime mDtEnd; // end time
    Person mOrganizer; // person (owner)
    QString mSummary; // summary string
    mutable QString mUid;
    //    FreeBusyPeriod::FreeBusyType mType;
};

//@endcond
Availability::Availability()
    : d(new Availability::Private)
{
}

Availability::~Availability() = default;

void Availability::setUid(const QString &uid)
{
    d->mUid = uid;
}

QString Availability::uid() const
{
    return d->mUid;
}

void Availability::setDtStart(const QDateTime &dt)
{
    d->mDtStart = dt;
}

QDateTime Availability::dtStart() const
{
    return d->mDtStart;
}

void Availability::setDtEnd(const QDateTime &dt)
{
    d->mDtEnd = dt;
}

QDateTime Availability::dtEnd() const
{
    return d->mDtEnd;
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

void Availability::setSummary(const QString &summary)
{
    d->mSummary = summary;
}

QString Availability::summary() const
{
    return d->mSummary;
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
    // d->availables.push_back(available); TODO facing error becaues = operator is private now.
}

QVector<Available> Availability::getAvailables() const
{
    return d->availables;
}
