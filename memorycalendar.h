/*
  This file is part of the kcalcore library.

  Copyright (c) 1998 Preston Brown <pbrown@kde.org>
  Copyright (c) 2001,2003 Cornelius Schumacher <schumacher@kde.org>

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
  defines the MemoryCalendar class.

  Very simple implementation of a Calendar that is only in memory

  @author Preston Brown \<pbrown@kde.org\>
  @author Cornelius Schumacher \<schumacher@kde.org\>
 */
#ifndef KCALCORE_MEMORYCALENDAR_H
#define KCALCORE_MEMORYCALENDAR_H

#include "kcalcore_export.h"
#include "calendar.h"

namespace KCalCore {

class CalFormat;

/**
  @brief
  This class provides a calendar stored in memory.
*/
class KCALCORE_EXPORT MemoryCalendar : public Calendar
{
  public:

    /**
      A shared pointer to a MemoryCalendar
    */
    typedef QSharedPointer<MemoryCalendar> Ptr;

    /**
      @copydoc
      Calendar::Calendar(const KDateTime::Spec &)
    */
    explicit MemoryCalendar( const KDateTime::Spec &timeSpec );

    /**
      @copydoc
      Calendar::Calendar(const QString &)
    */
    explicit MemoryCalendar( const QString &timeZoneId );

    /**
      @copydoc
      Calendar::~Calendar()
    */
    ~MemoryCalendar();

    /**
      Clears out the current calendar, freeing all used memory etc. etc.
    */
    void close();

    /**
      @copydoc
      Calendar::deleteIncidence()
    */
    bool deleteIncidence( const Incidence::Ptr &incidence );

    /**
       @copydoc
       Calendar::deleteIncidenceInstances
    */
    bool deleteIncidenceInstances( const Incidence::Ptr &incidence );

    /**
       @copydoc
       Calendar::addIncidence()
    */
    bool addIncidence( const Incidence::Ptr &incidence );

    // Event Specific Methods //

    /**
      @copydoc
      Calendar::addEvent()
    */
    bool addEvent( const Event::Ptr &event );

    /**
      @copydoc
      Calendar::deleteEvent()
    */
    bool deleteEvent( const Event::Ptr &event );

    /**
      @copydoc
      Calendar::deleteEventInstances()
    */
    bool deleteEventInstances( const Event::Ptr &event );

    /**
      @copydoc
      Calendar::deleteAllEvents()
    */
    void deleteAllEvents();

    /**
      @copydoc
      Calendar::rawEvents(EventSortField, SortDirection)const
    */
    Event::List rawEvents(
      EventSortField sortField = EventSortUnsorted,
      SortDirection sortDirection = SortDirectionAscending ) const;

    /**
      @copydoc
      Calendar::rawEvents(const QDate &, const QDate &, const KDateTime::Spec &, bool)const
    */
    Event::List rawEvents( const QDate &start, const QDate &end,
                           const KDateTime::Spec &timeSpec = KDateTime::Spec(),
                           bool inclusive = false ) const;

    /**
      Returns an unfiltered list of all Events which occur on the given date.

      @param date request unfiltered Event list for this QDate only.
      @param timeSpec time zone etc. to interpret @p date, or the calendar's
                      default time spec if none is specified
      @param sortField specifies the EventSortField.
      @param sortDirection specifies the SortDirection.

      @return the list of unfiltered Events occurring on the specified QDate.
    */
    Event::List rawEventsForDate(
      const QDate &date, const KDateTime::Spec &timeSpec = KDateTime::Spec(),
      EventSortField sortField = EventSortUnsorted,
      SortDirection sortDirection = SortDirectionAscending ) const;

    /**
      @copydoc
      Calendar::rawEventsForDate(const KDateTime &)const
    */
    Event::List rawEventsForDate( const KDateTime &dt ) const;

    /**
      @copydoc
      Calendar::event()
    */
    Event::Ptr event(
      const QString &uid,
      const KDateTime &recurrenceId = KDateTime() ) const;

    /**
      @copydoc
      Calendar::deletedEvent()
    */
    Event::Ptr deletedEvent(
      const QString &uid, const KDateTime &recurrenceId = KDateTime() ) const;

    /**
      @copydoc
      Calendar::deletedEvents(EventSortField, SortDirection)const
    */
    Event::List deletedEvents(
      EventSortField sortField = EventSortUnsorted,
      SortDirection sortDirection = SortDirectionAscending ) const;

    /**
      @copydoc
      Calendar::eventInstances(const Incidence::Ptr &, EventSortField, SortDirection)const
    */
    Event::List eventInstances(
      const Incidence::Ptr &event,
      EventSortField sortField = EventSortUnsorted,
      SortDirection sortDirection = SortDirectionAscending ) const;

  // To-do Specific Methods //

    /**
      @copydoc
      Calendar::addTodo()
    */
    bool addTodo( const Todo::Ptr &todo );

