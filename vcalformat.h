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

class KCALCORE_EXPORT VCalFormat : public CalFormat
{
  public:
    VCalFormat();
    virtual ~VCalFormat();

    /**
      @copydoc
      CalFormat::load()
    */
    bool load( const MemoryCalendar::Ptr &calendar, const QString &fileName );

    /**
      @copydoc
      CalFormat::save()
    */
    bool save( const MemoryCalendar::Ptr &calendar, const QString &fileName );

    /**
      @copydoc
      CalFormat::fromString()
    */
    bool fromString( const MemoryCalendar::Ptr &calendar, const QString &string,
                     bool deleted = false, const QString &notebook = QString() );

    /**
      @copydoc
      CalFormat::toString()
    */
    QString toString( const MemoryCalendar::Ptr &calendar, const QString &notebook = QString(),
                      bool deleted = false );

    /**
      @copydoc
      CalFormat::fromRawString()
    */
    bool fromRawString( const MemoryCalendar::Ptr &calendar, const QByteArray &string,
                        bool deleted = false, const QString &notebook = QString() );

  protected:
    /**
      Translates a VObject of the TODO type into an Event.
    */
    Todo::Ptr VTodoToEvent( VObject *vtodo );

    /**
      Translates a VObject into a Event and returns a pointer to it.
    */
    Event::Ptr VEventToEvent( VObject *vevent );

    /**
      Translates an Event into a VTodo-type VObject and return pointer.
    */
    VObject *eventToVTodo( const Todo::Ptr &anEvent );

    /**
      Translates an Event into a VObject and returns a pointer to it.
    */
    VObject *eventToVEvent( const Event::Ptr &anEvent );

    /**
      Takes a QDate and returns a string in the format YYYYMMDDTHHMMSS.
    */
    QString qDateToISO( const QDate &date );

    /**
      Takes a QDateTime and returns a string in format YYYYMMDDTHHMMSS.
    */
    QString kDateTimeToISO( const KDateTime &date, bool zulu = true );

    /**
      Takes a string in YYYYMMDDTHHMMSS format and returns a valid QDateTime.
    */
    KDateTime ISOToKDateTime( const QString &dtStr );

    /**
      Takes a string in the YYYYMMDD format and returns a valid QDate.
    */
    QDate ISOToQDate( const QString &dtStr );

    /**
      Takes a vCalendar tree of VObjects, and puts all of them that have the
      "event" property into the dictionary, todos in the todo-list, etc.
    */
    void populate( VObject *vcal, bool deleted = false, const QString &notebook = QString() );

    /**
      Takes a number 0 - 6 and returns the two letter string of that day,
      i.e. MO, TU, WE, etc.

      @param day number of the day to get a two letter name for. Range @c 0 - @c 6
    */
    const char *dayFromNum( int day );

    /** the reverse of the above function. */
    int numFromDay( const QString &day );

    Attendee::PartStat readStatus( const char *s ) const;
    QByteArray writeStatus( Attendee::PartStat status ) const;

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
