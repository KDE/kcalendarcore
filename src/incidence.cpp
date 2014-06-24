/*
  This file is part of the kcalcore library.

  Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the Incidence class.

  @brief
  Provides the class common to non-FreeBusy (Events, To-dos, Journals)
  calendar components known as incidences.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
*/

#include "incidence.h"
#include "calformat.h"

#include <KMimeType>
#include <KTemporaryFile>

#include <QTextDocument> // for Qt::escape() and Qt::mightBeRichText()
#include <QStringList>
#include <QTime>

using namespace KCalCore;

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class KCalCore::Incidence::Private
{
public:
    Private()
        : mRevision(0),
          mDescriptionIsRich(false),
          mSummaryIsRich(false),
          mLocationIsRich(false),
          mRecurrence(0),
          mStatus(StatusNone),
          mSecrecy(SecrecyPublic),
          mPriority(0),
          mGeoLatitude(INVALID_LATLON),
          mGeoLongitude(INVALID_LATLON),
          mHasGeo(false),
          mThisAndFuture(false),
          mLocalOnly(false)
    {
    }

    Private(const Private &p)
        : mCreated(p.mCreated),
          mRevision(p.mRevision),
          mDescription(p.mDescription),
          mDescriptionIsRich(p.mDescriptionIsRich),
          mSummary(p.mSummary),
          mSummaryIsRich(p.mSummaryIsRich),
          mLocation(p.mLocation),
          mLocationIsRich(p.mLocationIsRich),
          mCategories(p.mCategories),
          mRecurrence(0),
          mResources(p.mResources),
          mStatus(p.mStatus),
          mStatusString(p.mStatusString),
          mSecrecy(p.mSecrecy),
          mPriority(p.mPriority),
          mSchedulingID(p.mSchedulingID),
          mRelatedToUid(p.mRelatedToUid),
          mGeoLatitude(p.mGeoLatitude),
          mGeoLongitude(p.mGeoLongitude),
          mHasGeo(p.mHasGeo),
          mRecurrenceId(p.mRecurrenceId),
          mThisAndFuture(p.mThisAndFuture),
          mLocalOnly(false)
    {
    }

    void clear()
    {
        mAlarms.clear();
        mAttachments.clear();
        delete mRecurrence;
        mRecurrence = 0;
    }

    void init(Incidence *dest, const Incidence &src)
    {
        mRevision = src.d->mRevision;
        mCreated = src.d->mCreated;
        mDescription = src.d->mDescription;
        mSummary = src.d->mSummary;
        mCategories = src.d->mCategories;
        mRelatedToUid = src.d->mRelatedToUid;
        mResources = src.d->mResources;
        mStatusString = src.d->mStatusString;
        mStatus = src.d->mStatus;
        mSecrecy = src.d->mSecrecy;
        mPriority = src.d->mPriority;
        mLocation = src.d->mLocation;
        mGeoLatitude = src.d->mGeoLatitude;
        mGeoLongitude = src.d->mGeoLongitude;
        mHasGeo = src.d->mHasGeo;
        mRecurrenceId = src.d->mRecurrenceId;
        mThisAndFuture = src.d->mThisAndFuture;
        mLocalOnly = src.d->mLocalOnly;

        // Alarms and Attachments are stored in ListBase<...>, which is a QValueList<...*>.
        // We need to really duplicate the objects stored therein, otherwise deleting
        // i will also delete all attachments from this object (setAutoDelete...)
        foreach(Alarm::Ptr alarm, src.d->mAlarms) {
            Alarm::Ptr b(new Alarm(*alarm.data()));
            b->setParent(dest);
            mAlarms.append(b);
        }

        foreach(Attachment::Ptr attachment, src.d->mAttachments) {
            Attachment::Ptr a(new Attachment(*attachment));
            mAttachments.append(a);
        }

        if (src.d->mRecurrence) {
            mRecurrence = new Recurrence(*(src.d->mRecurrence));
            mRecurrence->addObserver(dest);
        } else {
            mRecurrence = 0;
        }
    }

    KDateTime mCreated;                 // creation datetime
    int mRevision;                      // revision number

    QString mDescription;               // description string
    bool mDescriptionIsRich;            // description string is richtext.
    QString mSummary;                   // summary string
    bool mSummaryIsRich;                // summary string is richtext.
    QString mLocation;                  // location string
    bool mLocationIsRich;               // location string is richtext.
    QStringList mCategories;            // category list
    mutable Recurrence *mRecurrence;    // recurrence
    Attachment::List mAttachments;      // attachments list
    Alarm::List mAlarms;                // alarms list
    QStringList mResources;             // resources list (not calendar resources)
    Status mStatus;                     // status
    QString mStatusString;              // status string, for custom status
    Secrecy mSecrecy;                   // secrecy
    int mPriority;                      // priority: 1 = highest, 2 = less, etc.
    QString mSchedulingID;              // ID for scheduling mails

    QMap<RelType,QString> mRelatedToUid;// incidence uid this is related to, for each relType
    float mGeoLatitude;                 // Specifies latitude in decimal degrees
    float mGeoLongitude;                // Specifies longitude in decimal degrees
    bool mHasGeo;                       // if incidence has geo data
    QHash<Attachment::Ptr,QString> mTempFiles; // Temporary files for writing attachments to.
    KDateTime mRecurrenceId;            // recurrenceId
    bool mThisAndFuture;
    bool mLocalOnly;                    // allow changes that won't go to the server
};
//@endcond

