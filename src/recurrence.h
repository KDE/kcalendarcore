/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 1998 Preston Brown <pbrown@kde.org>
  SPDX-FileCopyrightText: 2001, 2003 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2002, 2006 David Jarvie <djarvie@kde.org>
  SPDX-FileCopyrightText: 2005 Reinhold Kainhofer <reinhold@kainhofer.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KCALCORE_RECURRENCE_H
#define KCALCORE_RECURRENCE_H

#include "kcalendarcore_export.h"
#include "recurrencerule.h"
#include "period.h"

class QBitArray;
class QTimeZone;

namespace KCalendarCore
{
class RecurrenceRule;

/*!
  \class KCalendarCore::Recurrence
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Recurrence
  \brief This class represents a recurrence rule for a calendar incidence.

  It manages all recurrence rules, recurrence date/times, exception rules
  and exception date times that can appear inside calendar items.
  Each recurrence rule and exception rule is represented as an object
  of type RecurrenceRule.

  For the simple case where at most one recurrence
  rule is present, this class provides shortcut methods to set the type:
    setMinutely()
    setHourly()
    setDaily()
    setWeekly()
    setMonthly()
    setYearly()
  to set/get general information about the recurrence:
    setEndDate()
    setEndDateTime()
    duration()
    durationTo()
    setDuration()
    frequency()
    setFrequency()
  and to set/get specific information about the recurrence within the interval:
    days()
    monthDays()
    monthPositions()
    yearDays()
    yearDates()
    yearMonths()
    yearPositions()
    addMonthlyPos()
    addMonthlyDate()
    addYearlyDay()
    addYearlyDate()
    addYearlyPos()
    addYearlyMonth()
  These are all available so that you don't have to work on the RecurrenceRule
  objects themselves.
  In other words, in that simple situation the interface stays almost the
  same compared to the old Recurrence class, which allowed only one
  recurrence rule.

  As soon as your recurrence consists of multiple recurrence rules or exception
  rules, you cannot use the methods mentioned above any more (since each rule
  will have a different type and different settings). If you still call
  any of them, the set*ly methods will remove all rules and add one rule with
  the specified type. The add* and the other set* methods will change only
  the first recurrence rule, but leave the others untouched.
*/
class KCALENDARCORE_EXPORT Recurrence : public RecurrenceRule::RuleObserver
{
public:
    class RecurrenceObserver
    {
    public:
        virtual ~RecurrenceObserver();
        /*! This method will be called on each change of the recurrence object */
        virtual void recurrenceUpdated(Recurrence *r) = 0;
    };

    /*!
       \brief Describes how an event recurs, if at all.
       \value rNone
       \value rMinutely
       \value rHourly
       \value rDaily
       \value rWeekly
       \value rMonthlyPos
       \value rMonthlyDay
       \value rYearlyMonth
       \value rYearlyDay
       \value rYearlyPos
       \value rOther
       \value rMax
     */
    enum {
        rNone = 0,
        rMinutely = 0x001,
        rHourly = 0x0002,
        rDaily = 0x0003,
        rWeekly = 0x0004,
        rMonthlyPos = 0x0005,
        rMonthlyDay = 0x0006,
        rYearlyMonth = 0x0007,
        rYearlyDay = 0x0008,
        rYearlyPos = 0x0009,
        rOther = 0x000A,
        rMax = 0x00FF,
    };

    /*!
      Constructs an empty recurrence.
    */
    Recurrence();

    /*!
      Copy constructor.

      \a r instance to copy from

    */
    Recurrence(const Recurrence &r);

    /*!
      Destructor.
    */
    ~Recurrence() override;

    /*!
      Comparison operator for equality.

      \a r is the instance to compare with

      Returns true if recurrences are the same, false otherwise
    */
    bool operator==(const Recurrence &r) const;

    /*!
      Comparison operator for inequality.

      \a r is the instance to compare with

      Returns true if recurrences are the different, false if the same
    */
    bool operator!=(const Recurrence &r) const
    {
        return !operator==(r);
    }

    Recurrence &operator=(const Recurrence &r) = delete;

