/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  SPDX-FileCopyrightText: 2005 Rafal Rzepecki <divide@users.sourceforge.net>
  SPDX-FileCopyrightText: 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
  SPDX-FileContributor: Alvaro Manera <alvaro.manera@nokia.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the IncidenceBase class.

  @glossary @anchor incidence @b incidence:
  General term for a calendar component.
  Examples are events, to-dos, and journals.

  @glossary @anchor event @b event:
  An \l incidence that has a start and end time, typically representing some
  occurrence of social or personal importance. May be recurring.
  Examples are appointments, meetings, or holidays.

  @glossary @anchor to-do @b to-do:
  An \l incidence that has an optional start time and an optional due time
  typically representing some undertaking to be performed. May be recurring.
  Examples are "fix the bug" or "pay the bills".

  @glossary @anchor todo @b todo:
  See \l to-do.

  @glossary @anchor journal @b journal:
  An \l incidence with a start date that represents a diary or daily record
  of one's activities. May @b not be recurring.
*/

#ifndef KCALCORE_INCIDENCEBASE_H
#define KCALCORE_INCIDENCEBASE_H

#include "attendee.h"
#include "customproperties.h"
#include "duration.h"
#include "person.h"

#include <QDataStream>
#include <QDateTime>
#include <QSet>
#include <QSharedPointer>
#include <QUrl>

class QUrl;
class QDate;
class QTimeZone;

namespace KCalendarCore
{
/*! List of dates */
typedef QList<QDate> DateList;

/*! List of times */
typedef QList<QDateTime> DateTimeList;

class Event;
class Todo;
class Journal;
class FreeBusy;
class Visitor;
class IncidenceBasePrivate;

/*!
  \qmlvaluetype incidenceBase
  \inqmlmodule org.kde.kcalendarcore
  \nativetype KCalendarCore::IncidenceBase
  \brief
  An abstract class that provides a common base for all calendar incidence
  classes.

  define: organizer (person)
  define: uid (same as the attendee uid?)

  Several properties are not allowed for VFREEBUSY objects (see rfc:2445),
  so they are not in IncidenceBase. The hierarchy is:

  \list
    \li IncidenceBase
    \list
      \li FreeBusy
      \li Incidence
      \list
        \li KCalendarCore::Event
        \li KCalendarCore::Todo
        \li KCalendarCore::Journal
      \endlist
    \endlist
  \endlist

  So IncidenceBase contains all properties that are common to all classes,
  and Incidence contains all additional properties that are common to
  Events, Todos and Journals, but are not allowed for FreeBusy entries.
*/

/*!
  \class KCalendarCore::IncidenceBase
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/IncidenceBase
  \brief
  An abstract class that provides a common base for all calendar incidence
  classes.

  define: organizer (person)
  define: uid (same as the attendee uid?)

  Several properties are not allowed for VFREEBUSY objects (see rfc:2445),
  so they are not in IncidenceBase. The hierarchy is:

  \list
    \li IncidenceBase
    \list
      \li FreeBusy
      \li Incidence
      \list
        \li KCalendarCore::Event
        \li KCalendarCore::Todo
        \li KCalendarCore::Journal
      \endlist
    \endlist
  \endlist

  So IncidenceBase contains all properties that are common to all classes,
  and Incidence contains all additional properties that are common to
  Events, Todos and Journals, but are not allowed for FreeBusy entries.
*/
class KCALENDARCORE_EXPORT IncidenceBase : public CustomProperties
{
    Q_GADGET

    /*!
     * \qmlproperty string incidenceBase::uid
     */

    /*!
     * \property KCalendarCore::IncidenceBase::uid
     */
    Q_PROPERTY(QString uid READ uid WRITE setUid)

    /*!
     * \qmlproperty date incidenceBase::lastModified
     */

    /*!
     * \property KCalendarCore::IncidenceBase::lastModified
     */
    Q_PROPERTY(QDateTime lastModified READ lastModified WRITE setLastModified)

    /*!
     * \qmlproperty date incidenceBase::dtStart
     */