Incidence::Incidence()
    : IncidenceBase(), d(new KCalCore::Incidence::Private)
{
    recreate();
    resetDirtyFields();
}

Incidence::Incidence(const Incidence &i)
    : IncidenceBase(i),
      Recurrence::RecurrenceObserver(),
      d(new KCalCore::Incidence::Private(*i.d))
{
    d->init(this, i);
    resetDirtyFields();
}

Incidence::~Incidence()
{
    // Alarm has a raw incidence pointer, so we must set it to 0
    // so Alarm doesn't use it after Incidence is destroyed
    foreach(Alarm::Ptr alarm, d->mAlarms) {
        alarm->setParent(0);
    }

    delete d->mRecurrence;
    delete d;
}

//@cond PRIVATE
// A string comparison that considers that null and empty are the same
static bool stringCompare(const QString &s1, const QString &s2)
{
    return (s1.isEmpty() && s2.isEmpty()) || (s1 == s2);
}

//@endcond
IncidenceBase &Incidence::assign(const IncidenceBase &other)
{
    if (&other != this) {
        d->clear();
        //TODO: should relations be cleared out, as in destructor???
        IncidenceBase::assign(other);
        const Incidence *i = static_cast<const Incidence*>(&other);
        d->init(this, *i);
    }

    return *this;
}