    /*! Return the start date/time of the recurrence (Time for all-day recurrences will be 0:00).
     Returns the current start/time of the recurrence. */
    Q_REQUIRED_RESULT QDateTime startDateTime() const;
    /*! Return the start date/time of the recurrence */
    Q_REQUIRED_RESULT QDate startDate() const;
    /*!
       Sets the start of the recurrence.

       \a start the new start date or date/time of the recurrence.

       \a isAllDay if true, the recurrence is set to all-day.  Otherwise the recurrence is set
       to non-all-day.

    */
    void setStartDateTime(const QDateTime &start, bool isAllDay);

    /*!
       Sets whether the recurrence has no time, just a date.
       All-day means -- according to rfc2445 -- that the event has no time
       associated.
       N.B. This property is derived by default from whether setStartDateTime() is
       called with a date-only or date/time parameter.
       Returns whether the recurrence has a time (false) or it is just a date (true).
     */
    Q_REQUIRED_RESULT bool allDay() const;
    /*!
       Sets whether the dtstart is a all-day (i.e. has no time attached)

       \a allDay If the recurrence is for all-day item (true) or has a time associated (false).

     */
    void setAllDay(bool allDay);

    /*!
       Sets if recurrence is read-only (if \a readOnly is true) or can be changed (if \a readOnly is false).
     */
    void setRecurReadOnly(bool readOnly);

    /*!
       Returns true if the recurrence is read-only, or false if it can be changed.
     */
    Q_REQUIRED_RESULT bool recurReadOnly() const;

    /*!
       Returns whether the event recurs at all.
     */
    Q_REQUIRED_RESULT bool recurs() const;

    /*!
       Returns the event's recurrence status.  See the enumeration at the top
       of this file for possible values.
     */
    Q_REQUIRED_RESULT ushort recurrenceType() const;

    /*!
     * Returns the recurrence status for a recurrence rule.
     * See the enumeration at the top of this file for possible values.
     *
     * \a rrule the recurrence rule to get the type for
     *
     */
    static ushort recurrenceType(const RecurrenceRule *rrule);

    /*!
      Returns true if the date specified is one on which the event will recur.

      \a date is the date to check.

      \a timeZone is the time zone for the \a date.

    */
    bool recursOn(const QDate &date, const QTimeZone &timeZone) const;

    /*!
      Returns true if the date/time specified is one at which the event will
      recur. Times are rounded down to the nearest minute to determine the
      result.

      \a dt is the date/time to check.

    */
    bool recursAt(const QDateTime &dt) const;

    /*!
      Removes all recurrence rules. Recurrence dates and exceptions are
      not removed.
    */
    void unsetRecurs();

    /*!
      Removes all recurrence and exception rules and dates.
    */
    void clear();

    /*!
     * Returns a list of the times on the specified date at which the
     * recurrence will occur. The returned times should be interpreted in the
     * context of \a timeZone.
     *
     * \a date the date for which to find the recurrence times
     *
     * \a timeZone timeZone for \a date
     *
     */
    Q_REQUIRED_RESULT TimeList recurTimesOn(const QDate &date, const QTimeZone &timeZone) const;

    /*!
     * Returns a list of all the times at which the recurrence will occur
     * between two specified times.
     *
     * There is a (large) maximum limit to the number of times returned. If due to
     * this limit the list is incomplete, this is indicated by the last entry being
     * set to an invalid QDateTime value. If you need further values, call the
     * method again with a start time set to just after the last valid time returned.
     *
     * \a start inclusive start of interval
     *
     * \a end inclusive end of interval
     *
     * Returns list of date/time values
     */
    Q_REQUIRED_RESULT QList<QDateTime> timesInInterval(const QDateTime &start, const QDateTime &end) const;

    /*!
     * Returns the start date/time of the earliest recurrence with a start date/time after
     * the specified date/time.
     * If the recurrence has no time, the next date after the specified date is returned.
     *
     * \a preDateTime the date/time after which to find the recurrence.
     *
     * Returns start date/time of next recurrence (strictly later than the given
     *         QDateTime), or invalid date if none.
     */
    Q_REQUIRED_RESULT QDateTime getNextDateTime(const QDateTime &preDateTime) const;

    /*!
     * Returns the date and time of the last previous recurrence, before the specified date/time.
     * If a time later than 00:00:00 is specified and the recurrence has no time, 00:00:00 on
     * the specified date is returned if that date recurs.
     *
     * \a afterDateTime the date/time before which to find the recurrence.
     *
     * Returns date/time of previous recurrence (strictly earlier than the given
     *         QDateTime), or invalid date if none.
     */
    Q_REQUIRED_RESULT QDateTime getPreviousDateTime(const QDateTime &afterDateTime) const;

