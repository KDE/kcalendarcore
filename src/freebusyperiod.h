/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Period class.

  \brief
  Represents a period of time.
*/

#ifndef KCALCORE_FREEBUSYPERIOD_H
#define KCALCORE_FREEBUSYPERIOD_H

#include "kcalendarcore_export.h"
#include "period.h"

#include <QMetaType>

namespace KCalendarCore
{
/*!
  \class KCalendarCore::FreeBusyPeriod
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/FreeBusyPeriod
  \brief
  Represents a period of time.

  The period can be defined by either a start time and an end time or
  by a start time and a duration.
*/
class KCALENDARCORE_EXPORT FreeBusyPeriod : public Period
{
public:
    /*!
       \enum KCalendarCore::FreeBusyPeriod::FreeBusyType

       \value Free
       \value Busy
       \value BusyUnavailable
       \value BusyTentative
       \value Unknown
     */
    enum FreeBusyType {
        Free,
        Busy,
        BusyUnavailable,
        BusyTentative,
        Unknown,
    };

    /*!
       List of periods.
     */
    typedef QList<FreeBusyPeriod> List;

    /*!
      Constructs a period without a duration.
    */
    FreeBusyPeriod();

    /*!
      Constructs a period from \a start to \a end.

      \a start is the time the period begins.

      \a end is the time the period ends.

    */
    FreeBusyPeriod(const QDateTime &start, const QDateTime &end);

    /*!
      Constructs a period from \a start and lasting \a duration.

      \a start is the time when the period starts.

      \a duration is how long the period lasts.

    */
    FreeBusyPeriod(const QDateTime &start, const Duration &duration);

    /*!
      Constructs a period by copying another period object

      \a period is the period to copy

     */
    FreeBusyPeriod(const FreeBusyPeriod &period);

    /*!
      Constructs a period by copying another period object

      \a period is the period to copy

     */

    FreeBusyPeriod(const Period &period);

    /*!
      Destroys a period.
    */
    ~FreeBusyPeriod();

    /*!
      Sets this period equal to the \a other one.

      \a other is the other period to compare.

    */
    FreeBusyPeriod &operator=(const FreeBusyPeriod &other);

    /*!
      Sets the period summary.

      \a summary is the period summary string.

      \sa summary().
    */
    void setSummary(const QString &summary);

    /*!
      Returns the period summary.
      \sa setSummary()
    */
    Q_REQUIRED_RESULT QString summary() const;

    /*!
      Sets the period location.

      \a location is the period location string.

      \sa location().
    */
    void setLocation(const QString &location);

    /*!
      Returns the period location.
      \sa setLocation()
    */
    Q_REQUIRED_RESULT QString location() const;

    /*!
      Sets the free/busy type.

      \a type is the type of free/busy period

      \sa type().
      \since 5.0
    */
    void setType(FreeBusyType type);

    /*!
      Returns free/busy type
      \sa setType().
      \since 5.0
    */
    Q_REQUIRED_RESULT FreeBusyType type() const;

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond

    friend KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &stream, const KCalendarCore::FreeBusyPeriod &period);
    friend KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &stream, KCalendarCore::FreeBusyPeriod &period);
};

/*! Write \a period to the datastream \a stream, in binary format. */
KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &stream, const KCalendarCore::FreeBusyPeriod &period);

/*! Read a Period object into \a period from \a stream, in binary format. */
KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &stream, KCalendarCore::FreeBusyPeriod &period);
}

//@cond PRIVATE
Q_DECLARE_METATYPE(KCalendarCore::FreeBusyPeriod)
//@endcond

#endif
