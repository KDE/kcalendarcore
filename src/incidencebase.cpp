/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001,2004 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  SPDX-FileCopyrightText: 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
  SPDX-FileContributor: Alvaro Manera <alvaro.manera@nokia.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the IncidenceBase class.

  @brief
  An abstract base class that provides a common base for all calendar incidence
  classes.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
*/

#include "incidencebase.h"
#include "incidencebase_p.h"
#include "calformat.h"
#include "utils_p.h"
#include "visitor.h"

#include "kcalendarcore_debug.h"
#include <QTime>

#include <QStringList>

#define KCALCORE_MAGIC_NUMBER 0xCA1C012E
#define KCALCORE_SERIALIZATION_VERSION 1

using namespace KCalendarCore;

//@cond PRIVATE
void IncidenceBasePrivate::init(const IncidenceBasePrivate &other)
{
    mLastModified = other.mLastModified;
    mDtStart = other.mDtStart;
    mOrganizer = other.mOrganizer;
    mUid = other.mUid;
    mDuration = other.mDuration;
    mAllDay = other.mAllDay;
    mHasDuration = other.mHasDuration;

    mComments = other.mComments;
    mContacts = other.mContacts;

    mAttendees = other.mAttendees;
    mAttendees.reserve(other.mAttendees.count());
    mUrl = other.mUrl;
}

//@endcond

IncidenceBase::IncidenceBase()
    : d(new KCalendarCore::IncidenceBasePrivate)
{
    mReadOnly = false;
    setUid(CalFormat::createUniqueId());
}

IncidenceBase::IncidenceBase(const IncidenceBase &i)
    : CustomProperties(i)
    , d(new KCalendarCore::IncidenceBasePrivate(*i.d))
{
    mReadOnly = i.mReadOnly;
}

IncidenceBase::~IncidenceBase()
{
    delete d;
}

IncidenceBase &IncidenceBase::operator=(const IncidenceBase &other)
{
    Q_ASSERT(type() == other.type());

    startUpdates();

    // assign is virtual, will call the derived class's
    IncidenceBase &ret = assign(other);
    endUpdates();
    return ret;
}

IncidenceBase &IncidenceBase::assign(const IncidenceBase &other)
{
    CustomProperties::operator=(other);
    d->init(*other.d);
    mReadOnly = other.mReadOnly;
    d->mDirtyFields.clear();
    d->mDirtyFields.insert(FieldUnknown);
    return *this;
}

bool IncidenceBase::operator==(const IncidenceBase &i2) const
{
    if (i2.type() != type()) {
        return false;
    } else {
        // equals is virtual, so here we're calling the derived class method
        return equals(i2);
    }
}

bool IncidenceBase::operator!=(const IncidenceBase &i2) const
{
    return !operator==(i2);
}

bool IncidenceBase::equals(const IncidenceBase &other) const
{
    if (attendees().count() != other.attendees().count()) {
        // qCDebug(KCALCORE_LOG) << "Attendee count is different";
        return false;
    }

    // TODO Does the order of attendees in the list really matter?
    // Please delete this comment if you know it's ok, kthx
    const Attendee::List list = attendees();
    const Attendee::List otherList = other.attendees();

    if (list.size() != otherList.size()) {
        return false;
    }

    auto [it1, it2] = std::mismatch(list.cbegin(), list.cend(), otherList.cbegin(), otherList.cend());

    // Checking the iterator from one list only, since we've already checked
    // they are the same size
    if (it1 != list.cend()) {
        // qCDebug(KCALCORE_LOG) << "Attendees are different";
        return false;
    }

    if (!CustomProperties::operator==(other)) {
        // qCDebug(KCALCORE_LOG) << "Properties are different";
        return false;
    }

    // Don't compare lastModified, otherwise the operator is not
    // of much use. We are not comparing for identity, after all.
    // no need to compare mObserver

    bool a = ((dtStart() == other.dtStart()) || (!dtStart().isValid() && !other.dtStart().isValid()));
    bool b = organizer() == other.organizer();
    bool c = uid() == other.uid();
    bool d = allDay() == other.allDay();
    bool e = duration() == other.duration();
    bool f = hasDuration() == other.hasDuration();
    bool g = url() == other.url();

    // qCDebug(KCALCORE_LOG) << a << b << c << d << e << f << g;
    return a && b && c && d && e && f && g;
}

