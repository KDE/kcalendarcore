/*
  This file is part of the kcalcore library.

  Copyright (c) 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  Copyright (c) 2006 David Jarvie <software@astrojar.org.uk>

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

#include <KDateTime>

#include <ical.h>

class QDate;

namespace KCalCore {

class Alarm;
class Attachment;
class Attendee;
class Duration;
class Event;
class FreeBusy;
class ICalFormat;
class ICalTimeZones;
class Incidence;
class Journal;
class Recurrence;
class RecurrenceRule;
class Todo;

/**
  Tell the Libical library that we are using ICal Version 2.0.
  @internal
*/
#define _ICAL_VERSION "2.0"

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
    explicit ICalFormatImpl( ICalFormat *parent );

    /**
      Destructor.
    */
    virtual ~ICalFormatImpl();

    /**
      Updates a calendar with data from a raw iCalendar. Incidences already
      existing in @p calendar are not affected except that if a new incidence
      with the same UID is found, the existing incidence is replaced.
    */
    bool populate( const Calendar::Ptr &calendar, icalcomponent *fs,
                   bool deleted = false, const QString &notebook = QString() );

    icalcomponent *writeIncidence( const IncidenceBase::Ptr &incidence,
                                   iTIPMethod method = iTIPRequest );

    icalcomponent *writeTodo( const Todo::Ptr &todo, ICalTimeZones *tzlist = 0,
                              ICalTimeZones *tzUsedList = 0 );

    icalcomponent *writeEvent( const Event::Ptr &event, ICalTimeZones *tzlist = 0,
                               ICalTimeZones *tzUsedList = 0 );

    icalcomponent *writeJournal( const Journal::Ptr &journal, ICalTimeZones *tzlist = 0,
                                 ICalTimeZones *tzUsedList = 0 );

    icalcomponent *writeFreeBusy( const FreeBusy::Ptr &freebusy,
                                  iTIPMethod method = iTIPPublish );

    void writeIncidence( icalcomponent *parent, const Incidence::Ptr &incidence,
                         ICalTimeZones *tzlist = 0, ICalTimeZones *tzUsedList = 0 );

    icalproperty *writeDescription( const QString &description, bool isRich = false );
    icalproperty *writeSummary( const QString &summary, bool isRich = false );
    icalproperty *writeLocation( const QString &location, bool isRich = false );
    icalproperty *writeAttendee( const Attendee::Ptr &attendee );
    icalproperty *writeOrganizer( const Person::Ptr &organizer );
    icalproperty *writeAttachment( const Attachment::Ptr &attach );
    icalproperty *writeRecurrenceRule( Recurrence * );
    icalrecurrencetype writeRecurrenceRule( RecurrenceRule *recur );
    icalcomponent *writeAlarm( const Alarm::Ptr &alarm );

    QString extractErrorProperty( icalcomponent * );
    Todo::Ptr readTodo( icalcomponent *vtodo, ICalTimeZones *tzlist );
    Event::Ptr readEvent( icalcomponent *vevent, ICalTimeZones *tzlist );
    FreeBusy::Ptr readFreeBusy( icalcomponent *vfreebusy );
    Journal::Ptr readJournal( icalcomponent *vjournal, ICalTimeZones *tzlist );
    Attendee::Ptr readAttendee( icalproperty *attendee );
    Person::Ptr readOrganizer( icalproperty *organizer );
    Attachment::Ptr readAttachment( icalproperty *attach );
    void readIncidence( icalcomponent *parent, Incidence::Ptr incidence,
                        ICalTimeZones *tzlist );
    void readRecurrenceRule( icalproperty *rrule, Incidence::Ptr event );
    void readExceptionRule( icalproperty *rrule, Incidence::Ptr incidence );
    void readRecurrence( const struct icalrecurrencetype &r,
                         RecurrenceRule *recur );
    void readAlarm( icalcomponent *alarm, Incidence::Ptr incidence,
                    ICalTimeZones *tzlist );

    /**
      Returns the PRODID string loaded from calendar file.
    */
    QString loadedProductId() const;

    static icaltimetype writeICalDate( const QDate & );

    static QDate readICalDate(icaltimetype);

    static icaltimetype writeICalDateTime( const KDateTime & );

    static icaltimetype writeICalUtcDateTime( const KDateTime & );

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
    static icalproperty *writeICalDateTimeProperty( const icalproperty_kind kind,
                                                    const KDateTime &dt,
                                                    ICalTimeZones *tzlist = 0,
                                                    ICalTimeZones *tzUsedList = 0 );

    /**
      Converts a date/time from ICal format.
      If the property @p p specifies a time zone using the TZID parameter,
      a match is searched for in @p tzlist. If no match is found, the time zone
      is added to @p tzlist.

      @param p      property from which @p t has been obtained
      @param t      ICal format date/time
      @param tzlist time zones collection
      @param utc    UTC date/time is expected
      @return date/time, converted to UTC if @p utc is @c true
    */
    static KDateTime readICalDateTime( icalproperty *p, const icaltimetype &t,
                                       ICalTimeZones *tzlist, bool utc = false );

    /**
      Converts a UTC date/time from ICal format.
      If @p t is not a UTC date/time, it is treated as invalid.

      @param p ical parameter to read from
      @param t ICal format date/time
      @param tzlist time zones collection
      @return date/time, or invalid if @p t is not UTC
    */
    static KDateTime readICalUtcDateTime( icalproperty *p, icaltimetype &t,
                                          ICalTimeZones *tzlist = 0 )
    { return readICalDateTime( p, t, tzlist, true ); }

    /**
      Reads a date or date/time value from a property.

      @param p      ical parameter to read from
      @param tzlist time zones collection
      @param utc    true to read a UTC value, false to allow time zone
      to be specified.
      @return date or date/time, or invalid if property doesn't contain
      a time value.
    */
    static KDateTime readICalDateTimeProperty( icalproperty *p,
                                               ICalTimeZones *tzlist, bool utc = false );

    /**
      Reads a UTC date/time value from a property.
      @param p is a pointer to a valid icalproperty structure.
    */
    static KDateTime readICalUtcDateTimeProperty( icalproperty *p )
    { return readICalDateTimeProperty( p, 0, true ); }

    static icaldurationtype writeICalDuration( const Duration &duration );

    static Duration readICalDuration( icaldurationtype d );

    static icaldatetimeperiodtype writeICalDatePeriod( const QDate &date );

    icalcomponent *createCalendarComponent( const Calendar::Ptr &calendar = Calendar::Ptr() );

    icalcomponent *createScheduleComponent( const IncidenceBase::Ptr &incidence,
                                            iTIPMethod method );

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
