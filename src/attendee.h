/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Attendee class.
*/

#ifndef KCALCORE_ATTENDEE_H
#define KCALCORE_ATTENDEE_H

#include <QMetaType>
#include <QSharedDataPointer>

#include "customproperties.h"
#include "kcalendarcore_export.h"

namespace KCalendarCore
{
/*!
  \qmlvaluetype attendee
  \inqmlmodule org.kde.kcalendarcore
  \nativetype KCalendarCore::Attendee
  \brief
  Represents information related to an attendee of an Calendar Incidence,
  typically a meeting or task (to-do).

  Attendees are people with a name and (optional) email address who are
  invited to participate in some way in a meeting or task.  This class
  also tracks that status of the invitation: accepted; tentatively accepted;
  declined; delegated to another person; in-progress; completed.

  Attendees may optionally be asked to RSVP ("Respond Please") to
  the invitation.

  Note that each attendee be can optionally associated with a UID
  (unique identifier) derived from a Calendar Incidence, Email Message,
  or any other thing you want.
*/

/*!
  \class KCalendarCore::Attendee
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Attendee

  \brief
  Represents information related to an attendee of an Calendar Incidence,
  typically a meeting or task (to-do).

  Attendees are people with a name and (optional) email address who are
  invited to participate in some way in a meeting or task.  This class
  also tracks that status of the invitation: accepted; tentatively accepted;
  declined; delegated to another person; in-progress; completed.

  Attendees may optionally be asked to RSVP ("Respond Please") to
  the invitation.

  Note that each attendee be can optionally associated with a UID
  (unique identifier) derived from a Calendar Incidence, Email Message,
  or any other thing you want.
*/
class KCALENDARCORE_EXPORT Attendee
{
    Q_GADGET

    /*!
     * \qmlproperty bool attendee::isNull
     */

    /*!
     * \property KCalendarCore::Attendee::isNull
     */
    Q_PROPERTY(bool isNull READ isNull)

    /*!
     * \qmlproperty string attendee::name
     */

    /*!
     * \property KCalendarCore::Attendee::name
     */
    Q_PROPERTY(QString name READ name WRITE setName)

    /*!
     * \qmlproperty string attendee::fullName
     */

    /*!
     * \property KCalendarCore::Attendee::fullName
     */
    Q_PROPERTY(QString fullName READ fullName)

    /*!
     * \qmlproperty string attendee::email
     */

    /*!
     * \property KCalendarCore::Attendee::email
     */
    Q_PROPERTY(QString email READ email WRITE setEmail)

    /*!
     * \qmlproperty Role attendee::role
     */

    /*!
     * \property KCalendarCore::Attendee::role
     */
    Q_PROPERTY(Role role READ role WRITE setRole)

    /*!
     * \qmlproperty string attendee::uid
     */

    /*!
     * \property KCalendarCore::Attendee::uid
     */
    Q_PROPERTY(QString uid READ uid WRITE setUid)

    /*!
     * \qmlproperty PartStat attendee::status
     */

    /*!
     * \property KCalendarCore::Attendee::status
     */
    Q_PROPERTY(PartStat status READ status WRITE setStatus)

    /*!
     * \qmlproperty CuType attendee::cuType
     */

    /*!
     * \property KCalendarCore::Attendee::cuType
     */
    Q_PROPERTY(CuType cuType READ cuType WRITE setCuType)

    /*!
     * \qmlproperty bool attendee::rsvp
     */

    /*!
     * \property KCalendarCore::Attendee::rsvp
     */
    Q_PROPERTY(bool rsvp READ RSVP WRITE setRSVP)

    /*!
     * \qmlproperty string attendee::delegate
     */

    /*!
     * \property KCalendarCore::Attendee::delegate
     */
    Q_PROPERTY(QString delegate READ delegate WRITE setDelegate)

    /*!
     * \qmlproperty string attendee::delegator
     */