bool Incidence::equals(const IncidenceBase &incidence) const
{
    if (!IncidenceBase::equals(incidence)) {
        return false;
    }

    // If they weren't the same type IncidenceBase::equals would had returned false already
    const Incidence *i2 = static_cast<const Incidence *>(&incidence);

    if (alarms().count() != i2->alarms().count()) {
        return false;
    }

    Alarm::List::ConstIterator a1 = alarms().constBegin();
    Alarm::List::ConstIterator a1end = alarms().constEnd();
    Alarm::List::ConstIterator a2 = i2->alarms().constBegin();
    Alarm::List::ConstIterator a2end = i2->alarms().constEnd();
    for (; a1 != a1end && a2 != a2end; ++a1, ++a2) {
        if (**a1 == **a2) {
            continue;
        } else {
            return false;
        }
    }

    if (attachments().count() != i2->attachments().count()) {
        return false;
    }

    Attachment::List::ConstIterator att1 = attachments().constBegin();
    const Attachment::List::ConstIterator att1end = attachments().constEnd();
    Attachment::List::ConstIterator att2 = i2->attachments().constBegin();
    const Attachment::List::ConstIterator att2end = i2->attachments().constEnd();
    for (; att1 != att1end && att2 != att2end; ++att1, ++att2) {
        if (**att1 == **att2) {
            continue;
        } else {
            return false;
        }
    }

    bool recurrenceEqual = (d->mRecurrence == 0 && i2->d->mRecurrence == 0);
    if (!recurrenceEqual) {
        recurrence(); // create if doesn't exist
        i2->recurrence(); // create if doesn't exist
        recurrenceEqual = d->mRecurrence != 0 &&
                          i2->d->mRecurrence != 0 &&
                          *d->mRecurrence == *i2->d->mRecurrence;
    }

    return
        recurrenceEqual &&
        created() == i2->created() &&
        stringCompare(description(), i2->description()) &&
        stringCompare(summary(), i2->summary()) &&
        categories() == i2->categories() &&
        stringCompare(relatedTo(), i2->relatedTo()) &&
        resources() == i2->resources() &&
        d->mStatus == i2->d->mStatus &&
        (d->mStatus == StatusNone ||
         stringCompare(d->mStatusString, i2->d->mStatusString)) &&
        secrecy() == i2->secrecy() &&
        priority() == i2->priority() &&
        stringCompare(location(), i2->location()) &&
        stringCompare(schedulingID(), i2->schedulingID()) &&
        recurrenceId() == i2->recurrenceId() &&
        thisAndFuture() == i2->thisAndFuture();
}

QString Incidence::instanceIdentifier() const
{
    if (hasRecurrenceId()) {
        return uid() + recurrenceId().toString();
    }
    return uid();
}

void Incidence::recreate()
{
    const KDateTime nowUTC = KDateTime::currentUtcDateTime();
    setCreated(nowUTC);

    setSchedulingID(QString(), CalFormat::createUniqueId());
    setRevision(0);
    setLastModified(nowUTC);
}

void Incidence::setLastModified(const KDateTime &lm)
{
    if (!d->mLocalOnly) {
        IncidenceBase::setLastModified(lm);
    }
}

void Incidence::setReadOnly(bool readOnly)
{
    IncidenceBase::setReadOnly(readOnly);
    if (d->mRecurrence) {
        d->mRecurrence->setRecurReadOnly(readOnly);
    }
}

void Incidence::setLocalOnly(bool localOnly)
{
    if (mReadOnly) {
        return;
    }
    d->mLocalOnly = localOnly;
}

bool Incidence::localOnly() const
{
    return d->mLocalOnly;
}

void Incidence::setAllDay(bool allDay)
{
    if (mReadOnly) {
        return;
    }
    if (d->mRecurrence) {
        d->mRecurrence->setAllDay(allDay);
    }
    IncidenceBase::setAllDay(allDay);
}

void Incidence::setCreated(const KDateTime &created)
{
    if (mReadOnly || d->mLocalOnly) {
        return;
    }

    d->mCreated = created.toUtc();
    setFieldDirty(FieldCreated);

// FIXME: Shouldn't we call updated for the creation date, too?
//  updated();
}

KDateTime Incidence::created() const
{
    return d->mCreated;
}

void Incidence::setRevision(int rev)
{
    if (mReadOnly || d->mLocalOnly) {
        return;
    }

    update();

    d->mRevision = rev;
    setFieldDirty(FieldRevision);
    updated();
}

int Incidence::revision() const
{
    return d->mRevision;
}

void Incidence::setDtStart(const KDateTime &dt)
{
    if (d->mRecurrence) {
        d->mRecurrence->setStartDateTime(dt);
    }
    IncidenceBase::setDtStart(dt);
}

void Incidence::shiftTimes(const KDateTime::Spec &oldSpec,
                           const KDateTime::Spec &newSpec)
{
    IncidenceBase::shiftTimes(oldSpec, newSpec);
    if (d->mRecurrence) {
        d->mRecurrence->shiftTimes(oldSpec, newSpec);
    }
    for (int i = 0, end = d->mAlarms.count();  i < end;  ++i) {
        d->mAlarms[i]->shiftTimes(oldSpec, newSpec);
    }
}

