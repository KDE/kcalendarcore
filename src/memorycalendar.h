/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 1998 Preston Brown <pbrown@kde.org>
  SPDX-FileCopyrightText: 2001, 2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the MemoryCalendar class.

  Very simple implementation of a Calendar that is only in memory
 */
#ifndef KCALCORE_MEMORYCALENDAR_H
#define KCALCORE_MEMORYCALENDAR_H

#include "calendar.h"
#include "kcalendarcore_export.h"

namespace KCalendarCore
{
/*!
  \class KCalendarCore::MemoryCalendar
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/MemoryCalendar
  \brief
  This class provides a calendar stored in memory.
*/
class KCALENDARCORE_EXPORT MemoryCalendar : public Calendar
{
    Q_OBJECT
public:
    /*!
      A shared pointer to a MemoryCalendar
    */
    typedef QSharedPointer<MemoryCalendar> Ptr;

    /*!
      Constructs a MemoryCalendar object with a specified time zone \a timeZone.
      The time zone is used as the default for creating or
      modifying incidences in the Calendar. The time zone does
      not alter existing incidences.

      \a timeZone is the time specification

    */
    explicit MemoryCalendar(const QTimeZone &timeZone);

    /*!
      Constructs a MemoryCalendar object using a time zone ID.
      The time zone ID is used as the default for creating or modifying
      incidences in the Calendar. The time zone does not alter existing
      incidences.

      \a timeZoneId is a string containing a time zone ID, which is
      assumed to be valid.  If no time zone is found, the viewing time
      specification is set to local time zone.

      Example: "Europe/Berlin"
    */
    explicit MemoryCalendar(const QByteArray &timeZoneId);

    /*!
      Destroys the calendar.
    */
    ~MemoryCalendar() override;

    /*!
      \reimp
    */
    void doSetTimeZone(const QTimeZone &timeZone) override;

    /*!
      \reimp
    */
    bool deleteIncidence(const Incidence::Ptr &incidence) override;

    /*!
       \reimp
    */
    bool deleteIncidenceInstances(const Incidence::Ptr &incidence) override;

    /*!
       \reimp
    */
    bool addIncidence(const Incidence::Ptr &incidence) override;

    // Event Specific Methods //

    /*!
      \reimp
    */
    bool addEvent(const Event::Ptr &event) override;

    /*!
      \reimp
    */
    bool deleteEvent(const Event::Ptr &event) override;

    /*!
      \reimp
    */
    bool deleteEventInstances(const Event::Ptr &event) override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Event::List rawEvents(EventSortField sortField = EventSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Event::List rawEvents(const QDate &start, const QDate &end, const QTimeZone &timeZone = {}, bool inclusive = false) const override;

    /*!
      Returns an unfiltered list of all Events which occur on the given date.

      \a date request unfiltered Event list for this QDate only.

      \a timeZone time zone to interpret \a date, or the calendar's
                      default time zone if none is specified

      \a sortField specifies the EventSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of unfiltered Events occurring on the specified QDate.
    */
    Q_REQUIRED_RESULT Event::List rawEventsForDate(const QDate &date,
                                                   const QTimeZone &timeZone = {},
                                                   EventSortField sortField = EventSortUnsorted,
                                                   SortDirection sortDirection = SortDirectionAscending) const override;

    /*!
     * Returns an incidence by \a identifier.
     * \sa Incidence::instanceIdentifier()
     * \since 4.11
     */
    Incidence::Ptr instance(const QString &identifier) const;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Event::Ptr event(const QString &uid, const QDateTime &recurrenceId = {}) const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Event::List eventInstances(const Incidence::Ptr &event,
                                                 EventSortField sortField = EventSortUnsorted,
                                                 SortDirection sortDirection = SortDirectionAscending) const override;

    // To-do Specific Methods //

    /*!
      \reimp
    */
    bool addTodo(const Todo::Ptr &todo) override;

    /*!
      \reimp
    */
    bool deleteTodo(const Todo::Ptr &todo) override;

    /*!
      \reimp
    */
    bool deleteTodoInstances(const Todo::Ptr &todo) override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Todo::List rawTodos(TodoSortField sortField = TodoSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const override;

    /*!
       \reimp
    */
    Q_REQUIRED_RESULT Todo::List rawTodos(const QDate &start, const QDate &end, const QTimeZone &timeZone = {}, bool inclusive = false) const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Todo::List rawTodosForDate(const QDate &date) const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Todo::Ptr todo(const QString &uid, const QDateTime &recurrenceId = {}) const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Todo::List
    todoInstances(const Incidence::Ptr &todo, TodoSortField sortField = TodoSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const override;

    // Journal Specific Methods //

    /*!
      \reimp
    */
    bool addJournal(const Journal::Ptr &journal) override;

    /*!
      \reimp
    */
    bool deleteJournal(const Journal::Ptr &journal) override;

    /*!
      \reimp
    */
    bool deleteJournalInstances(const Journal::Ptr &journal) override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Journal::List rawJournals(JournalSortField sortField = JournalSortUnsorted,
                                                SortDirection sortDirection = SortDirectionAscending) const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Journal::List rawJournalsForDate(const QDate &date) const override;

    /*!
      \reimp
    */
    Journal::Ptr journal(const QString &uid, const QDateTime &recurrenceId = {}) const override;

    /*!
      \reimp
                                          JournalSortField, SortDirection)const
    */
    Q_REQUIRED_RESULT Journal::List journalInstances(const Incidence::Ptr &journal,
                                                     JournalSortField sortField = JournalSortUnsorted,
                                                     SortDirection sortDirection = SortDirectionAscending) const override;

    // Alarm Specific Methods //

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT Alarm::List alarms(const QDateTime &from, const QDateTime &to, bool excludeBlockedAlarms = false) const override;

    /*!
      Return true if the memory calendar is updating the lastModified field
      of incidence owned by the calendar on any incidence change.

      \since 5.80
    */
    bool updateLastModifiedOnChange() const;

    /*!
      Govern if the memory calendar is changing the lastModified field of incidence
      it owns, on incidence updates.

      \a update, when true, the lastModified field of an incidence owned by the
      calendar is set to the current date time on any change of the incidence.

      \since 5.80
    */
    void setUpdateLastModifiedOnChange(bool update);

    /*!
      \reimp
    */
    void incidenceUpdate(const QString &uid, const QDateTime &recurrenceId) override;

    /*!
      \reimp
    */
    void incidenceUpdated(const QString &uid, const QDateTime &recurrenceId) override;

    using QObject::event; // prevent warning about hidden virtual method

protected:
    /*!
      \reimp
    */
    void virtual_hook(int id, void *data) override;

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond

    Q_DISABLE_COPY(MemoryCalendar)
};

}

#endif
