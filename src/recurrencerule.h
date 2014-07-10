/*
  This file is part of the kcalcore library.

  Copyright (c) 1998 Preston Brown <pbrown@kde.org>
  Copyright (c) 2001,2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (c) 2002,2006,2007 David Jarvie <software@astrojar.org.uk>
  Copyright (c) 2005, Reinhold Kainhofer <reinhold@kainhofer.com>

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
#ifndef KCALCORE_RECURRENCERULE_H
#define KCALCORE_RECURRENCERULE_H

#include "kcalcore_export.h"
#include "sortablelist.h"

#include <KDateTime>

namespace KCalCore {

// These two are duplicates wrt. incidencebase.h
typedef SortableList<KDateTime> DateTimeList;
typedef SortableList<QDate> DateList;
/* List of times */
typedef SortableList<QTime> TimeList;

/**
  This class represents a recurrence rule for a calendar incidence.
*/
class KCALCORE_EXPORT RecurrenceRule
{
public:
    class RuleObserver
    {
    public:
        virtual ~RuleObserver();
        /** This method is called on each change of the recurrence object */
        virtual void recurrenceChanged(RecurrenceRule *) = 0;
    };
    typedef QList<RecurrenceRule*> List;

    /** enum for describing the frequency how an event recurs, if at all. */
    enum PeriodType {
        rNone = 0,
        rSecondly,
        rMinutely,
        rHourly,
        rDaily,
        rWeekly,
        rMonthly,
        rYearly
    };

    /** structure for describing the n-th weekday of the month/year. */
    class KCALCORE_EXPORT WDayPos //krazy:exclude=dpointer
    {
    public:
        explicit WDayPos(int ps = 0, short dy = 0);
        void setDay(short dy);
        short day() const;
        void setPos(int ps);
        int pos() const;

        bool operator==(const RecurrenceRule::WDayPos &pos2) const;
        bool operator!=(const RecurrenceRule::WDayPos &pos2) const;

    protected:
        short mDay; // Weekday, 1=monday, 7=sunday
        int mPos;   // week of the day (-1 for last, 1 for first, 0 for all weeks)
        // Bounded by -366 and +366, 0 means all weeks in that period

        friend KCALCORE_EXPORT QDataStream& operator<<(QDataStream &out, const KCalCore::RecurrenceRule::WDayPos &);
        friend KCALCORE_EXPORT QDataStream& operator>>(QDataStream &in, KCalCore::RecurrenceRule::WDayPos &);
    };

    // Q_DECLARE_TYPEINFO(RecurrenceRule::WDayPos, Q_MOVABLE_TYPE) TODO_KDE5

    RecurrenceRule();
    RecurrenceRule(const RecurrenceRule &r);
    ~RecurrenceRule();

    bool operator==(const RecurrenceRule &r) const;
    bool operator!=(const RecurrenceRule &r) const  {
        return !operator==(r);
    }
    RecurrenceRule &operator=(const RecurrenceRule &r);

    /** Set if recurrence is read-only or can be changed. */
    void setReadOnly(bool readOnly);

    /**
      Returns true if the recurrence is read-only; false if it can be changed.
    */
    bool isReadOnly() const;

    /**
      Returns the event's recurrence status.  See the enumeration at the top
      of this file for possible values.
    */
    bool recurs() const;
    void setRecurrenceType(PeriodType period);
    PeriodType recurrenceType() const;

    /** Turns off recurrence for the event. */
    void clear();

    /**
      Returns the recurrence frequency, in terms of the recurrence time period type.
    */
    uint frequency() const;

    /**
      Sets the recurrence frequency, in terms of the recurrence time period type.
    */
    void setFrequency(int freq);

    /**
      Returns the recurrence start date/time.
      Note that the recurrence does not necessarily occur on the start date/time.
      For this to happen, it must actually match the rule.
    */
    KDateTime startDt() const;

    /**
      Sets the recurrence start date/time.
      Note that setting the start date/time does not make the recurrence occur
      on that date/time, it simply sets a lower limit to when the recurrences
      take place (this applies only for the by- rules, not for i.e. an hourly
      rule where the startDt is the first occurrence).

      Note that setting @p start to a date-only value does not make an all-day
      recurrence: to do this, call setAllDay(true).

      @param start the recurrence's start date and time
    */
    void setStartDt(const KDateTime &start);

    /** Returns whether the start date has no time associated. All-Day
        means -- according to rfc2445 -- that the event has no time associate. */
    bool allDay() const;

    /** Sets whether the dtstart is all-day (i.e. has no time attached)
     *
     * @param allDay Whether start datetime is all-day
     */
    void setAllDay(bool allDay);

    /** Returns the date and time of the last recurrence.
     * An invalid date is returned if the recurrence has no end.
     * @param result if non-null, *result is updated to true if successful,
     * or false if there is no recurrence or its end date cannot be determined.
     */
    KDateTime endDt(bool *result = 0) const;

    /** Sets the date and time of the last recurrence.
     * @param endDateTime the ending date/time after which to stop recurring. */
    void setEndDt(const KDateTime &endDateTime);

    /**
     * Returns -1 if the event recurs infinitely, 0 if the end date is set,
     * otherwise the total number of recurrences, including the initial occurrence.
     */
    int duration() const;

    /** Sets the total number of times the event is to occur, including both the
     * first and last. */
    void setDuration(int duration);

    /** Returns the number of recurrences up to and including the date/time specified. */
    int durationTo(const KDateTime &dt) const;

    /** Returns the number of recurrences up to and including the date specified. */
    int durationTo(const QDate &date) const;