    /*!
     * \property KCalendarCore::IncidenceBase::dtStart
     */
    Q_PROPERTY(QDateTime dtStart READ dtStart WRITE setDtStart)

    /*!
     * \qmlproperty bool incidenceBase::allDay
     */

    /*!
     * \property KCalendarCore::IncidenceBase::allDay
     */
    Q_PROPERTY(bool allDay READ allDay WRITE setAllDay)

    /*!
     * \qmlproperty KCalendarCore::Person incidenceBase::organizer
     */

    /*!
     * \property KCalendarCore::IncidenceBase::organizer
     */
    Q_PROPERTY(KCalendarCore::Person organizer READ organizer WRITE setOrganizer)

    /*!
     * \qmlproperty list<KCalendarCore::Attendee> incidenceBase::attendees
     */

    /*!
     * \property KCalendarCore::IncidenceBase::attendees
     */
    Q_PROPERTY(QList<KCalendarCore::Attendee> attendees READ attendees)

    /*!
     * \qmlproperty url incidenceBase::url
     */

    /*!
     * \property KCalendarCore::IncidenceBase::url
     */
    Q_PROPERTY(QUrl url READ url WRITE setUrl)

public:
    /*!
      A shared pointer to an IncidenceBase.
    */
    typedef QSharedPointer<IncidenceBase> Ptr;

    /*!
      \enum KCalendarCore::IncidenceBase::IncidenceType
      \brief The different types of incidences, per RFC2445.

      \value TypeEvent
      Type is an event.
      \value TypeTodo
      Type is a to-do.
      \value TypeJournal
      Type is a journal.
      \value TypeFreeBusy
      Type is a free/busy.
      \value TypeUnknown
      Type unknown.

      \sa type(), typeStr()
    */
    enum IncidenceType {
        TypeEvent = 0,
        TypeTodo,
        TypeJournal,
        TypeFreeBusy,
        TypeUnknown,
    };

    /*!
      \enum KCalendarCore::IncidenceBase::DateTimeRole
      \brief The different types of incidence date/times roles.

      \value RoleAlarmStartOffset
      Role for an incidence alarm's starting offset date/time.
      \value RoleAlarmEndOffset
      Role for an incidence alarm's ending offset date/time.
      \value RoleSort
      Role for an incidence's date/time used when sorting.
      \value RoleCalendarHashing
      Role for looking up an incidence in a Calendar.
      \value RoleStartTimeZone
      Role for determining an incidence's starting timezone.
      \value RoleEndTimeZone
      Role for determining an incidence's ending timezone.
      \value RoleEndRecurrenceBase
      \value RoleEnd
      Role for determining an incidence's dtEnd, will return
      an invalid QDateTime if the incidence does not support dtEnd.
      \value RoleDisplayEnd
      Role used for display purposes, represents the end boundary
      if an incidence supports dtEnd.
      \value RoleAlarm
      Role for determining the date/time of the first alarm.
      Returns invalid time if the incidence doesn't have any alarm.
      \value RoleRecurrenceStart
      Role for determining the start of the recurrence.
      Currently that's DTSTART for an event and DTDUE for a to-do.
      \note If the incidence is a to-do, recurrence should be
      calculated having DTSTART for a reference, not DT-DUE.
      This is one place KCalendarCore isn't compliant with RFC2445.
      \value RoleDisplayStart
      Role for display purposes, represents the start boundary of an
      incidence. To-dos return dtDue here, for historical reasons.
      \value RoleDnD
      Role for determining new start and end dates after a DnD.

      \sa dateTime()
    */
    enum DateTimeRole {
        RoleAlarmStartOffset = 0,
        RoleAlarmEndOffset,
        RoleSort,
        RoleCalendarHashing,
        RoleStartTimeZone,
        RoleEndTimeZone,
        RoleEndRecurrenceBase,
        RoleEnd,
        RoleDisplayEnd,
        RoleAlarm,
        RoleRecurrenceStart,
        RoleDisplayStart,
        RoleDnD,
    };

