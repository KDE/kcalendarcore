/*
  This file is part of the kcalcore library.

  Copyright (c) 1998 Preston Brown <pbrown@kde.org>
  Copyright (c) 2001-2003 Cornelius Schumacher <schumacher@kde.org>

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
  defines the VCalFormat base class.

  This class implements the vCalendar format. It provides methods for
  loading/saving/converting vCalendar format data into the internal
  representation as Calendar and Incidences.

  @brief
  vCalendar format implementation.

  @author Preston Brown \<pbrown@kde.org\>
  @author Cornelius Schumacher \<schumacher@kde.org\>
*/

#ifndef KCALCORE_VCALFORMAT_H
#define KCALCORE_VCALFORMAT_H

#include "kcalcore_export.h"
#include "attendee.h"
#include "calformat.h"
#include "event.h"
#include "todo.h"
#include "journal.h"

struct VObject;

class KDateTime;

class QDate;

#define _VCAL_VERSION "1.0"

namespace KCalCore {

class Event;
class Todo;

/**
  @brief
  vCalendar format implementation.

  This class implements the vCalendar format. It provides methods for
  loading/saving/converting vCalendar format data into the internal
  representation as Calendar and Incidences.
*/
class KCALCORE_EXPORT VCalFormat : public CalFormat
{
  public:
    /**
      Constructor a new vCalendar Format object.
    */
    VCalFormat();

    /**
      Destructor.
    */
    virtual ~VCalFormat();

    /**
      @copydoc
      CalFormat::load()
    */
    bool load( const Calendar::Ptr &calendar, const QString &fileName );

    /**
      @copydoc
      CalFormat::save()
    */
    bool save( const Calendar::Ptr &calendar, const QString &fileName );

    /**
      @copydoc
      CalFormat::fromString()
    */
    bool fromString( const Calendar::Ptr &calendar, const QString &string,
                     bool deleted = false, const QString &notebook = QString() );

    /**
      @copydoc
      CalFormat::toString()
    */
    QString toString( const Calendar::Ptr &calendar, const QString &notebook = QString(),
                      bool deleted = false );

    /**
      @copydoc
      CalFormat::fromRawString()
    */
    bool fromRawString( const Calendar::Ptr &calendar, const QByteArray &string,
                        bool deleted = false, const QString &notebook = QString() );

  protected:
    /**
      Translates a VObject of the TODO type into an Event.
      @param vtodo is a pointer to a valid VObject object.
    */
    Todo::Ptr VTodoToEvent( VObject *vtodo );

    /**
      Translates a VObject into a Event and returns a pointer to it.
      @param vevent is a pointer to a valid VObject object.
    */
    Event::Ptr VEventToEvent( VObject *vevent );

    /**
      Translates an Event into a VEvent-type VObject and returns a pointer to it.
      @param event is a pointer to a valid Event object.
    */
    VObject *eventToVEvent( const Event::Ptr &event );

    /**
      Parse TZ tag from vtimezone.
    */
    QString parseTZ( const QByteArray &timezone ) const;

    /**
      Parse DAYLIGHT tag from vtimezone.
    */
    QString parseDst( QByteArray &timezone ) const;

    /**
      Translates a Todo into a VTodo-type VObject and return pointer.
      @param todo is a pointer to a valid Todo object.
    */
    VObject *eventToVTodo( const Todo::Ptr &todo );

    /**
      Takes a QDate and returns a string in the format YYYYMMDDTHHMMSS.
      @param date is the date to format.
    */
    QString qDateToISO( const QDate &date );

    /**
      Takes a KDateTime and returns a string in format YYYYMMDDTHHMMSS.
      @param date is the date to format.
      @param zulu if true, then shift the date to UTC.
    */
    QString kDateTimeToISO( const KDateTime &date, bool zulu = true );

    /**
      Takes a string in YYYYMMDDTHHMMSS format and returns a valid KDateTime.
      @param dtStr is a QString containing the date to convert. If this value
      is invalid, then KDateTime() is returned.
    */
    KDateTime ISOToKDateTime( const QString &dtStr );

    /**
      Takes a string in the YYYYMMDD format and returns a valid QDate.
      @param dtStr is a QString containing the date to convert. If this value
      is invalid, then KDateTime() is returned.
    */
    QDate ISOToQDate( const QString &dtStr );

    /**
      Parse one of the myriad of ISO8601 timezone offset formats, e.g.
      +- hh : mm
      +- hh mm
      +- hh

      @param s string to be parsed.
      @param result timezone offset in seconds, if parse succeeded.
      @return Whether the parse succeeded or not.
    */
    bool parseTZOffsetISO8601( const QString &s, int &result );

    /**
      Takes a vCalendar tree of VObjects, and puts all of them that have the
      "event" property into the dictionary, todos in the todo-list, etc.
    */
    void populate( VObject *vcal, bool deleted = false, const QString &notebook = QString() );

    /**
      Takes a number 0 - 6 and returns the two letter string of that day,
      i.e. MO, TU, WE, etc.

      @param day number of the day to get a two letter name for. Range @c 0 - @c 6
      @see numFromDay().
    */
    const char *dayFromNum( int day );

    /**
      Converts a two letter representation of the day (i.e. MO, TU, WE, etc) and
      returns a number 0-6 corresponding to that ordinal day of the week.
      @param day is the QString containing the two letter day representation.
      @see dayFromNum().
    */
    int numFromDay( const QString &day );

    /**
      Converts a status string into an Attendee::PartStat.
      @param s is a null-terminated character string containing the status to convert.

      @return a valid Attendee::PartStat.  If the string provided is empty, null,
      or the contents are unrecognized, then Attendee::NeedsAction is returned.
    */
    Attendee::PartStat readStatus( const char *s ) const;

    /**
      Converts an Attendee::PartStat into a QByteArray string.
      @param status is the Attendee::PartStat to convert.

      @return a QByteArray containing the status string.
    */
    QByteArray writeStatus( Attendee::PartStat status ) const;

    void readCustomProperties( VObject *o, const Incidence::Ptr &i );
    void writeCustomProperties( VObject *o, const Incidence::Ptr &i );

  protected:
    /**
      @copydoc
      IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook( int id, void *data );

  private:
    /**
      The Pilot synchronization states.
    */
    enum PilotState {
      SYNCNONE = 0,
      SYNCMOD = 1,
      SYNCDEL = 3
    };

    //@cond PRIVATE
    Q_DISABLE_COPY( VCalFormat )
    class Private;
    Private *const d;
    //@endcond
};

}

#endif