void Incidence::setDescription(const QString &description, bool isRich)
{
    if (mReadOnly) {
        return;
    }
    update();
    d->mDescription = description;
    d->mDescriptionIsRich = isRich;
    setFieldDirty(FieldDescription);
    updated();
}

void Incidence::setDescription(const QString &description)
{
    setDescription(description, Qt::mightBeRichText(description));
}

QString Incidence::description() const
{
    return d->mDescription;
}

QString Incidence::richDescription() const
{
    if (descriptionIsRich()) {
        return d->mDescription;
    } else {
        return Qt::escape(d->mDescription).replace(QLatin1Char('\n'), QStringLiteral("<br/>"));
    }
}

bool Incidence::descriptionIsRich() const
{
    return d->mDescriptionIsRich;
}

void Incidence::setSummary(const QString &summary, bool isRich)
{
    if (mReadOnly) {
        return;
    }
    update();
    d->mSummary = summary;
    d->mSummaryIsRich = isRich;
    setFieldDirty(FieldSummary);
    updated();
}

void Incidence::setSummary(const QString &summary)
{
    setSummary(summary, Qt::mightBeRichText(summary));
}

QString Incidence::summary() const
{
    return d->mSummary;
}

QString Incidence::richSummary() const
{
    if (summaryIsRich()) {
        return d->mSummary;
    } else {
        return Qt::escape(d->mSummary).replace(QLatin1Char('\n'), QStringLiteral("<br/>"));
    }
}

bool Incidence::summaryIsRich() const
{
    return d->mSummaryIsRich;
}

void Incidence::setCategories(const QStringList &categories)
{
    if (mReadOnly) {
        return;
    }

    update();
    d->mCategories = categories;
    updated();
}

void Incidence::setCategories(const QString &catStr)
{
    if (mReadOnly) {
        return;
    }
    update();
    setFieldDirty(FieldCategories);

    d->mCategories.clear();

    if (catStr.isEmpty()) {
        updated();
        return;
    }

    d->mCategories = catStr.split(QLatin1Char(','));

    QStringList::Iterator it;
    for (it = d->mCategories.begin(); it != d->mCategories.end(); ++it) {
        *it = (*it).trimmed();
    }

    updated();
}

QStringList Incidence::categories() const
{
    return d->mCategories;
}

QString Incidence::categoriesStr() const
{
    return d->mCategories.join(QStringLiteral(","));
}

void Incidence::setRelatedTo(const QString &relatedToUid, RelType relType)
{
    // TODO: RFC says that an incidence can have more than one related-to field
    // even for the same relType.

    if (d->mRelatedToUid[relType] != relatedToUid) {
        update();
        d->mRelatedToUid[relType] = relatedToUid;
        setFieldDirty(FieldRelatedTo);
        updated();
    }
}

QString Incidence::relatedTo(RelType relType) const
{
    return d->mRelatedToUid.value(relType);
}

// %%%%%%%%%%%%  Recurrence-related methods %%%%%%%%%%%%%%%%%%%%

Recurrence *Incidence::recurrence() const
{
    if (!d->mRecurrence) {
        d->mRecurrence = new Recurrence();
        d->mRecurrence->setStartDateTime(dateTime(RoleRecurrenceStart));
        d->mRecurrence->setAllDay(allDay());
        d->mRecurrence->setRecurReadOnly(mReadOnly);
        d->mRecurrence->addObserver(const_cast<KCalCore::Incidence*>(this));
    }

    return d->mRecurrence;
}

void Incidence::clearRecurrence()
{
    delete d->mRecurrence;
    d->mRecurrence = 0;
}

ushort Incidence::recurrenceType() const
{
    if (d->mRecurrence) {
        return d->mRecurrence->recurrenceType();
    } else {
        return Recurrence::rNone;
    }
}

bool Incidence::recurs() const
{
    if (d->mRecurrence) {
        return d->mRecurrence->recurs();
    } else {
        return false;
    }
}