    /*! Returns frequency of recurrence, in terms of the recurrence time period type. */
    Q_REQUIRED_RESULT int frequency() const;

    /*! Sets the frequency of recurrence to \a freq, in terms of the recurrence time period type. */
    void setFrequency(int freq);

    /*!
     * Returns -1 if the event recurs infinitely, 0 if the end date is set,
     * otherwise the total number of recurrences, including the initial occurrence.
     */
    Q_REQUIRED_RESULT int duration() const;

    /*!
     * Sets the total number of times the event is to occur to \a duration, including both the
     * first and last.
     * */
    void setDuration(int duration);

    /*!
     * Returns the number of recurrences up to and including the date/time specified.
     * \warning This function can be very time consuming - use it sparingly!
     */
    Q_REQUIRED_RESULT int durationTo(const QDateTime &dt) const;

    /*!
     * Returns the number of recurrences up to and including the \a date specified.
     * \warning This function can be very time consuming - use it sparingly!
     */
    Q_REQUIRED_RESULT int durationTo(const QDate &date) const;

    /*!
     * Returns the date/time of the last recurrence.
     * An invalid date is returned if the recurrence has no end.
     */
    Q_REQUIRED_RESULT QDateTime endDateTime() const;

    /*!
     * Returns the date of the last recurrence.
     * An invalid date is returned if the recurrence has no end.
     */
    Q_REQUIRED_RESULT QDate endDate() const;

    /*!
     * Sets the date of the last recurrence. The end time is set to the recurrence start time.
     *
     * \a endDate the ending date after which to stop recurring. If the
     * recurrence is not all-day, the end time will be 23:59.
     */
    void setEndDate(const QDate &endDate);

    /*!
     * Sets the date and time of the last recurrence.
     *
     * \a endDateTime the ending date/time after which to stop recurring.
     */
    void setEndDateTime(const QDateTime &endDateTime);

    /*!
      Shift the times of the recurrence so that they appear at the same clock
      time as before but in a new time zone. The shift is done from a viewing
      time zone rather than from the actual recurrence time zone.

      For example, shifting a recurrence whose start time is 09:00 America/New York,
      using an old viewing time zone (\a oldZone) of Europe/London, to a new time
      zone (\a newZone) of Europe/Paris, will result in the time being shifted
      from 14:00 (which is the London time of the recurrence start) to 14:00 Paris
      time.

      \a oldZone the time specification which provides the clock times

      \a newZone the new time specification

    */
    void shiftTimes(const QTimeZone &oldZone, const QTimeZone &newZone);

    /*!
        Sets an event to recur minutely. By default infinite recurrence is used.
        To set an end date use the method setEndDate and to set the number
        of occurrences use setDuration.

        This method clears all recurrence rules and adds one rule with a
        minutely recurrence. All other recurrence components (recurrence
        date/times, exception date/times and exception rules) are not
        modified.

        \a freq the frequency to recur, e.g. 2 is every other minute

     */
    void setMinutely(int freq);

    /*!
        Sets an event to recur hourly. By default infinite recurrence is used.
        The minute of the recurrence is taken from the start date (if you
        need to change it, you will have to modify the defaultRRule's
        byMinute list manually.
        To set an end date use the method setEndDate and to set the number
        of occurrences use setDuration.

        This method clears all recurrence rules and adds one rule with a
        hourly recurrence. All other recurrence components (recurrence
        date/times, exception date/times and exception rules) are not
        modified.

        \a freq the frequency to recur, e.g. 2 is every other hour

     */
    void setHourly(int freq);

    /*!
        Sets an event to recur daily. By default infinite recurrence is used.
        The minute and second of the recurrence is taken from the start date
        (if you need to change them, you will have to modify the defaultRRule's
        byMinute list manually.
        To set an end date use the method setEndDate and to set the number
        of occurrences use setDuration.

        This method clears all recurrence rules and adds one rule with a
        daily recurrence. All other recurrence components (recurrence
        date/times, exception date/times and exception rules) are not
        modified.

        \a freq the frequency to recur, e.g. 2 is every other day

     */
    void setDaily(int freq);

