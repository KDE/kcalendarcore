/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KCALCORE_SCHEDULEMESSAGE_H
#define KCALCORE_SCHEDULEMESSAGE_H

#include "incidencebase.h"

#include "kcalendarcore_export.h"

namespace KCalendarCore
{
class IncidenceBase;

/*!
  \enum KCalendarCore::iTIPMethod
  \brief iTIP methods.

  \value iTIPPublish
  Event, to-do, journal or freebusy posting.
  \value iTIPRequest
  Event, to-do or freebusy scheduling request.
  \value iTIPReply
  Event, to-do or freebusy reply to request.
  \value iTIPAdd
  Event, to-do or journal additional property request.
  \value iTIPCancel
  Event, to-do or journal cancellation notice.
  \value iTIPRefresh
  Event or to-do description update request.
  \value iTIPCounter
  Event or to-do submit counter proposal.
  \value iTIPDeclineCounter
  Event or to-do decline a counter proposal.
  \value iTIPNoMethod
  No method.
*/
enum iTIPMethod {
    iTIPPublish,
    iTIPRequest,
    iTIPReply,
    iTIPAdd,
    iTIPCancel,
    iTIPRefresh,
    iTIPCounter,
    iTIPDeclineCounter,
    iTIPNoMethod,
};

/*!
  \class KCalendarCore::ScheduleMessage
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/ScheduleMessage
  \brief
  A Scheduling message class.

  This class provides an encapsulation of a scheduling message.
  It associates an incidence with an iTIPMethod and status information.
*/
class KCALENDARCORE_EXPORT ScheduleMessage
{
public:
    /*!
      \enum KCalendarCore::ScheduleMessage::Status
      \brief Message status.

      \value PublishNew
      New message posting.
      \value PublishUpdate
      Updated message.
      \value Obsolete
      obsolete.
      \value RequestNew
      Request new message posting.
      \value RequestUpdate
      Request updated message.
      \value Unknown
      No status.
    */
    enum Status {
        PublishNew,
        PublishUpdate,
        Obsolete,
        RequestNew,
        RequestUpdate,
        Unknown,
    };

    /*!
      A shared pointer to a ScheduleMessage.
    */
    typedef QSharedPointer<ScheduleMessage> Ptr;

    /*!
      Creates a scheduling message with method as defined in iTIPMethod and a status.
      \a incidence a pointer to a valid Incidence to be associated with this message.

      \a method an iTIPMethod.

      \a status a Status.

    */
    ScheduleMessage(const IncidenceBase::Ptr &incidence, iTIPMethod method, Status status);

    /*!
      Destructor.
    */
    ~ScheduleMessage();

    /*!
      Returns the event associated with this message.
    */
    IncidenceBase::Ptr event() const;

    /*!
      Returns the iTIP method associated with this message.
    */
    Q_REQUIRED_RESULT iTIPMethod method() const;

    /*!
      Returns a machine-readable (not translatable) name for a iTIP method.

      \a method an iTIPMethod.

    */
    Q_REQUIRED_RESULT static QString methodName(iTIPMethod method);

    /*!
      Returns the status of this message.
    */
    Q_REQUIRED_RESULT Status status() const;

    /*!
      Returns the error message if there is any.
    */
    Q_REQUIRED_RESULT QString error() const;

private:
    //@cond PRIVATE
    Q_DISABLE_COPY(ScheduleMessage)
    class Private;
    Private *const d;
    //@endcond
};

}

#endif
