/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2004 Reinhold Kainhofer <reinhold@kainhofer.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the FreeBusy class.
*/

#ifndef KCALCORE_FREEBUSY_H
#define KCALCORE_FREEBUSY_H

#include "event.h"
#include "freebusyperiod.h"
#include "incidencebase.h"
#include "kcalendarcore_export.h"
#include "period.h"

#include <QMetaType>

namespace KCalendarCore
{

class FreeBusyPrivate;

/*!
  \class KCalendarCore::FreeBusy
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/FreeBusy
  \brief
  Provides information about the free/busy time of a calendar.

  A free/busy is a collection of Periods.

  \sa Period.
*/
class KCALENDARCORE_EXPORT FreeBusy : public IncidenceBase
{
    friend KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &s, const QSharedPointer<KCalendarCore::FreeBusy> &freebusy);
    friend KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &s, QSharedPointer<KCalendarCore::FreeBusy> &freebusy);

public:
    /*!
      A shared pointer to a FreeBusy object.

      \deprecated [6.32] Use QSharedPointer<FreeBusy> instead.
    */
#if KCALENDARCORE_ENABLE_DEPRECATED_SINCE(6, 32)
    KCALENDARCORE_DEPRECATED_VERSION(6, 32, "Use QSharedPointer<FreeBusy> instead")
    typedef QSharedPointer<FreeBusy> Ptr;
#endif

    /*!
      List of FreeBusy objects.

      \deprecated [6.32] Use QList<QSharedPointer<FreeBusy>> instead.
    */
#if KCALENDARCORE_ENABLE_DEPRECATED_SINCE(6, 32)
    KCALENDARCORE_DEPRECATED_VERSION(6, 32, "Use QList<QSharedPointer<FreeBusy>> instead")
    typedef QList<QSharedPointer<FreeBusy>> List;
#endif

    /*!
      Constructs an free/busy without any periods.
    */
    FreeBusy();

    /*!
      Copy constructor.

      \a other is the free/busy to copy.

    */
    FreeBusy(const FreeBusy &other);

    /*!
      Constructs a free/busy from a list of periods.

      \a busyPeriods is a list of periods.

    */
    explicit FreeBusy(const QList<Period> &busyPeriods);

    /*!
      Constructs a free/busy from a list of periods.

      \a busyPeriods is a list of periods.

    */
    explicit FreeBusy(const QList<FreeBusyPeriod> &busyPeriods);

    /*!
      Constructs a free/busy from a single period.

      \a start is the start date/time of the period.

      \a end is the end date/time of the period.

    */
    FreeBusy(const QDateTime &start, const QDateTime &end);

    /*!
      Constructs a freebusy for a specified list of events given a single period.

      \a events is a list of events.

      \a start is the start date/time of the period.

      \a end is the end date/time of the period.

    */
    FreeBusy(const QList<QSharedPointer<Event>> &events, const QDateTime &start, const QDateTime &end);

    /*!
      Destroys a free/busy.
    */
    ~FreeBusy() override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT IncidenceType type() const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT QByteArray typeStr() const override;

    /*!
      Sets the start date/time for the free/busy. Note that this date/time
      may be later or earlier than all periods within the free/busy.

      \a start is a QDateTime specifying an start datetime.

      \sa IncidenceBase::dtStart(), setDtEnd().
    */
    void setDtStart(const QDateTime &start) override;

    /*!
      Sets the end datetime for the free/busy. Note that this datetime
      may be later or earlier than all periods within the free/busy.

      \a end is a QDateTime specifying an end datetime.

      \sa dtEnd(), setDtStart().
    */
    void setDtEnd(const QDateTime &end);

    /*!
      Returns the end datetime for the free/busy.
      FIXME: calling addPeriod() does not change mDtEnd. Is that incorrect?
      \sa setDtEnd().
    */
    Q_REQUIRED_RESULT virtual QDateTime dtEnd() const;

    /*!
      \reimp
    */
    void shiftTimes(const QTimeZone &oldZone, const QTimeZone &newZone) override;

    /*!
      Returns the list of all periods within the free/busy.
    */
    Q_REQUIRED_RESULT QList<Period> busyPeriods() const;

    /*!
      Returns the list of all periods within the free/busy.
    */
    Q_REQUIRED_RESULT QList<FreeBusyPeriod> fullBusyPeriods() const;

    /*!
      Adds a period to the freebusy list and sorts the list.

      \a start is the start datetime of the period.

      \a end is the end datetime of the period.

    */
    void addPeriod(const QDateTime &start, const QDateTime &end);

    /*!
      Adds a period to the freebusy list and sorts the list.

      \a start is the start datetime of the period.

      \a duration is the Duration of the period.

    */
    void addPeriod(const QDateTime &start, const Duration &duration);

    /*!
      Adds a list of periods to the freebusy object and then sorts that list.
      Use this if you are adding many items, instead of the addPeriod method,
      to avoid sorting repeatedly.

      \a list is a list of Period objects.

    */
    void addPeriods(const QList<Period> &list);

    /*!
      Adds a list of periods to the freebusy object and then sorts that list.
      Use this if you are adding many items, instead of the addPeriod method,
      to avoid sorting repeatedly.

      \a list is a list of FreeBusyPeriod objects.

    */
    void addPeriods(const QList<FreeBusyPeriod> &list);

    /*!
      Sorts the list of free/busy periods into ascending order.
    */
    void sortList();

    /*!
      Merges another free/busy into this free/busy.

      \a freebusy is a pointer to a valid FreeBusy object.

    */
    void merge(const QSharedPointer<FreeBusy> &freebusy);

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
       Returns the Akonadi specific sub MIME type of a KCalendarCore::FreeBusy.
    */
    Q_REQUIRED_RESULT static QLatin1String freeBusyMimeType();

protected:
    /*!
      Compare this with \a freebusy for equality.

      \a freebusy is the FreeBusy to compare.

      Returns true if \a freebusy is equal to this object, or false if they are different.
    */
    bool equals(const IncidenceBase &freebusy) const override;

    /*!
      \reimp
    */
    IncidenceBase &assign(const IncidenceBase &other) override;

    /*!
      \reimp
    */
    void virtual_hook(VirtualHook id, void *data) override;

private:
    Q_DECLARE_PRIVATE(FreeBusy)

    /*!
      \reimp
    */
    bool accept(Visitor &v, const QSharedPointer<IncidenceBase> &incidence) override;

    /*!
      Disabled, otherwise could be dangerous if you subclass FreeBusy.
      Use IncidenceBase::operator= which is safe because it calls
      virtual function assign().

      \a other is another FreeBusy object to assign to this one.

     */
    FreeBusy &operator=(const FreeBusy &other) = delete;
};

/*!
  Serializes the \a freebusy object into the \a stream.
*/
KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &stream, const QSharedPointer<KCalendarCore::FreeBusy> &freebusy);
/*!
  Initializes the \a freebusy object from the \a stream.
*/
KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &stream, QSharedPointer<KCalendarCore::FreeBusy> &freebusy);
}

//@cond PRIVATE
Q_DECLARE_TYPEINFO(QSharedPointer<KCalendarCore::FreeBusy>, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(QSharedPointer<KCalendarCore::FreeBusy>)
//@endcond

#endif