    /*!
        Sets an event to recur weekly. By default infinite recurrence is used.
        To set an end date use the method setEndDate and to set the number
        of occurrences use setDuration.

        This method clears all recurrence rules and adds one rule with a
        weekly recurrence. All other recurrence components (recurrence
        date/times, exception date/times and exception rules) are not
        modified.

        \a freq the frequency to recur, e.g. every other week etc.

        \a weekStart the first day of the week (Monday=1 .. Sunday=7, default is Monday).

     */
    void setWeekly(int freq, int weekStart = 1);
    /*!
        Sets an event to recur weekly. By default infinite recurrence is used.
        To set an end date use the method setEndDate and to set the number
        of occurrences use setDuration.

        This method clears all recurrence rules and adds one rule with a
        weekly recurrence. All other recurrence components (recurrence
        date/times, exception date/times and exception rules) are not
        modified.

        \a freq the frequency to recur, e.g. every other week etc.

        \a days a 7 bit array indicating which days on which to recur (bit 0 = Monday).

        \a weekStart the first day of the week (Monday=1 .. Sunday=7, default is Monday).

     */
    void setWeekly(int freq, const QBitArray &days, int weekStart = 1);

    /*!
       Adds days to the weekly day recurrence list.

       \a days a 7 bit array indicating which days on which to recur (bit 0 = Monday).

     */
    void addWeeklyDays(const QBitArray &days);
    /*!
     * Returns the first day of the week.  Uses only the
     * first RRULE if present (i.e. a second RRULE as well as all EXRULES are
     * ignored!
     * Returns Weekday of the first day of the week (Monday=1 .. Sunday=7)
     */
    int weekStart() const;

    /*! Returns week day mask (bit 0 = Monday). */
    Q_REQUIRED_RESULT QBitArray days() const; // Emulate the old behavior

    /*!
        Sets an event to recur monthly. By default infinite recurrence is used.
        The date of the monthly recurrence will be taken from the start date
        unless you explicitly add one or more recurrence dates with
        addMonthlyDate or a recurrence position in the month (e.g. first
        monday) using addMonthlyPos.
        To set an end date use the method setEndDate and to set the number
        of occurrences use setDuration.

        This method clears all recurrence rules and adds one rule with a
        monthly recurrence. All other recurrence components (recurrence
        date/times, exception date/times and exception rules) are not
        modified.

        \a freq the frequency to recur, e.g. 3 for every third month.

     */
    void setMonthly(int freq);

    /*!
     * Adds a position (e.g. first monday) to the monthly recurrence rule.
     *
     * \a pos the position in the month for the recurrence, with valid
     * values being 1-5 (5 weeks max in a month).
     *
     * \a days the days for the position to recur on (bit 0 = Monday).
     *
     * Example: pos = 2, and bits 0 and 2 are set in days:
     * the rule is to repeat every 2nd Monday and Wednesday in the month.
     */
    void addMonthlyPos(short pos, const QBitArray &days);
    void addMonthlyPos(short pos, ushort day);

    void setMonthlyPos(const QList<RecurrenceRule::WDayPos> &monthlyDays);

    /*!
     * Adds a date (e.g. the 15th of each month) to the monthly day
     * recurrence list.
     *
     * \a day the date in the month to recur.
     *
     */
    void addMonthlyDate(short day);

    void setMonthlyDate(const QList<int> &monthlyDays);

    /*! Returns list of day positions in months. */
    Q_REQUIRED_RESULT QList<RecurrenceRule::WDayPos> monthPositions() const;

    // Emulate old behavior
    /*! Returns list of day numbers of a  month. */
    Q_REQUIRED_RESULT QList<int> monthDays() const;

    /*!
       Sets an event to recur yearly. By default, this will recur every year
        on the same date (e.g. every year on April 15 if the start date was
        April 15).
        The day of the year can be specified with addYearlyDay().
        The day of the month can be specified with addYearlyByDate
        If both a month and a day ar specified with addYearlyMonth and
        addYearlyDay, the day is understood as day number within the month.

        A position (e.g. 3rd Sunday of year/month, or last Friday of year/month)
        can be specified with addYearlyPos. Again, if a month is specified,
        this position is understood as within that month, otherwise within
        the year.

        By default infinite recurrence is used. To set an end date use the
        method setEndDate and to set the number of occurrences use setDuration.

        This method clears all recurrence rules and adds one rule with a
        yearly recurrence. All other recurrence components (recurrence
        date/times, exception date/times and exception rules) are not
        modified.

        \a freq the frequency to recur, e.g. 3 for every third year.

     */
    void setYearly(int freq);

