/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Event class.
*/
#ifndef KCALCORE_EVENT_H
#define KCALCORE_EVENT_H

#include "incidence.h"
#include "kcalendarcore_export.h"

#include <QTimeZone>

namespace KCalendarCore
{

class EventPrivate;

/*!
  \qmlvaluetype event
  \inqmlmodule org.kde.kcalendarcore
  \nativetype KCalendarCore::Event
  \brief
  This class provides an Event in the sense of RFC2445.
*/

/*!
  \class KCalendarCore::Event
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Event
  \brief
  This class provides an Event in the sense of RFC2445.
*/
class KCALENDARCORE_EXPORT Event : public Incidence
{
    Q_GADGET

    /*!
     * \qmlproperty date event::dtEnd
     */

    /*!
     * \property KCalendarCore::Event::dtEnd
     */
    Q_PROPERTY(QDateTime dtEnd READ dtEnd WRITE setDtEnd)

    /*!
     * \qmlproperty KCalendarCore::Event::Transparency event::transparency
     */

    /*!
     * \property KCalendarCore::Event::transparency
     */
    Q_PROPERTY(KCalendarCore::Event::Transparency transparency READ transparency WRITE setTransparency)

public:
    /*!
      \enum KCalendarCore::Event::Transparency
      \brief The different Event transparency types.
      \value Opaque
      Event appears in free/busy time.
      \value Transparent
      Event does \b not appear in free/busy time.
    */
    enum Transparency {
        Opaque,
        Transparent,
    };
    Q_ENUM(Transparency)

    /*!
      A shared pointer to an Event object.
    */
    typedef QSharedPointer<Event> Ptr;

    /*!
      List of events.
    */
    typedef QList<Ptr> List;

    ///@cond PRIVATE
    // needed for Akonadi polymorphic payload support
    typedef Incidence SuperClass;
    ///@endcond

    /*!
      Constructs an event.
    */
    Event();

    /*!
      Copy constructor.

      \a other is the event to copy.

    */
    Event(const Event &other);

    /*!
      Costructs an event out of an incidence
      This constructs allows to make it easy to create an event from a todo.

      \a other is the incidence to copy.

      \since 4.14
    */
    Event(const Incidence &other); // krazy:exclude=explicit (copy ctor)

    /*!
      Destroys the event.
    */
    ~Event() override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT IncidenceType type() const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT QByteArray typeStr() const override;

    /*!
      Returns an exact copy of this Event. The caller owns the returned object.
    */
    Event *clone() const override;

    /*!
      Sets the incidence starting date/time.

      \a dt is the starting date/time.

      \sa IncidenceBase::dtStart().
    */
    void setDtStart(const QDateTime &dt) override;

    /*!
      Sets the event end date and time.
      Important note for all day events: the end date is inclusive,
      the event will still occur during dtEnd(). When serializing to iCalendar
      DTEND will be dtEnd()+1, because the RFC states that DTEND is exclusive.

      \a dtEnd is a QDateTime specifying when the event ends.

      \sa dtEnd(), dateEnd().
    */
    void setDtEnd(const QDateTime &dtEnd);

    /*!
      Returns the event end date and time.
      Important note for all day events: the returned end date is inclusive,
      the event will still occur during dtEnd(). When serializing to iCalendar
      DTEND will be dtEnd()+1, because the RFC states that DTEND is exclusive.
      \sa setDtEnd().
    */
    virtual QDateTime dtEnd() const;

    /*!
      Returns the date when the event ends. This might be different from
      dtEnd().date, since the end date/time is non-inclusive. So timed events
      ending at 0:00 have their end date on the day before.
    */
    Q_REQUIRED_RESULT QDate dateEnd() const;

    /*!
      Returns whether the event has an end date/time.
    */
    Q_REQUIRED_RESULT bool hasEndDate() const;

    /*!
      Returns true if the event spans multiple days, otherwise return false.

      For recurring events, it returns true if the first occurrence spans multiple days,
      otherwise returns false. Other occurrences might have a different span due to day light
      savings changes.

      \a zone If set, looks if the event is multiday for the given zone.
      If not set, looks if event this multiday for its zone.
    */
    Q_REQUIRED_RESULT bool isMultiDay(const QTimeZone &zone = {}) const;

    /*!
      \reimp
    */
    void shiftTimes(const QTimeZone &oldZone, const QTimeZone &newZone) override;

    /*!
      Sets the event's time transparency level.

      \a transparency is the event Transparency level.

    */
    void setTransparency(Transparency transparency);

    /*!
      Returns the event's time transparency level.
    */
    Q_REQUIRED_RESULT Transparency transparency() const;

    /*!
      Sets the duration of this event.

      \a duration is the event Duration.

    */
    void setDuration(const Duration &duration) override;

    /*!
      \reimp
    */
    void setAllDay(bool allDay) override;

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
    Q_REQUIRED_RESULT bool supportsGroupwareCommunication() const override;

    /*!
       Returns the Akonadi specific sub MIME type of a KCalendarCore::Event.
    */
    Q_REQUIRED_RESULT static QLatin1String eventMimeType();

protected:
    /*!
      Compares two events for equality.

      \a event is the event to compare.

      Returns true if \a event is equal to this object, or false if they are different.
    */
    bool equals(const IncidenceBase &event) const override;

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
      Disabled, otherwise could be dangerous if you subclass Event.
      Use IncidenceBase::operator= which is safe because it calls
      virtual function assign().
      \a other is another Event object to assign to this one.
     */
    Event &operator=(const Event &other) = delete;

    // For polymorphic serialization
    void serialize(QDataStream &out) const override;
    void deserialize(QDataStream &in) override;

    //@cond PRIVATE
    Q_DECLARE_PRIVATE(Event)
    //@endcond
};

} // namespace KCalendarCore

//@cond PRIVATE
Q_DECLARE_TYPEINFO(KCalendarCore::Event::Ptr, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(KCalendarCore::Event::Ptr)
Q_DECLARE_METATYPE(KCalendarCore::Event *)
//@endcond

#endif
