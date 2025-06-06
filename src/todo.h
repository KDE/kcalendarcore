/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2009 Allen Winter <winter@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Todo class.
*/

#ifndef KCALCORE_TODO_H
#define KCALCORE_TODO_H

#include "incidence.h"
#include "kcalendarcore_export.h"

namespace KCalendarCore
{

class TodoPrivate;

/*!
  \class KCalendarCore::Todo
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Todo
  \brief
  Provides a To-do in the sense of RFC2445.
*/
class KCALENDARCORE_EXPORT Todo : public Incidence
{
public:
    /*!
      A shared pointer to a Todo object.
    */
    typedef QSharedPointer<Todo> Ptr;

    /*!
      List of to-dos.
    */
    typedef QList<Ptr> List;

    ///@cond PRIVATE
    // needed for Akonadi polymorphic payload support
    typedef Incidence SuperClass;
    ///@endcond

    /*!
      Constructs an empty to-do.
    */
    Todo();

    /*!
      Copy constructor.

      \a other is the to-do to copy.

    */
    Todo(const Todo &other);

    /*!
      Costructs a todo out of an incidence
      This constructs allows to make it easy to create a todo from an event.

      \a other is the incidence to copy.

      \since 4.14
     */
    Todo(const Incidence &other);

    /*!
      Destroys a to-do.
    */
    ~Todo() override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT IncidenceType type() const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT QByteArray typeStr() const override;

    /*!
      Returns an exact copy of this todo. The returned object is owned by the caller.
      Returns A pointer to a Todo containing an exact copy of this object.
    */
    Todo *clone() const override;

    /*!
      Sets due date and time.

      \a dtDue The due date/time.

      \a first If true and the todo recurs, the due date of the first
      occurrence will be set. If false and the todo recurs, the occurrence at
      that date/time becomes the current occurrence. If the todo does not recur,
      the due date of the todo will be set.

    */
    void setDtDue(const QDateTime &dtDue, bool first = false);

    /*!
      Returns the todo due datetime.

      \a first If true and the todo recurs, the due datetime of the first
      occurrence will be returned. If false and recurrent, the datetime of the
      current occurrence will be returned. If non-recurrent, the normal due
      datetime will be returned.

      Returns A QDateTime containing the todo due datetime.
    */
    Q_REQUIRED_RESULT QDateTime dtDue(bool first = false) const;

    /*!
      Returns if the todo has a due datetime.
      Returns true if the todo has a due datetime; false otherwise.
    */
    Q_REQUIRED_RESULT bool hasDueDate() const;

    /*!
      Returns if the todo has a start datetime.
      Returns true if the todo has a start datetime; false otherwise.
    */
    Q_REQUIRED_RESULT bool hasStartDate() const;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT QDateTime dtStart() const override;

    /*!
      Returns the start datetime of the todo.

      \a first If true, the start datetime of the todo will be returned;
      also, if the todo recurs, the start datetime of the first occurrence
      will be returned.
      If false and the todo recurs, the relative start datetime will be returned,
      based on the datetime returned by dtRecurrence().

      Returns A QDateTime for the start datetime of the todo.
    */
    Q_REQUIRED_RESULT QDateTime dtStart(bool first) const;

    /*!
      Returns whether the todo is completed or not.
      Returns true if the todo is 100% completed, has status \c StatusCompleted,
      or has a completed date; false otherwise.

      \sa isOverdue, isInProgress(), isOpenEnded(), isNotStarted(bool),
      setCompleted(), percentComplete()
    */
    Q_REQUIRED_RESULT bool isCompleted() const;

    /*!
      Sets completion percentage and status.

      \a completed If  \c true, percentage complete is set to 100%, and
      status is set to \c StatusCompleted;  the completion date is \b not set or cleared.
      If \c false, percentage complete is set to 0%,
      status is set to \c StatusNone, and the completion date is cleared.


      \sa isCompleted(), percentComplete(), hasCompletedDate()
    */
    void setCompleted(bool completed);

    /*!
      Returns what percentage of the to-do is completed.
      Returns The percentage complete of the to-do as an integer between 0 and 100, inclusive.
      \sa setPercentComplete(), isCompleted()
    */
    Q_REQUIRED_RESULT int percentComplete() const;

    /*!
      Sets what percentage of the to-do is completed.

      To prevent inconsistency, if \a percent is not 100, completed() is cleared,
      and if status() is StatusCompleted it is reset to StatusNone.

      \a percent is the completion percentage.  Values greater than 100 are
      treated as 100; values less than p are treated as 0.

      \sa isCompleted(), setCompleted()
    */
    void setPercentComplete(int percent);

    /*!
      Returns the to-do was completion datetime.

      Returns A QDateTime for the completion datetime of the to-do.
      \sa hasCompletedDate()
    */
    Q_REQUIRED_RESULT QDateTime completed() const;

    /*!
      Marks this Todo, or its current recurrence, as completed.

      If the todo does not recur, its completion percentage is set to 100%,
      and its completion date is set to \a completeDate.  If its status is not
      StatusNone, it is set to StatusCompleted.

      \note
      If \a completeDate is invalid, the completion date is cleared, but the
      todo is still "complete".

      If the todo recurs, the first incomplete recurrence is marked complete.

      \a completeDate is the to-do completion date.

      \sa completed(), hasCompletedDate()
    */
    void setCompleted(const QDateTime &completeDate);

