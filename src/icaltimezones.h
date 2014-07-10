/*
  This file is part of the kcalcore library.

  Copyright (c) 2005-2007 David Jarvie <djarvie@kde.org>

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
#ifndef KCALCORE_ICALTIMEZONES_H
#define KCALCORE_ICALTIMEZONES_H

#include "kcalcore_export.h"

#include <KTimeZone>

#include <QtCore/QMap>

#ifndef ICALCOMPONENT_H
typedef struct icalcomponent_impl icalcomponent;
#endif
#ifndef ICALTIMEZONE_DEFINED
#define ICALTIMEZONE_DEFINED
typedef struct _icaltimezone  icaltimezone;
#endif

namespace KCalCore {

class ICalTimeZone;
class ICalTimeZoneSource;
class ICalTimeZoneData;
class ICalTimeZonesPrivate;
class ICalTimeZonePrivate;
class ICalTimeZoneSourcePrivate;
class ICalTimeZoneDataPrivate;

/**
 * The ICalTimeZones class represents a time zone database which consists of a
 * collection of individual iCalendar time zone definitions.
 *
 * Each individual time zone is defined in a ICalTimeZone instance. The time zones in the
 * collection are indexed by name, which must be unique within the collection.
 *
 * Different calendars could define the same time zone differently. As a result,
 * to avoid conflicting definitions, each calendar should normally have its own
 * ICalTimeZones collection.
 *
 * This class is analogous to KTimeZones, but holds ICalTimeZone instances
 * rather than generic KTimeZone instances.
 *
 * @short Represents a collection of iCalendar time zones
 * @author David Jarvie <djarvie@kde.org>
 */
class KCALCORE_EXPORT ICalTimeZones
{
public:
    /**
     * Constructs an empty time zone collection.
     */
    ICalTimeZones();

    /**
     * Copy constructor.
     * @param rhs is the ICalTimeZones instance to copy.
     */
    ICalTimeZones(const ICalTimeZones &rhs);

    /**
     * Assignment operator.
     * @param rhs is the ICalTimeZones instance to assign to this one.
     */
    ICalTimeZones &operator=(const ICalTimeZones &rhs);

    /**
     * Destructor.
     */
    ~ICalTimeZones();

    /**
     * Returns the time zone with the given name.
     * Note that the ICalTimeZone returned remains a member of the ICalTimeZones
     * collection, and should not be deleted without calling remove() first.
     *
     * @param name name of time zone
     * @return time zone, or invalid if not found
     */
    ICalTimeZone zone(const QString &name) const;

    /**
     * Returns the time zone with similar zone information regardless of
     * the time zone names. This feature was added for Microsoft ActiveSync
     * which may have the same timezone specification separately for every
     * incidence.
     * Note that the ICalTimeZone returned remains a member of the ICalTimeZones
     * collection, and should not be deleted without calling remove() first.
     *
     * @see ICalTimeZone parse( MSTimeZone *tz, ICalTimeZones &zones );
     * @param zone zone information to look for
     * @return time zone, or invalid if not found
     */
    ICalTimeZone zone(const ICalTimeZone &zone) const;

    typedef QMap<QString, ICalTimeZone> ZoneMap;

    /**
     * Returns all the time zones defined in this collection.
     *
     * @return time zone collection
     */
    const ZoneMap zones() const;

    /**
     * Adds a time zone to the collection.
     * The time zone's name must be unique within the collection.
     *
     * @param zone time zone to add
     * @return @c true if successful, @c false if zone's name duplicates one
     * already in the collection
     */
    bool add(const ICalTimeZone &zone);

    /**
     * Removes a time zone from the collection.
     *
     * @param zone time zone to remove
     * @return the time zone which was removed, or invalid if not found
     */
    ICalTimeZone remove(const ICalTimeZone &zone);

    /**
     * Removes a time zone from the collection.
     *
     * @param name name of time zone to remove
     * @return the time zone which was removed, or invalid if not found
     */
    ICalTimeZone remove(const QString &name);

    /**
     * Clears the collection.
     */
    void clear();

    /**
     * Returns the number of zones kept in memory.
     *
     * @return The number of zones
     */
    int count();

private:
    //@cond PRIVATE
    ICalTimeZonesPrivate *const d;
    //@endcond
};

/**
 * The ICalTimeZone class represents an iCalendar VTIMEZONE component.
 *
 * ICalTimeZone instances are normally created by ICalTimeZoneSource::parse().
 *
 * @short An iCalendar time zone
 * @see ICalTimeZoneSource, ICalTimeZoneData
 * @author David Jarvie <djarvie@kde.org>
 */