    /*!
     * \property KCalendarCore::Attendee::delegator
     */
    Q_PROPERTY(QString delegator READ delegator WRITE setDelegator)

public:
    /*!
      \enum KCalendarCore::Attendee::PartStat
      \brief The different types of participant status.
      The meaning is specific to the incidence type in context.

      \value NeedsAction
      Event, to-do or journal needs action (default).
      \value Accepted
      Event, to-do or journal accepted.
      \value Declined
      Event, to-do or journal declined.
      \value Tentative
      Event or to-do tentatively accepted.
      \value Delegated
      Event or to-do delegated.
      \value Completed
      To-do completed.
      \value InProcess
      To-do in process of being completed.
      \value None
    */
    enum PartStat {
        NeedsAction,
        Accepted,
        Declined,
        Tentative,
        Delegated,
        Completed,
        InProcess,
        None,
    };
    Q_ENUM(PartStat)

    /*!
      \enum KCalendarCore::Attendee::Role
      \brief The different types of participation roles.

      \value ReqParticipant
      Participation is required (default).
      \value OptParticipant
      Participation is optional.
      \value NonParticipant
      Non-Participant; copied for information purposes.
      \value Chair
      Chairperson.
    */
    enum Role {
        ReqParticipant,
        OptParticipant,
        NonParticipant,
        Chair,
    };
    Q_ENUM(Role)

    /*!
      \enum KCalendarCore::Attendee::CuType
      \brief The different types of a participant.

      \since 4.14

      \value Individual
      An individual (default).
      \value Group
      A group of individuals.
      \value Resource
      A physical resource.
      \value Room
      A room resource.
      \value Unknown
      Otherwise not known.
    */
    enum CuType {
        Individual,
        Group,
        Resource,
        Room,
        Unknown,
        /*
         * Parameters that have to set via the QString variant of @setCuType() and \cuType()
         * x-name         ; Experimental cuType
         * iana-token     ; Other IANA-registered
         */
    };
    Q_ENUM(CuType)

    /*!
      List of attendees.
    */
    typedef QList<Attendee> List;

    /*! Create a null Attendee. */
    Attendee();

    /*!
      Constructs an attendee consisting of a person name (\a name) and
      email address (\a email); invitation status and Role;
      an optional RSVP flag and UID.

      \a name is person name of the attendee.

      \a email is person email address of the attendee.

      \a rsvp if true, the attendee is requested to reply to invitations.

      \a status is the PartStat status of the attendee.

      \a role is the Role of the attendee.

      \a uid is the UID of the attendee.

    */
    Attendee(const QString &name, const QString &email, bool rsvp = false, PartStat status = None, Role role = ReqParticipant, const QString &uid = QString());

    /*!
      Constructs an attendee by copying another attendee.

      \a attendee is the attendee to be copied.

    */
    Attendee(const Attendee &attendee);

    /*!
      Destroys the attendee.
    */
    ~Attendee();

    /*!
     * Returns \c true if this is a default-constructed Attendee instance.
     */
    bool isNull() const;

    /*!
      Returns the name of the attendee.
    */
    Q_REQUIRED_RESULT QString name() const;

    /*!
      Sets the name of the attendee to \a name.
    */
    void setName(const QString &name);

    /*!
      Returns the full name and email address of this attendee
      Returns A QString containing the person's full name in the form
        "FirstName LastName \<mail@domain\>".
    */
    Q_REQUIRED_RESULT QString fullName() const;

    /*!
      Returns the email address for this attendee.
    */
    Q_REQUIRED_RESULT QString email() const;

    /*!
      Sets the email address for this attendee to \a email.
    */
    void setEmail(const QString &email);

    /*!
      Sets the Role of the attendee to \a role.

      \a role is the Role to use for the attendee.

      \sa role()
    */
    void setRole(Role role);

    /*!
      Returns the Role of the attendee.

      \sa setRole()
    */
    Q_REQUIRED_RESULT Role role() const;

    /*!
      Sets the UID of the attendee to \a uid.

      \a uid is the UID to use for the attendee.

      \sa uid()
    */
    void setUid(const QString &uid);

    /*!
      Returns the UID of the attendee.

      \sa setUid()
    */
    Q_REQUIRED_RESULT QString uid() const;

