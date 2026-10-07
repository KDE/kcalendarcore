/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the ICalFormat class.
*/
#ifndef KCALCORE_ICALFORMAT_H
#define KCALCORE_ICALFORMAT_H

#include "calformat.h"
#include "freebusy.h"
#include "incidence.h"
#include "kcalendarcore_export.h"
#include "schedulemessage.h"

namespace KCalendarCore
{
class FreeBusy;
class ICalFormatPrivate;
class Incidence;
class IncidenceBase;
class RecurrenceRule;

/*!
  \class KCalendarCore::ICalFormat
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/ICalFormat
  \brief
  iCalendar format implementation.

  This class implements the iCalendar format. It provides methods for
  loading/saving/converting iCalendar format data into the internal
  representation as Calendar and Incidences.

  \warning When importing/loading to a Calendar, there is only duplicate
  check if those Incidences are loaded into the Calendar. If they are not
  loaded it will create duplicates.
*/
class KCALENDARCORE_EXPORT ICalFormat : public CalFormat
{
public:
    /*!
      Constructor a new iCalendar Format object.
    */
    ICalFormat();

    /*!
      Destructor.
    */
    ~ICalFormat() override;

    /*!
      \reimp
    */
    bool load(const QSharedPointer<Calendar> &calendar, const QString &fileName) override;

    /*!
      \reimp
    */
    bool save(const QSharedPointer<Calendar> &calendar, const QString &fileName) override;

    // make CalFromat::fromString(const QSharedPointer<Calendar> &calendar, const QString&, const QString&) visible here as well
    using CalFormat::fromString;

    /*!
      Parses a string, returning the first iCal component as an Incidence.

      \a string is a QString containing the data to be parsed.

      Returns non-zero pointer if the parsing was successful; 0 otherwise.
      \sa CalFormat::fromString(), fromRawString()
    */
    QSharedPointer<Incidence> fromString(const QString &string);

    /*!
      Parses a bytearray, returning the first iCal component as an Incidence, ignoring timezone information.

      This function is significantly faster than fromString by avoiding the overhead of parsing timezone information.
      Timezones are instead solely interpreted by using system-timezones.

      \a string is a utf8 QByteArray containing the data to be parsed.

      Returns non-zero pointer if the parsing was successful; 0 otherwise.
      \sa fromString(const QString &), fromRawString()
    */
    QSharedPointer<Incidence> readIncidence(const QByteArray &string);

    /*!
      Parses a string and fills a RecurrenceRule object with the information.

      \a rule is a pointer to a RecurrenceRule object.

      \a string is a QString containing the data to be parsed.

      Returns true if successful; false otherwise.
    */
    Q_REQUIRED_RESULT bool fromString(RecurrenceRule *rule, const QString &string); // TODO KF7: make this static

    /*!
      Parses a string representation of a duration.

      \a duration iCal representation of a duration.

      \since 5.95
    */
    Q_REQUIRED_RESULT Duration durationFromString(const QString &duration) const; // TODO KF7: make this static

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT bool fromRawString(const QSharedPointer<Calendar> &calendar, const QByteArray &string) override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT QString toString(const QSharedPointer<Calendar> &calendar) override;

    /*!
      Converts an Incidence to a QString.

      \a incidence is a pointer to an Incidence object to be converted
      into a QString.

      Returns the QString will be Null if the conversion was unsuccessful.
    */
    Q_REQUIRED_RESULT QString toString(const QSharedPointer<Incidence> &incidence);

    /*!
      Converts an Incidence to a QByteArray.

      \a incidence is a pointer to an Incidence object to be converted
      into a QByteArray.

      Returns the QString will be Null if the conversion was unsuccessful.
      \since 4.7
    */
    Q_REQUIRED_RESULT QByteArray toRawString(const QSharedPointer<Incidence> &incidence);

    /*!
      Converts a RecurrenceRule to a QString.

      \a rule is a pointer to a RecurrenceRule object to be converted
      into a QString.

      Returns the QString will be Null if the conversion was unsuccessful.
    */
    Q_REQUIRED_RESULT QString toString(RecurrenceRule *rule);

    /*!
      Converts a Duration to an iCal string.

      \a duration a Duration object.

      Returns iCal formatted duration
      \since 5.95
    */
    Q_REQUIRED_RESULT QString toString(const Duration &duration) const;

    /*!
      Converts an Incidence to iCalendar formatted text.

      \a incidence is a pointer to an Incidence object to be converted
      into iCal formatted text.

      Returns the QString will be Null if the conversion was unsuccessful.
    */
    Q_REQUIRED_RESULT QString toICalString(const QSharedPointer<Incidence> &incidence);

    /*!
      Creates a scheduling message string for an Incidence.

      \a incidence is a pointer to an IncidenceBase object to be scheduled.

      \a method is a Scheduler::Method

      Returns a QString containing the message if successful; 0 otherwise.
    */
    Q_REQUIRED_RESULT QString createScheduleMessage(const QSharedPointer<IncidenceBase> &incidence, iTIPMethod method); // TODO KF7 return a QByteArray instead

#if KCALENDARCORE_ENABLE_DEPRECATED_SINCE(6, 30)
    /*!
      Parses a Calendar scheduling message string into ScheduleMessage object.

      \a calendar is a pointer to a Calendar object associated with the
      scheduling message.

      \a string is a QString containing the data to be parsed.

      Returns a pointer to a ScheduleMessage object if successful; 0 otherwise.
      The calling routine may later free the return memory.
    */
    KCALENDARCORE_DEPRECATED_VERSION(6, 30, "use the QByteArray overload instead")
    QSharedPointer<ScheduleMessage> parseScheduleMessage(const QSharedPointer<Calendar> &calendar, const QString &string);
#endif

    /*!
      Parses a Calendar scheduling message string into ScheduleMessage object.

      \a calendar is a pointer to a Calendar object associated with the
      scheduling message. This calendar will also be searched for pre-existing
      instances of the incidence referred to in the scheduling message.

      \a messageText is containing the data to be parsed.

      Returns a pointer to a ScheduleMessage object if successful; 0 otherwise.
      The calling routine may later free the return memory.

      \since 6.30 (took a QString as argument previously)
    */
    [[nodiscard]] QSharedPointer<ScheduleMessage> parseScheduleMessage(const QSharedPointer<Calendar> &calendar, const QByteArray &messageText);

    /*!
      Converts a QString into a FreeBusy object.

      \a string is a QString containing the data to be parsed.
      Returns a pointer to a FreeBusy object if successful; 0 otherwise.

      \note Do not attempt to free the FreeBusy memory from the calling routine.
    */
    QSharedPointer<FreeBusy> parseFreeBusy(const QString &string);

    /*!
      Sets the iCalendar time zone.

      \a timeZone is the time zone to set.

      \sa timeZone().
    */
    void setTimeZone(const QTimeZone &timeZone);

    /*!
      Returns the iCalendar time zone.
      \sa setTimeZone().
    */
    Q_REQUIRED_RESULT QTimeZone timeZone() const;

    /*!
      Returns the timezone id string used by the iCalendar; an empty string
      if the iCalendar does not have a timezone.
    */
    Q_REQUIRED_RESULT QByteArray timeZoneId() const;

private:
    //@cond PRIVATE
    Q_DISABLE_COPY(ICalFormat)
    Q_DECLARE_PRIVATE(ICalFormat)
    //@endcond
};

}

#endif
