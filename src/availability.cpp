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

// TODO
void Availability::setDtEnd(const QDateTime &dtEnd)
{
}

QDateTime Availability::dtEnd() const
{
    return mDtEnd;
}

void Availability::setOrganizer(const Person &organizer)
{
    mOrganizer = organizer;
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
    return mOrganizer;
}

void Availability::setSummary(const QString &summary)
{
    mSummary = summary;
}

QString Availability::summary() const
{
    return mSummary;
}
