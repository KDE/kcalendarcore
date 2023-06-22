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
    QVector<Available> availables;
    QDateTime mDtEnd; // end time
    Person mOrganizer; // person (owner)
    QString mSummary; // summary string
    mutable QString mUid;

public:
    void setUid(const QString &uid);

    QString uid() const;

    void setDtEnd(const QDateTime &dt);

    QDateTime dtEnd() const;

    void setOrganizer(const Person &organizer);

    void setOrganizer(const QString &o);

    Person organizer() const;

    void setSummary(const QString &summary);

    QString summary() const;

    void addNewAvailable(const Available &available);
};

void Availability::Private::setUid(const QString &uid)
{
    mUid = uid;
}

QString Availability::Private::uid() const
{
    return mUid;
}

void Availability::Private::setDtEnd(const QDateTime &dt)
{
    mDtEnd = dt;
}

QDateTime Availability::Private::dtEnd() const
{
    return mDtEnd;
}

void Availability::Private::setOrganizer(const Person &organizer)
{
    mOrganizer = organizer;
}

void Availability::Private::setOrganizer(const QString &o)
{
    QString mail(o);
    if (mail.startsWith(QLatin1String("MAILTO:"), Qt::CaseInsensitive)) {
        mail.remove(0, 7);
    }

    // split the string into full name plus email.
    const Person organizer = Person::fromFullName(mail);
    setOrganizer(organizer);
}

Person Availability::Private::organizer() const
{
    return mOrganizer;
}

void Availability::Private::setSummary(const QString &summary)
{
    mSummary = summary;
}

QString Availability::Private::summary() const
{
    return mSummary;
}

void Availability::Private::addNewAvailable(const Available &available)
{
    availables.push_back(available);
}

//@endcond
Availability::Availability()
    : d(new Availability::Private)
{
}

Availability::~Availability() = default;

void Availability::setUid(const QString &uid)
{
    d->setUid(uid);
}

QString Availability::uid() const
{
    return d->uid();
}

void Availability::setDtEnd(const QDateTime &dt)
{
    d->setDtEnd(dt);
}

QDateTime Availability::dtEnd() const
{
    return d->dtEnd();
}

void Availability::setOrganizer(const Person &organizer)
{
    d->setOrganizer(organizer);
}

void Availability::setOrganizer(const QString &o)
{
    d->setOrganizer(o);
}

Person Availability::organizer() const
{
    return d->organizer();
}

void Availability::setSummary(const QString &summary)
{
    d->setSummary(summary);
}

QString Availability::summary() const
{
    return d->summary();
}

void Availability::addNewAvailable(const Available &available)
{
    // TODO processing?
    d->addNewAvailable(available);
}