    /*!
      \enum KCalendarCore::IncidenceBase::Field
      \brief The different types of incidence fields.

      \value FieldDtStart
      Field representing the DTSTART component.
      \value FieldDtEnd
      Field representing the DTEND component.
      \value FieldLastModified
      Field representing the LAST-MODIFIED component.
      \value FieldDescription
      Field representing the DESCRIPTION component.
      \value FieldSummary
      Field representing the SUMMARY component.
      \value FieldLocation
      Field representing the LOCATION component.
      \value FieldCompleted
      Field representing the COMPLETED component.
      \value FieldPercentComplete
      Field representing the PERCENT-COMPLETE component.
      \value FieldDtDue
      Field representing the DUE component.
      \value FieldCategories
      Field representing the CATEGORIES component.
      \value FieldRelatedTo
      Field representing the RELATED-TO component.
      \value FieldRecurrence
      Field representing the EXDATE, EXRULE, RDATE, and RRULE components.
      \value FieldAttachment
      Field representing the ATTACH component.
      \value FieldSecrecy
      Field representing the CLASS component.
      \value FieldStatus
      Field representing the STATUS component.
      \value FieldTransparency
      Field representing the TRANSPARENCY component.
      \value FieldResources
      Field representing the RESOURCES component.
      \value FieldPriority
      Field representing the PRIORITY component.
      \value FieldGeoLatitude
      Field representing the latitude part of the GEO component.
      \value FieldGeoLongitude
      Field representing the longitude part of the GEO component.
      \value FieldRecurrenceId
      Field representing the RECURRENCE-ID component.
      \value FieldAlarms
      Field representing the VALARM component.
      \value FieldSchedulingId
      Field representing the X-KDE-LIBKCAL-ID component.
      \value FieldAttendees
      Field representing the ATTENDEE component.
      \value FieldOrganizer
      Field representing the ORGANIZER component.
      \value FieldCreated
      Field representing the CREATED component.
      \value FieldRevision
      Field representing the SEQUENCE component.
      \value FieldDuration
      Field representing the DURATION component.
      \value FieldContact
      Field representing the CONTACT component.
      \value FieldComment
      Field representing the COMMENT component.
      \value FieldUid
      Field representing the UID component.
      \value FieldUnknown
      Something changed. Always set when you use the assignment operator.
      \value FieldUrl
      Field representing the URL component.
      \value FieldConferences
      Field representing the CONFERENCE component.
      \value FieldColor
      Field representing the COLOR component.
    */
    enum Field {
        FieldDtStart,
        FieldDtEnd,
        FieldLastModified,
        FieldDescription,
        FieldSummary,
        FieldLocation,
        FieldCompleted,
        FieldPercentComplete,
        FieldDtDue,
        FieldCategories,
        FieldRelatedTo,
        FieldRecurrence,
        FieldAttachment,
        FieldSecrecy,
        FieldStatus,
        FieldTransparency,
        FieldResources,
        FieldPriority,
        FieldGeoLatitude,
        FieldGeoLongitude,
        FieldRecurrenceId,
        FieldAlarms,
        FieldSchedulingId,
        FieldAttendees,
        FieldOrganizer,
        FieldCreated,
        FieldRevision,
        FieldDuration,
        FieldContact,
        FieldComment,
        FieldUid,
        FieldUnknown,
        FieldUrl,
        FieldConferences,
        FieldColor,
    };

    /*!
      \class KCalendarCore::IncidenceBase::IncidenceObserver
      \inmodule KCalendarCore
      \brief The IncidenceObserver class.
    */
    class KCALENDARCORE_EXPORT IncidenceObserver
    {
    public:
        /*!
          Destroys the IncidenceObserver.
        */
        virtual ~IncidenceObserver();

        /*!
          The IncidenceObserver interface.
          This function is called before any changes are made.

          \a uid is the string containing the incidence \l uid.

          \a recurrenceId is possible recurrenceid of incidence.

        */
        virtual void incidenceUpdate(const QString &uid, const QDateTime &recurrenceId) = 0;