    /*!
      Sets the PartStat of the attendee to \a status.

      \a status is the PartStat to use for the attendee.

      \sa status()
    */
    void setStatus(PartStat status);

    /*!
      Returns the PartStat of the attendee.

      \sa setStatus()
    */
    Q_REQUIRED_RESULT PartStat status() const;

    /*!
      Sets the CuType of the attendee to \a cuType.

      \a cuType is the CuType to use for the attendee.

      \sa cuType()

      \since 4.14
    */
    void setCuType(CuType cuType);

    /*!
      Sets the CuType of the attendee to \a cuType.

      \a cuType is the CuType to use for the attendee.

      \sa cuType()

      \since 4.14
    */
    void setCuType(const QString &cuType);

    /*!
      Returns the CuType of the attendee.

      \sa setCuType()

      \since 4.14
    */
    Q_REQUIRED_RESULT CuType cuType() const;

    /*!
      Returns the CuType of the attendee.

      \sa setCuType()

      \since 4.14
    */
    Q_REQUIRED_RESULT QString cuTypeStr() const;

    /*!
      Sets the RSVP flag of the attendee to \a rsvp.

      \a rsvp if set (true), the attendee is requested to reply to
      invitations.

      \sa RSVP()
    */
    void setRSVP(bool rsvp);

    /*!
      Returns the attendee RSVP flag.

      \sa setRSVP()
    */
    Q_REQUIRED_RESULT bool RSVP() const;

    /*!
      Compares this with \a attendee for equality.

      \a attendee is the attendee to compare.

      Returns true if \a attendee is equal to this object, or false if they are different.
    */
    bool operator==(const Attendee &attendee) const;

    /*!
      Compares this with \a attendee for inequality.

      \a attendee the attendee to compare.

      Returns true if \a attendee is not equal to this object, or false if they are equal.
    */
    bool operator!=(const Attendee &attendee) const;

    /*!
      Sets the delegate.

      \a delegate is a string containing a MAILTO URI of those delegated
      to attend the meeting.

      \sa delegate(), setDelegator().
    */
    void setDelegate(const QString &delegate);

    /*!
      Returns the delegate.
      \sa setDelegate().
    */
    Q_REQUIRED_RESULT QString delegate() const;

    /*!
      Sets the delegator.

      \a delegator is a string containing a MAILTO URI of those who
      have delegated their meeting attendance.

      \sa delegator(), setDelegate().
    */
    void setDelegator(const QString &delegator);

    /*!
      Returns the delegator.
      \sa setDelegator().
    */
    Q_REQUIRED_RESULT QString delegator() const;

    /*!
      Adds a custom property. If the property already exists it will be overwritten.

      \a xname is the name of the property.

      \a xvalue is its value.

    */
    void setCustomProperty(const QByteArray &xname, const QString &xvalue);

    /*!
      Returns a reference to the CustomProperties object
    */
    Q_REQUIRED_RESULT CustomProperties &customProperties();

    /*!
      Returns a const reference to the CustomProperties object
    */
    const CustomProperties &customProperties() const;

    /*!
      Sets this attendee equal to \a attendee.

      \a attendee is the attendee to copy.

    */
    Attendee &operator=(const Attendee &attendee);

private:
    //@cond PRIVATE
    class Private;
    QSharedDataPointer<Private> d;
    //@endcond

    friend KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &s, const KCalendarCore::Attendee &attendee);
    friend KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &s, KCalendarCore::Attendee &attendee);
};

/*!
  Serializes an Attendee object into a data stream.

  \a stream is a QDataStream.

  \a attendee is a pointer to a Attendee object to be serialized.

*/
KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &stream, const KCalendarCore::Attendee &attendee);

/*!
  Initializes an Attendee object from a data stream.

  \a stream is a QDataStream.

  \a attendee is a pointer to a Attendee object to be initialized.

*/
KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &stream, KCalendarCore::Attendee &attendee);
}

//@cond PRIVATE
Q_DECLARE_TYPEINFO(KCalendarCore::Attendee, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(KCalendarCore::Attendee)
//@endcond

#endif
