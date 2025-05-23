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
#ifndef KCALCORE_PERIOD_H
#define KCALCORE_PERIOD_H

#include "duration.h"
#include "kcalendarcore_export.h"

#include <QDataStream>
#include <QDateTime>
#include <QList>
#include <QMetaType>

class QTimeZone;

namespace KCalendarCore
{
/*!
  \class KCalendarCore::Period
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Period
  \brief The period can be defined by either a start time and an end time or
  by a start time and a duration.
*/
class KCALENDARCORE_EXPORT Period
{
public:
    /*!
       List of periods.
     */
    typedef QList<Period> List;

    /*!
      Constructs a period without a duration.
    */
    Period();

    /*!
      Constructs a period from \a start to \a end.

      \a start the time the period begins.

      \a end the time the period ends.

    */
    Period(const QDateTime &start, const QDateTime &end);

    /*!
      Constructs a period from \a start and lasting \a duration.

      \a start the time when the period starts.

      \a duration how long the period lasts.

    */
    Period(const QDateTime &start, const Duration &duration);

    /*!
      Constructs a period by copying another period object

      \a period the period to copy

     */

    Period(const Period &period);

    /*!
      Destroys a period.
    */
    ~Period();

    /*!
      Returns true if the start of this period is earlier than the start of
      the \a other one.

      \a other is the other period to compare.

    */
    bool operator<(const Period &other) const;

    /*!
      Returns true if the start of this period is later than the start of
      the \a other one.

      \a other the other period to compare

    */
    bool operator>(const Period &other) const
    {
        return other.operator<(*this);
    }

    /*!
      Returns true if this period is equal to the \a other one.
      Even if their start and end times are the same, two periods are
      considered not equal if one is defined in terms of a duration and the
      other in terms of a start and end time.

      \a other the other period to compare

    */
    bool operator==(const Period &other) const;

    /*!
      Returns true if this period is not equal to the \a other one.

      \a other the other period to compare

      \sa operator==()
    */
    bool operator!=(const Period &other) const
    {
        return !operator==(other);
    }

    /*!
      Sets this period equal to the \a other one.

      \a other is the other period to compare.

    */
    Period &operator=(const Period &other);

    /*!
      Returns true if the Period is not empty.
      \since 5.87
    */
    Q_REQUIRED_RESULT bool isValid() const;

    /*!
      Returns when this period starts.
    */
    Q_REQUIRED_RESULT QDateTime start() const;

    /*!
      Returns when this period ends.
    */
    Q_REQUIRED_RESULT QDateTime end() const;

    /*!
      Returns the duration of the period.

      If the period is defined in terms of a start and end time, the duration
      is computed from these. In this case, if the time of day in start() and
      end() is equal, and their time specifications (i.e. time zone etc.) are
      the same, the duration will be set in terms of days. Otherwise, the
      duration will be set in terms of seconds.

      If the period is defined in terms of a duration, that duration is
      returned unchanged.
    */
    Q_REQUIRED_RESULT Duration duration() const;

    /*!
      Returns the duration of the period.

      If the period is defined in terms of a start and end time, the duration
      is first computed from these.

      If \a type is Days, and the duration is not an exact number of days,
      the duration will be rounded down to the nearest whole number of days.

      \a type the unit of time to use (seconds or days)

    */
    Q_REQUIRED_RESULT Duration duration(Duration::Type type) const;

    /*!
      Returns true if this period has a set duration, false
      if it just has a start and an end.
    */
    Q_REQUIRED_RESULT bool hasDuration() const;

    /*!
      Shift the times of the period so that they appear at the same clock
      time as before but in a new time zone. The shift is done from a viewing
      time zone rather than from the actual period time zone.

      For example, shifting a period whose start time is 09:00 America/New York,
      using an old viewing time zone (\a oldZone) of Europe/London, to a new
      time zone (\a newZone) of Europe/Paris, will result in the time being
      shifted from 14:00 (which is the London time of the period start) to
      14:00 Paris time.

      \a oldZone the time zone which provides the clock times

      \a newZone the new time zone

    */
    void shiftTimes(const QTimeZone &oldZone, const QTimeZone &newZone);

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond

    friend KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &stream, const KCalendarCore::Period &period);

    friend KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &stream, KCalendarCore::Period &period);
};

/*! Write \a period to the datastream \a stream, in binary format. */
KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &stream, const KCalendarCore::Period &period);

/*! Read a Period object into \a period from \a stream, in binary format. */
KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &stream, KCalendarCore::Period &period);

/*!
  Return a hash value for a Period argument.

  \a key is a Period.

*/
KCALENDARCORE_EXPORT size_t qHash(const KCalendarCore::Period &key, size_t seed = 0);
}

//@cond PRIVATE
Q_DECLARE_METATYPE(KCalendarCore::Period)
Q_DECLARE_TYPEINFO(KCalendarCore::Period, Q_RELOCATABLE_TYPE);
//@endcond

#endif
