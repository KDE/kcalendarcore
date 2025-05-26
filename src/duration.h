/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2007 David Jarvie <djarvie@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Duration class.
*/

#ifndef KCALCORE_DURATION_H
#define KCALCORE_DURATION_H

#include "kcalendarcore_export.h"

#include <QDataStream>
#include <QHash>
#include <QMetaType>

class QDateTime;

namespace KCalendarCore
{
/*!
  \class KCalendarCore::Duration
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Duration
  \brief
  Represents a span of time measured in seconds or days.

  A duration is a span of time measured in seconds or days.  Construction can
  be done by specifying a stop and end time, or simply by specifying the number
  of seconds or days.

  Much of the time, it does not matter whether a duration is specified in
  seconds or in days. But it does make a difference when a duration is used to
  define a time period encompassing a daylight saving time change.
*/
class KCALENDARCORE_EXPORT Duration
{
public:
    /*!
      \enum KCalendarCore::Duration::Type
      \brief The unit of time used to define the duration.
      \value Seconds
      Duration is a number of seconds.
      \value Days
      Duration is a number of days.
    */
    enum Type {
        Seconds,
        Days,
    };

    /*!
      Constructs a duration of 0 seconds.
    */
    Duration();

    /*!
      Constructs a duration from \a start to \a end.

      If the time of day in \a start and \a end is equal, and their time
      specifications (i.e. time zone etc.) are the same, the duration will be
      set in terms of days. Otherwise, the duration will be set in terms of
      seconds.

      \a start is the time the duration begins.

      \a end is the time the duration ends.

    */
    Duration(const QDateTime &start, const QDateTime &end);

    /*!
      Constructs a duration from \a start to \a end.

      If \a type is Days, and the time of day in \a start's time zone differs
      between \a start and \a end, the duration will be rounded down to the
      nearest whole number of days.

      \a start is the time the duration begins.

      \a end is the time the duration ends.

      \a type the unit of time to use (seconds or days)

    */
    Duration(const QDateTime &start, const QDateTime &end, Type type);

    // Keep the following implicit since instances are often used in integer evaluations.
    /*!
      Constructs a duration with a number of seconds or days.

      \a duration the number of seconds or days in the duration

      \a type the unit of time to use (seconds or days)

    */
    Duration(int duration, Type type = Seconds);

    /*!
      Constructs a duration by copying another duration object.

      \a duration is the duration to copy.

    */
    Duration(const Duration &duration);

    /*!
      Destroys a duration.
    */
    ~Duration();

    /*!
      Sets this duration equal to \a duration.

      \a duration is the duration to copy.

    */
    Duration &operator=(const Duration &duration);

    /*!
      Returns true if this duration is non-zero.
    */
    operator bool() const;

    /*!
      Returns true if this duration is zero.
    */
    bool operator!() const
    {
        return !operator bool();
    }

    /*!
      Returns true if this duration is smaller than the \a other.

      \a other is the other duration to compare.

    */
    bool operator<(const Duration &other) const;

    /*!
      Returns true if this duration is smaller than or equal to the \a other.

      \a other is the other duration to compare.

    */
    bool operator<=(const Duration &other) const
    {
        return !other.operator<(*this);
    }

    /*!
      Returns true if this duration is greater than the \a other.

      \a other is the other duration to compare.

    */
    bool operator>(const Duration &other) const
    {
        return other.operator<(*this);
    }

    /*!
      Returns true if this duration is greater than or equal to the \a other.

      \a other is the other duration to compare.

    */
    bool operator>=(const Duration &other) const
    {
        return !operator<(other);
    }

    /*!
      Returns true if this duration is equal to the \a other.
      Daily and non-daily durations are always considered unequal, since a
      day's duration may differ from 24 hours if it happens to span a daylight
      saving time change.

      \a other the other duration to compare

    */
    bool operator==(const Duration &other) const;

    /*!
      Returns true if this duration is not equal to the \a other.
      Daily and non-daily durations are always considered unequal, since a
      day's duration may differ from 24 hours if it happens to span a daylight
      saving time change.

      \a other is the other duration to compare.

    */
    bool operator!=(const Duration &other) const
    {
        return !operator==(other);
    }

    /*!
      Adds another duration to this one.
      If one is in terms of days and the other in terms of seconds,
      the result is in terms of seconds.
      \a other the other duration to add
    */
    Duration &operator+=(const Duration &other);

    /*!
      Adds two durations.
      If one is in terms of days and the other in terms of seconds,
      the result is in terms of seconds.

      \a other the other duration to add

      Returns combined duration
    */
    Duration operator+(const Duration &other) const
    {
        return Duration(*this) += other;
    }

    /*!
      Returns the negative of this duration.
    */
    Duration operator-() const;

    /*!
      Subtracts another duration from this one.
      If one is in terms of days and the other in terms of seconds,
      the result is in terms of seconds.

      \a other the other duration to subtract

    */
    Duration &operator-=(const Duration &other);

    /*!
      Returns the difference between another duration and this.
      If one is in terms of days and the other in terms of seconds,
      the result is in terms of seconds.

      \a other the other duration to subtract

      Returns difference in durations
    */
    Duration operator-(const Duration &other) const
    {
        return Duration(*this) -= other;
    }

    /*!
      Multiplies this duration by a value.

      \a value value to multiply by

    */
    Duration &operator*=(int value);

    /*!
      Multiplies a duration by a value.

      \a value value to multiply by

      Returns resultant duration
    */
    Duration operator*(int value) const
    {
        return Duration(*this) *= value;
    }

    /*!
      Divides this duration by a value.

      \a value value to divide by
    */
    Duration &operator/=(int value);

    /*!
      Divides a duration by a value.

      \a value value to divide by

      Returns resultant duration
    */
    Duration operator/(int value) const
    {
        return Duration(*this) /= value;
    }

    /*!
      Computes a duration end time by adding the number of seconds or
      days in the duration to the specified \a start time.

      \a start is the start time.

      Returns end time.
    */
    Q_REQUIRED_RESULT QDateTime end(const QDateTime &start) const;

    /*!
      Returns the time units (seconds or days) used to specify the duration.
    */
    Q_REQUIRED_RESULT Type type() const;

    /*!
      Returns whether the duration is specified in terms of days rather
      than seconds.
    */
    Q_REQUIRED_RESULT bool isDaily() const;

    /*!
      Returns the length of the duration in seconds.
    */
    Q_REQUIRED_RESULT int asSeconds() const;

    /*!
      Returns the length of the duration in days. If the duration is
      not an exact number of days, it is rounded down to return the
      number of whole days.
    */
    Q_REQUIRED_RESULT int asDays() const;

    /*!
      Returns the length of the duration in seconds or days.

      Returns if isDaily(), duration in days, else duration in seconds
    */
    Q_REQUIRED_RESULT int value() const;

    /*!
      Returns true if the duration is 0 seconds.
    */
    Q_REQUIRED_RESULT bool isNull() const;

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond

    friend KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &s, const KCalendarCore::Duration &);
    friend KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &s, KCalendarCore::Duration &);
};

/*!
 * Duration serializer.
 *
 * \since 4.12
 */
KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &out, const KCalendarCore::Duration &);

/*!
 * Duration deserializer.
 *
 * \since 4.12
 */
KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &in, KCalendarCore::Duration &);

inline size_t qHash(const Duration &duration, size_t seed = 0)
{
    return qHashMulti(seed, duration.isDaily(), duration.asSeconds());
}

}

Q_DECLARE_METATYPE(KCalendarCore::Duration)

#endif