    /*!
     * Adds day number of year within a yearly recurrence.
     * By default infinite recurrence is used. To set an end date use the
     * method setEndDate and to set the number of occurrences use setDuration.
     *
     * \a day the day of the year for the event. E.g. if day is 60, this
     *            means Feb 29 in leap years and March 1 in non-leap years.
     *
     */
    void addYearlyDay(int day);

    void setYearlyDay(const QList<int> &days);

    /*!
     * Adds date within a yearly recurrence. The month(s) for the recurrence
     * can be specified with addYearlyMonth(), otherwise the month of the
     * start date is used.
     *
     * By default infinite recurrence is used. To set an end date use the
     * method setEndDate and to set the number of occurrences use setDuration.
     *
     * \a date the day of the month for the event
     *
     */
    void addYearlyDate(int date);

    void setYearlyDate(const QList<int> &dates);

    /*!
     * Adds month in yearly recurrence. You can specify specific day numbers
     * within the months (by calling addYearlyDate()) or specific day positions
     * within the month (by calling addYearlyPos).
     *
     * \a _rNum the month in which the event shall recur.
     *
     */
    void addYearlyMonth(short _rNum);

    void setYearlyMonth(const QList<int> &months);

    /*!
     * Adds position within month/year within a yearly recurrence. If months
     * are specified (via addYearlyMonth()), the parameters are understood as
     * position within these months, otherwise within the year.
     *
     * By default infinite recurrence is used.
     * To set an end date use the method setEndDate and to set the number
     * of occurrences use setDuration.
     *
     * \a pos the position in the month/year for the recurrence, with valid
     * values being 1 to 53 and -1 to -53 (53 weeks max in a year).
     *
     * \a days the days for the position to recur on (bit 0 = Monday).
     *
     * Example: pos = 2, and bits 0 and 2 are set in days
     * If months are specified (via addYearlyMonth), e.g. March, the rule is
     * to repeat every year on the 2nd Monday and Wednesday of March.
     * If no months are specified, the file is to repeat every year on the
     * 2nd Monday and Wednesday of the year.
     */
    void addYearlyPos(short pos, const QBitArray &days);

    void setYearlyPos(const QList<RecurrenceRule::WDayPos> &days);

    /*!
     * Returns the day numbers within a yearly recurrence.
     * Returns the days of the year for the event. E.g. if the list contains
     *         60, this means the recurrence happens on day 60 of the year, i.e.
     *         on Feb 29 in leap years and March 1 in non-leap years.
     */
    Q_REQUIRED_RESULT QList<int> yearDays() const;

    /*!
     * Returns the dates within a yearly recurrence.
     * Returns the days of the month for the event. E.g. if the list contains
     *         13, this means the recurrence happens on the 13th of the month.
     *         The months for the recurrence can be obtained through
     *         yearlyMonths(). If this list is empty, the month of the start
     *         date is used.
     */
    Q_REQUIRED_RESULT QList<int> yearDates() const;

    /*!
     * Returns the months within a yearly recurrence.
     * Returns the months for the event. E.g. if the list contains
     *         11, this means the recurrence happens in November.
     *         The days for the recurrence can be obtained either through
     *         yearDates() if they are given as dates within the month or
     *         through yearlyPositions() if they are given as positions within the
     *         month. If none is specified, the date of the start date is used.
     */
    Q_REQUIRED_RESULT QList<int> yearMonths() const;

    /*!
     * Returns the positions within a yearly recurrence.
     * Returns the positions for the event, either within a month (if months
     *         are set through addYearlyMonth()) or within the year.
     *         E.g. if the list contains {Pos=3, Day=5}, this means the third
     *         friday. If a month is set this position is understoodas third
     *         Friday in the given months, otherwise as third Friday of the
     *         year.
     */
    Q_REQUIRED_RESULT QList<RecurrenceRule::WDayPos> yearPositions() const;