        /*!
          The IncidenceObserver interface.
          This function is called after changes are completed.

          \a uid is the string containing the incidence \l uid.

          \a recurrenceId is possible recurrenceid of incidence.

        */
        virtual void incidenceUpdated(const QString &uid, const QDateTime &recurrenceId) = 0;
    };

    IncidenceBase() = delete;

    /*!
      Constructs an empty IncidenceBase.

      \a p (non-null) a Private data object provided by the instantiated
      class (Event, Todo, Journal, FreeBusy).  It takes ownership of the object.

    */
    KCALENDARCORE_NO_EXPORT explicit IncidenceBase(IncidenceBasePrivate *p);

    /*!
      Destroys the IncidenceBase.
    */
    ~IncidenceBase() override;

    /*!
      Assignment operator.
      All data belonging to derived classes are also copied.
      \sa assign().
      The caller guarantees that both types match.

      \code
      if ( i1.type() == i2.type() ) {
        i1 = i2;
      } else {
        qCDebug(KCALCORE_LOG) << "Invalid assignment!";
      }
      \endcode

      Dirty field FieldUnknown will be set.

      \a other is the IncidenceBase to assign.

     */
    IncidenceBase &operator=(const IncidenceBase &other);

    /*!
      Compares this with IncidenceBase \a ib for equality.
      All data belonging to derived classes are also compared.
      \sa equals().

      \a ib is the IncidenceBase to compare against.

      Returns true if the incidences are equal; false otherwise.
    */
    bool operator==(const IncidenceBase &ib) const;

    /*!
      Compares this with IncidenceBase \a ib for inequality.

      \a ib is the IncidenceBase to compare against.

      Returns true if the incidences are /not/ equal; false otherwise.
    */
    bool operator!=(const IncidenceBase &ib) const;

    /*!
     Accept IncidenceVisitor. A class taking part in the visitor mechanism
     has to provide this implementation:
     \code
       bool accept(Visitor &v) { return v.visit(this); }
     \endcode

     \a v is a reference to a Visitor object.

     \a incidence is a valid IncidenceBase object for visiting.

    */
    virtual bool accept(Visitor &v, const IncidenceBase::Ptr &incidence);

    /*!
      Returns the incidence type.
    */
    virtual IncidenceType type() const = 0;

    /*!
      Prints the type of incidence as a string.
    */
    virtual QByteArray typeStr() const = 0;

    /*!
      Sets the unique id for the incidence to \a uid.

      \a uid is the string containing the incidence \l uid.

      \sa uid()
    */
    void setUid(const QString &uid);

    /*!
      Returns the unique id (\l uid) for the incidence.
      \sa setUid()
    */
    Q_REQUIRED_RESULT QString uid() const;

    /*!
      Returns the uri for the incidence, of form urn:x-ical:\<uid\>
    */
    Q_REQUIRED_RESULT QUrl uri() const;

    /*!
      Sets the time the incidence was last modified to \a lm.
      It is stored as a UTC date/time.

      \a lm is the QDateTime when the incidence was last modified.

      \sa lastModified()
    */
    virtual void setLastModified(const QDateTime &lm);

    /*!
      Returns the time the incidence was last modified.
      \sa setLastModified()
    */
    Q_REQUIRED_RESULT QDateTime lastModified() const;

    /*!
      Sets the organizer for the incidence.

      \a organizer is a non-null Person to use as the incidence \l organizer.

      \sa organizer(), setOrganizer(const QString &)
    */
    void setOrganizer(const Person &organizer);

    /*!
      Sets the incidence organizer to any string \a organizer.

      \a organizer is a string to use as the incidence \l organizer.

      \sa organizer()
    */
    void setOrganizer(const QString &organizer);

    /*!
      Returns the Person associated with this incidence.
      If no Person was set through setOrganizer(), a default Person()
      is returned.
      \sa setOrganizer()
    */
    Person organizer() const;

