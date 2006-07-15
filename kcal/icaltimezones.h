/*
    This file is part of the kcal library.

    Copyright (c) 2005 David Jarvie <software@astrojar.org.uk>

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

#ifndef KCAL_ICALTIMEZONES_H
#define KCAL_ICALTIMEZONES_H

#include <ktimezones.h>

#include "libkcal_export.h"

namespace KCal {

class ICalTimeZoneSource;
class ICalTimeZoneData;
class ICalTimeZonePrivate;
class ICalTimeZoneSourcePrivate;
class ICalTimeZoneDataPrivate;
class Recurrence;


/**
 * The ICalTimeZone class represents an iCalendar VTIMEZONE component.
 *
 * ICalTimeZone instances are normally created by ICalTimeZoneSource::parse().
 *
 * @short An iCalendar time zone
 * @see ICalTimeZoneSource, ICalTimeZoneData
 * @author David Jarvie <software@astrojar.org.uk>.
 */
class LIBKCAL_EXPORT ICalTimeZone : public KTimeZone
{
  public:
    /**
     * Creates a time zone. This constructor is normally called from ICalTimeZoneSource::parse().
     *
     * @param source   iCalendar VTIMEZONE reader and parser
     * @param name     time zone's unique name within the iCalendar object
     * @param data     parsed VTIMEZONE data
     */
    ICalTimeZone(ICalTimeZoneSource *source, const QString &name, ICalTimeZoneData *data);
    ICalTimeZone(const ICalTimeZone &);
    virtual ~ICalTimeZone();

    ICalTimeZone &operator=(const ICalTimeZone &);

    /**
     * Returns the name of the city for this time zone, if any. There is no fixed
     * format for the name.
     *
     * @return city name
     */
    QString city() const;

    /**
     * Returns the URL of the published VTIMEZONE definition, if any.
     *
     * @return URL
     */
    QByteArray url() const;

    /**
     * Returns the LAST-MODIFIED time of the VTIMEZONE, if any.
     *
     * @return time, or QDateTime() if none
     */
    QDateTime lastModified() const;

    /**
     * Returns the offset of this time zone to UTC at the given local date/time.
     * Because of daylight savings time shifts, the date/time may occur twice. Optionally,
     * the offsets at both occurrences of @p dateTime are calculated.
     *
     * @param zoneDateTime the date/time at which the offset is to be calculated. This
     *                     is interpreted as a local time in this time zone. An error
     *                     occurs if @p zoneDateTime.timeSpec() is not Qt::LocalTime, or
     *                     if @p zoneDateTime falls outside the time periods covered by
     *                     the time zone definitions.
     * @param secondOffset if non-null, and the @p zoneDateTime occurs twice, receives the
     *                     UTC offset for the second occurrence. Otherwise, it is set
     *                     the same as the return value.
     * @return offset in seconds, or 0 if error. If @p zoneDateTime occurs twice, it is the
     *         offset at the first occurrence which is returned.
     */
    virtual int offsetAtZoneTime(const QDateTime &zoneDateTime, int *secondOffset = 0) const;

    /**
     * Returns the offset of this time zone to UTC at the given UTC date/time.
     *
     * If @p utcDateTime is earlier than any of the defined phases for the time zone,
     * the 'previous UTC offset' for the earliest phase is returned. An error occurs
     * if @p utcDateTime is later than any of the defined phases.
     *
     * @param utcDateTime the UTC date/time at which the offset is to be calculated.
     *                    An error occurs if @p utcDateTime.timeSpec() is not Qt::UTC,
     *                    or if @p utcDateTime falls outside the time periods covered by
     *                    the time zone definitions.
     * @return offset in seconds, or 0 if error
     */
    virtual int offsetAtUtc(const QDateTime &utcDateTime) const;

    /**
     * Returns the offset of this time zone to UTC at a specified UTC time.
     *
     * Note that time_t has a more limited range than QDateTime, so consider using
     * offsetAtUtc() instead.
     *
     * @param t the UTC time at which the offset is to be calculated, measured in seconds
     *          since 00:00:00 UTC 1st January 1970 (as returned by time(2)). An error
     *          occurs if @p t falls outside the time periods covered by the time zone
     *          definitions.
     * @return offset in seconds, or 0 if error
     */
    virtual int offset(time_t t) const;

    /**
     * Returns whether daylight savings time is in operation at the given UTC date/time.
     *
     * @param utcDateTime the UTC date/time. An error occurs if
     *                    @p utcDateTime.timeSpec() is not Qt::UTC.
     * @return @c true if daylight savings time is in operation, @c false otherwise
     */
    virtual bool isDstAtUtc(const QDateTime &utcDateTime) const;

    /**
     * Returns whether daylight savings time is in operation at a specified UTC time.
     *
     * Note that time_t has a more limited range than QDateTime, so consider using
     * offsetAtUtc() instead.
     *
     * @param t the UTC time, measured in seconds since 00:00:00 UTC 1st January 1970
     *          (as returned by time(2))
     * @return @c true if daylight savings time is in operation, @c false otherwise
     */
    virtual bool isDst(time_t t) const;

  private:
    ICalTimeZonePrivate *d;
};


/**
 * A class which reads and parses iCalendar VTIMEZONE components.
 *
 * ICalTimeZoneSource is used to parse VTIMEZONE components and create
 * ICalTimeZone instances to represent them.
 *
 * @short Reader and parser for iCalendar time zone data
 * @see ICalTimeZone, ICalTimeZoneData
 * @author David Jarvie <software@astrojar.org.uk>.
 */
