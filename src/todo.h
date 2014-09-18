/*
  This file is part of the kcalcore library.

  Copyright (c) 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2009 Allen Winter <winter@kde.org>

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
  defines the Todo class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Allen Winter \<winter@kde.org\>
*/

#ifndef KCALCORE_TODO_H
#define KCALCORE_TODO_H

#include "kcalcore_export.h"
#include "incidence.h"
#include "supertrait.h"

namespace KCalCore {

/**
  @brief
  Provides a To-do in the sense of RFC2445.
*/
class KCALCORE_EXPORT Todo : public Incidence
{
public:
    /**
      A shared pointer to a Todo object.
    */
    typedef QSharedPointer<Todo> Ptr;

    /**
      List of to-dos.
    */
    typedef QVector<Ptr> List;

    /**
      Constructs an empty to-do.
    */
    Todo();

    /**
      Copy constructor.
      @param other is the to-do to copy.
    */
    Todo(const Todo &other);

    /**
      Costructs a todo out of an incidence
      This constructs allows to make it easy to create a todo from an event.
      @param other is the incidence to copy.
      @since 4.14
     */
    Todo(const Incidence &other);

    /**
      Destroys a to-do.
    */
    ~Todo();

    /**
      @copydoc IncidenceBase::type()
    */
    IncidenceType type() const;

    /**
      @copydoc IncidenceBase::typeStr()
    */
    QByteArray typeStr() const;

    /**
      Returns an exact copy of this todo. The returned object is owned by the caller.
      @return A pointer to a Todo containing an exact copy of this object.
    */
    Todo *clone() const;

    /**
      Sets due date and time.

      @param dtDue The due date/time.
      @param first If true and the todo recurs, the due date of the first
      occurrence will be returned. If false and recurrent, the date of the
      current occurrence will be returned. If non-recurrent, the normal due
      date will be returned.
    */
    void setDtDue(const KDateTime &dtDue, bool first = false);

    /**
      Returns the todo due datetime.

      @param first If true and the todo recurs, the due datetime of the first
      occurrence will be returned. If false and recurrent, the datetime of the
      current occurrence will be returned. If non-recurrent, the normal due
      datetime will be returned.
      @return A KDateTime containing the todo due datetime.
    */
    KDateTime dtDue(bool first = false) const;

    /**
      Returns if the todo has a due datetime.
      @return true if the todo has a due datetime; false otherwise.
    */
    bool hasDueDate() const;

    /**
      Sets if the todo has a due datetime.
      @param hasDueDate true if todo has a due datetime, otherwise false
      @deprecated Use setDtDue( KDateTime() )
    */
    KCALCORE_DEPRECATED void setHasDueDate(bool hasDueDate);

    /**
      Returns if the todo has a start datetime.
      @return true if the todo has a start datetime; false otherwise.
    */
    bool hasStartDate() const;

    /**
      Sets if the todo has a start datetime.
      @param hasStartDate true if todo has a start datetime, otherwise false.
      @deprecated Use setDtStart( KDateTime() )
    */
    KCALCORE_DEPRECATED void setHasStartDate(bool hasStartDate);

    /**
      @copydoc IncidenceBase::dtStart()
    */
    virtual KDateTime dtStart() const;

    /**
      Returns the start datetime of the todo.

      @param first If true, the start datetime of the todo will be returned;
      also, if the todo recurs, the start datetime of the first occurrence
      will be returned.
      If false and the todo recurs, the relative start datetime will be returned,
      based on the datetime returned by dtRecurrence().
      @return A KDateTime for the start datetime of the todo.
    */
    KDateTime dtStart(bool first) const;

    /**
      Sets the start datetime of the todo.
      @param dtStart is the to-do start datetime.
    */ //TODO_KDE5: Remove (see IncidenceBase)
    void setDtStart(const KDateTime &dtStart);

    /**
      Returns if the todo is 100% completed.
      @return true if the todo is 100% completed; false otherwise.

      @see isOverdue, isInProgress(), isOpenEnded(), isNotStarted(bool),
      setCompleted(), percentComplete()
    */
    bool isCompleted() const;

    /**
      Sets completed state.

      @param completed If true set completed state to 100%, if false set
      completed state to 0%.

      @see isCompleted(), percentComplete()
    */
    void setCompleted(bool completed);

    /**
      Returns what percentage of the to-do is completed.
      @return The percentage complete of the to-do as an integer between 0 and 100, inclusive.
      @see setPercentComplete(), isCompleted()
    */
    int percentComplete() const;

    /**
      Sets what percentage of the to-do is completed. Valid values are in the
      range from 0 to 100.

      @param percent is the completion percentage, which as integer value
      between 0 and 100, inclusive.

      @see isCompleted(), setCompleted()
    */
    void setPercentComplete(int percent);

