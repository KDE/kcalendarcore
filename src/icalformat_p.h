/*
  This file is part of the kcalcore library.

  Copyright (c) 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  Copyright (c) 2006 David Jarvie <software@astrojar.org.uk>
  Copyright (C) 2012  Christian Mollekopf <mollekopf@kolabsys.com>

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
  defines the internal ICalFormatImpl class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
  @author David Jarvie \<software@astrojar.org.uk\>
*/
#ifndef KCALCORE_ICALFORMAT_P_H
#define KCALCORE_ICALFORMAT_P_H

#include "exceptions.h"
#include "freebusy.h"
#include "todo.h"
#include "journal.h"
#include "event.h"
#include "person.h"
#include "calendar.h"
#include "schedulemessage.h"

#include <libical/ical.h>

class QDate;

namespace KCalendarCore
{

class Alarm;
class Attachment;
class Attendee;
class Duration;
class Event;
class FreeBusy;
class ICalFormat;
class ICalTimeZoneCache;
class Incidence;
class Journal;
class Recurrence;
class RecurrenceRule;
class Todo;

using TimeZoneList = QVector<QTimeZone>;

/**
  Tell the Libical library that we are using ICal Version 2.0.
  @internal
*/
#define _ICAL_VERSION "2.0"

/**
  Version of this library implementation
  @internal
*/
#define _ICAL_IMPLEMENTATION_VERSION "1.0"

/**
  @brief
  This class provides the libical dependent functions for ICalFormat.

  This class implements the iCalendar format. It provides methods for
  loading/saving/converting iCalendar format data into the internal
  representation as Calendar and Incidences.

  @internal
*/
class ICalFormatImpl
{
public:
    /**
      Construct a new iCal format for calendar object.
      @param parent is a pointer to a valid ICalFormat object.
    */
    explicit ICalFormatImpl(ICalFormat *parent);

    /**
      Destructor.
    */
    virtual ~ICalFormatImpl();

    /**
      Updates a calendar with data from a raw iCalendar. Incidences already
      existing in @p calendar are not affected except that if a new incidence
      with the same UID is found, the existing incidence is replaced.
    */
    bool populate(const Calendar::Ptr &calendar, icalcomponent *fs,
                  bool deleted = false, const QString &notebook = QString());

    Incidence::Ptr readOneIncidence(icalcomponent *calendar, const ICalTimeZoneCache *tzlist);

    icalcomponent *writeIncidence(const IncidenceBase::Ptr &incidence,
                                  iTIPMethod method = iTIPRequest,
                                  TimeZoneList *tzUsedList = nullptr);

    icalcomponent *writeTodo(const Todo::Ptr &todo, TimeZoneList *tzUsedList = nullptr);

    icalcomponent *writeEvent(const Event::Ptr &event, TimeZoneList *tzUsedList = nullptr);

    icalcomponent *writeJournal(const Journal::Ptr &journal, TimeZoneList *tzUsedList = nullptr);

    icalcomponent *writeFreeBusy(const FreeBusy::Ptr &freebusy,
                                 iTIPMethod method = iTIPPublish);

    void writeIncidence(icalcomponent *parent, const Incidence::Ptr &incidence, TimeZoneList *tzUsedList = nullptr);

    icalproperty *writeDescription(const QString &description, bool isRich = false);
    icalproperty *writeSummary(const QString &summary, bool isRich = false);
    icalproperty *writeLocation(const QString &location, bool isRich = false);
    icalproperty *writeAttendee(const Attendee &attendee);
    icalproperty *writeOrganizer(const Person &organizer);
    icalproperty *writeAttachment(const Attachment &attach);
    icalproperty *writeRecurrenceRule(Recurrence *);
    icalrecurrencetype writeRecurrenceRule(RecurrenceRule *recur);
    icalcomponent *writeAlarm(const Alarm::Ptr &alarm);