bool Incidence::recursOn(const QDate &date,
                         const KDateTime::Spec &timeSpec) const
{
    return d->mRecurrence && d->mRecurrence->recursOn(date, timeSpec);
}

bool Incidence::recursAt(const KDateTime &qdt) const
{
    return d->mRecurrence && d->mRecurrence->recursAt(qdt);
}

QList<KDateTime> Incidence::startDateTimesForDate(const QDate &date,
        const KDateTime::Spec &timeSpec) const
{
    KDateTime start = dtStart();
    KDateTime end = dateTime(RoleEndRecurrenceBase);

    QList<KDateTime> result;

    // TODO_Recurrence: Also work if only due date is given...
    if (!start.isValid() && ! end.isValid()) {
        return result;
    }

    // if the incidence doesn't recur,
    KDateTime kdate(date, timeSpec);
    if (!recurs()) {
        if (!(start > kdate || end < kdate)) {
            result << start;
        }
        return result;
    }

    int days = start.daysTo(end);
    // Account for possible recurrences going over midnight, while the original event doesn't
    QDate tmpday(date.addDays(-days - 1));
    KDateTime tmp;
    while (tmpday <= date) {
        if (recurrence()->recursOn(tmpday, timeSpec)) {
            QList<QTime> times = recurrence()->recurTimesOn(tmpday, timeSpec);
            foreach(const QTime &time, times) {
                tmp = KDateTime(tmpday, time, start.timeSpec());
                if (endDateForStart(tmp) >= kdate) {
                    result << tmp;
                }
            }
        }
        tmpday = tmpday.addDays(1);
    }
    return result;
}

QList<KDateTime> Incidence::startDateTimesForDateTime(const KDateTime &datetime) const
{
    KDateTime start = dtStart();
    KDateTime end = dateTime(RoleEndRecurrenceBase);

    QList<KDateTime> result;

    // TODO_Recurrence: Also work if only due date is given...
    if (!start.isValid() && ! end.isValid()) {
        return result;
    }

    // if the incidence doesn't recur,
    if (!recurs()) {
        if (!(start > datetime || end < datetime)) {
            result << start;
        }
        return result;
    }

    int days = start.daysTo(end);
    // Account for possible recurrences going over midnight, while the original event doesn't
    QDate tmpday(datetime.date().addDays(-days - 1));
    KDateTime tmp;
    while (tmpday <= datetime.date()) {
        if (recurrence()->recursOn(tmpday, datetime.timeSpec())) {
            // Get the times during the day (in start date's time zone) when recurrences happen
            QList<QTime> times = recurrence()->recurTimesOn(tmpday, start.timeSpec());
            foreach(const QTime &time, times) {
                tmp = KDateTime(tmpday, time, start.timeSpec());
                if (!(tmp > datetime || endDateForStart(tmp) < datetime)) {
                    result << tmp;
                }
            }
        }
        tmpday = tmpday.addDays(1);
    }
    return result;
}

KDateTime Incidence::endDateForStart(const KDateTime &startDt) const
{
    KDateTime start = dtStart();
    KDateTime end = dateTime(RoleEndRecurrenceBase);
    if (!end.isValid()) {
        return start;
    }
    if (!start.isValid()) {
        return end;
    }

    return startDt.addSecs(start.secsTo(end));
}

void Incidence::addAttachment(const Attachment::Ptr &attachment)
{
    if (mReadOnly || !attachment) {
        return;
    }

    Q_ASSERT(!d->mAttachments.contains(attachment));

    update();
    d->mAttachments.append(attachment);
    setFieldDirty(FieldAttachment);
    updated();
}

void Incidence::deleteAttachment(const Attachment::Ptr &attachment)
{
    int index = d->mAttachments.indexOf(attachment);
    if (index > -1) {
        setFieldDirty(FieldAttachment);
        d->mAttachments.remove(index);
    }
}

void Incidence::deleteAttachments(const QString &mime)
{
    Attachment::List result;
    Attachment::List::Iterator it = d->mAttachments.begin();
    while (it != d->mAttachments.end()) {
        if ((*it)->mimeType() != mime) {
            result += *it;
        }
        ++it;
    }
    d->mAttachments = result;
    setFieldDirty(FieldAttachment);
}

