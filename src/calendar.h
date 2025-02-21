/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 1998 Preston Brown <pbrown@kde.org>
  SPDX-FileCopyrightText 2001, 2003, 2004 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  SPDX-FileCopyrightText: 2006 David Jarvie <djarvie@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Calendar class.
*/

/*

TODO: KDE5:

This API needs serious cleaning up:
- Most (all) methods aren't async ( deleteIncidence(), addIncidence(), load(), save(), ... )
  so it's not very easy to make a derived class that loads from akonadi.

- It has too many methods. Why do we need fooEvent()/fooJournal()/fooTodo() when fooIncidence()
  should be enough.

*/

#ifndef KCALCORE_CALENDAR_H
#define KCALCORE_CALENDAR_H

#include "customproperties.h"
#include "event.h"
#include "incidence.h"
#include "journal.h"
#include "kcalendarcore_export.h"
#include "todo.h"

#include <QDateTime>
#include <QIcon>
#include <QObject>
#include <QTimeZone>

/*!
 * \namespace KCalendarCore
 * \inmodule KCalendarCore
 *
 * \brief Namespace for all KCalendarCore types.
 */
namespace KCalendarCore
{
Q_NAMESPACE_EXPORT(KCALENDARCORE_EXPORT)

class CalFilter;
class Person;
class ICalFormat;

/*!
  \enum KCalendarCore::SortDirection
  \brief Calendar Incidence sort directions.

  \value SortDirectionAscending
  Sort in ascending order (first to last).
  \value SortDirectionDescending
  Sort in descending order (last to first).
*/
enum SortDirection {
    SortDirectionAscending,
    SortDirectionDescending,
};

/*!
  \enum KCalendarCore::EventSortField
  \brief Calendar Event sort keys.

  \value EventSortUnsorted
  Do not sort Events.
  \value EventSortStartDate
  Sort Events chronologically, by start date.
  \value EventSortEndDate
  Sort Events chronologically, by end date.
  \value EventSortSummary
  Sort Events alphabetically, by summary.
*/
enum EventSortField {
    EventSortUnsorted,
    EventSortStartDate,
    EventSortEndDate,
    EventSortSummary,
};

/*!
  \enum KCalendarCore::TodoSortField
  \brief Calendar Todo sort keys.

  \since 5.83.

  \value TodoSortUnsorted
  Do not sort Todos.
  \value TodoSortStartDate
  Sort Todos chronologically, by start date.
  \value TodoSortDueDate
  Sort Todos chronologically, by due date.
  \value TodoSortPriority
  Sort Todos by priority.
  \value TodoSortPercentComplete
  Sort Todos by percentage completed.
  \value TodoSortSummary
  Sort Todos alphabetically, by summary.
  \value TodoSortCreated
  Sort Todos chronologically, by creation date.
  \value TodoSortCategories
  Sort Todos by categories (tags)
*/
enum TodoSortField {
    TodoSortUnsorted,
    TodoSortStartDate,
    TodoSortDueDate,
    TodoSortPriority,
    TodoSortPercentComplete,
    TodoSortSummary,
    TodoSortCreated,
    TodoSortCategories,
};

/*!
  \enum KCalendarCore::JournalSortField
  \brief Calendar Journal sort keys.

  \value JournalSortUnsorted
  Do not sort Journals.
  \value JournalSortDate
  Sort Journals chronologically by date.
  \value JournalSortSummary
  Sort Journals alphabetically, by summary.
*/
enum JournalSortField {
    JournalSortUnsorted,
    JournalSortDate,
    JournalSortSummary,
};

/*!
  \enum KCalendarCore::AccessMode
  \brief The calendar's access mode, i.e. whether it can be written to or is read only.
  \since 5.85

  \value ReadOnly
  \value ReadWrite
*/
enum AccessMode {
    ReadOnly,
    ReadWrite,
};
Q_ENUM_NS(AccessMode)

/*!
  \qmlvaluetype calendar
  \inqmlmodule org.kde.kcalendarcore
  \nativetype KCalendarCore::Calendar
  \brief
  Represents the main calendar class.

  A calendar contains information like incidences (events, to-dos, journals),
  alarms, time zones, and other useful information.

  This is an abstract base class defining the interface to a calendar.
  It is implemented by subclasses like MemoryCalendar, which use different
  methods to store and access the data.

  \b {Ownership of Incidences}:

  Incidence ownership is handled by the following policy: as soon as an
  incidence (or any other subclass of IncidenceBase) is added to the
  Calendar by an add...() method it is owned by the Calendar object.
  The Calendar takes care of deleting the incidence using the delete...()
  methods. All Incidences returned by the query functions are returned
  as pointers so that changes to the returned Incidences are immediately
  visible in the Calendar.
*/

/*!
  \class KCalendarCore::Calendar
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Calendar

  \brief
  Represents the main calendar class.

  A calendar contains information like incidences (events, to-dos, journals),
  alarms, time zones, and other useful information.

  This is an abstract base class defining the interface to a calendar.
  It is implemented by subclasses like MemoryCalendar, which use different
  methods to store and access the data.

  \b {Ownership of Incidences}:

  Incidence ownership is handled by the following policy: as soon as an
  incidence (or any other subclass of IncidenceBase) is added to the
  Calendar by an add...() method it is owned by the Calendar object.
  The Calendar takes care of deleting the incidence using the delete...()
  methods. All Incidences returned by the query functions are returned
  as pointers so that changes to the returned Incidences are immediately
  visible in the Calendar.

  \warning Do not attempt to 'delete' any Incidence
  object you get from Calendar -- use the delete...() methods.
*/
class KCALENDARCORE_EXPORT Calendar : public QObject, public CustomProperties, public IncidenceBase::IncidenceObserver
{
    Q_OBJECT

