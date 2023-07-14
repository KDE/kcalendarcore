/*
  This file is part of the kcalendarcore library.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the AvailableBase class.

*/

#include "availablebase.h"
#include "availablebase_p.h"
#include "customproperties.h"
#include "kcalendarcore_debug.h"

#include "calformat.h"
#include <QDateTime>
#include <QTextDocument> // for .toHtmlEscaped() and Qt::mightBeRichText()

using namespace KCalendarCore;

//@cond PRIVATE
void AvailableBase::AvailableBasePrivate::init(const AvailableBase::AvailableBasePrivate &other)
{
    mUid = other.mUid;
    mCreated = other.mCreated;
    mDtStamp = other.mDtStamp;
    mDtStart = other.mDtStart;
    mDtEnd = other.mDtEnd;
    mDuration = other.mDuration;
    mLastModified = other.mLastModified;

    mSummary = other.mSummary;
    mSummaryIsRich = other.mSummaryIsRich;

    mLocationIsRich = other.mLocationIsRich;
    mLocation = other.mLocation;

    mDescription = other.mDescription;
    mDescriptionIsRich = other.mDescriptionIsRich;

    mCategories = other.mCategories;

    mComments = other.mComments;

    mContacts = other.mContacts;
}
//@endcond

AvailableBase::AvailableBase(AvailableBasePrivate *p)
    : d(p)
{
    setUid(CalFormat::createUniqueId());
}

AvailableBase::AvailableBase(const AvailableBase &i, AvailableBasePrivate *p)
    : CustomProperties(i)
    , d(p)
{
}

AvailableBase::~AvailableBase()
{
}

AvailableBase &AvailableBase::operator=(const AvailableBase &other)
{
    Q_ASSUME(type() == other.type());

    // this will call derived class's assign
    AvailableBase &ret = assign(other);

    return ret;
}

AvailableBase &AvailableBase::assign(const AvailableBase &other)
{
    CustomProperties::operator=(other);
    d->init(*other.d);
    return *this;
}

bool AvailableBase::operator==(const AvailableBase &i2) const
{
    if (i2.type() != type()) {
        return false;
    } else {
        // equals is called from derived class
        return equals(i2);
    }
}

// TODO understand equals and ==
bool AvailableBase::equals(const AvailableBase &availableBase) const
{
    if (availableBase.type() != type()) {
        return false;
    }
    return true;
}

void AvailableBase::setUid(const QString &uid)
{
    if (d->mUid != uid) {
        d->mUid = uid;
    }
}

QString AvailableBase::uid() const
{
    return d->mUid;
}

void AvailableBase::setCreated(const QDateTime &created)
{
    d->mCreated = created.toUTC();
    const auto ct = d->mCreated.time();
    // Remove milliseconds
    d->mCreated.setTime(QTime(ct.hour(), ct.minute(), ct.second()));
}

QDateTime AvailableBase::created() const
{
    return d->mCreated;
}

void AvailableBase::setDtStamp(const QDateTime &dt)
{
    d->mDtStamp = dt;
}

QDateTime AvailableBase::dtStamp() const
{
    return d->mDtStamp;
}

void AvailableBase::setDtStart(const QDateTime &dt)
{
    d->mDtStart = dt;
}

QDateTime AvailableBase::dtStart() const
{
    return d->mDtStart;
}

void AvailableBase::setDtEnd(const QDateTime &dt)
{
    d->mDtEnd = dt;
}

QDateTime AvailableBase::dtEnd() const
{
    return d->mDtEnd;
}

void AvailableBase::setDuration(const Duration &duration)
{
    d->mDuration = duration;
}

Duration AvailableBase::duration() const
{
    return d->mDuration;
}

void AvailableBase::setLastModified(const QDateTime &lm)
{
    // Convert to UTC and remove milliseconds part.
    QDateTime current = lm.toUTC();
    QTime t = current.time();
    t.setHMS(t.hour(), t.minute(), t.second(), 0);
    current.setTime(t);

    d->mLastModified = current;
}

QDateTime AvailableBase::lastModified() const
{
    return d->mLastModified;
}

void AvailableBase::setSummary(const QString &summary)
{
    d->mSummary = summary;
}

QString AvailableBase::summary() const
{
    return d->mSummary;
}

bool AvailableBase::summaryIsRich() const
{
    return d->mSummaryIsRich; // TODO
}

void AvailableBase::setLocation(const QString &location, bool isRich)
{
    if (d->mLocation != location || d->mLocationIsRich != isRich) {
        d->mLocation = location;
        d->mLocationIsRich = isRich;
    }
}

QString AvailableBase::location() const
{
    return d->mLocation;
}

void AvailableBase::setDescription(const QString &description, bool isRich)
{
    d->mDescription = description;
    d->mDescriptionIsRich = isRich;
}

void AvailableBase::setDescription(const QString &description)
{
    setDescription(description, Qt::mightBeRichText(description));
}

QString AvailableBase::description() const
{
    return d->mDescription;
}

QString AvailableBase::richDescription() const
{
    if (descriptionIsRich()) {
        return d->mDescription;
    } else {
        return d->mDescription.toHtmlEscaped().replace(QLatin1Char('\n'), QStringLiteral("<br/>"));
    }
}

bool AvailableBase::descriptionIsRich() const
{
    return d->mDescriptionIsRich;
}

void AvailableBase::setCategories(const QStringList &categories)
{
    d->mCategories = categories;
}

void AvailableBase::setCategories(const QString &catStr)
{
    d->mCategories.clear();

    if (catStr.isEmpty()) {
        return;
    }

    d->mCategories = catStr.split(QLatin1Char(','));

    for (auto &category : d->mCategories) {
        category = category.trimmed();
    }
}

QStringList AvailableBase::categories() const
{
    return d->mCategories;
}

QString AvailableBase::categoriesStr() const
{
    return d->mCategories.join(QLatin1Char(','));
}

void AvailableBase::addComment(const QString &comment)
{
    d->mComments += comment;
}

bool AvailableBase::removeComment(const QString &comment)
{
    auto it = std::find(d->mComments.begin(), d->mComments.end(), comment);
    bool found = it != d->mComments.end();
    if (found) {
        d->mComments.erase(it);
    }
    return found;
}

void AvailableBase::clearComments()
{
    d->mComments.clear();
}

QStringList AvailableBase::comments() const
{
    return d->mComments;
}

void AvailableBase::addContact(const QString &contact)
{
    if (!contact.isEmpty()) {
        d->mContacts += contact;
    }
}

bool AvailableBase::removeContact(const QString &contact)
{
    auto it = std::find(d->mContacts.begin(), d->mContacts.end(), contact);
    bool found = it != d->mContacts.end();
    if (found) {
        d->mContacts.erase(it);
    }
    return found;
}

void AvailableBase::clearContacts()
{
    d->mContacts.clear();
}

QStringList AvailableBase::contacts() const
{
    return d->mContacts;
}