Attachment::List Incidence::attachments() const
{
    return d->mAttachments;
}

Attachment::List Incidence::attachments(const QString &mime) const
{
    Attachment::List attachments;
    foreach(Attachment::Ptr attachment, d->mAttachments) {
        if (attachment->mimeType() == mime) {
            attachments.append(attachment);
        }
    }
    return attachments;
}

void Incidence::clearAttachments()
{
    setFieldDirty(FieldAttachment);
    d->mAttachments.clear();
}

QString Incidence::writeAttachmentToTempFile(const Attachment::Ptr &attachment) const
{
    if (d->mTempFiles.contains(attachment)) {
        return d->mTempFiles.value(attachment);
    }
    KTemporaryFile *file = new KTemporaryFile();

    QStringList patterns = KMimeType::mimeType(attachment->mimeType())->patterns();

    if (!patterns.empty()) {
        file->setSuffix(QString(patterns.first()).remove(QLatin1Char('*')));
    }
    file->setAutoRemove(true);
    file->open();
    // read-only not to give the idea that it could be written to
    file->setPermissions(QFile::ReadUser);
    file->write(QByteArray::fromBase64(attachment->data()));
    d->mTempFiles.insert(attachment, file->fileName());
    file->close();
    return d->mTempFiles.value(attachment);
}

void Incidence::clearTempFiles()
{
    QHash<Attachment::Ptr,QString>::const_iterator it = d->mTempFiles.constBegin();
    const QHash<Attachment::Ptr,QString>::const_iterator end = d->mTempFiles.constEnd();
    for (; it != end; ++it) {
        QFile::remove(it.value());
    }
    d->mTempFiles.clear();
}

void Incidence::setResources(const QStringList &resources)
{
    if (mReadOnly) {
        return;
    }

    update();
    d->mResources = resources;
    setFieldDirty(FieldResources);
    updated();
}

QStringList Incidence::resources() const
{
    return d->mResources;
}

void Incidence::setPriority(int priority)
{
    if (mReadOnly) {
        return;
    }

    update();
    d->mPriority = priority;
    setFieldDirty(FieldPriority);
    updated();
}

int Incidence::priority() const
{
    return d->mPriority;
}

void Incidence::setStatus(Incidence::Status status)
{
    if (mReadOnly || status == StatusX) {
        return;
    }

    update();
    d->mStatus = status;
    d->mStatusString.clear();
    setFieldDirty(FieldStatus);
    updated();
}

void Incidence::setCustomStatus(const QString &status)
{
    if (mReadOnly) {
        return;
    }

    update();
    d->mStatus = status.isEmpty() ? StatusNone : StatusX;
    d->mStatusString = status;
    setFieldDirty(FieldStatus);
    updated();
}

Incidence::Status Incidence::status() const
{
    return d->mStatus;
}

QString Incidence::customStatus() const
{
    if (d->mStatus == StatusX) {
        return d->mStatusString;
    } else {
        return QString();
    }
}

void Incidence::setSecrecy(Incidence::Secrecy secrecy)
{
    if (mReadOnly) {
        return;
    }

    update();
    d->mSecrecy = secrecy;
    setFieldDirty(FieldSecrecy);
    updated();
}

Incidence::Secrecy Incidence::secrecy() const
{
    return d->mSecrecy;
}

Alarm::List Incidence::alarms() const
{
    return d->mAlarms;
}

Alarm::Ptr Incidence::newAlarm()
{
    Alarm::Ptr alarm(new Alarm(this));
    d->mAlarms.append(alarm);
    return alarm;
}

void Incidence::addAlarm(const Alarm::Ptr &alarm)
{
    update();
    d->mAlarms.append(alarm);
    setFieldDirty(FieldAlarms);
    updated();
}

void Incidence::removeAlarm(const Alarm::Ptr &alarm)
{
    const int index = d->mAlarms.indexOf(alarm);
    if (index > -1) {
        update();
        d->mAlarms.remove(index);
        setFieldDirty(FieldAlarms);
        updated();
    }
}