    /**
      Returns the to-do was completion datetime.

      @return A KDateTime for the completeion datetime of the to-do.
      @see hasCompletedDate()
    */
    KDateTime completed() const;

    /**
      Sets date and time of completion.

      @param completeDate is the to-do completion date.
      @see completed(), hasCompletedDate()
    */
    void setCompleted(const KDateTime &completeDate);

    /**
      Returns if the to-do has a completion datetime.

      @return true if the to-do has a date associated with completion; false otherwise.
      @see setCompleted(), completed()
    */
    bool hasCompletedDate() const;

    /**
      Returns true, if the to-do is in-progress (started, or >0% completed);
      otherwise return false. If the to-do is overdue, then it is not
      considered to be in-progress.

      @param first If true, the start and due dates of the todo will be used;
      also, if the todo recurs, the start date and due date of the first
      occurrence will be used.
      If false and the todo recurs, the relative start and due dates will be
      used, based on the date returned by dtRecurrence().
      @see isOverdue(), isCompleted(), isOpenEnded(), isNotStarted(bool)
    */
    bool isInProgress(bool first) const;

    /**
      Returns true, if the to-do is open-ended (no due date); false otherwise.
      @see isOverdue(), isCompleted(), isInProgress(), isNotStarted(bool)
    */
    bool isOpenEnded() const;

    /**
      Returns true, if the to-do has yet to be started (no start date and 0%
      completed); otherwise return false.

      @param first If true, the start date of the todo will be used;
      also, if the todo recurs, the start date of the first occurrence
      will be used.
      If false and the todo recurs, the relative start date will be used,
      based on the date returned by dtRecurrence().
      @see isOverdue(), isCompleted(), isInProgress(), isOpenEnded()
    */
    bool isNotStarted(bool first) const;

    /**
      @copydoc IncidenceBase::shiftTimes()
    */
    virtual void shiftTimes(const KDateTime::Spec &oldSpec,
                            const KDateTime::Spec &newSpec);

    /**
      @copydoc IncidenceBase::setAllDay().
    */
    void setAllDay(bool allDay);

    /**
      Sets the due date/time of the current occurrence if recurrent.

      @param dt is the
    */
    void setDtRecurrence(const KDateTime &dt);

    /**
      Returns the due date/time of the current occurrence if recurrent.
    */
    KDateTime dtRecurrence() const;

    /**
      Returns true if the @p date specified is one on which the to-do will
      recur. Todos are a special case, hence the overload. It adds an extra
      check, which make it return false if there's an occurrence between
      the recur start and today.

      @param date is the date to check.
      @param timeSpec is the
    */
    virtual bool recursOn(const QDate &date,
                          const KDateTime::Spec &timeSpec) const;

    /**
      Returns true if this todo is overdue (e.g. due date is lower than today
      and not completed), else false.
      @see isCompleted(), isInProgress(), isOpenEnded(), isNotStarted(bool)
     */
    bool isOverdue() const;

    /**
      @copydoc IncidenceBase::dateTime()
    */
    KDateTime dateTime(DateTimeRole role) const;

    /**
      @copydoc IncidenceBase::setDateTime()
    */
    void setDateTime(const KDateTime &dateTime, DateTimeRole role);

    /**
       @copydoc IncidenceBase::mimeType()
    */
    QLatin1String mimeType() const;

    /**
       @copydoc Incidence::iconName()
    */
    QLatin1String iconName(const KDateTime &recurrenceId = KDateTime()) const;

    /**
       Returns the Akonadi specific sub MIME type of a KCalCore::Todo.
    */
    static QLatin1String todoMimeType();

protected:
    /**
      Compare this with @p todo for equality.
      @param todo is the to-do to compare.
    */
    virtual bool equals(const IncidenceBase &todo) const;

    /**
      @copydoc IncidenceBase::assign()
    */
    virtual IncidenceBase &assign(const IncidenceBase &other);

    /**
      @copydoc IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook(int id, void *data);

private:
    /**
      @copydoc IncidenceBase::accept()
    */
    bool accept(Visitor &v, IncidenceBase::Ptr incidence);

    /**
      Disabled, otherwise could be dangerous if you subclass Todo.
      Use IncidenceBase::operator= which is safe because it calls
      virtual function assign().
      @param other is another Todo object to assign to this one.
     */
    Todo &operator=(const Todo &other);

    // For polymorfic serialization
    void serialize(QDataStream &out);
    void deserialize(QDataStream &in);

    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond
};

} // namespace KCalCore

//@cond PRIVATE
Q_DECLARE_TYPEINFO(KCalCore::Todo::Ptr, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(KCalCore::Todo::Ptr)
Q_DECLARE_METATYPE(KCalCore::Todo*)
namespace KPIMUtils {
// super class trait specialization
template <> struct SuperClass<KCalCore::Todo> : public SuperClassTrait<KCalCore::Incidence> {};
}
//@endcond

#endif