    QString extractErrorProperty(icalcomponent *);
    Todo::Ptr readTodo(icalcomponent *vtodo, const ICalTimeZoneCache *tzList);
    Event::Ptr readEvent(icalcomponent *vevent, const ICalTimeZoneCache *tzList);
    FreeBusy::Ptr readFreeBusy(icalcomponent *vfreebusy);
    Journal::Ptr readJournal(icalcomponent *vjournal, const ICalTimeZoneCache *tzList);
    Attendee readAttendee(icalproperty *attendee);
    Person readOrganizer(icalproperty *organizer);
    Attachment readAttachment(icalproperty *attach);
    void readIncidence(icalcomponent *parent, const Incidence::Ptr &incidence, const ICalTimeZoneCache *tzList);
    void readRecurrenceRule(icalproperty *rrule, const Incidence::Ptr &event);
    void readExceptionRule(icalproperty *rrule, const Incidence::Ptr &incidence);
    void readRecurrence(const struct icalrecurrencetype &r, RecurrenceRule *recur);
    void readAlarm(icalcomponent *alarm, const Incidence::Ptr &incidence);

    /**
      Returns the PRODID string loaded from calendar file.
    */
    QString loadedProductId() const;

    static icaltimetype writeICalDate(const QDate &);

    static QDate readICalDate(const icaltimetype &);

    static icaltimetype writeICalDateTime(const QDateTime &, bool dayOnly = false);

    static icaltimetype writeICalUtcDateTime(const QDateTime &, bool dayOnly = false);

    /**
      Creates an ical property from a date/time value.
      If a time zone is specified for the value, a TZID parameter is inserted
      into the ical property, @p tzlist and @p tzUsedList are updated to include
      the time zone. Note that while @p tzlist owns any time zone instances in
      its collection, @p tzUsedList does not.

      @param kind   kind of property
      @param dt     date/time value
      @param tzlist time zones collection
      @param tzUsedList time zones collection, only updated if @p tzlist
      is also specified
      @return property, or null if error. It is the caller's responsibility
      to free the returned property.
    */
    static icalproperty *writeICalDateTimeProperty(const icalproperty_kind kind,
                                                   const QDateTime &dt,
                                                   TimeZoneList *tzUsedList = nullptr);

    /**
      Converts a date/time from ICal format.

      @param p      property from which @p t has been obtained
      @param t      ICal format date/time
      @param utc    UTC date/time is expected
      @return date/time, converted to UTC if @p utc is @c true
    */
    static QDateTime readICalDateTime(icalproperty *p, const icaltimetype &t,
                                      const ICalTimeZoneCache *tzList,
                                      bool utc = false);

    /**
      Converts a UTC date/time from ICal format.
      If @p t is not a UTC date/time, it is treated as invalid.

      @param p ical parameter to read from
      @param t ICal format date/time
      @return date/time, or invalid if @p t is not UTC
    */
    static QDateTime readICalUtcDateTime(icalproperty *p, icaltimetype &t,
                                         const ICalTimeZoneCache *tzList = nullptr)
    {
        return readICalDateTime(p, t, tzList, true);
    }

    /**
      Reads a date or date/time value from a property.

      @param p      ical parameter to read from
      @param utc    true to read a UTC value, false to allow time zone
      to be specified.
      @return date or date/time, or invalid if property doesn't contain
      a time value.
    */
    static QDateTime readICalDateTimeProperty(icalproperty *p,
                                              const ICalTimeZoneCache *tzList,
                                              bool utc = false, bool *allDay = nullptr);

    /**
      Reads a UTC date/time value from a property.
      @param p is a pointer to a valid icalproperty structure.
    */
    static QDateTime readICalUtcDateTimeProperty(icalproperty *p, const ICalTimeZoneCache *tzList,
                                                 bool *allDay = nullptr)
    {
        return readICalDateTimeProperty(p, tzList, true, allDay);
    }

    static icaldurationtype writeICalDuration(const Duration &duration);

    static Duration readICalDuration(const icaldurationtype &d);

    static icaldatetimeperiodtype writeICalDatePeriod(const QDate &date);

    icalcomponent *createCalendarComponent(const Calendar::Ptr &calendar = Calendar::Ptr());

    icalcomponent *createScheduleComponent(const IncidenceBase::Ptr &incidence,
                                           iTIPMethod method);

protected:
    // void dumpIcalRecurrence( const icalrecurrencetype &r );

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond
};

}

#endif