    /**
      Shift the times of the rule so that they appear at the same clock
      time as before but in a new time zone. The shift is done from a viewing
      time zone rather than from the actual rule time zone.

      For example, shifting a rule whose start time is 09:00 America/New York,
      using an old viewing time zone (@p oldSpec) of Europe/London, to a new time
      zone (@p newSpec) of Europe/Paris, will result in the time being shifted
      from 14:00 (which is the London time of the rule start) to 14:00 Paris
      time.

      @param oldSpec the time specification which provides the clock times
      @param newSpec the new time specification
    */
    void shiftTimes(const KDateTime::Spec &oldSpec, const KDateTime::Spec &newSpec);

    /** Returns true if the date specified is one on which the event will
     * recur. The start date returns true only if it actually matches the rule.
     *
     * @param date date to check
     * @param timeSpec time specification for @p date
     */
    bool recursOn(const QDate &date, const KDateTime::Spec &timeSpec) const;

    /** Returns true if the date/time specified is one at which the event will
     * recur. Times are rounded down to the nearest minute to determine the result.
     * The start date/time returns true only if it actually matches the rule.
     *
     * @param dt the date+time to check for recurrency
     */
    bool recursAt(const KDateTime &dt) const;

    /** Returns true if the date matches the rules. It does not necessarily
        mean that this is an actual occurrence. In particular, the method does
        not check if the date is after the end date, or if the frequency interval
        matches.

        @param dt the date+time to check for matching the rules
     */
    bool dateMatchesRules(const KDateTime &dt) const;

    /** Returns a list of the times on the specified date at which the
     * recurrence will occur. The returned times should be interpreted in the
     * context of @p timeSpec.
     * @param date the date for which to find the recurrence times
     * @param timeSpec time specification for @p date
     */
    TimeList recurTimesOn(const QDate &date, const KDateTime::Spec &timeSpec) const;

    /** Returns a list of all the times at which the recurrence will occur
     * between two specified times.
     *
     * There is a (large) maximum limit to the number of times returned. If due to
     * this limit the list is incomplete, this is indicated by the last entry being
     * set to an invalid KDateTime value. If you need further values, call the
     * method again with a start time set to just after the last valid time returned.
     * @param start inclusive start of interval
     * @param end inclusive end of interval
     * @return list of date/time values
     */
    DateTimeList timesInInterval(const KDateTime &start, const KDateTime &end) const;

    /** Returns the date and time of the next recurrence, after the specified date/time.
     * If the recurrence has no time, the next date after the specified date is returned.
     * @param preDateTime the date/time after which to find the recurrence.
     * @return date/time of next recurrence, or invalid date if none.
     */
    KDateTime getNextDate(const KDateTime &preDateTime) const;

    /** Returns the date and time of the last previous recurrence, before the specified date/time.
     * If a time later than 00:00:00 is specified and the recurrence has no time, 00:00:00 on
     * the specified date is returned if that date recurs.
     * @param afterDateTime the date/time before which to find the recurrence.
     * @return date/time of previous recurrence, or invalid date if none.
     */
    KDateTime getPreviousDate(const KDateTime &afterDateTime) const;

    void setBySeconds(const QList<int> &bySeconds);
    void setByMinutes(const QList<int> &byMinutes);
    void setByHours(const QList<int> &byHours);

    void setByDays(const QList<WDayPos> &byDays);
    void setByMonthDays(const QList<int> &byMonthDays);
    void setByYearDays(const QList<int> &byYearDays);
    void setByWeekNumbers(const QList<int> &byWeekNumbers);
    void setByMonths(const QList<int> &byMonths);
    void setBySetPos(const QList<int> &bySetPos);
    void setWeekStart(short weekStart);

    const QList<int> &bySeconds() const;
    const QList<int> &byMinutes() const;
    const QList<int> &byHours() const;

    const QList<WDayPos> &byDays() const;
    const QList<int> &byMonthDays() const;
    const QList<int> &byYearDays() const;
    const QList<int> &byWeekNumbers() const;
    const QList<int> &byMonths() const;
    const QList<int> &bySetPos() const;
    short weekStart() const;

    /**
      Set the RRULE string for the rule.
      This is merely stored for future reference. The string is not used in any way
      by the RecurrenceRule.

      @param rrule the RRULE string
     */
    void setRRule(const QString &rrule);
    QString rrule() const;

    void setDirty();
    /**
      Installs an observer. Whenever some setting of this recurrence
      object is changed, the recurrenceUpdated( Recurrence* ) method
      of each observer will be called to inform it of changes.
      @param observer the Recurrence::Observer-derived object, which
      will be installed as an observer of this object.
    */
    void addObserver(RuleObserver *observer);

    /**
      Removes an observer that was added with addObserver. If the
      given object was not an observer, it does nothing.
      @param observer the Recurrence::Observer-derived object to
      be removed from the list of observers of this object.
    */
    void removeObserver(RuleObserver *observer);

    /**
      Debug output.
    */
    void dump() const;

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond

    friend KCALCORE_EXPORT QDataStream& operator<<(QDataStream &out, const KCalCore::RecurrenceRule *);
    friend KCALCORE_EXPORT QDataStream& operator>>(QDataStream &in, const KCalCore::RecurrenceRule *);
};

/**
 * RecurrenceRule serializer and deserializer.
 * @since 4.12
 */
KCALCORE_EXPORT QDataStream& operator<<(QDataStream &out, const KCalCore::RecurrenceRule *);
KCALCORE_EXPORT QDataStream& operator>>(QDataStream &in, const KCalCore::RecurrenceRule *);

/**
 * RecurrenceRule::WDayPos serializer and deserializer.
 * @since 4.12
 */
KCALCORE_EXPORT QDataStream& operator<<(QDataStream &out, const KCalCore::RecurrenceRule::WDayPos &);
KCALCORE_EXPORT QDataStream& operator>>(QDataStream &in, KCalCore::RecurrenceRule::WDayPos &);

}

#endif