void Incidence::clearAlarms()
{
    update();
    d->mAlarms.clear();
    setFieldDirty(FieldAlarms);
    updated();
}

bool Incidence::hasEnabledAlarms() const
{
    foreach(Alarm::Ptr alarm, d->mAlarms) {
        if (alarm->enabled()) {
            return true;
        }
    }
    return false;
}

void Incidence::setLocation(const QString &location, bool isRich)
{
    if (mReadOnly) {
        return;
    }

    update();
    d->mLocation = location;
    d->mLocationIsRich = isRich;
    setFieldDirty(FieldLocation);
    updated();
}

void Incidence::setLocation(const QString &location)
{
    setLocation(location, Qt::mightBeRichText(location));
}

QString Incidence::location() const
{
    return d->mLocation;
}

QString Incidence::richLocation() const
{
    if (locationIsRich()) {
        return d->mLocation;
    } else {
        return Qt::escape(d->mLocation).replace(QLatin1Char('\n'), QStringLiteral("<br/>"));
    }
}

bool Incidence::locationIsRich() const
{
    return d->mLocationIsRich;
}

void Incidence::setSchedulingID(const QString &sid, const QString &uid)
{
    d->mSchedulingID = sid;
    if (!uid.isEmpty()) {
        setUid(uid);
    }
    setFieldDirty(FieldSchedulingId);
}

QString Incidence::schedulingID() const
{
    if (d->mSchedulingID.isNull()) {
        // Nothing set, so use the normal uid
        return uid();
    }
    return d->mSchedulingID;
}

bool Incidence::hasGeo() const
{
    return d->mHasGeo;
}

void Incidence::setHasGeo(bool hasGeo)
{
    if (mReadOnly) {
        return;
    }

    if (hasGeo == d->mHasGeo) {
        return;
    }

    update();
    d->mHasGeo = hasGeo;
    setFieldDirty(FieldGeoLatitude);
    setFieldDirty(FieldGeoLongitude);
    updated();
}

float Incidence::geoLatitude() const
{
    return d->mGeoLatitude;
}

void Incidence::setGeoLatitude(float geolatitude)
{
    if (mReadOnly) {
        return;
    }

    update();
    d->mGeoLatitude = geolatitude;
    setFieldDirty(FieldGeoLatitude);
    updated();
}

float Incidence::geoLongitude() const
{
    return d->mGeoLongitude;
}

void Incidence::setGeoLongitude(float geolongitude)
{
    if (!mReadOnly) {
        update();
        d->mGeoLongitude = geolongitude;
        setFieldDirty(FieldGeoLongitude);
        updated();
    }
}

bool Incidence::hasRecurrenceId() const
{
    return d->mRecurrenceId.isValid();
}

KDateTime Incidence::recurrenceId() const
{
    return d->mRecurrenceId;
}

void Incidence::setThisAndFuture(bool thisAndFuture)
{
    d->mThisAndFuture = thisAndFuture;
}

bool Incidence::thisAndFuture() const
{
    return d->mThisAndFuture;
}

void Incidence::setRecurrenceId(const KDateTime &recurrenceId)
{
    if (!mReadOnly) {
        update();
        d->mRecurrenceId = recurrenceId;
        setFieldDirty(FieldRecurrenceId);
        updated();
    }
}

/** Observer interface for the recurrence class. If the recurrence is changed,
    this method will be called for the incidence the recurrence object
    belongs to. */
void Incidence::recurrenceUpdated(Recurrence *recurrence)
{
    if (recurrence == d->mRecurrence) {
        update();
        setFieldDirty(FieldRecurrence);
        updated();
    }
}

//@cond PRIVATE
#define ALT_DESC_FIELD "X-ALT-DESC"
#define ALT_DESC_PARAMETERS QStringLiteral("FMTTYPE=text/html")
//@endcond