bool IncidenceBase::accept(Visitor &v, const IncidenceBase::Ptr &incidence)
{
    Q_UNUSED(v);
    Q_UNUSED(incidence);
    return false;
}

void IncidenceBase::setUid(const QString &uid)
{
    if (d->mUid != uid) {
        update();
        d->mUid = uid;
        d->mDirtyFields.insert(FieldUid);
        updated();
    }
}

QString IncidenceBase::uid() const
{
    return d->mUid;
}

void IncidenceBase::setLastModified(const QDateTime &lm)
{
    // DON'T! updated() because we call this from
    // Calendar::updateEvent().

    d->mDirtyFields.insert(FieldLastModified);

    // Convert to UTC and remove milliseconds part.
    QDateTime current = lm.toUTC();
    QTime t = current.time();
    t.setHMS(t.hour(), t.minute(), t.second(), 0);
    current.setTime(t);

    d->mLastModified = current;
}

QDateTime IncidenceBase::lastModified() const
{
    return d->mLastModified;
}

void IncidenceBase::setOrganizer(const Person &organizer)
{
    update();
    // we don't check for readonly here, because it is
    // possible that by setting the organizer we are changing
    // the event's readonly status...
    d->mOrganizer = organizer;

    d->mDirtyFields.insert(FieldOrganizer);

    updated();
}

void IncidenceBase::setOrganizer(const QString &o)
{
    QString mail(o);
    if (mail.startsWith(QLatin1String("MAILTO:"), Qt::CaseInsensitive)) {
        mail.remove(0, 7);
    }

    // split the string into full name plus email.
    const Person organizer = Person::fromFullName(mail);
    setOrganizer(organizer);
}

Person IncidenceBase::organizer() const
{
    return d->mOrganizer;
}

void IncidenceBase::setReadOnly(bool readOnly)
{
    mReadOnly = readOnly;
}

bool IncidenceBase::isReadOnly() const
{
    return mReadOnly;
}

void IncidenceBase::setDtStart(const QDateTime &dtStart)
{
    //  if ( mReadOnly ) return;

    if (!dtStart.isValid() && type() != IncidenceBase::TypeTodo) {
        qCWarning(KCALCORE_LOG) << "Invalid dtStart";
    }

    if (d->mDtStart != dtStart || d->mDtStart.timeSpec() != dtStart.timeSpec()) {
        update();
        d->mDtStart = dtStart;
        d->mDirtyFields.insert(FieldDtStart);
        updated();
    }
}

QDateTime IncidenceBase::dtStart() const
{
    return d->mDtStart;
}

bool IncidenceBase::allDay() const
{
    return d->mAllDay;
}

void IncidenceBase::setAllDay(bool f)
{
    if (mReadOnly || f == d->mAllDay) {
        return;
    }
    update();
    d->mAllDay = f;
    if (d->mDtStart.isValid()) {
        d->mDirtyFields.insert(FieldDtStart);
    }
    updated();
}

void IncidenceBase::shiftTimes(const QTimeZone &oldZone, const QTimeZone &newZone)
{
    update();
    d->mDtStart = d->mDtStart.toTimeZone(oldZone);
    d->mDtStart.setTimeZone(newZone);
    d->mDirtyFields.insert(FieldDtStart);
    updated();
}

void IncidenceBase::addComment(const QString &comment)
{
    update();
    d->mComments += comment;
    d->mDirtyFields.insert(FieldComment);
    updated();
}

bool IncidenceBase::removeComment(const QString &comment)
{
    auto it = std::find(d->mComments.begin(), d->mComments.end(), comment);
    bool found = it != d->mComments.end();
    if (found) {
        update();
        d->mComments.erase(it);
        d->mDirtyFields.insert(FieldComment);
        updated();
    }
    return found;
}

void IncidenceBase::clearComments()
{
    update();
    d->mDirtyFields.insert(FieldComment);
    d->mComments.clear();
    updated();
}

QStringList IncidenceBase::comments() const
{
    return d->mComments;
}

void IncidenceBase::addContact(const QString &contact)
{
    if (!contact.isEmpty()) {
        update();
        d->mContacts += contact;
        d->mDirtyFields.insert(FieldContact);
        updated();
    }
}

bool IncidenceBase::removeContact(const QString &contact)
{
    auto it = std::find(d->mContacts.begin(), d->mContacts.end(), contact);
    bool found = it != d->mContacts.end();
    if (found) {
        update();
        d->mContacts.erase(it);
        d->mDirtyFields.insert(FieldContact);
        updated();
    }
    return found;
}