    /*!
      Sets readonly status.

      \a readOnly if set, the incidence is read-only; else the incidence
      can be modified.

      \sa isReadOnly().
    */
    virtual void setReadOnly(bool readOnly);

    /*!
      Returns true the object is read-only; false otherwise.
      \sa setReadOnly()
    */
    Q_REQUIRED_RESULT bool isReadOnly() const;

    /*!
      Sets the incidence's starting date/time with a QDateTime.

      \a dtStart is the incidence start date/time.

      \sa dtStart().
    */
    virtual void setDtStart(const QDateTime &dtStart);

    /*!
      Returns an incidence's starting date/time as a QDateTime.
      \sa setDtStart().
    */
    virtual QDateTime dtStart() const;

    /*!
      Sets the incidence duration.

      \a duration the incidence duration

      \sa duration()
    */
    virtual void setDuration(const Duration &duration);

    /*!
      Returns the length of the incidence duration.
      \sa setDuration()
    */
    Q_REQUIRED_RESULT Duration duration() const;

    /*!
      Sets if the incidence has a duration.

      \a hasDuration true if the incidence has a duration; false otherwise.

      \sa hasDuration()
    */
    void setHasDuration(bool hasDuration);

    /*!
      Returns true if the incidence has a duration; false otherwise.
      \sa setHasDuration()
    */
    Q_REQUIRED_RESULT bool hasDuration() const;

    /*!
      Returns true or false depending on whether the incidence is all-day.
      i.e. has a date but no time attached to it.
      \sa setAllDay()
    */
    Q_REQUIRED_RESULT bool allDay() const;

    /*!
      Sets whether the incidence is all-day, i.e. has a date but no time
      attached to it.

      \a allDay sets whether the incidence is all-day.

      \sa allDay()
    */
    virtual void setAllDay(bool allDay);

    /*!
      Shift the times of the incidence so that they appear at the same clock
      time as before but in a new time zone. The shift is done from a viewing
      time zone rather than from the actual incidence time zone.

      For example, shifting an incidence whose start time is 09:00
      America/New York, using an old viewing time zone (\a oldZone)
      of Europe/London, to a new time zone (\a newZone) of Europe/Paris,
      will result in the time being shifted from 14:00 (which is the London
      time of the incidence start) to 14:00 Paris time.

      \a oldZone the time zone which provides the clock times

      \a newZone the new time zone

    */
    virtual void shiftTimes(const QTimeZone &oldZone, const QTimeZone &newZone);

    /*!
      Adds a comment to the incidence. Does not add a linefeed character; simply
      appends the text as specified.

      \a comment is the QString containing the comment to add.

      \sa removeComment().
    */
    void addComment(const QString &comment);

    /*!
      Removes a comment from the incidence. Removes the first comment whose
      string is an exact match for the specified string in \a comment.

      \a comment is the QString containing the comment to remove.

      Returns true if match found, false otherwise.
      \sa addComment().
     */
    Q_REQUIRED_RESULT bool removeComment(const QString &comment);

    /*!
      Deletes all incidence comments.
    */
    void clearComments();

    /*!
      Returns all incidence comments as a list of strings.
    */
    Q_REQUIRED_RESULT QStringList comments() const;

    /*!
      Adds a contact to thieincidence. Does not add a linefeed character; simply
      appends the text as specified.

      \a contact is the QString containing the contact to add.

      \sa removeContact().
    */
    void addContact(const QString &contact);

    /*!
      Removes a contact from the incidence. Removes the first contact whose
      string is an exact match for the specified string in \a contact.

      \a contact is the QString containing the contact to remove.

      Returns true if match found, false otherwise.
      \sa addContact().
     */
    Q_REQUIRED_RESULT bool removeContact(const QString &contact);

    /*!
      Deletes all incidence contacts.
    */
    void clearContacts();

    /*!
      Returns all incidence contacts as a list of strings.
    */
    Q_REQUIRED_RESULT QStringList contacts() const;

    /*!
      Add Attendee to this incidence.

      \a attendee is the attendee to add

      \a doUpdate If true the Observers are notified, if false they are not.

    */
    void addAttendee(const Attendee &attendee, bool doUpdate = true);

