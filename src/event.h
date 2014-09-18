/*
  This file is part of the kcalcore library.

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
  defines the Event class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/
#ifndef KCALCORE_EVENT_H
#define KCALCORE_EVENT_H

#include "kcalcore_export.h"
#include "incidence.h"
#include "supertrait.h"

namespace KCalCore {

/**
  @brief
  This class provides an Event in the sense of RFC2445.
*/
class KCALCORE_EXPORT Event : public Incidence
{
public:
    /**
      The different Event transparency types.
    */
    enum Transparency {
        Opaque,      /**< Event appears in free/busy time */
        Transparent  /**< Event does @b not appear in free/busy time */
    };

    /**
      A shared pointer to an Event object.
    */
    typedef QSharedPointer<Event> Ptr;

    /**
      List of events.
    */
    typedef QVector<Ptr> List;

    /**
      Constructs an event.
    */
    Event();

    /**
      Copy constructor.
      @param other is the event to copy.
    */
    Event(const Event &other);

    /**
      Costructs an event out of an incidence
      This constructs allows to make it easy to create an event from a todo.
      @param other is the incidence to copy.
      @since 4.14
    */
    Event(const Incidence &other);

    /**
      Destroys the event.
    */
    ~Event();

    /**
      @copydoc
      IncidenceBase::type()
    */
    IncidenceType type() const;

    /**
      @copydoc
      IncidenceBase::typeStr()
    */
    QByteArray typeStr() const;

    /**
      Returns an exact copy of this Event. The caller owns the returned object.
    */
    Event *clone() const;

    /**
      Sets the incidence starting date/time.

      @param dt is the starting date/time.
      @see IncidenceBase::dtStart().
    */
    virtual void setDtStart(const KDateTime &dt);

    /**
      Sets the event end date and time.
      Important note for all day events: the end date is inclusive,
      the event will still occur during dtEnd(). When serializing to iCalendar
      DTEND will be dtEnd()+1, because the RFC states that DTEND is exclusive.
      @param dtEnd is a KDateTime specifying when the event ends.
      @see dtEnd(), dateEnd().
    */
    void setDtEnd(const KDateTime &dtEnd);

    /**
      Returns the event end date and time.
      Important note for all day events: the returned end date is inclusive,
      the event will still occur during dtEnd(). When serializing to iCalendar
      DTEND will be dtEnd()+1, because the RFC states that DTEND is exclusive.
      @see setDtEnd().
    */
    virtual KDateTime dtEnd() const;

    /**
      Returns the date when the event ends. This might be different from
      dtEnd().date, since the end date/time is non-inclusive. So timed events
      ending at 0:00 have their end date on the day before.
    */
    QDate dateEnd() const;

    /**
      Sets whether the event has an end date/time.
      @param b If set, indicates the event has an end date.
      @deprecated Use setDtEnd( KDateTime() ) instead of setHasEndDate( false )
    */
    KCALCORE_DEPRECATED void setHasEndDate(bool b);

    /**
      Returns whether the event has an end date/time.
    */
    bool hasEndDate() const;

    /**
      Returns true if the event spans multiple days, otherwise return false.

      For recurring events, it returns true if the first occurrence spans multiple days,
      otherwise returns false. Other occurrences might have a different span due to day light
      savings changes.

      @param spec If set, looks if the event is multiday for the given spec.
      If not set, looks if event this multiday for its spec.
    */
    bool isMultiDay(const KDateTime::Spec &spec = KDateTime::Spec()) const;

    /**
      @copydoc
      IncidenceBase::shiftTimes()
    */
    virtual void shiftTimes(const KDateTime::Spec &oldSpec,
                            const KDateTime::Spec &newSpec);

    /**
      Sets the event's time transparency level.
      @param transparency is the event Transparency level.
    */
    void setTransparency(Transparency transparency);

    /**
      Returns the event's time transparency level.
    */
    Transparency transparency() const;

    /**
      Sets the duration of this event.
      @param duration is the event Duration.
    */
    void setDuration(const Duration &duration);

    /**
      @copydoc
      IncidenceBase::setAllDay().
    */
    void setAllDay(bool allDay);

    /**
      @copydoc
      IncidenceBase::dateTime()
    */
    KDateTime dateTime(DateTimeRole role) const;

    /**
      @copydoc
      IncidenceBase::setDateTime()
    */
    void setDateTime(const KDateTime &dateTime, DateTimeRole role);

    /**
      @copydoc
      IncidenceBase::mimeType()
    */
    QLatin1String mimeType() const;

    /**
       @copydoc
       Incidence::iconName()
    */
    QLatin1String iconName(const KDateTime &recurrenceId = KDateTime()) const;

    /**
       Returns the Akonadi specific sub MIME type of a KCalCore::Event.
    */
    static QLatin1String eventMimeType();

protected:
    /**
      Compares two events for equality.
      @param event is the event to compare.
    */
    virtual bool equals(const IncidenceBase &event) const;

    /**
      @copydoc
      IncidenceBase::assign()
    */
    virtual IncidenceBase &assign(const IncidenceBase &other);

    /**
      @copydoc
      IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook(int id, void *data);

private:
    /**
      @copydoc
      IncidenceBase::accept()
    */
    bool accept(Visitor &v, IncidenceBase::Ptr incidence);

    /**
      Disabled, otherwise could be dangerous if you subclass Event.
      Use IncidenceBase::operator= which is safe because it calls
      virtual function assign().
      @param other is another Event object to assign to this one.
     */
    Event &operator=(const Event &other);

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
Q_DECLARE_TYPEINFO(KCalCore::Event::Ptr, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(KCalCore::Event::Ptr)
Q_DECLARE_METATYPE(KCalCore::Event*)
//@endcond

//@cond PRIVATE
namespace Akonadi {
// super class trait specialization
template <> struct SuperClass<KCalCore::Event> : public SuperClassTrait<KCalCore::Incidence> {};
}
//@endcond

#endif