class KCALCORE_EXPORT ICalTimeZone : public KTimeZone //krazy:exclude=dpointer
    //(no d-pointer for KTimeZone derived classes)
{
public:
    /**
     * Constructs a null time zone. A null time zone is invalid.
     *
     * @see isValid()
     */
    ICalTimeZone();

    /**
     * Creates a time zone. This constructor is normally called from
     * ICalTimeZoneSource::parse().
     *
     * @param source   iCalendar VTIMEZONE reader and parser
     * @param name     time zone's unique name within the iCalendar object
     * @param data     parsed VTIMEZONE data
     */
    ICalTimeZone(ICalTimeZoneSource *source, const QString &name, ICalTimeZoneData *data);

    /**
     * Constructor which converts a KTimeZone to an ICalTimeZone instance.
     *
     * @param tz KTimeZone instance
     * @param earliest earliest date for which time zone data should be stored
     */
    explicit ICalTimeZone(const KTimeZone &tz, const QDate &earliest = QDate());

    /**
     * Destructor.
     */
    virtual ~ICalTimeZone();

    /**
     * Returns the name of the city for this time zone, if any. There is no
     * fixed format for the name.
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
     * Returns the VTIMEZONE string which represents this time zone.
     *
     * @return VTIMEZONE string
     */
    QByteArray vtimezone() const;

    /**
     * Returns the ICal timezone structure which represents this time zone.
     * The caller is responsible for freeing the returned structure using
     * icaltimezone_free().
     *
     * @return icaltimezone structure
     */
    icaltimezone *icalTimezone() const;

    /**
     * Update the definition of the time zone to be identical to another
     * ICalTimeZone instance. A prerequisite is that the two instances must
     * have the same name.
     *
     * The purpose of this method is to enable updates of ICalTimeZone
     * definitions when a calendar is reloaded, without invalidating pointers
     * to the instance (particularly pointers held by KDateTime objects).
     *
     * @param other time zone whose definition is to be used
     * @return true if definition was updated (i.e. names are the same)
     */
    bool update(const ICalTimeZone &other);

    /**
     * Returns a standard UTC time zone, with name "UTC".
     *
     * @note The ICalTimeZone returned by this method does not belong to any
     * ICalTimeZones collection. Any ICalTimeZones instance may contain its own
     * UTC ICalTimeZone defined by its time zone source data, but that will be
     * a different instance than this ICalTimeZone.
     *
     * @return UTC time zone
     */
    static ICalTimeZone utc();

protected:
    /**
      @copydoc
      IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook(int id, void *data);

private:
    // d-pointer is in ICalTimeZoneBackend.
    // This is a requirement for classes inherited from KTimeZone.
};

/**
 * Backend class for KICalTimeZone class.
 *
 * This class implements KICalTimeZone's constructors and virtual methods. A
 * backend class is required for all classes inherited from KTimeZone to
 * allow KTimeZone virtual methods to work together with reference counting of
 * private data.
 *
 * @short Backend class for KICalTimeZone class
 * @see KTimeZoneBackend, KICalTimeZone, KTimeZone
 * @ingroup timezones
 * @author David Jarvie <djarvie@kde.org>
 */
class KCALCORE_EXPORT ICalTimeZoneBackend : public KTimeZoneBackend
{
public:
    /** Implements ICalTimeZone::ICalTimeZone(). */
    ICalTimeZoneBackend();
    /**
     * Implements ICalTimeZone::ICalTimeZone().
     *
     * @param source      iCalendar VTIMEZONE reader and parser
     * @param name        time zone's unique name within the iCalendar object
     * @param countryCode ISO 3166 2-character country code, empty if unknown
     * @param latitude    in degrees (between -90 and +90), UNKNOWN if not known
     * @param longitude   in degrees (between -180 and +180), UNKNOWN if not known
     * @param comment     description of the time zone, if any
     */
    ICalTimeZoneBackend(ICalTimeZoneSource *source, const QString &name,
                        const QString &countryCode = QString(),
                        float latitude = KTimeZone::UNKNOWN,
                        float longitude = KTimeZone::UNKNOWN,
                        const QString &comment = QString());

    /** Implements ICalTimeZone::ICalTimeZone().
     *
     * @param tz KTimeZone instance
     * @param earliest earliest date for which time zone data should be stored
     */
    ICalTimeZoneBackend(const KTimeZone &tz, const QDate &earliest);

    virtual ~ICalTimeZoneBackend();

    /**
     * Creates a copy of this instance.
     *
     * @return new copy
     */
    virtual KTimeZoneBackend *clone() const;

    /**
     * Returns the class name of the data represented by this instance.
     *
     * @return "ICalTimeZone"
     */
    virtual QByteArray type() const;

    /**
     * Implements ICalTimeZone::hasTransitions().
     *
     * Return whether daylight saving transitions are available for the time zone.
     *
     * @param caller calling ICalTimeZone object
     * @return @c true
     */
    virtual bool hasTransitions(const KTimeZone *caller) const;

protected:
    /**
      @copydoc
      IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook(int id, void *data);

private:
    //@cond PRIVATE
    ICalTimeZonePrivate *d; //krazy:exclude=dpointer
    //(non-const d-pointer for KTimeZoneBackend-derived classes)
    //@endcond
};

/**
 * Placeholhers for Microsoft and ActiveSync timezone data.
 * @see http://msdn.microsoft.com/en-us/library/ms725481(VS.85).aspx
 * (TIME_ZONE_INFORMATION Structure)
 */

typedef struct _MSSystemTime {
    qint16 wYear;
    qint16 wMonth;
    qint16 wDayOfWeek;
    qint16 wDay;
    qint16 wHour;
    qint16 wMinute;
    qint16 wSecond;
    qint16 wMilliseconds;
} MSSystemTime;

typedef struct _MSTimeZone {
    long         Bias;
    QString      StandardName;
    MSSystemTime StandardDate;
    long         StandardBias;
    QString      DaylightName;
    MSSystemTime DaylightDate;
    long         DaylightBias;
} MSTimeZone;

/**
 * A class which reads and parses iCalendar VTIMEZONE components, and accesses
 * libical time zone data.
 *
 * ICalTimeZoneSource is used to parse VTIMEZONE components and create
 * ICalTimeZone instances to represent them.
 *
 * @short Reader and parser for iCalendar time zone data
 * @see ICalTimeZone, ICalTimeZoneData
 * @author David Jarvie <djarvie@kde.org>
 */
class KCALCORE_EXPORT ICalTimeZoneSource : public KTimeZoneSource
{
public:
    /**
     * Constructs an iCalendar time zone source.
     */
    ICalTimeZoneSource();

    /**
     * Destructor.
     */
    virtual ~ICalTimeZoneSource();

    /**
     * Creates an ICalTimeZone instance containing the detailed information
     * parsed from an iCalendar VTIMEZONE component.
     *
     * @param vtimezone the VTIMEZONE component from which data is to be extracted
     * @return an ICalTimeZone instance containing the parsed data, or invalid on error
     */
    ICalTimeZone parse(icalcomponent *vtimezone);

    /**
     * Creates an ICalTimeZone instance for each iCalendar VTIMEZONE component
     * within a CALENDAR component. The ICalTimeZone instances are added to a
     * ICalTimeZones collection.
     *
     * If an error occurs while processing any time zone, any remaining time
     * zones are left unprocessed.
     *
     * @param calendar the CALENDAR component from which data is to be extracted
     * @param zones    the time zones collection to which the ICalTimeZone
     *                 instances are to be added
     * @return @c false if any error occurred (either parsing a VTIMEZONE
     *         component or adding an ICalTimeZone to @p zones), @c true otherwise
     */
    bool parse(icalcomponent *calendar, ICalTimeZones &zones);

    /**
     * Creates an ICalTimeZone instance containing the detailed information
     * contained in an MSTimeZone structure.
     *
     * @param tz the MSTimeZone structure from which data is to be extracted
     * @return an ICalTimeZone instance containing the time zone data, or invalid on error
     */
    ICalTimeZone parse(MSTimeZone *tz);

    /**
     * Creates an ICalTimeZone instance and adds it to a ICalTimeZones
     * collection or returns an existing instance for the MSTimeZone component.
     *
     * @param tz       the MSTimeZone structure to parse
     * @param zones    the time zones collection to which the ICalTimeZone
     *                 instances are to be added
     * @return an ICalTimeZone instance containing the time zone data, or invalid on error
     */
    ICalTimeZone parse(MSTimeZone *tz, ICalTimeZones &zones);

    /**
     * Creates an ICalTimeZone instance containing the detailed information
     * contained in a QStringList produced by vcalformat, and adds it to a
     * ICalTimeZones collection or returns an existing instance for the vcal
     * component.
     *
     * @param name     the name of timezone
     * @param tzList   the QStringList to parse
     * @param zones    the time zones collection to which the ICalTimeZone
     *                 instances are to be added
     * @return an ICalTimeZone instance containing the time zone data, or invalid on error
     */
    ICalTimeZone parse(const QString &name, const QStringList &tzList, ICalTimeZones &zones);

    /**
     * Creates an ICalTimeZone instance containing the detailed information
     * contained in a QStringList produced by vcalformat.
     *
     * @param name   the name of timezone
     * @param tzList the QStringList from which data is to be extracted
     * @return an ICalTimeZone instance containing the time zone data, or invalid on error
     */
    ICalTimeZone parse(const QString &name, const QStringList &tzList);

    /**
     * Reads an iCalendar file and creates an ICalTimeZone instance for each
     * VTIMEZONE component within it. The ICalTimeZone instances are added to a
     * ICalTimeZones collection.
     *
     * If an error occurs while processing any time zone, any remaining time
     * zones are left unprocessed.
     *
     * @param fileName the file from which data is to be extracted
     * @param zones    the time zones collection to which the ICalTimeZone
     *                 instances are to be added
     * @return @c false if any error occurred, @c true otherwise
     */
    bool parse(const QString &fileName, ICalTimeZones &zones);

    /**
     * Creates an ICalTimeZone instance containing the detailed information
     * contained in an icaltimezone structure.
     *
     * Note that an icaltimezone instance may internally refer to a built-in
     * (i.e. system) time zone, in which case the data obtained from @p tz will
     * actually be derived from the built-in time zone rather than from a
     * VTIMEZONE component.
     *
     * @param tz the icaltimezone structure from which data is to be extracted
     * @return an ICalTimeZone instance containing the time zone data, or invalid on error
     */
    ICalTimeZone parse(icaltimezone *tz);

    /**
     * Creates an ICalTimeZone instance for a standard time zone. The system
     * time zone definition is used in preference; otherwise, the built-in
     * libical time zone definition is used.
     *
     * @param zone time zone name, which may optionally include the libical
     *             prefix string
     * @param icalBuiltIn @p true to fetch only the libical built-in time zone,
     *                    and ignore system time zone definitions
     * @return an ICalTimeZone instance containing the time zone data, or invalid on error
     */
    ICalTimeZone standardZone(const QString &zone, bool icalBuiltIn = false);

    /**
     * Returns the prefix string used in the TZID field in built-in libical
     * time zones. The prefix string starts and ends with '/'. The name
     * normally used for the time zone is obtained by stripping the prefix and
     * the following characters up to the next '/', inclusive.
     *
     * @return prefix string
     */
    static QByteArray icalTzidPrefix();

    using KTimeZoneSource::parse; // prevent warning about hidden virtual method

protected:
    /**
      @copydoc
      IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook(int id, void *data);

private:
    //@cond PRIVATE
    ICalTimeZoneSourcePrivate *const d;
    //@endcond
};

/**
 * Parsed iCalendar VTIMEZONE data.
 *
 * This class is used by the ICalTimeZoneSource class to pass parsed
 * data to an ICalTimeZone intance.
 *
 * @short Parsed iCalendar time zone data
 * @see ICalTimeZone, ICalTimeZoneSource
 * @author David Jarvie <djarvie@kde.org>
 */
class KCALCORE_EXPORT ICalTimeZoneData : public KTimeZoneData
{
    friend class ICalTimeZoneSource;

public:
    /**
     * Default constructor.
     */
    ICalTimeZoneData();

    /**
     * Copy constructor.
     *
     * @param rhs instance to copy from
     */
    ICalTimeZoneData(const ICalTimeZoneData &rhs);

    /**
     * Constructor which converts a KTimeZoneData to an ICalTimeZoneData instance.
     * If @p data is for a system time zone (i.e. @p tz is a KSystemTimeZone
     * instance), the full time zone data is read from the system time zone
     * database if possible; otherwise, the built-in libical time zone's data
     * is used.
     *
     * @param rhs KTimeZoneData instance
     * @param tz  time zone which @p rhs belongs to
     * @param earliest earliest date for which time zone data should be stored
     */
    ICalTimeZoneData(const KTimeZoneData &rhs, const KTimeZone &tz, const QDate &earliest);

    /**
     * Destructor.
     */
    virtual ~ICalTimeZoneData();

    /**
     * Assignment operator.
     *
     * @param rhs instance to copy from
     * @return this instance
     */
    ICalTimeZoneData &operator=(const ICalTimeZoneData &rhs);

    /**
     * Creates a new copy of this object.
     * The caller is responsible for deleting the copy.
     *
     * @return copy of this instance
     */
    virtual KTimeZoneData *clone() const;

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
     * Returns the VTIMEZONE string which represents this time zone.
     *
     * @return VTIMEZONE string
     */
    QByteArray vtimezone() const;

    /**
     * Returns the ICal timezone structure which represents this time zone.
     * The caller is responsible for freeing the returned structure using
     * icaltimezone_free().
     *
     * @return icaltimezone structure
     */
    icaltimezone *icalTimezone() const;

    /**
     * Return whether daylight saving transitions are available for the time zone.
     *
     * @return @c true
     */
    virtual bool hasTransitions() const;

protected:
    /**
      @copydoc
      IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook(int id, void *data);

private:
    //@cond PRIVATE
    ICalTimeZoneDataPrivate *const d;
    //@endcond
};

}

#endif