    /*!
      Removes all attendees from the incidence.
    */
    void clearAttendees();

    /*!
       Set the attendees of this incidence.
       This replaces all previously set attendees, unlike addAttendee.

       \a attendees is a list of attendees.

       \a doUpdate If true the Observers are notified, if false they are not.

    */
    void setAttendees(const Attendee::List &attendees, bool doUpdate = true);

    /*!
      Returns a list of incidence attendees.
      All pointers in the list are valid.
    */
    Q_REQUIRED_RESULT Attendee::List attendees() const;

    /*!
      Returns the number of incidence attendees.
    */
    Q_REQUIRED_RESULT int attendeeCount() const;

    /*!
      Returns the attendee with the specified email address.

      \a email is a QString containing an email address of the
      form "FirstName LastName <emailaddress>".

      \sa attendeeByMails(), attendeeByUid().
    */
    Attendee attendeeByMail(const QString &email) const;

    /*!
      Returns the first incidence attendee with one of the specified
      email addresses.

      \a emails is a list of QStrings containing email addresses of the
      form "FirstName LastName <emailaddress>".

      \a email is a QString containing a single email address to search
      in addition to the list specified in \a emails.

      \sa attendeeByMail(), attendeeByUid().
    */
    Attendee attendeeByMails(const QStringList &emails, const QString &email = QString()) const;

    /*!
      Returns the incidence attendee with the specified attendee UID.

      \a uid is a QString containing an attendee UID.

      \sa attendeeByMail(), attendeeByMails().
    */
    Attendee attendeeByUid(const QString &uid) const;

    /*!
      Sets the incidences url.

      This property can be used to point to a more dynamic rendition of the incidence.
      I.e. a website related to the incidence.

      \a url is the URL of the incidence.

      \sa url()
      \since 4.12
    */
    void setUrl(const QUrl &url);

    /*!
      Returns the url.
      Returns incidences url value
      \sa setUrl()
      \since 4.12
    */
    Q_REQUIRED_RESULT QUrl url() const;

    /*!
      Register observer. The observer is notified when the observed object
      changes.

      \a observer is a pointer to an IncidenceObserver object that will be
      watching this incidence.

      \sa unRegisterObserver()
    */
    void registerObserver(IncidenceObserver *observer);

    /*!
      Unregister observer. It isn't notified anymore about changes.

      \a observer is a pointer to an IncidenceObserver object that will be
      watching this incidence.

      \sa registerObserver().
    */
    void unRegisterObserver(IncidenceObserver *observer);

    /*!
      Call this to notify the observers after the IncidenceBase object will be
      changed.
    */
    void update();

    /*!
      Call this to notify the observers after the IncidenceBase object has
      changed.
    */
    void updated();

    /*!
      Call this when a group of updates is going to be made. This suppresses
      change notifications until endUpdates() is called, at which point
      updated() will automatically be called.
    */
    void startUpdates();

    /*!
      Call this when a group of updates is complete, to notify observers that
      the instance has changed. This should be called in conjunction with
      startUpdates().
    */
    void endUpdates();

    /*!
      Returns a date/time corresponding to the specified DateTimeRole.

      \a role is a DateTimeRole.

    */
    virtual QDateTime dateTime(DateTimeRole role) const = 0;

    /*!
      Sets the date/time corresponding to the specified DateTimeRole.

      \a dateTime is QDateTime value to set.

      \a role is a DateTimeRole.

    */
    virtual void setDateTime(const QDateTime &dateTime, DateTimeRole role) = 0;

    /*!
      Returns the Akonadi specific sub MIME type of a KCalendarCore::IncidenceBase item,
      e.g. getting "application/x-vnd.akonadi.calendar.event" for a KCalendarCore::Event.
    */
    virtual QLatin1String mimeType() const = 0;

    /*!
      Returns the incidence recurrenceId.
    */
    virtual QDateTime recurrenceId() const;