    /**
      @copydoc
      Calendar::deleteTodo()
    */
    bool deleteTodo( const Todo::Ptr &todo );

    /**
      @copydoc
      Calendar::deleteTodoInstances()
    */
    bool deleteTodoInstances( const Todo::Ptr &todo );

    /**
      @copydoc
      Calendar::deleteAllTodos()
    */
    void deleteAllTodos();

    /**
      @copydoc
      Calendar::rawTodos(TodoSortField, SortDirection)const
    */
    Todo::List rawTodos(
      TodoSortField sortField = TodoSortUnsorted,
      SortDirection sortDirection = SortDirectionAscending ) const;

    /**
       @copydoc
       Calendar::rawTodos(const QDate &, const QDate &, const KDateTime::Spec &, bool)const
    */
    Todo::List rawTodos(
      const QDate &start, const QDate &end,
      const KDateTime::Spec &timespec = KDateTime::Spec(),
      bool inclusive = false ) const;

    /**
      @copydoc
      Calendar::rawTodosForDate()
    */
    Todo::List rawTodosForDate( const QDate &date ) const;

    /**
      @copydoc
      Calendar::todo()
    */
    Todo::Ptr todo( const QString &uid,
                    const KDateTime &recurrenceId = KDateTime() ) const;

    /**
      @copydoc
      Calendar::deletedTodo()
    */
    Todo::Ptr deletedTodo( const QString &uid, const KDateTime &recurrenceId = KDateTime() ) const;

    /**
      @copydoc
      Calendar::deletedTodos(TodoSortField, SortDirection)const
    */
    Todo::List deletedTodos(
      TodoSortField sortField = TodoSortUnsorted,
      SortDirection sortDirection = SortDirectionAscending ) const;

    /**
      @copydoc
      Calendar::todoInstances(const Incidence::Ptr &, TodoSortField, SortDirection)const
    */
    Todo::List todoInstances( const Incidence::Ptr &todo,
                              TodoSortField sortField = TodoSortUnsorted,
                              SortDirection sortDirection = SortDirectionAscending ) const;

  // Journal Specific Methods //

    /**
      @copydoc
      Calendar::addJournal()
    */
    bool addJournal( const Journal::Ptr &journal );

    /**
      @copydoc
      Calendar::deleteJournal()
    */
    bool deleteJournal( const Journal::Ptr &journal );

    /**
      @copydoc
      Calendar::deleteJournalInstances()
    */
    bool deleteJournalInstances( const Journal::Ptr &journal );

    /**
      @copydoc
      Calendar::deleteAllJournals()
    */
    void deleteAllJournals();

    /**
      @copydoc
      Calendar::rawJournals()
    */
    Journal::List rawJournals(
      JournalSortField sortField = JournalSortUnsorted,
      SortDirection sortDirection = SortDirectionAscending ) const;

    /**
      @copydoc
      Calendar::rawJournalsForDate()
    */
    Journal::List rawJournalsForDate( const QDate &date ) const;

    /**
      @copydoc
      Calendar::journal()
    */
    Journal::Ptr journal( const QString &uid,
                          const KDateTime &recurrenceId = KDateTime() ) const;

    /**
      @copydoc
      Calendar::deletedJournal()
    */
    Journal::Ptr deletedJournal( const QString &uid,
                                 const KDateTime &recurrenceId = KDateTime() ) const;

    /**
      @copydoc
      Calendar::deletedJournals(JournalSortField, SortDirection)const
    */
    Journal::List deletedJournals(
      JournalSortField sortField = JournalSortUnsorted,
      SortDirection sortDirection = SortDirectionAscending ) const;

    /**
      @copydoc
      Calendar::journalInstances(const Incidence::Ptr &, JournalSortField, SortDirection)const
    */
    Journal::List journalInstances( const Incidence::Ptr &journal,
                                 JournalSortField sortField = JournalSortUnsorted,
                                 SortDirection sortDirection = SortDirectionAscending ) const;

  // Alarm Specific Methods //

    /**
      @copydoc
      Calendar::alarms()
    */
    Alarm::List alarms( const KDateTime &from, const KDateTime &to ) const;

    /**
      Return a list of Alarms that occur before the specified timestamp.

      @param to is the ending timestamp.
      @return the list of Alarms occurring before the specified KDateTime.
    */
    Alarm::List alarmsTo( const KDateTime &to ) const;

    /**
      @copydoc
      Calendar::incidenceUpdate(const QString &,const KDateTime &)
    */
    void incidenceUpdate( const QString &uid, const KDateTime &recurrenceId );

    /**
      @copydoc
      Calendar::incidenceUpdated(const QString &,const KDateTime &)
    */
    void incidenceUpdated( const QString &uid, const KDateTime &recurrenceId );

    using QObject::event;   // prevent warning about hidden virtual method

  protected:
    /**
      @copydoc
      IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook( int id, void *data );

  private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond

    Q_DISABLE_COPY( MemoryCalendar )
};

}

#endif