    /*!
     * \qmlproperty string calendar::productId
     */

    /*!
     * \property KCalendarCore::Calendar::productId
     */
    Q_PROPERTY(QString productId READ productId WRITE setProductId) // clazy:exclude=qproperty-without-notify

    /*!
     * \qmlproperty KCalendarCore::Person calendar::owner
     */

    /*!
     * \property KCalendarCore::Calendar::owner
     */
    Q_PROPERTY(KCalendarCore::Person owner READ owner WRITE setOwner NOTIFY ownerChanged)

    /*!
     * \qmlproperty string calendar::id
     */

    /*!
     * \property KCalendarCore::Calendar::id
     */
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)

    /*!
     * \qmlproperty string calendar::name
     */

    /*!
     * \property KCalendarCore::Calendar::name
     */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    /*!
     * \qmlproperty Icon calendar::icon
     */

    /*!
     * \property KCalendarCore::Calendar::icon
     */
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)

    /*!
     * \qmlproperty KCalendarCore::AccessMode calendar::accessMode
     */

    /*!
     * \property KCalendarCore::Calendar::accessMode
     */
    Q_PROPERTY(KCalendarCore::AccessMode accessMode READ accessMode WRITE setAccessMode NOTIFY accessModeChanged)

    /*!
     * \qmlproperty bool calendar::isLoading
     */

    /*!
     * \property KCalendarCore::Calendar::isLoading
     */
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    /*!
      A shared pointer to a Calendar
    */
    typedef QSharedPointer<Calendar> Ptr;

    /*!
      Constructs a calendar with a specified time zone \a timeZone.
      The time zone is used as the default for creating or
      modifying incidences in the Calendar. The time zone does
      not alter existing incidences.

      \a timeZone is the time specification

    */
    explicit Calendar(const QTimeZone &timeZone);

    /*!
      Construct Calendar object using a time zone ID.
      The time zone ID is used as the default for creating or modifying
      incidences in the Calendar. The time zone does not alter existing
      incidences.

      \a timeZoneId is a string containing a time zone ID, which is
      assumed to be valid.  If no time zone is found, the viewing time
      specification is set to local time zone.

      Example: "Europe/Berlin"
    */
    explicit Calendar(const QByteArray &timeZoneId);

    /*!
      Destroys the calendar.
    */
    ~Calendar() override;

    /*!
      Sets the calendar Product ID to \a id.

      \a id is a string containing the Product ID.

      \sa productId()
    */
    void setProductId(const QString &id);

    /*!
      Returns the calendar's Product ID.

      \sa setProductId()
    */
    Q_REQUIRED_RESULT QString productId() const;

    /*!
      Sets the owner of the calendar to \a owner.

      \a owner is a Person object. Must be a non-null pointer.

      \sa owner()
    */
    void setOwner(const Person &owner);

    /*!
      Returns the owner of the calendar.

      Returns the owner Person object.

      \sa setOwner()
    */
    Q_REQUIRED_RESULT Person owner() const;

    /*!
      Sets the default time specification zone used for creating
      or modifying incidences in the Calendar.

      \a timeZone The time zone

    */
    void setTimeZone(const QTimeZone &timeZone);

    /*!
       Get the time zone used for creating or
       modifying incidences in the Calendar.

       Returns time specification
    */
    Q_REQUIRED_RESULT QTimeZone timeZone() const;

    /*!
      Sets the time zone ID used for creating or modifying incidences in the
      Calendar. This method has no effect on existing incidences.

      \a timeZoneId is a string containing a time zone ID, which is
      assumed to be valid. The time zone ID is used to set the time zone
      for viewing Incidence date/times. If no time zone is found, the
      viewing time specification is set to local time zone.

      Example: "Europe/Berlin"

      \sa setTimeZone()
    */
    void setTimeZoneId(const QByteArray &timeZoneId);

    /*!
      Returns the time zone ID used for creating or modifying incidences in
      the calendar.

      Returns the string containing the time zone ID, or empty string if the
              creation/modification time specification is not a time zone.
    */
    Q_REQUIRED_RESULT QByteArray timeZoneId() const;

    /*!
      Shifts the times of all incidences so that they appear at the same clock
      time as before but in a new time zone. The shift is done from a viewing
      time zone rather than from the actual incidence time zone.

      For example, shifting an incidence whose start time is 09:00 America/New York,
      using an old viewing time zone (\a oldZone) of Europe/London, to a new time
      zone (\a newZone) of Europe/Paris, will result in the time being shifted
      from 14:00 (which is the London time of the incidence start) to 14:00 Paris
      time.

      \a oldZone the time zone which provides the clock times

      \a newZone the new time zone

    */
    void shiftTimes(const QTimeZone &oldZone, const QTimeZone &newZone);

    /*!
      Sets if the calendar has been modified.

      \a modified is true if the calendar has been modified since open
      or last save.

      \sa isModified()
    */
    void setModified(bool modified);

    /*!
      Determine the calendar's modification status.

      Returns true if the calendar has been modified since open or last save.

      \sa setModified()
    */
    Q_REQUIRED_RESULT bool isModified() const;

    /*!
     * Returns a unique identifier for this calendar.
     * \since 5.85
     * \sa setId()
     */
    QString id() const;

    /*!
     * Sets a unique identifier \a id for this calendar.
     * \since 5.85
     * \sa id()
     */
    void setId(const QString &id);

    /*!
     * Returns the user-visible name for this calendar.
     * \since 5.85
     * \sa setName()
     */
    QString name() const;

    /*!
     * Sets the user-visible \a name for this calendar.
     * \since 5.85
     * \sa name()
     */
    void setName(const QString &name);

    /*!
     * Returns this calendar's icon.
     * \since 5.85
     */
    QIcon icon() const;

    /*!
     * Sets this calendar's \a icon.
     * \since 5.85
     * \sa icon()
     */
    void setIcon(const QIcon &icon);

    /*!
     * Returns this calendar's AccessMode, i.e. whether it is writable or read-only.
     * Defaults to ReadWrite.
     * \since 5.85
     * \sa setAccessMode()
     */
    AccessMode accessMode() const;

    /*!
     * Sets this calendar's AccessMode to \a mode, i.e. whether it is writable or read-only.
     * \since 5.85
     * \sa accessMode()
     */
    void setAccessMode(const AccessMode mode);

    /*!
     * Returns \c true if the calendar is still loading its data and thus
     * read access will not return complete (or even any) results.
     * \since 5.96
     */
    bool isLoading() const;

    /*!
      Returns a list of all categories used by Incidences in this Calendar.

      Returns a QStringList containing all the categories.
    */
    Q_REQUIRED_RESULT QStringList categories() const;

    // Incidence Specific Methods //

    /*!
       Call this to tell the calendar that you're adding a batch of incidences.
       So it doesn't, for example, ask the destination for each incidence.

        \sa endBatchAdding()
    */
    virtual void startBatchAdding();

    /*!
       Tells the Calendar that you stopped adding a batch of incidences.

       \sa startBatchAdding()
     */
    virtual void endBatchAdding();

    /*!
       Returns true if batch adding is in progress
    */
    Q_REQUIRED_RESULT bool batchAdding() const;

    /*!
      Inserts an Incidence into the calendar.

      \a incidence is a pointer to the Incidence to insert.

      Returns true if the Incidence was successfully inserted; false otherwise.

      \sa deleteIncidence()
    */
    virtual bool addIncidence(const Incidence::Ptr &incidence);

    /*!
      Removes an Incidence from the calendar.

      \a incidence is a pointer to the Incidence to remove.

      Returns true if the Incidence was successfully removed; false otherwise.

      \sa addIncidence()
    */
    virtual bool deleteIncidence(const Incidence::Ptr &incidence);

    /*!
      Returns a filtered list of all Incidences for this Calendar.

      Returns the list of all filtered Incidences.
    */
    virtual Incidence::List incidences() const;

    /*!
      Returns a filtered list of all Incidences which occur on the given date.

      \a date request filtered Incidence list for this QDate only.

      Returns the list of filtered Incidences occurring on the specified date.
    */
    virtual Incidence::List incidences(const QDate &date) const;

    /*!
      Returns an unfiltered list of all Incidences for this Calendar.

      Returns the list of all unfiltered Incidences.
    */
    virtual Incidence::List rawIncidences() const;

    /*!
      Returns an unfiltered list of all exceptions of this recurring incidence.

      \a incidence incidence to check

      Returns the list of all unfiltered exceptions.
    */
    virtual Incidence::List instances(const Incidence::Ptr &incidence) const;

    /*!
      Returns the Incidence associated with the given unique identifier.

      \a uid is a unique identifier string.

      \a recurrenceId is possible recurrenceid of incidence, default is null

      Returns a pointer to the Incidence.
      A null pointer is returned if no such Incidence exists.
    */
    Incidence::Ptr incidence(const QString &uid, const QDateTime &recurrenceId = {}) const;

    /*!
      Delete all incidences that are instances of recurring incidence \a incidence.

      \a incidence is a pointer to a deleted Incidence

      Returns true if delete was successful; false otherwise
    */
    virtual bool deleteIncidenceInstances(const Incidence::Ptr &incidence) = 0;

    /*!
      Returns the Incidence associated with the given scheduling identifier.

      \a sid is a unique scheduling identifier string.

      Returns a pointer to the Incidence.
      A null pointer is returned if no such Incidence exists.
    */
    virtual Incidence::Ptr incidenceFromSchedulingID(const QString &sid) const;

    /*!
      Searches all events and todos for an incidence with this
      scheduling identifier. Returns a list of matching results.

      \a sid is a unique scheduling identifier string.

     */
    virtual Incidence::List incidencesFromSchedulingID(const QString &sid) const;

    /*!
      Create a merged list of Events, Todos, and Journals.

      \a events is an Event list to merge.

      \a todos is a Todo list to merge.

      \a journals is a Journal list to merge.

      Returns a list of merged Incidences.
    */
    static Incidence::List mergeIncidenceList(const Event::List &events, const Todo::List &todos, const Journal::List &journals);

    /*!
      Flag that a change to a Calendar Incidence is starting.

      \a incidence is a pointer to the Incidence that will be changing.

    */
    virtual bool beginChange(const Incidence::Ptr &incidence);

    /*!
      Flag that a change to a Calendar Incidence has completed.

      \a incidence is a pointer to the Incidence that was changed.

    */
    virtual bool endChange(const Incidence::Ptr &incidence);

    /*!
      Creates an exception for an occurrence from a recurring Incidence.

      The returned exception is not automatically inserted into the calendar.

      \a incidence is a pointer to a recurring Incidence.

      \a recurrenceId specifies the specific occurrence for which the
      exception applies.

      \a thisAndFuture specifies if the exception applies only this specific
      occcurrence or also to all future occurrences.

      Returns a pointer to a new exception incidence with \a recurrenceId set.
      \since 4.11
    */
    static Incidence::Ptr createException(const Incidence::Ptr &incidence, const QDateTime &recurrenceId, bool thisAndFuture = false);

    // Event Specific Methods //

    /*!
      Inserts an Event into the calendar.

      \a event is a pointer to the Event to insert.

      Returns true if the Event was successfully inserted; false otherwise.

      \sa deleteEvent()
    */
    virtual bool addEvent(const Event::Ptr &event) = 0;

    /*!
      Removes an Event from the calendar.

      \a event is a pointer to the Event to remove.

      Returns true if the Event was successfully remove; false otherwise.

      \sa addEvent()
    */
    virtual bool deleteEvent(const Event::Ptr &event) = 0;

    /*!
      Delete all events that are instances of recurring event \a event.

      \a event is a pointer to a deleted Event

      Returns true if delete was successful; false otherwise
    */
    virtual bool deleteEventInstances(const Event::Ptr &event) = 0;

    /*!
      Sort a list of Events.

      \a eventList the list of events that should be sorted. The list is sorted in place and returned.

      \a sortField specifies the EventSortField.

      \a sortDirection specifies the SortDirection.

      Returns a list of Events sorted as specified.
      \since 5.95
    */
    static Event::List sortEvents(Event::List &&eventList, EventSortField sortField, SortDirection sortDirection);

    /*!
      Returns a sorted, filtered list of all Events for this Calendar.

      \a sortField specifies the EventSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of all filtered Events sorted as specified.
    */
    virtual Event::List events(EventSortField sortField = EventSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const;

    /*!
      Returns a filtered list of all Events which occur on the given timestamp.

      \a dt request filtered Event list for this QDateTime only.

      Returns the list of filtered Events occurring on the specified timestamp.
    */
    Q_REQUIRED_RESULT Event::List events(const QDateTime &dt) const;

    /*!
      Returns a filtered list of all Events occurring within a date range.

      \a start is the starting date.

      \a end is the ending date.

      \a timeZone time zone to interpret \a start and \a end,
                      or the calendar's default time zone if none is specified

      \a inclusive if true only Events which are completely included
      within the date range are returned.

      Returns the list of filtered Events occurring within the specified
      date range.
    */
    Q_REQUIRED_RESULT Event::List events(const QDate &start, const QDate &end, const QTimeZone &timeZone = {}, bool inclusive = false) const;

    /*!
      Returns a sorted, filtered list of all Events which occur on the given
      date.  The Events are sorted according to \a sortField and
      \a sortDirection.

      \a date request filtered Event list for this QDate only.

      \a timeZone time zone to interpret \a date
                      or the calendar's default time zone if none is specified

      \a sortField specifies the EventSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of sorted, filtered Events occurring on \a date.
    */
    Q_REQUIRED_RESULT Event::List events(const QDate &date,
                                         const QTimeZone &timeZone = {},
                                         EventSortField sortField = EventSortUnsorted,
                                         SortDirection sortDirection = SortDirectionAscending) const;

    /*!
      Returns a sorted, unfiltered list of all Events for this Calendar.

      \a sortField specifies the EventSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of all unfiltered Events sorted as specified.
    */
    virtual Event::List rawEvents(EventSortField sortField = EventSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const = 0;

    /*!
      Returns an unfiltered list of all Events occurring within a date range.

      \a start is the starting date

      \a end is the ending date

      \a timeZone time zone to interpret \a start and \a end,
                      or the calendar's default time zone if none is specified

      \a inclusive if true only Events which are completely included
      within the date range are returned.

      Returns the list of unfiltered Events occurring within the specified
      date range.
    */
    virtual Event::List rawEvents(const QDate &start, const QDate &end, const QTimeZone &timeZone = {}, bool inclusive = false) const = 0;

    /*!
      Returns a sorted, unfiltered list of all Events which occur on the given
      date.  The Events are sorted according to \a sortField and
      \a sortDirection.

      \a date request unfiltered Event list for this QDate only

      \a timeZone time zone to interpret \a date,
                      or the calendar's default time zone if none is specified

      \a sortField specifies the EventSortField

      \a sortDirection specifies the SortDirection

      Returns the list of sorted, unfiltered Events occurring on \a date
    */
    virtual Event::List rawEventsForDate(const QDate &date,
                                         const QTimeZone &timeZone = {},
                                         EventSortField sortField = EventSortUnsorted,
                                         SortDirection sortDirection = SortDirectionAscending) const = 0;

    /*!
      Returns the Event associated with the given unique identifier.

      \a uid is a unique identifier string.

      \a recurrenceId is possible recurrenceId of event, default is null

      Returns a pointer to the Event.
      A null pointer is returned if no such Event exists.
    */
    virtual Event::Ptr event(const QString &uid, const QDateTime &recurrenceId = {}) const = 0;

    /*!
      Returns a sorted, unfiltered list of all possible instances for this recurring Event.

      \a event event to check for. Caller guarantees it's of type Event.

      \a sortField specifies the EventSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of all unfiltered event instances sorted as specified.
    */
    virtual Event::List
    eventInstances(const Incidence::Ptr &event, EventSortField sortField = EventSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const = 0;

    // Todo Specific Methods //

    /*!
      Inserts a Todo into the calendar.

      \a todo is a pointer to the Todo to insert.

      Returns true if the Todo was successfully inserted; false otherwise.

      \sa deleteTodo()
    */
    virtual bool addTodo(const Todo::Ptr &todo) = 0;

    /*!
      Removes a Todo from the calendar.

      \a todo is a pointer to the Todo to remove.

      Returns true if the Todo was successfully removed; false otherwise.

      \sa addTodo()
    */
    virtual bool deleteTodo(const Todo::Ptr &todo) = 0;

    /*!
      Delete all to-dos that are instances of recurring to-do \a todo.

      \a todo is a pointer to a deleted Todo

      Returns true if delete was successful; false otherwise
    */
    virtual bool deleteTodoInstances(const Todo::Ptr &todo) = 0;

    /*!
      Sort a list of Todos.

      \a todoList the list of todos that should be sorted. The list is sorted in place and returned.

      \a sortField specifies the TodoSortField.

      \a sortDirection specifies the SortDirection.

      Returns a list of Todos sorted as specified.

      \since 5.95
    */
    static Todo::List sortTodos(Todo::List &&todoList, TodoSortField sortField, SortDirection sortDirection);

    /*!
      Returns a sorted, filtered list of all Todos for this Calendar.

      \a sortField specifies the TodoSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of all filtered Todos sorted as specified.
    */
    virtual Todo::List todos(TodoSortField sortField = TodoSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const;

    /*!
      Returns a filtered list of all Todos which are due on the specified date.

      \a date request filtered Todos due on this QDate.

      Returns the list of filtered Todos due on the specified date.
    */
    virtual Todo::List todos(const QDate &date) const;

    /*!
      Returns a filtered list of all Todos occurring within a date range.

      \a start is the starting date

      \a end is the ending date

      \a timeZone time zone to interpret \a start and \a end,
                      or the calendar's default time zone if none is specified

      \a inclusive if true only Todos which are completely included
      within the date range are returned.

      Returns the list of filtered Todos occurring within the specified
      date range.
    */
    virtual Todo::List todos(const QDate &start, const QDate &end, const QTimeZone &timeZone = {}, bool inclusive = false) const;

    /*!
      Returns a sorted, unfiltered list of all Todos for this Calendar.

      \a sortField specifies the TodoSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of all unfiltered Todos sorted as specified.
    */
    virtual Todo::List rawTodos(TodoSortField sortField = TodoSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const = 0;

    /*!
      Returns an unfiltered list of all Todos which due on the specified date.

      \a date request unfiltered Todos due on this QDate.

      Returns the list of unfiltered Todos due on the specified date.
    */
    virtual Todo::List rawTodosForDate(const QDate &date) const = 0;

    /*!
      Returns an unfiltered list of all Todos occurring within a date range.

      \a start is the starting date

      \a end is the ending date

      \a timeZone time zone to interpret \a start and \a end,
                      or the calendar's default time zone if none is specified

      \a inclusive if true only Todos which are completely included
      within the date range are returned.

      Returns the list of unfiltered Todos occurring within the specified
      date range.
    */
    virtual Todo::List rawTodos(const QDate &start, const QDate &end, const QTimeZone &timeZone = {}, bool inclusive = false) const = 0;

    /*!
      Returns the Todo associated with the given unique identifier.

      \a uid is a unique identifier string.

      \a recurrenceId is possible recurrenceId of todo, default is null

      Returns a pointer to the Todo.
      A null pointer is returned if no such Todo exists.
    */
    virtual Todo::Ptr todo(const QString &uid, const QDateTime &recurrenceId = {}) const = 0;

    /*!
      Returns a sorted, unfiltered list of all possible instances for this recurring Todo.

      \a todo todo to check for. Caller guarantees it's of type Todo.

      \a sortField specifies the TodoSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of all unfiltered todo instances sorted as specified.
    */
    virtual Todo::List
    todoInstances(const Incidence::Ptr &todo, TodoSortField sortField = TodoSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const = 0;

    // Journal Specific Methods //

    /*!
      Inserts a Journal into the calendar.

      \a journal is a pointer to the Journal to insert.

      Returns true if the Journal was successfully inserted; false otherwise.

      \sa deleteJournal()
    */
    virtual bool addJournal(const Journal::Ptr &journal) = 0;

    /*!
      Removes a Journal from the calendar.

      \a journal is a pointer to the Journal to remove.

      Returns true if the Journal was successfully removed; false otherwise.

      \sa addJournal()
    */
    virtual bool deleteJournal(const Journal::Ptr &journal) = 0;

    /*!
      Delete all journals that are instances of recurring journal \a journal.

      \a journal is a pointer to a deleted Journal

      Returns true if delete was successful; false otherwise
    */
    virtual bool deleteJournalInstances(const Journal::Ptr &journal) = 0;

    /*!
      Sort a list of Journals.

      \a journalList the list of journals that should be sorted. The list is sorted in place and returned.

      \a sortField specifies the JournalSortField.

      \a sortDirection specifies the SortDirection.

      Returns a list of Journals sorted as specified.
      \since 5.95
    */
    static Journal::List sortJournals(Journal::List &&journalList, JournalSortField sortField, SortDirection sortDirection);

    /*!
      Returns a sorted, filtered list of all Journals for this Calendar.

      \a sortField specifies the JournalSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of all filtered Journals sorted as specified.
    */
    virtual Journal::List journals(JournalSortField sortField = JournalSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const;

    /*!
      Returns a filtered list of all Journals for on the specified date.

      \a date request filtered Journals for this QDate only.

      Returns the list of filtered Journals for the specified date.
    */
    virtual Journal::List journals(const QDate &date) const;

    /*!
      Returns a sorted, unfiltered list of all Journals for this Calendar.

      \a sortField specifies the JournalSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of all unfiltered Journals sorted as specified.
    */
    virtual Journal::List rawJournals(JournalSortField sortField = JournalSortUnsorted, SortDirection sortDirection = SortDirectionAscending) const = 0;

    /*!
      Returns an unfiltered list of all Journals for on the specified date.

      \a date request unfiltered Journals for this QDate only.

      Returns the list of unfiltered Journals for the specified date.
    */
    virtual Journal::List rawJournalsForDate(const QDate &date) const = 0;

    /*!
      Returns the Journal associated with the given unique identifier.

      \a uid is a unique identifier string.

      \a recurrenceId is possible recurrenceId of journal, default is null

      Returns a pointer to the Journal.
      A null pointer is returned if no such Journal exists.
    */
    virtual Journal::Ptr journal(const QString &uid, const QDateTime &recurrenceId = {}) const = 0;

    /*!
      Returns a sorted, unfiltered list of all instances for this recurring Journal.

      \a journal journal to check for. Caller guarantees it's of type Journal.

      \a sortField specifies the JournalSortField.

      \a sortDirection specifies the SortDirection.

      Returns the list of all unfiltered journal instances sorted as specified.
    */
    virtual Journal::List journalInstances(const Incidence::Ptr &journal,
                                           JournalSortField sortField = JournalSortUnsorted,
                                           SortDirection sortDirection = SortDirectionAscending) const = 0;

    // Filter Specific Methods //

    /*!
      Sets the calendar filter.

      \a filter a pointer to a CalFilter object which will be
      used to filter Calendar Incidences. The Calendar takes
      ownership of \a filter.

      \sa filter()
    */
    void setFilter(CalFilter *filter);

    /*!
      Returns the calendar filter.

      Returns a pointer to the calendar CalFilter.
      A null pointer is returned if no such CalFilter exists.

      \sa setFilter()
    */
    CalFilter *filter() const;

    // Alarm Specific Methods //

    /*!
      Returns a list of Alarms within a time range for this Calendar.

      \a from is the starting timestamp.

      \a to is the ending timestamp.

      \a excludeBlockedAlarms if true, alarms belonging to blocked collections aren't returned.

      Returns the list of Alarms for the for the specified time range.
    */
    virtual Alarm::List alarms(const QDateTime &from, const QDateTime &to, bool excludeBlockedAlarms = false) const = 0;

    /*!
      Return a list of Alarms that occur before the specified timestamp.

      \a to is the ending timestamp.

      Returns the list of Alarms occurring before the specified QDateTime.

      \since 5.77
    */
    Q_REQUIRED_RESULT Alarm::List alarmsTo(const QDateTime &to) const;

    // Observer Specific Methods //

    /*!
      \class KCalendarCore::Calendar::CalendarObserver
      \inmodule KCalendarCore

      \brief The CalendarObserver class.
    */
    class KCALENDARCORE_EXPORT CalendarObserver // krazy:exclude=dpointer
    {
    public:
        /*!
          Destructor.
        */
        virtual ~CalendarObserver();

        /*!
          Notify the Observer that a Calendar has been modified.

          \a modified set if the calendar has been modified.

          \a calendar is a pointer to the Calendar object that
          is being observed.

        */
        virtual void calendarModified(bool modified, Calendar *calendar);

        /*!
          Notify the Observer that an Incidence has been inserted.

          \a incidence is a pointer to the Incidence that was inserted.

        */
        virtual void calendarIncidenceAdded(const Incidence::Ptr &incidence);

        /*!
          Notify the Observer that an Incidence has been modified.

          \a incidence is a pointer to the Incidence that was modified.

        */
        virtual void calendarIncidenceChanged(const Incidence::Ptr &incidence);

        /*!
          Notify the Observer that an Incidence will be removed.

          \a incidence is a pointer to the Incidence that will be removed.

        */
        virtual void calendarIncidenceAboutToBeDeleted(const Incidence::Ptr &incidence);

        /*!
          Notify the Observer that an Incidence has been removed.

          \a incidence is a pointer to the Incidence that was removed.

          \a calendar is a pointer to the calendar where the incidence was part of,
                          because the incidence was deleted, there is now way to determine the calendar

          \since 4.83.0
        */
        virtual void calendarIncidenceDeleted(const Incidence::Ptr &incidence, const Calendar *calendar);

        /*!
          Notify the Observer that an addition of Incidence has been canceled.

          \a incidence is a pointer to the Incidence that was removed.

        */
        virtual void calendarIncidenceAdditionCanceled(const Incidence::Ptr &incidence);
    };

    /*!
      Registers an Observer for this Calendar.

      \a observer is a pointer to an Observer object that will be
      watching this Calendar.

      \sa unregisterObserver()
     */
    void registerObserver(CalendarObserver *observer);

    /*!
      Unregisters an Observer for this Calendar.

      \a observer is a pointer to an Observer object that has been
      watching this Calendar.

      \sa registerObserver()
     */
    void unregisterObserver(CalendarObserver *observer);

    using QObject::event; // prevent warning about hidden virtual method

protected:
    /*!
      The Observer interface. So far not implemented.

      \a uid is the UID for the Incidence that has been updated.

      \a recurrenceId is possible recurrenceid of incidence.

    */
    void incidenceUpdated(const QString &uid, const QDateTime &recurrenceId) override;

    /*!
      Let Calendar subclasses set the time specification.

      \a timeZone is the time specification (time zone, etc.) for
                      viewing Incidence dates.\n

    */
    virtual void doSetTimeZone(const QTimeZone &timeZone);

    /*!
      Let Calendar subclasses notify that they inserted an Incidence.

      \a incidence is a pointer to the Incidence object that was inserted.

    */
    void notifyIncidenceAdded(const Incidence::Ptr &incidence);

    /*!
      Let Calendar subclasses notify that they modified an Incidence.

      \a incidence is a pointer to the Incidence object that was modified.

    */
    void notifyIncidenceChanged(const Incidence::Ptr &incidence);

    /*!
      Let Calendar subclasses notify that they will remove an Incidence.

      \a incidence is a pointer to the Incidence object that will be removed.

    */
    void notifyIncidenceAboutToBeDeleted(const Incidence::Ptr &incidence);

    /*!
      Let Calendar subclasses notify that they removed an Incidence.

      \a incidence is a pointer to the Incidence object that has been removed.

    */
    void notifyIncidenceDeleted(const Incidence::Ptr &incidence);

    /*!
      Let Calendar subclasses notify that they canceled addition of an Incidence.

      \a incidence is a pointer to the Incidence object that addition as canceled.

    */
    void notifyIncidenceAdditionCanceled(const Incidence::Ptr &incidence);

    /*!
      \reimp
    */
    void customPropertyUpdated() override;

    /*!
      Let Calendar subclasses notify that they enabled an Observer.

      \a enabled if true tells the calendar that a subclass has
      enabled an Observer.

    */
    void setObserversEnabled(bool enabled);

    /*!
      Appends alarms of incidence in interval to list of alarms.

      \a alarms is a List of Alarms to be appended onto.

      \a incidence is a pointer to an Incidence containing the Alarm
      to be appended.

      \a from is the lower range of the next Alarm repetition.

      \a to is the upper range of the next Alarm repetition.

    */
    void appendAlarms(Alarm::List &alarms, const Incidence::Ptr &incidence, const QDateTime &from, const QDateTime &to) const;

    /*!
      Appends alarms of recurring events in interval to list of alarms.

      \a alarms is a List of Alarms to be appended onto.

      \a incidence is a pointer to an Incidence containing the Alarm
      to be appended.

      \a from is the lower range of the next Alarm repetition.

      \a to is the upper range of the next Alarm repetition.

    */
    void appendRecurringAlarms(Alarm::List &alarms, const Incidence::Ptr &incidence, const QDateTime &from, const QDateTime &to) const;

    /*!
     * Sets the loading state of this calendar to \a isLoading.
     * This is false by default and only needs to be called for calendars
     * that implement asynchronous loading.
     * \since 5.96
     * \sa isLoading()
     */
    void setIsLoading(bool isLoading);

    /*
      TODO: appears to be unused
    */
    virtual void virtual_hook(int id, void *data);

Q_SIGNALS:
    /*!
      Emitted when setFilter() is called.
      \since 4.11
     */
    void filterChanged();

    /*!
     * Emitted when the id changes.
     * \since 5.85
     * \sa id()
     */
    void idChanged();

    /*!
     * Emitted when the name changes.
     * \since 5.85
     * \sa name()
     */
    void nameChanged();

    /*!
     * Emitted when the icon name changes.
     * \since 5.85
     * \sa icon()
     */
    void iconChanged();

    /*!
     * Emitted when the AccessMode changes.
     * \since 5.85
     * \sa accessMode()
     */
    void accessModeChanged();

    /*!
     * Emitted when the owner changes.
     * \since 5.85
     * \sa owner()
     */
    void ownerChanged();

    /*!
     * Emitted when the loading state changed.
     * \since 5.96
     * \sa isLoading()
     */
    void isLoadingChanged();

private:
    friend class ICalFormat;

    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond

    Q_DISABLE_COPY(Calendar)
};

}

Q_DECLARE_METATYPE(KCalendarCore::Calendar::Ptr)

#endif