    /*!
       Returns a QSet with all Fields that were changed since the incidence was created
       or resetDirtyFields() was called.

       \sa resetDirtyFields()
    */
    QSet<IncidenceBase::Field> dirtyFields() const;

    /*!
       Sets which fields are dirty.
       \sa dirtyFields()
       \since 4.8
     */
    void setDirtyFields(const QSet<IncidenceBase::Field> &);

    /*!
       Resets dirty fields.
       \sa dirtyFields()
    */
    void resetDirtyFields();

    /*!
     * Constant that identifies KCalendarCore data in a binary stream.
     *
     * \since 4.12
     */
    Q_REQUIRED_RESULT static quint32 magicSerializationIdentifier();

protected:
    /*!
       Marks Field \a field as dirty.

       \a field is the Field type to mark as dirty.

       \sa dirtyFields()
    */
    void setFieldDirty(IncidenceBase::Field field);

    /*!
      \reimp
    */
    void customPropertyUpdate() override;

    /*!
      \reimp
    */
    void customPropertyUpdated() override;

    IncidenceBase(const IncidenceBase &) = delete;

    /*!
      Constructs an IncidenceBase as a copy of another IncidenceBase object.

      \a ib is the IncidenceBase to copy.

      \a p (non-null) a Private data object provided by the instantiated
      class (Event, Todo, Journal, FreeBusy).  It takes ownership of the object.

    */
    KCALENDARCORE_NO_EXPORT IncidenceBase(const IncidenceBase &ib, IncidenceBasePrivate *p);

    /*!
      Provides polymorfic comparison for equality.
      Only called by IncidenceBase::operator==() which guarantees that

      \a incidenceBase is of the right type.

      \a incidenceBase is the IncidenceBase to compare against.

      Returns true if the incidences are equal; false otherwise.
    */
    virtual bool equals(const IncidenceBase &incidenceBase) const;

    /*!
      Provides polymorphic assignment.

      \a other is the IncidenceBase to assign.

    */
    virtual IncidenceBase &assign(const IncidenceBase &other);

    /*!
     * Sub-type specific serialization.
     *
     * \a out is the output data stream
     *
     */
    virtual void serialize(QDataStream &out) const;

    /*!
     * Sub-type specific deserialization.
     *
     * \a in is the input data stream
     *
     */
    virtual void deserialize(QDataStream &in);

    enum VirtualHook {};

    /*!
      Standard trick to add virtuals later.

      \a id is any integer unique to this class which we will use to identify the method
             to be called.

      \a data is a pointer to some glob of data, typically a struct.

    */
    virtual void virtual_hook(VirtualHook id, void *data) = 0;

    /*!
      Identifies a read-only incidence.
    */
    bool mReadOnly;

    Q_DECLARE_PRIVATE(IncidenceBase)

protected:
    IncidenceBasePrivate *const d_ptr;

private:
    friend KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &stream, const KCalendarCore::IncidenceBase::Ptr &);

    friend KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &stream, KCalendarCore::IncidenceBase::Ptr &);
};

/*!
 * Compare two QDateTimes \a dt1 and \a dt2 for extended equality.
 *
 * QDateTime::operator==() in Qt 5.12 returns true if its operands represent
 * the same instant in time, regardless of their time zones or TimeSpecs (and
 * contrary to the documentation).  This function returns true if and only if
 * their times, time zones, and TimeSpecs are equal, or both are invalid().
 *
 * \since 5.93
 */
KCALENDARCORE_EXPORT bool identical(const QDateTime &dt1, const QDateTime &dt2);

/*!
 * Incidence serializer.
 *
 * \since 4.12
 */
KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &out, const KCalendarCore::IncidenceBase::Ptr &);

/*!
 * Incidence deserializer.
 *
 * \since 4.12
 */
KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &in, KCalendarCore::IncidenceBase::Ptr &);
}

Q_DECLARE_METATYPE(KCalendarCore::IncidenceBase *)
Q_DECLARE_METATYPE(KCalendarCore::IncidenceBase::Ptr)

#endif