    /*! Upper date limit for recurrences */
    static const QDate MAX_DATE;

    /*!
      Debug output.
    */
    void dump() const;

    // RRULE
    Q_REQUIRED_RESULT RecurrenceRule::List rRules() const;
    /*!
      Add a recurrence rule to the recurrence.

      \a rrule the recurrence rule to add

     */
    void addRRule(RecurrenceRule *rrule);

    /*!
      Remove a recurrence rule from the recurrence.

      \a rrule is not deleted; it is the responsibility of the caller
      to ensure that it is deleted.

      \a rrule the recurrence rule to remove

     */
    void removeRRule(RecurrenceRule *rrule);

    /*!
      Remove a recurrence rule from the recurrence and delete it.

      \a rrule the recurrence rule to remove

     */
    void deleteRRule(RecurrenceRule *rrule);

    // EXRULE
    Q_REQUIRED_RESULT RecurrenceRule::List exRules() const;

    /*!
      Add an exception rule to the recurrence.

      \a exrule the exception rule to add

     */
    void addExRule(RecurrenceRule *exrule);

    /*!
      Remove an exception rule from the recurrence.

      \a exrule is not deleted; it is the responsibility of the caller
      to ensure that it is deleted.

      \a exrule the exception rule to remove

     */
    void removeExRule(RecurrenceRule *exrule);

    /*!
      Remove an exception rule from the recurrence and delete it.

      \a exrule the exception rule to remove

     */
    void deleteExRule(RecurrenceRule *exrule);

    // RDATE
    Q_REQUIRED_RESULT QList<QDateTime> rDateTimes() const;
    Q_REQUIRED_RESULT DateList rDates() const;
    /*!
      Inquire if the given RDATE is associated to a PERIOD.

      \a rdate a given RDATE in date time format.

      Returns a Period, invalid if there is no period associated to \a rdate.
      \since 5.87
     */
    Q_REQUIRED_RESULT Period rDateTimePeriod(const QDateTime &rdate) const;
    void setRDateTimes(const QList<QDateTime> &rdates);
    void setRDates(const DateList &rdates);
    void addRDateTime(const QDateTime &rdate);
    void addRDate(const QDate &rdate);
    /*!
      Add a RDATE defined as a PERIOD. The start date-time of the period is added
      to the recurring date time list, and will be listed by a call to
      rDateTimes(). Use then rDateTimePeriod() to fetch the associated
      period definition.

      \a period a given RDATE in period definition.

      \since 5.87
     */
    void addRDateTimePeriod(const Period &period);

    // ExDATE
    Q_REQUIRED_RESULT QList<QDateTime> exDateTimes() const;
    Q_REQUIRED_RESULT DateList exDates() const;
    void setExDateTimes(const QList<QDateTime> &exdates);
    void setExDates(const DateList &exdates);
    void addExDateTime(const QDateTime &exdate);
    void addExDate(const QDate &exdate);

    RecurrenceRule *defaultRRule(bool create = false) const;
    RecurrenceRule *defaultRRuleConst() const;
    void updated();

    /*!
      Installs an observer. Whenever some setting of this recurrence
      object is changed, the recurrenceUpdated( Recurrence* ) method
      of each observer will be called to inform it of changes.

      \a observer the Recurrence::Observer-derived object, which
      will be installed as an observer of this object.

    */
    void addObserver(RecurrenceObserver *observer);
    /*!
      Removes an observer that was added with addObserver. If the
      given object was not an observer, it does nothing.

      \a observer the Recurrence::Observer-derived object to
      be removed from the list of observers of this object.

    */
    void removeObserver(RecurrenceObserver *observer);

    void recurrenceChanged(RecurrenceRule *) override;

protected:
    RecurrenceRule *setNewRecurrenceType(RecurrenceRule::PeriodType type, int freq);

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond

    friend KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &out, KCalendarCore::Recurrence *);
    friend KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &in, KCalendarCore::Recurrence *);
};

/*!
 * Recurrence serializer.
 * \since 4.12
 *
 * \a out is the output data stream
 *
 */
KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &out, KCalendarCore::Recurrence *);

/*!
 * Recurrence deserializer.
 * \since 4.12
 *
 * \a in is the input data stream
 *
 */
KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &in, KCalendarCore::Recurrence *);

}

#endif