bool Incidence::hasAltDescription() const
{
    const QString value = nonKDECustomProperty(ALT_DESC_FIELD);
    const QString parameter = nonKDECustomPropertyParameters(ALT_DESC_FIELD);

    return parameter == ALT_DESC_PARAMETERS && !value.isEmpty();
}

void Incidence::setAltDescription(const QString &altdescription)
{
    if (altdescription.isEmpty()) {
        removeNonKDECustomProperty(ALT_DESC_FIELD);
    } else {
        setNonKDECustomProperty(ALT_DESC_FIELD,
                                altdescription,
                                ALT_DESC_PARAMETERS);
    }
}

QString Incidence::altDescription() const
{
    if (!hasAltDescription()) {
        return QString();
    } else {
        return nonKDECustomProperty(ALT_DESC_FIELD);
    }
}

bool Incidence::supportsGroupwareCommunication() const
{
    return type() == TypeEvent || type() == TypeTodo;
}

/** static */
QStringList Incidence::mimeTypes()
{
    return QStringList() << QStringLiteral("text/calendar")
           << KCalCore::Event::eventMimeType()
           << KCalCore::Todo::todoMimeType()
           << KCalCore::Journal::journalMimeType();
}

void Incidence::serialize(QDataStream &out)
{
    out << d->mCreated << d->mRevision << d->mDescription << d->mDescriptionIsRich << d->mSummary
        << d->mSummaryIsRich << d->mLocation << d->mLocationIsRich << d->mCategories
        << d->mResources << d->mStatusString << d->mPriority << d->mSchedulingID
        << d->mGeoLatitude << d->mGeoLongitude << d->mHasGeo << d->mRecurrenceId << d->mThisAndFuture
        << d->mLocalOnly << d->mStatus << d->mSecrecy << (d->mRecurrence ? true : false)
        << d->mAttachments.count() << d->mAlarms.count() << d->mRelatedToUid;

    if (d->mRecurrence)
        out << d->mRecurrence;

    foreach(const Attachment::Ptr &attachment, d->mAttachments) {
        out << attachment;
    }

    foreach(const Alarm::Ptr &alarm, d->mAlarms) {
        out << alarm;
    }
}

void Incidence::deserialize(QDataStream &in)
{
    quint32 status, secrecy;
    bool hasRecurrence;
    int attachmentCount, alarmCount;
    QMap<int,QString> relatedToUid;
    in >> d->mCreated >> d->mRevision >> d->mDescription >> d->mDescriptionIsRich >> d->mSummary
       >> d->mSummaryIsRich >> d->mLocation >> d->mLocationIsRich >> d->mCategories
       >> d->mResources >> d->mStatusString >> d->mPriority >> d->mSchedulingID
       >> d->mGeoLatitude >> d->mGeoLongitude >> d->mHasGeo >> d->mRecurrenceId >> d->mThisAndFuture
       >> d->mLocalOnly >> status >> secrecy >> hasRecurrence >> attachmentCount >> alarmCount
       >> relatedToUid;

    if (hasRecurrence) {
        d->mRecurrence = new Recurrence();
        d->mRecurrence->addObserver(const_cast<KCalCore::Incidence*>(this));
        in >> d->mRecurrence;
    }

    d->mAttachments.clear();
    d->mAlarms.clear();

    for (int i=0; i<attachmentCount; ++i) {
        Attachment::Ptr attachment = Attachment::Ptr(new Attachment(QString()));
        in >> attachment;
        d->mAttachments.append(attachment);
    }

    for (int i=0; i<alarmCount; ++i) {
        Alarm::Ptr alarm = Alarm::Ptr(new Alarm(this));
        in >> alarm;
        d->mAlarms.append(alarm);
    }

    d->mStatus = static_cast<Incidence::Status>(status);
    d->mSecrecy = static_cast<Incidence::Secrecy>(secrecy);

    d->mRelatedToUid.clear();
    foreach(int key, relatedToUid.keys()) { //krazy:exclude=foreach
        d->mRelatedToUid.insert(static_cast<Incidence::RelType>(key), relatedToUid.value(key));
    }


}