void IncidenceBase::clearContacts()
{
    update();
    d->mDirtyFields.insert(FieldContact);
    d->mContacts.clear();
    updated();
}

QStringList IncidenceBase::contacts() const
{
    return d->mContacts;
}

void IncidenceBase::addAttendee(const Attendee &a, bool doupdate)
{
    if (a.isNull() || mReadOnly) {
        return;
    }
    Q_ASSERT(!a.uid().isEmpty());

    if (doupdate) {
        update();
    }

    d->mAttendees.append(a);
    if (doupdate) {
        d->mDirtyFields.insert(FieldAttendees);
        updated();
    }
}

Attendee::List IncidenceBase::attendees() const
{
    return d->mAttendees;
}

int IncidenceBase::attendeeCount() const
{
    return d->mAttendees.count();
}

void IncidenceBase::setAttendees(const Attendee::List &attendees, bool doUpdate)
{
    if (mReadOnly) {
        return;
    }

    if (doUpdate) {
        update();
    }

    // don't simply assign, we need the logic in addAttendee here too
    clearAttendees();
    d->mAttendees.reserve(attendees.size());
    for (const auto &a : attendees) {
        addAttendee(a, false);
    }

    if (doUpdate) {
        d->mDirtyFields.insert(FieldAttendees);
        updated();
    }
}

void IncidenceBase::clearAttendees()
{
    if (mReadOnly) {
        return;
    }
    update();
    d->mDirtyFields.insert(FieldAttendees);
    d->mAttendees.clear();
    updated();
}

Attendee IncidenceBase::attendeeByMail(const QString &email) const
{
    auto it = std::find_if(d->mAttendees.cbegin(), d->mAttendees.cend(), [&email](const Attendee &att) {
        return att.email() == email;
    });

    return it != d->mAttendees.cend() ? *it : Attendee{};
}

Attendee IncidenceBase::attendeeByMails(const QStringList &emails, const QString &email) const
{
    QStringList mails = emails;
    if (!email.isEmpty()) {
        mails.append(email);
    }

    auto it = std::find_if(d->mAttendees.cbegin(), d->mAttendees.cend(), [&mails](const Attendee &a) {
        return mails.contains(a.email());
    });

    return it != d->mAttendees.cend() ? *it : Attendee{};
}

Attendee IncidenceBase::attendeeByUid(const QString &uid) const
{
    auto it = std::find_if(d->mAttendees.cbegin(), d->mAttendees.cend(), [&uid](const Attendee &a) {
        return a.uid() == uid;
    });
    return it != d->mAttendees.cend() ? *it : Attendee{};
}

void IncidenceBase::setDuration(const Duration &duration)
{
    update();
    d->mDuration = duration;
    setHasDuration(true);
    d->mDirtyFields.insert(FieldDuration);
    updated();
}

Duration IncidenceBase::duration() const
{
    return d->mDuration;
}

void IncidenceBase::setHasDuration(bool hasDuration)
{
    d->mHasDuration = hasDuration;
}

bool IncidenceBase::hasDuration() const
{
    return d->mHasDuration;
}

void IncidenceBase::setUrl(const QUrl &url)
{
    update();
    d->mDirtyFields.insert(FieldUrl);
    d->mUrl = url;
    updated();
}

QUrl IncidenceBase::url() const
{
    return d->mUrl;
}

void IncidenceBase::registerObserver(IncidenceBase::IncidenceObserver *observer)
{
    if (observer && !d->mObservers.contains(observer)) {
        d->mObservers.append(observer);
    }
}

void IncidenceBase::unRegisterObserver(IncidenceBase::IncidenceObserver *observer)
{
    d->mObservers.removeAll(observer);
}

void IncidenceBase::update()
{
    if (!d->mUpdateGroupLevel) {
        d->mUpdatedPending = true;
        const auto rid = recurrenceId();
        for (IncidenceObserver *o : qAsConst(d->mObservers)) {
            o->incidenceUpdate(uid(), rid);
        }
    }
}

void IncidenceBase::updated()
{
    if (d->mUpdateGroupLevel) {
        d->mUpdatedPending = true;
    } else {
        const auto rid = recurrenceId();
        for (IncidenceObserver *o : qAsConst(d->mObservers)) {
            o->incidenceUpdated(uid(), rid);
        }
    }
}

void IncidenceBase::startUpdates()
{
    update();
    ++d->mUpdateGroupLevel;
}