class LIBKCAL_EXPORT ICalTimeZoneSource : public KTimeZoneSource
{
  public:
    /**
     * Constructs an iCalendar time zone source.
     */
    ICalTimeZoneSource();
    virtual ~ICalTimeZoneSource();

    /**
     * Creates an ICalTimeZone instance containing the detailed information parsed
     * from a VTIMEZONE component.
     *
     * @param vtimezone the VTIMEZONE component from which data is to be extracted
     * @return a ICalTimeZone instance containing the parsed data.
     *         The caller is responsible for deleting the ICalTimeZone instance.
     *         Null is returned on error.
     */
    ICalTimeZone *parse(icalcomponent *vtimezone);

    /**
     * Creates an ICalTimeZone instance for each VTIMEZONE component within a
     * CALENDAR component. The ICalTimeZone instances are added to a KTimeZones
     * collection.
     *
     * If an error occurs while processing any time zone, any remaining time zones
     * are left unprocessed.
     *
     * @param calendar the CALENDAR component from which data is to be extracted
     * @param zones    the time zones collection to which the ICalTimeZone
     *                 instances are to be added
     * @return @c false if any error occurred (either parsing a VTIMEZONE component
     *         or adding an ICalTimeZone to @p zones), @c true otherwise
     */
    bool parse(icalcomponent *calendar, KTimeZones &zones);

    /**
     * Reads an iCalendar file and creates an ICalTimeZone instance for each
     * VTIMEZONE component within it. The ICalTimeZone instances are added to a
     * KTimeZones collection.
     *
     * If an error occurs while processing any time zone, any remaining time zones
     * are left unprocessed.
     *
     * @param fileName the file from which data is to be extracted
     * @param zones    the time zones collection to which the ICalTimeZone
     *                 instances are to be added
     * @return @c false if any error occurred, @c true otherwise
     */
    bool parse(const QString &fileName, KTimeZones &zones);

  private:
    ICalTimeZoneSourcePrivate *d;
};


/**
 * Parsed iCalendar VTIMEZONE data.
 *
 * This class is used by the ICalTimeZoneSource class to pass parsed
 * data to an ICalTimeZone intance.
 *
 * @short Parsed iCalendar time zone data
 * @see ICalTimeZone, ICalTimeZoneSource
 * @author David Jarvie <software@astrojar.org.uk>.
 */
class LIBKCAL_EXPORT ICalTimeZoneData : public KTimeZoneData
{
    friend class ICalTimeZoneSource;

  public:

    struct Phase
    {
      Phase()  : recur(0) {}
      Phase(const Phase &p);
      ~Phase();
      bool operator<(const Phase& p) const  { return start < p.start; }    // needed by qSort()

      /**
       * Find the first start time of the phase, strictly after a given UTC or local time.
       *
       * @param dt date/time
       * @return UTC phase start date/time, or invalid if none
       */
      QDateTime nextStart(const QDateTime &dt) const;

      /**
       * Find the nearest start time of the phase, at or before a given UTC or local time.
       *
       * @param dt date/time
       * @return UTC phase start date/time, or invalid if none
       */
      QDateTime previousStart(const QDateTime &dt) const;

      QList<int>         tznameIndex; /**< tznames indexes: may include translations */
      QDateTime          start;       /**< UTC time of start of this phase */
      int                offset;      /**< offset (in seconds) to add to UTC during this phase */
      int                prevOffset;  /**< offset (in seconds) to add to UTC immediately before this phase */
      QString            comment;     /**< optional comment */
      Recurrence*        recur;       /**< recurrence rules for the start of this phase */
      bool               isDst;       /**< true if daylight savings time, false if standard time */
    };

    ICalTimeZoneData();
    virtual ~ICalTimeZoneData();

    /**
     * Creates a new copy of this object.
     * The caller is responsible for deleting the copy.
     *
     * @return copy of this instance
     */
    virtual KTimeZoneData *clone();

    /**
     * Returns the complete list of time zone abbreviations.
     *
     * @return the list of abbreviations.
     */
    virtual QList<QByteArray> abbreviations() const;

    /**
     * Returns the time zone abbreviation current at a specified time.
     *
     * @param utcDateTime UTC date/time. An error occurs if
     *                    @p utcDateTime.timeSpec() is not Qt::UTC.
     * @return time zone abbreviation, or empty string if error
     */
    virtual QByteArray abbreviation(const QDateTime &utcDateTime) const;

    /**
     * Returns the complete list of UTC offsets for the time zone.
     *
     * @return the sorted list of UTC offsets
     */
    virtual QList<int> utcOffsets() const;

    /**
     * Find the timezone phase which is current at a given UTC or local time.
     *
     * @param dt date/time
     * @param start if non-null, receives the UTC start time of the phase
     * @return phase, or null if error
     */
    const Phase *phase(const QDateTime &dt, QDateTime *start = 0) const;

    QString           location;       /**< name of city for this time zone */
    QByteArray        url;            /**< URL of published VTIMEZONE definition (optional) */
    QDateTime         lastModified;   /**< time of last modification of the VTIMEZONE component (optional) */
    QList<QByteArray> tznames;        /**< time zone name abbreviations (e.g. EDT, BST) */
    QList<Phase*>     phases;         /**< time zone observances, ordered by start date/time */

private:
    ICalTimeZoneDataPrivate *d;
};

}

#endif