    /*!
      Returns if the to-do has a completion datetime.

      Returns true if the to-do has a date associated with completion; false otherwise.
      \sa setCompleted(), completed()
    */
    bool hasCompletedDate() const;

    /*!
      Returns true, if the to-do is in-progress (started, or >0% completed);
      otherwise return false. If the to-do is overdue, then it is not
      considered to be in-progress.

      \a first If true, the start and due dates of the todo will be used;
      also, if the todo recurs, the start date and due date of the first
      occurrence will be used.
      If false and the todo recurs, the relative start and due dates will be
      used, based on the date returned by dtRecurrence().

      \sa isOverdue(), isCompleted(), isOpenEnded(), isNotStarted(bool)
    */
    Q_REQUIRED_RESULT bool isInProgress(bool first) const;

    /*!
      Returns true, if the to-do is open-ended (no due date); false otherwise.
      \sa isOverdue(), isCompleted(), isInProgress(), isNotStarted(bool)
    */
    Q_REQUIRED_RESULT bool isOpenEnded() const;

    /*!
      Returns true, if the to-do has yet to be started (no start date and 0%
      completed); otherwise return false.

      \a first If true, the start date of the todo will be used;
      also, if the todo recurs, the start date of the first occurrence
      will be used.
      If false and the todo recurs, the relative start date will be used,
      based on the date returned by dtRecurrence().

      \sa isOverdue(), isCompleted(), isInProgress(), isOpenEnded()
    */
    Q_REQUIRED_RESULT bool isNotStarted(bool first) const;

    /*!
      \reimp
    */
    void shiftTimes(const QTimeZone &oldZone, const QTimeZone &newZone) override;

    /*!
      \reimp
    */
    void setAllDay(bool allDay) override;

    /*!
      Identify the earliest uncompleted occurrence of a recurring Todo.

      \a dt Normally, the start date-time of the occurrence.
        For backwards compatibility, if the Todo does not have a \c dtStart()
        then pass the occurrence's due date-time.
    */
    void setDtRecurrence(const QDateTime &dt);

    /*!
      Returns an identifier for the earliest uncompleted occurrence of a
      recurring Todo.

      \note
      Do not rely on the returned value to determine whether the Todo is
      completed; use \c isCompleted() instead.
    */
    Q_REQUIRED_RESULT QDateTime dtRecurrence() const;

    /*!
      Returns true if the \a date specified is one on which the to-do will
      recur. Todos are a special case, hence the overload. It adds an extra
      check, which make it return false if there's an occurrence between
      the recur start and today.

      \a date is the date to check.
      \a timeZone is the time zone
    */
    bool recursOn(const QDate &date, const QTimeZone &timeZone) const override;

    /*!
      Returns true if this todo is overdue (e.g. due date is lower than today
      and not completed), else false.
      \sa isCompleted(), isInProgress(), isOpenEnded(), isNotStarted(bool)
     */
    bool isOverdue() const;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT QDateTime dateTime(DateTimeRole role) const override;

    /*!
      \reimp
    */
    void setDateTime(const QDateTime &dateTime, DateTimeRole role) override;

    /*!
       \reimp
    */
    Q_REQUIRED_RESULT QLatin1String mimeType() const override;

    /*!
       \reimp
    */
    Q_REQUIRED_RESULT QLatin1String iconName(const QDateTime &recurrenceId = {}) const override;

    /*!
       \reimp
    */
    bool supportsGroupwareCommunication() const override;

    /*!
       Returns the Akonadi specific sub MIME type of a KCalendarCore::Todo.
    */
    Q_REQUIRED_RESULT static QLatin1String todoMimeType();

protected:
    /*!
      Compare this with \a todo for equality.

      \a todo is the to-do to compare.

      Returns true if \a todo is equal to this object, or false if they are different.
    */
    bool equals(const IncidenceBase &todo) const override;

    /*!
      \reimp
    */
    IncidenceBase &assign(const IncidenceBase &other) override;

    /*!
      \reimp
    */
    void virtual_hook(VirtualHook id, void *data) override;

private:
    /*!
      \reimp
    */
    bool accept(Visitor &v, const IncidenceBase::Ptr &incidence) override;

    /*!
      Disabled, otherwise could be dangerous if you subclass Todo.
      Use IncidenceBase::operator= which is safe because it calls
      virtual function assign().
      \a other is another Todo object to assign to this one.
     */
    Todo &operator=(const Todo &other) = delete;

    // For polymorphic serialization
    void serialize(QDataStream &out) const override;
    void deserialize(QDataStream &in) override;

    //@cond PRIVATE
    Q_DECLARE_PRIVATE(Todo)
    //@endcond
};

} // namespace KCalendarCore

//@cond PRIVATE
Q_DECLARE_TYPEINFO(KCalendarCore::Todo::Ptr, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(KCalendarCore::Todo::Ptr)
Q_DECLARE_METATYPE(KCalendarCore::Todo *)
//@endcond

#endif