void IncidenceBase::endUpdates()
{
    if (d->mUpdateGroupLevel > 0) {
        if (--d->mUpdateGroupLevel == 0 && d->mUpdatedPending) {
            d->mUpdatedPending = false;
            updated();
        }
    }
}

void IncidenceBase::customPropertyUpdate()
{
    update();
}

void IncidenceBase::customPropertyUpdated()
{
    updated();
}

QDateTime IncidenceBase::recurrenceId() const
{
    return QDateTime();
}

void IncidenceBase::resetDirtyFields()
{
    d->mDirtyFields.clear();
}

QSet<IncidenceBase::Field> IncidenceBase::dirtyFields() const
{
    return d->mDirtyFields;
}

void IncidenceBase::setFieldDirty(IncidenceBase::Field field)
{
    d->mDirtyFields.insert(field);
}

QUrl IncidenceBase::uri() const
{
    return QUrl(QStringLiteral("urn:x-ical:") + uid());
}

void IncidenceBase::setDirtyFields(const QSet<IncidenceBase::Field> &dirtyFields)
{
    d->mDirtyFields = dirtyFields;
}

void IncidenceBase::serialize(QDataStream &out) const
{
    Q_UNUSED(out);
}

void IncidenceBase::deserialize(QDataStream &in)
{
    Q_UNUSED(in);
}

/** static */
quint32 IncidenceBase::magicSerializationIdentifier()
{
    return KCALCORE_MAGIC_NUMBER;
}

QDataStream &KCalendarCore::operator<<(QDataStream &out, const KCalendarCore::IncidenceBase::Ptr &i)
{
    if (!i) {
        return out;
    }

    out << static_cast<quint32>(KCALCORE_MAGIC_NUMBER); // Magic number to identify KCalendarCore data
    out << static_cast<quint32>(KCALCORE_SERIALIZATION_VERSION);
    out << static_cast<qint32>(i->type());

    out << *(static_cast<CustomProperties *>(i.data()));
    serializeQDateTimeAsKDateTime(out, i->d->mLastModified);
    serializeQDateTimeAsKDateTime(out, i->d->mDtStart);
    out << i->organizer() << i->d->mUid << i->d->mDuration << i->d->mAllDay << i->d->mHasDuration << i->d->mComments << i->d->mContacts
        << i->d->mAttendees.count() << i->d->mUrl;

    for (const Attendee &attendee : qAsConst(i->d->mAttendees)) {
        out << attendee;
    }

    // Serialize the sub-class data.
    i->serialize(out);

    return out;
}

QDataStream &KCalendarCore::operator>>(QDataStream &in, KCalendarCore::IncidenceBase::Ptr &i)
{
    if (!i) {
        return in;
    }

    qint32 attendeeCount;
    qint32 type;
    quint32 magic;
    quint32 version;

    in >> magic;

    if (magic != KCALCORE_MAGIC_NUMBER) {
        qCWarning(KCALCORE_LOG) << "Invalid magic on serialized data";
        return in;
    }

    in >> version;

    if (version > KCALCORE_MAGIC_NUMBER) {
        qCWarning(KCALCORE_LOG) << "Invalid version on serialized data";
        return in;
    }

    in >> type;

    in >> *(static_cast<CustomProperties *>(i.data()));
    deserializeKDateTimeAsQDateTime(in, i->d->mLastModified);
    deserializeKDateTimeAsQDateTime(in, i->d->mDtStart);
    in >> i->d->mOrganizer >> i->d->mUid >> i->d->mDuration >> i->d->mAllDay >> i->d->mHasDuration >> i->d->mComments >> i->d->mContacts >> attendeeCount
        >> i->d->mUrl;

    i->d->mAttendees.clear();
    i->d->mAttendees.reserve(attendeeCount);
    for (int it = 0; it < attendeeCount; it++) {
        Attendee attendee;
        in >> attendee;
        i->d->mAttendees.append(attendee);
    }

    // Deserialize the sub-class data.
    i->deserialize(in);

    return in;
}

IncidenceBase::IncidenceObserver::~IncidenceObserver()
{
}

QVariantList IncidenceBase::attendeesVariant() const
{
    QVariantList l;
    l.reserve(d->mAttendees.size());
    std::transform(d->mAttendees.begin(), d->mAttendees.end(), std::back_inserter(l), [](const Attendee &a) {
        return QVariant::fromValue(a);
    });
    return l;
}
