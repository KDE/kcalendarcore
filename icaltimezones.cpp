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
#include <config-kcalcore.h>

#include "icaltimezones.h"
#include "icalformat.h"
#include "icalformat_p.h"
#include "recurrence.h"
#include "recurrencerule.h"

#include <KDebug>
#include <KDateTime>
#include <KSystemTimeZone>

#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

extern "C" {
  #include <ical.h>
  #include <icaltimezone.h>
}

#if defined(HAVE_UUID_UUID_H)
#include <uuid/uuid.h>
#endif

using namespace KCalCore;

// Minimum repetition counts for VTIMEZONE RRULEs
static const int minRuleCount = 5;   // for any RRULE
static const int minPhaseCount = 8;  // for separate STANDARD/DAYLIGHT component

// Convert an ical time to QDateTime, preserving the UTC indicator
static QDateTime toQDateTime( const icaltimetype &t )
{
  return QDateTime( QDate( t.year, t.month, t.day ),
                    QTime( t.hour, t.minute, t.second ),
                    ( t.is_utc ? Qt::UTC : Qt::LocalTime ) );
}

// Maximum date for time zone data.
// It's not sensible to try to predict them very far in advance, because
// they can easily change. Plus, it limits the processing required.
static QDateTime MAX_DATE()
{
  static QDateTime dt;
  if ( !dt.isValid() ) {
    dt = QDateTime( QDate::currentDate().addYears( 20 ), QTime( 0, 0, 0 ) );
  }
  return dt;
}

static icaltimetype writeLocalICalDateTime( const QDateTime &utc, int offset )
{
  QDateTime local = utc.addSecs( offset );
  icaltimetype t = icaltime_null_time();
  t.year = local.date().year();
  t.month = local.date().month();
  t.day = local.date().day();
  t.hour = local.time().hour();
  t.minute = local.time().minute();
  t.second = local.time().second();
  t.is_date = 0;
  t.zone = 0;
  t.is_utc = 0;
  return t;
}

namespace KCalCore {

/******************************************************************************/

//@cond PRIVATE
class ICalTimeZonesPrivate
{
  public:
    ICalTimeZonesPrivate() {}
    ICalTimeZones::ZoneMap zones;
};
//@endcond

ICalTimeZones::ICalTimeZones()
  : d( new ICalTimeZonesPrivate )
{
}

ICalTimeZones::ICalTimeZones( const ICalTimeZones &rhs )
  : d( new ICalTimeZonesPrivate() )
{
  d->zones = rhs.d->zones;
}

ICalTimeZones &ICalTimeZones::operator=( const ICalTimeZones &rhs )
{
  // check for self assignment
  if ( &rhs == this ) {
    return *this;
  }
  d->zones = rhs.d->zones;
  return *this;
}

ICalTimeZones::~ICalTimeZones()
{
  delete d;
}

const ICalTimeZones::ZoneMap ICalTimeZones::zones() const
{
  return d->zones;
}

bool ICalTimeZones::add( const ICalTimeZone &zone )
{
  if ( !zone.isValid() ) {
    return false;
  }
  if ( d->zones.find( zone.name() ) != d->zones.end() ) {
    return false;    // name already exists
  }

  d->zones.insert( zone.name(), zone );
  return true;
}

ICalTimeZone ICalTimeZones::remove( const ICalTimeZone &zone )
{
  if ( zone.isValid() ) {
    for ( ZoneMap::Iterator it = d->zones.begin(), end = d->zones.end();  it != end;  ++it ) {
      if ( it.value() == zone ) {
        d->zones.erase( it );
        return ( zone == ICalTimeZone::utc() ) ? ICalTimeZone() : zone;
      }
    }
  }
  return ICalTimeZone();
}

ICalTimeZone ICalTimeZones::remove( const QString &name )
{
  if ( !name.isEmpty() ) {
    ZoneMap::Iterator it = d->zones.find( name );
    if ( it != d->zones.end() ) {
      ICalTimeZone zone = it.value();
      d->zones.erase(it);
      return ( zone == ICalTimeZone::utc() ) ? ICalTimeZone() : zone;
    }
  }
  return ICalTimeZone();
}

void ICalTimeZones::clear()
{
  d->zones.clear();
}

int ICalTimeZones::count()
{
  return d->zones.count();
}

ICalTimeZone ICalTimeZones::zone( const QString &name ) const
{
  if ( !name.isEmpty() ) {
    ZoneMap::ConstIterator it = d->zones.constFind( name );
    if ( it != d->zones.constEnd() ) {
      return it.value();
    }
  }
  return ICalTimeZone();   // error
}

ICalTimeZone ICalTimeZones::zone( const ICalTimeZone &zone ) const
{
  if ( zone.isValid() ) {
    QMapIterator<QString, ICalTimeZone> it(d->zones);
    while ( it.hasNext() ) {
      it.next();
      ICalTimeZone tz = it.value();
      QList<KTimeZone::Transition> list1 = tz.transitions();
      QList<KTimeZone::Transition> list2 = zone.transitions();
      if ( list1.size() == list2.size() ) {
        int i = 0;
        int matches = 0;
        for ( ; i < list1.size(); ++i ) {
          KTimeZone::Transition t1 = list1.at( i );
          KTimeZone::Transition t2 = list2.at( i );
          if ( ( t1.time() == t2.time() ) &&
               ( t1.phase().utcOffset() == t2.phase().utcOffset() ) &&
               ( t1.phase().isDst() == t2.phase().isDst() ) ) {
            matches++;
          }
        }
        if ( matches == i ) {
          // Existing zone has all the transitions of the given zone.
          return tz;
        }
      }
    }
  }
  return ICalTimeZone(); // not found
}

/******************************************************************************/

ICalTimeZoneBackend::ICalTimeZoneBackend()
  : KTimeZoneBackend()
{}

ICalTimeZoneBackend::ICalTimeZoneBackend( ICalTimeZoneSource *source,
                                          const QString &name,
                                          const QString &countryCode,
                                          float latitude, float longitude,
                                          const QString &comment )
  : KTimeZoneBackend( source, name, countryCode, latitude, longitude, comment )
{}

ICalTimeZoneBackend::ICalTimeZoneBackend( const KTimeZone &tz, const QDate &earliest )
  : KTimeZoneBackend( 0, tz.name(), tz.countryCode(), tz.latitude(), tz.longitude(), tz.comment() )
{
  Q_UNUSED( earliest );
}

ICalTimeZoneBackend::~ICalTimeZoneBackend()
{}

KTimeZoneBackend *ICalTimeZoneBackend::clone() const
{
  return new ICalTimeZoneBackend( *this );
}

QByteArray ICalTimeZoneBackend::type() const
{
  return "ICalTimeZone";
}

bool ICalTimeZoneBackend::hasTransitions( const KTimeZone *caller ) const
{
  Q_UNUSED( caller );
  return true;
}

void ICalTimeZoneBackend::virtual_hook( int id, void *data )
{
  Q_UNUSED( id );
  Q_UNUSED( data );
}

/******************************************************************************/

ICalTimeZone::ICalTimeZone()
  : KTimeZone( new ICalTimeZoneBackend() )
{}

ICalTimeZone::ICalTimeZone( ICalTimeZoneSource *source, const QString &name,
                            ICalTimeZoneData *data )
  : KTimeZone( new ICalTimeZoneBackend( source, name ) )
{
  setData( data );
}

ICalTimeZone::ICalTimeZone( const KTimeZone &tz, const QDate &earliest )
  : KTimeZone( new ICalTimeZoneBackend( 0, tz.name(), tz.countryCode(),
                                        tz.latitude(), tz.longitude(),
                                        tz.comment() ) )
{
  const KTimeZoneData *data = tz.data( true );
  if ( data ) {
    const ICalTimeZoneData *icaldata = dynamic_cast<const ICalTimeZoneData*>( data );
    if ( icaldata ) {
      setData( new ICalTimeZoneData( *icaldata ) );
    } else {
      setData( new ICalTimeZoneData( *data, tz, earliest ) );
    }
  }
}

ICalTimeZone::~ICalTimeZone()
{}

QString ICalTimeZone::city() const
{
  const ICalTimeZoneData *dat = static_cast<const ICalTimeZoneData*>( data() );
  return dat ? dat->city() : QString();
}

QByteArray ICalTimeZone::url() const
{
  const ICalTimeZoneData *dat = static_cast<const ICalTimeZoneData*>( data() );
  return dat ? dat->url() : QByteArray();
}

QDateTime ICalTimeZone::lastModified() const
{
  const ICalTimeZoneData *dat = static_cast<const ICalTimeZoneData*>( data() );
  return dat ? dat->lastModified() : QDateTime();
}

QByteArray ICalTimeZone::vtimezone() const
{
  const ICalTimeZoneData *dat = static_cast<const ICalTimeZoneData*>( data() );
  return dat ? dat->vtimezone() : QByteArray();
}

icaltimezone *ICalTimeZone::icalTimezone() const
{
  const ICalTimeZoneData *dat = static_cast<const ICalTimeZoneData*>( data() );
  return dat ? dat->icalTimezone() : 0;
}

bool ICalTimeZone::update( const ICalTimeZone &other )
{
  if ( !updateBase( other ) ) {
    return false;
  }

  KTimeZoneData *otherData = other.data() ? other.data()->clone() : 0;
  setData( otherData, other.source() );
  return true;
}

ICalTimeZone ICalTimeZone::utc()
{
  static ICalTimeZone utcZone;
  if ( !utcZone.isValid() ) {
    ICalTimeZoneSource tzs;
    utcZone = tzs.parse( icaltimezone_get_utc_timezone() );
  }
  return utcZone;
}

void ICalTimeZone::virtual_hook( int id, void *data )
{
  Q_UNUSED( id );
  Q_UNUSED( data );
}
/******************************************************************************/

//@cond PRIVATE
class ICalTimeZoneDataPrivate
{
  public:
    ICalTimeZoneDataPrivate() : icalComponent(0) {}
    ~ICalTimeZoneDataPrivate()
    {
      if ( icalComponent ) {
        icalcomponent_free( icalComponent );
      }
    }
    icalcomponent *component() const { return icalComponent; }
    void setComponent( icalcomponent *c )
    {
      if ( icalComponent ) {
        icalcomponent_free( icalComponent );
      }
      icalComponent = c;
    }
    QString       location;       // name of city for this time zone
    QByteArray    url;            // URL of published VTIMEZONE definition (optional)
    QDateTime     lastModified;   // time of last modification of the VTIMEZONE component (optional)
  private:
    icalcomponent *icalComponent; // ical component representing this time zone
};
//@endcond

ICalTimeZoneData::ICalTimeZoneData()
  : d ( new ICalTimeZoneDataPrivate() )
{
}

ICalTimeZoneData::ICalTimeZoneData( const ICalTimeZoneData &rhs )
  : KTimeZoneData( rhs ),
    d( new ICalTimeZoneDataPrivate() )
{
  d->location = rhs.d->location;
  d->url = rhs.d->url;
  d->lastModified = rhs.d->lastModified;
  d->setComponent( icalcomponent_new_clone( rhs.d->component() ) );
}

ICalTimeZoneData::ICalTimeZoneData( const KTimeZoneData &rhs,
                                    const KTimeZone &tz, const QDate &earliest )
  : KTimeZoneData( rhs ),
    d( new ICalTimeZoneDataPrivate() )
{
  // VTIMEZONE RRULE types
  enum {
    DAY_OF_MONTH          = 0x01,
    WEEKDAY_OF_MONTH      = 0x02,
    LAST_WEEKDAY_OF_MONTH = 0x04
  };

  if ( tz.type() == "KSystemTimeZone" ) {
    // Try to fetch a system time zone in preference, on the grounds
    // that system time zones are more likely to be up to date than
    // built-in libical ones.
    icalcomponent *c = 0;
    KTimeZone ktz = KSystemTimeZones::readZone( tz.name() );
    if ( ktz.isValid() ) {
      if ( ktz.data(true) ) {
        ICalTimeZone icaltz( ktz, earliest );
        icaltimezone *itz = icaltz.icalTimezone();
        c = icalcomponent_new_clone( icaltimezone_get_component( itz ) );
        icaltimezone_free( itz, 1 );
      }
    }
    if ( !c ) {
      // Try to fetch a built-in libical time zone.
      icaltimezone *itz = icaltimezone_get_builtin_timezone( tz.name().toUtf8() );
      c = icalcomponent_new_clone( icaltimezone_get_component( itz ) );
    }
    if ( c ) {
      // TZID in built-in libical time zones has a standard prefix.
      // To make the VTIMEZONE TZID match TZID references in incidences
      // (as required by RFC2445), strip off the prefix.
      icalproperty *prop = icalcomponent_get_first_property( c, ICAL_TZID_PROPERTY );
      if ( prop ) {
        icalvalue *value = icalproperty_get_value( prop );
        const char *tzid = icalvalue_get_text( value );
        QByteArray icalprefix = ICalTimeZoneSource::icalTzidPrefix();
        int len = icalprefix.size();
        if ( !strncmp( icalprefix, tzid, len ) ) {
          const char *s = strchr( tzid + len, '/' );    // find third '/'
          if ( s ) {
            QByteArray tzidShort( s + 1 ); // deep copy of string (needed by icalvalue_set_text())
            icalvalue_set_text( value, tzidShort );

            // Remove the X-LIC-LOCATION property, which is only used by libical
            prop = icalcomponent_get_first_property( c, ICAL_X_PROPERTY );
            const char *xname = icalproperty_get_x_name( prop );
            if ( xname && !strcmp( xname, "X-LIC-LOCATION" ) ) {
              icalcomponent_remove_property( c, prop );
            }
          }
        }
      }
    }
    d->setComponent( c );
  } else {
    // Write the time zone data into an iCal component
    icalcomponent *tzcomp = icalcomponent_new(ICAL_VTIMEZONE_COMPONENT);
    icalcomponent_add_property( tzcomp, icalproperty_new_tzid( tz.name().toUtf8() ) );
//    icalcomponent_add_property(tzcomp, icalproperty_new_location( tz.name().toUtf8() ));

    // Compile an ordered list of transitions so that we can know the phases
    // which occur before and after each transition.
    QList<KTimeZone::Transition> transits = transitions();
    if ( earliest.isValid() ) {
      // Remove all transitions earlier than those we are interested in
      for ( int i = 0, end = transits.count();  i < end;  ++i ) {
        if ( transits[i].time().date() >= earliest ) {
          if ( i > 0 ) {
            transits.erase( transits.begin(), transits.begin() + i );
          }
          break;
        }
      }
    }
    int trcount = transits.count();
    QVector<bool> transitionsDone(trcount);
    transitionsDone.fill(false);

    // Go through the list of transitions and create an iCal component for each
    // distinct combination of phase after and UTC offset before the transition.
    icaldatetimeperiodtype dtperiod;
    dtperiod.period = icalperiodtype_null_period();
    for ( ; ; ) {
      int i = 0;
      for ( ;  i < trcount && transitionsDone[i];  ++i ) {
        ;
      }
      if ( i >= trcount ) {
        break;
      }
      // Found a phase combination which hasn't yet been processed
      int preOffset = ( i > 0 ) ? transits[i - 1].phase().utcOffset() : rhs.previousUtcOffset();
      KTimeZone::Phase phase = transits[i].phase();
      if ( phase.utcOffset() == preOffset ) {
        transitionsDone[i] = true;
        while ( ++i < trcount ) {
          if ( transitionsDone[i] ||
               transits[i].phase() != phase ||
               transits[i - 1].phase().utcOffset() != preOffset ) {
            continue;
          }
          transitionsDone[i] = true;
        }
        continue;
      }
      icalcomponent *phaseComp =
        icalcomponent_new( phase.isDst() ? ICAL_XDAYLIGHT_COMPONENT : ICAL_XSTANDARD_COMPONENT );
      QList<QByteArray> abbrevs = phase.abbreviations();
      for ( int a = 0, aend = abbrevs.count();  a < aend;  ++a ) {
        icalcomponent_add_property( phaseComp,
                                    icalproperty_new_tzname(
                                      static_cast<const char*>( abbrevs[a]) ) );
      }
      if ( !phase.comment().isEmpty() ) {
        icalcomponent_add_property( phaseComp,
                                    icalproperty_new_comment( phase.comment().toUtf8() ) );
      }
      icalcomponent_add_property( phaseComp,
                                  icalproperty_new_tzoffsetfrom( preOffset ) );
      icalcomponent_add_property( phaseComp,
                                  icalproperty_new_tzoffsetto( phase.utcOffset() ) );
      // Create a component to hold initial RRULE if any, plus all RDATEs
      icalcomponent *phaseComp1 = icalcomponent_new_clone( phaseComp );
      icalcomponent_add_property( phaseComp1,
                                  icalproperty_new_dtstart(
                                    writeLocalICalDateTime( transits[i].time(), preOffset ) ) );
      bool useNewRRULE = false;

      // Compile the list of UTC transition dates/times, and check
      // if the list can be reduced to an RRULE instead of multiple RDATEs.
      QTime time;
      QDate date;
      int year = 0, month = 0, daysInMonth = 0, dayOfMonth = 0; // avoid compiler warnings
      int dayOfWeek = 0;      // Monday = 1
      int nthFromStart = 0;   // nth (weekday) of month
      int nthFromEnd = 0;     // nth last (weekday) of month
      int newRule;
      int rule = 0;
      QList<QDateTime> rdates;// dates which (probably) need to be written as RDATEs
      QList<QDateTime> times;
      QDateTime qdt = transits[i].time();   // set 'qdt' for start of loop
      times += qdt;
      transitionsDone[i] = true;
      do {
        if ( !rule ) {
          // Initialise data for detecting a new rule
          rule = DAY_OF_MONTH | WEEKDAY_OF_MONTH | LAST_WEEKDAY_OF_MONTH;
          time = qdt.time();
          date = qdt.date();
          year = date.year();
          month = date.month();
          daysInMonth = date.daysInMonth();
          dayOfWeek = date.dayOfWeek();   // Monday = 1
          dayOfMonth = date.day();
          nthFromStart = ( dayOfMonth - 1 ) / 7 + 1;   // nth (weekday) of month
          nthFromEnd = ( daysInMonth - dayOfMonth ) / 7 + 1;   // nth last (weekday) of month
        }
        if ( ++i >= trcount ) {
          newRule = 0;
          times += QDateTime();   // append a dummy value since last value in list is ignored
        } else {
          if ( transitionsDone[i] ||
               transits[i].phase() != phase ||
               transits[i - 1].phase().utcOffset() != preOffset ) {
            continue;
          }
          transitionsDone[i] = true;
          qdt = transits[i].time();
          if ( !qdt.isValid() ) {
            continue;
          }
          newRule = rule;
          times += qdt;
          date = qdt.date();
          if ( qdt.time() != time ||
               date.month() != month ||
               date.year() != ++year ) {
            newRule = 0;
          } else {
            int day = date.day();
            if ( ( newRule & DAY_OF_MONTH ) && day != dayOfMonth ) {
              newRule &= ~DAY_OF_MONTH;
            }
            if ( newRule & ( WEEKDAY_OF_MONTH | LAST_WEEKDAY_OF_MONTH ) ) {
              if ( date.dayOfWeek() != dayOfWeek ) {
                newRule &= ~( WEEKDAY_OF_MONTH | LAST_WEEKDAY_OF_MONTH );
              } else {
                if ( ( newRule & WEEKDAY_OF_MONTH ) &&
                     ( day - 1 ) / 7 + 1 != nthFromStart ) {
                  newRule &= ~WEEKDAY_OF_MONTH;
                }
                if ( ( newRule & LAST_WEEKDAY_OF_MONTH ) &&
                     ( daysInMonth - day ) / 7 + 1 != nthFromEnd ) {
                  newRule &= ~LAST_WEEKDAY_OF_MONTH;
                }
              }
            }
          }
        }
        if ( !newRule ) {
          // The previous rule (if any) no longer applies.
          // Write all the times up to but not including the current one.
          // First check whether any of the last RDATE values fit this rule.
          int yr = times[0].date().year();
          while ( !rdates.isEmpty() ) {
            qdt = rdates.last();
            date = qdt.date();
            if ( qdt.time() != time  ||
                 date.month() != month ||
                 date.year() != --yr ) {
              break;
            }
            int day  = date.day();
            if ( rule & DAY_OF_MONTH ) {
              if ( day != dayOfMonth ) {
                break;
              }
            } else {
              if ( date.dayOfWeek() != dayOfWeek ||
                   ( ( rule & WEEKDAY_OF_MONTH ) &&
                     ( day - 1 ) / 7 + 1 != nthFromStart ) ||
                   ( ( rule & LAST_WEEKDAY_OF_MONTH ) &&
                     ( daysInMonth - day ) / 7 + 1 != nthFromEnd ) ) {
                break;
              }
            }
            times.prepend( qdt );
            rdates.pop_back();
          }
          if ( times.count() > ( useNewRRULE ? minPhaseCount : minRuleCount ) ) {
            // There are enough dates to combine into an RRULE
            icalrecurrencetype r;
            icalrecurrencetype_clear( &r );
            r.freq = ICAL_YEARLY_RECURRENCE;
            r.count = ( year >= 2030 ) ? 0 : times.count() - 1;
            r.by_month[0] = month;
            if ( rule & DAY_OF_MONTH ) {
              r.by_month_day[0] = dayOfMonth;
            } else if ( rule & WEEKDAY_OF_MONTH ) {
              r.by_day[0] = ( dayOfWeek % 7 + 1 ) + ( nthFromStart * 8 );   // Sunday = 1
            } else if ( rule & LAST_WEEKDAY_OF_MONTH ) {
              r.by_day[0] = -( dayOfWeek % 7 + 1 ) - ( nthFromEnd * 8 );   // Sunday = 1
            }
            icalproperty *prop = icalproperty_new_rrule( r );
            if ( useNewRRULE ) {
              // This RRULE doesn't start from the phase start date, so set it into
              // a new STANDARD/DAYLIGHT component in the VTIMEZONE.
              icalcomponent *c = icalcomponent_new_clone( phaseComp );
              icalcomponent_add_property(
                c, icalproperty_new_dtstart( writeLocalICalDateTime( times[0], preOffset ) ) );
              icalcomponent_add_property( c, prop );
              icalcomponent_add_component( tzcomp, c );
            } else {
              icalcomponent_add_property( phaseComp1, prop );
            }
          } else {
            // Save dates for writing as RDATEs
            for ( int t = 0, tend = times.count() - 1;  t < tend;  ++t ) {
              rdates += times[t];
            }
          }
          useNewRRULE = true;
          // All date/time values but the last have been added to the VTIMEZONE.
          // Remove them from the list.
          qdt = times.last();   // set 'qdt' for start of loop
          times.clear();
          times += qdt;
        }
        rule = newRule;
      } while ( i < trcount );

      // Write remaining dates as RDATEs
      for ( int rd = 0, rdend = rdates.count();  rd < rdend;  ++rd ) {
        dtperiod.time = writeLocalICalDateTime( rdates[rd], preOffset );
        icalcomponent_add_property( phaseComp1, icalproperty_new_rdate( dtperiod ) );
      }
      icalcomponent_add_component( tzcomp, phaseComp1 );
      icalcomponent_free( phaseComp );
    }

    d->setComponent( tzcomp );
  }
}

ICalTimeZoneData::~ICalTimeZoneData()
{
  delete d;
}

ICalTimeZoneData &ICalTimeZoneData::operator=( const ICalTimeZoneData &rhs )
{
  // check for self assignment
  if ( &rhs == this ) {
    return *this;
  }

  KTimeZoneData::operator=( rhs );
  d->location = rhs.d->location;
  d->url = rhs.d->url;
  d->lastModified = rhs.d->lastModified;
  d->setComponent( icalcomponent_new_clone( rhs.d->component() ) );
  return *this;
}

KTimeZoneData *ICalTimeZoneData::clone() const
{
  return new ICalTimeZoneData( *this );
}

QString ICalTimeZoneData::city() const
{
  return d->location;
}

QByteArray ICalTimeZoneData::url() const
{
  return d->url;
}

QDateTime ICalTimeZoneData::lastModified() const
{
  return d->lastModified;
}

QByteArray ICalTimeZoneData::vtimezone() const
{
  QByteArray result( icalcomponent_as_ical_string( d->component() ) );
  icalmemory_free_ring();
  return result;
}

icaltimezone *ICalTimeZoneData::icalTimezone() const
{
  icaltimezone *icaltz = icaltimezone_new();
  if ( !icaltz ) {
    return 0;
  }
  icalcomponent *c = icalcomponent_new_clone( d->component() );
  if ( !icaltimezone_set_component( icaltz, c ) ) {
    icalcomponent_free( c );
    icaltimezone_free( icaltz, 1 );
    return 0;
  }
  return icaltz;
}

bool ICalTimeZoneData::hasTransitions() const
{
  return true;
}

void ICalTimeZoneData::virtual_hook( int id, void *data )
{
  Q_UNUSED( id );
  Q_UNUSED( data );
}

/******************************************************************************/

//@cond PRIVATE
class ICalTimeZoneSourcePrivate
{
  public:
    static QList<QDateTime> parsePhase( icalcomponent *, bool daylight,
                                        int &prevOffset, KTimeZone::Phase & );
    static QByteArray icalTzidPrefix;

#if defined(HAVE_UUID_UUID_H)
    static void parseTransitions( const MSSystemTime &date, const KTimeZone::Phase &phase,
                                  int prevOffset, QList<KTimeZone::Transition> &transitions );
#endif
};

QByteArray ICalTimeZoneSourcePrivate::icalTzidPrefix;
//@endcond

ICalTimeZoneSource::ICalTimeZoneSource()
  : KTimeZoneSource( false ),
    d( 0 )
{
}

ICalTimeZoneSource::~ICalTimeZoneSource()
{
}

bool ICalTimeZoneSource::parse( const QString &fileName, ICalTimeZones &zones )
{
  QFile file( fileName );
  if ( !file.open( QIODevice::ReadOnly ) ) {
    return false;
  }
  QTextStream ts( &file );
  ts.setCodec( "ISO 8859-1" );
  QByteArray text = ts.readAll().trimmed().toLatin1();
  file.close();

  bool result = false;
  icalcomponent *calendar = icalcomponent_new_from_string( text.data() );
  if ( calendar ) {
    if ( icalcomponent_isa( calendar ) == ICAL_VCALENDAR_COMPONENT ) {
      result = parse( calendar, zones );
    }
    icalcomponent_free( calendar );
  }
  return result;
}

bool ICalTimeZoneSource::parse( icalcomponent *calendar, ICalTimeZones &zones )
{
  for ( icalcomponent *c = icalcomponent_get_first_component( calendar, ICAL_VTIMEZONE_COMPONENT );
        c;  c = icalcomponent_get_next_component( calendar, ICAL_VTIMEZONE_COMPONENT ) ) {
    ICalTimeZone zone = parse( c );
    if ( !zone.isValid() ) {
      return false;
    }
    ICalTimeZone oldzone = zones.zone( zone.name() );
    if ( oldzone.isValid() ) {
      // The zone already exists in the collection, so update the definition
      // of the zone rather than using a newly created one.
      oldzone.update( zone );
    } else if ( !zones.add( zone ) ) {
      return false;
    }
  }
  return true;
}

ICalTimeZone ICalTimeZoneSource::parse( icalcomponent *vtimezone )
{
  QString name;
  QString xlocation;
  ICalTimeZoneData *data = new ICalTimeZoneData();

  // Read the fixed properties which can only appear once in VTIMEZONE
  icalproperty *p = icalcomponent_get_first_property( vtimezone, ICAL_ANY_PROPERTY );
  while ( p ) {
    icalproperty_kind kind = icalproperty_isa( p );
    switch ( kind ) {

    case ICAL_TZID_PROPERTY:
      name = QString::fromUtf8( icalproperty_get_tzid( p ) );
      break;

    case ICAL_TZURL_PROPERTY:
      data->d->url = icalproperty_get_tzurl( p );
      break;

    case ICAL_LOCATION_PROPERTY:
      // This isn't mentioned in RFC2445, but libical reads it ...
      data->d->location = QString::fromUtf8( icalproperty_get_location( p ) );
      break;

    case ICAL_X_PROPERTY:
    {   // use X-LIC-LOCATION if LOCATION is missing
      const char *xname = icalproperty_get_x_name( p );
      if ( xname && !strcmp( xname, "X-LIC-LOCATION" ) ) {
        xlocation = QString::fromUtf8( icalproperty_get_x( p ) );
      }
      break;
    }
    case ICAL_LASTMODIFIED_PROPERTY:
    {
      icaltimetype t = icalproperty_get_lastmodified(p);
      if ( t.is_utc ) {
        data->d->lastModified = toQDateTime( t );
      } else {
        kDebug() << "LAST-MODIFIED not UTC";
      }
      break;
    }
    default:
      break;
    }
    p = icalcomponent_get_next_property( vtimezone, ICAL_ANY_PROPERTY );
  }

  if ( name.isEmpty() ) {
    kDebug() << "TZID missing";
    delete data;
    return ICalTimeZone();
  }
  if ( data->d->location.isEmpty() && !xlocation.isEmpty() ) {
    data->d->location = xlocation;
  }
  QString prefix = QString::fromUtf8( icalTzidPrefix() );
  if ( name.startsWith( prefix ) ) {
    // Remove the prefix from libical built in time zone TZID
    int i = name.indexOf( '/', prefix.length() );
    if ( i > 0 ) {
      name = name.mid( i + 1 );
    }
  }
  //kDebug() << "---zoneId: \"" << name << '"';

  /*
   * Iterate through all time zone rules for this VTIMEZONE,
   * and create a Phase object containing details for each one.
   */
  int prevOffset = 0;
  QList<KTimeZone::Transition> transitions;
  QDateTime earliest;
  QList<KTimeZone::Phase> phases;
  for ( icalcomponent *c = icalcomponent_get_first_component( vtimezone, ICAL_ANY_COMPONENT );
        c;  c = icalcomponent_get_next_component( vtimezone, ICAL_ANY_COMPONENT ) )
  {
    int prevoff = 0;
    KTimeZone::Phase phase;
    QList<QDateTime> times;
    icalcomponent_kind kind = icalcomponent_isa( c );
    switch ( kind ) {

    case ICAL_XSTANDARD_COMPONENT:
      //kDebug() << "---standard phase: found";
      times = ICalTimeZoneSourcePrivate::parsePhase( c, false, prevoff, phase );
      break;

    case ICAL_XDAYLIGHT_COMPONENT:
      //kDebug() << "---daylight phase: found";
      times = ICalTimeZoneSourcePrivate::parsePhase( c, true, prevoff, phase );
      break;

    default:
      kDebug() << "Unknown component:" << int( kind );
      break;
    }
    int tcount = times.count();
    if ( tcount ) {
      phases += phase;
      for ( int t = 0;  t < tcount;  ++t ) {
        transitions += KTimeZone::Transition( times[t], phase );
      }
      if ( !earliest.isValid() || times[0] < earliest ) {
        prevOffset = prevoff;
        earliest = times[0];
      }
    }
  }
  data->setPhases( phases, prevOffset );
  // Remove any "duplicate" transitions, i.e. those where two consecutive
  // transitions have the same phase.
  qSort( transitions );
  for ( int t = 1, tend = transitions.count();  t < tend; ) {
    if ( transitions[t].phase() == transitions[t - 1].phase() ) {
      transitions.removeAt( t );
      --tend;
    } else {
      ++t;
    }
  }
  data->setTransitions( transitions );

  data->d->setComponent( icalcomponent_new_clone( vtimezone ) );
  kDebug() << "VTIMEZONE" << name;
  return ICalTimeZone( this, name, data );
}

#if defined(HAVE_UUID_UUID_H)
ICalTimeZone ICalTimeZoneSource::parse( MSTimeZone *tz, ICalTimeZones &zones )
{
  ICalTimeZone zone = parse( tz );
  if ( !zone.isValid() ) {
    return ICalTimeZone(); // error
  }
  ICalTimeZone oldzone = zones.zone( zone );
  if ( oldzone.isValid() ) {
    // A similar zone already exists in the collection, so don't add this
    // new zone, return old zone instead.
    return oldzone;
  } else if ( zones.add( zone ) ) {
    // No similar zone, add and return new one.
    return zone;
  }
  return ICalTimeZone(); // error
}

ICalTimeZone ICalTimeZoneSource::parse( MSTimeZone *tz )
{
  ICalTimeZoneData kdata;

  // General properties.
  uuid_t uuid;
  char suuid[64];
  uuid_generate_random( uuid );
  uuid_unparse( uuid, suuid );
  QString name = QString( suuid );

  // Create phases.
  QList<KTimeZone::Phase> phases;

  QList<QByteArray> standardAbbrevs;
  standardAbbrevs += tz->StandardName.toAscii();
  KTimeZone::Phase standardPhase( ( tz->Bias + tz->StandardBias ) * -60, standardAbbrevs, false,
                                  "Microsoft TIME_ZONE_INFORMATION" );
  phases += standardPhase;

  QList<QByteArray> daylightAbbrevs;
  daylightAbbrevs += tz->DaylightName.toAscii();
  KTimeZone::Phase daylightPhase( ( tz->Bias + tz->DaylightBias ) * -60, daylightAbbrevs, true,
                                  "Microsoft TIME_ZONE_INFORMATION" );
  phases += daylightPhase;

  int prevOffset = 0;
  kdata.setPhases( phases, prevOffset );

  // Create transitions
  QList<KTimeZone::Transition> transitions;
  ICalTimeZoneSourcePrivate::parseTransitions(
    tz->StandardDate, standardPhase, prevOffset, transitions );
  ICalTimeZoneSourcePrivate::parseTransitions(
    tz->DaylightDate, daylightPhase, prevOffset, transitions );

  qSort( transitions );
  kdata.setTransitions( transitions );

  ICalTimeZoneData *idata = new ICalTimeZoneData( kdata, KTimeZone( name ), QDate() );

  return ICalTimeZone( this, name, idata );
}
#endif // HAVE_UUID_UUID_H

ICalTimeZone ICalTimeZoneSource::parse( const QString &name, const QStringList &tzList,
                                        ICalTimeZones &zones )
{
  ICalTimeZone zone = parse( name, tzList );
  if ( !zone.isValid() ) {
    return ICalTimeZone(); // error
  }

  ICalTimeZone oldzone = zones.zone( zone );
  // First off see if the zone is same as oldzone - _exactly_ same
  if ( oldzone.isValid() ) {
    return oldzone;
  }

  oldzone = zones.zone( name );
  if ( oldzone.isValid() ) {
    // The zone already exists, so update
    oldzone.update( zone );
    return zone;
  } else if ( zones.add( zone ) ) {
    // No similar zone, add and return new one.
    return zone;
  }
  return ICalTimeZone(); // error
}

ICalTimeZone ICalTimeZoneSource::parse( const QString &name, const QStringList &tzList )
{
  ICalTimeZoneData kdata;
  QList<KTimeZone::Phase> phases;
  QList<KTimeZone::Transition> transitions;
  bool daylight;

  for ( QStringList::ConstIterator it = tzList.begin(); it != tzList.end(); ++it ) {
    QString value = *it;
    daylight = false;
    QString tzName = value.mid( 0, value.indexOf( ";" ) );
    value = value.mid( ( value.indexOf( ";" ) + 1 ) );
    QString tzOffset = value.mid( 0, value.indexOf( ";" ) );
    value = value.mid( ( value.indexOf( ";" ) + 1 ) );
    QString tzDaylight = value.mid( 0, value.indexOf( ";" ) );
    KDateTime tzDate = KDateTime::fromString( value.mid( ( value.lastIndexOf( ";" ) + 1 ) ) );
    if ( tzDaylight == "true" ) {
      daylight = true;
    }

    KTimeZone::Phase tzPhase( tzOffset.toInt(),
                              QByteArray( tzName.toAscii() ), daylight, "VCAL_TZ_INFORMATION" );
    phases += tzPhase;
    transitions += KTimeZone::Transition( tzDate.dateTime(), tzPhase );
  }

  kdata.setPhases( phases, 0 );
  qSort( transitions );
  kdata.setTransitions( transitions );

  ICalTimeZoneData *idata = new ICalTimeZoneData( kdata, KTimeZone( name ), QDate() );
  return ICalTimeZone( this, name, idata );
}

#if defined(HAVE_UUID_UUID_H)
//@cond PRIVATE
void ICalTimeZoneSourcePrivate::parseTransitions( const MSSystemTime &date,
                                                  const KTimeZone::Phase &phase, int prevOffset,
                                                  QList<KTimeZone::Transition> &transitions )
{
  // NOTE that we need to set start and end times and they cannot be
  // to far in either direction to avoid bloating the transitions list
  KDateTime klocalStart( QDateTime( QDate( 2000, 1, 1 ), QTime( 0, 0, 0 ) ),
                         KDateTime::Spec::ClockTime() );
  KDateTime maxTime( MAX_DATE(), KDateTime::Spec::ClockTime() );

  if ( date.wYear ) {
    // Absolute change time.
    if ( date.wYear >= 1601 && date.wYear <= 30827 &&
         date.wMonth >= 1 && date.wMonth <= 12 &&
         date.wDay >= 1 && date.wDay <= 31 ) {
      QDate dt( date.wYear, date.wMonth, date.wDay );
      QTime tm( date.wHour, date.wMinute, date.wSecond, date.wMilliseconds );
      QDateTime datetime( dt, tm );
      if ( datetime.isValid() ) {
        transitions += KTimeZone::Transition( datetime, phase );
      }
    }
  } else {
    // The normal way, for example: 'First Sunday in April at 02:00'.
    if ( date.wDayOfWeek >= 0 && date.wDayOfWeek <= 6 &&
         date.wMonth >= 1 && date.wMonth <= 12 &&
         date.wDay >= 1 && date.wDay <= 5 ) {
      RecurrenceRule r;
      r.setRecurrenceType( RecurrenceRule::rYearly );
      r.setDuration( -1 );
      r.setFrequency( 1 );
      QList<int> lst;
      lst.append( date.wMonth );
      r.setByMonths( lst );
      QList<RecurrenceRule::WDayPos> wdlst;
      RecurrenceRule::WDayPos pos;
      pos.setDay( date.wDayOfWeek ? date.wDayOfWeek : 7 );
      pos.setPos( date.wDay < 5 ? date.wDay : -1 );
      wdlst.append( pos );
      r.setByDays( wdlst );
      r.setStartDt( klocalStart );
      r.setWeekStart( 1 );
      DateTimeList dtl = r.timesInInterval( klocalStart, maxTime );
      for ( int i = 0, end = dtl.count();  i < end;  ++i ) {
        QDateTime utc = dtl[i].dateTime();
        utc.setTimeSpec( Qt::UTC );
        transitions += KTimeZone::Transition( utc.addSecs( -prevOffset ), phase );
      }
    }
  }
}
//@endcond
#endif // HAVE_UUID_UUID_H

ICalTimeZone ICalTimeZoneSource::parse( icaltimezone *tz )
{
  /* Parse the VTIMEZONE component stored in the icaltimezone structure.
   * This is both easier and provides more complete information than
   * extracting already parsed data from icaltimezone.
   */
  return tz ? parse( icaltimezone_get_component( tz ) ) : ICalTimeZone();
}

//@cond PRIVATE
QList<QDateTime> ICalTimeZoneSourcePrivate::parsePhase( icalcomponent *c,
                                                        bool daylight,
                                                        int &prevOffset,
                                                        KTimeZone::Phase &phase )
{
  QList<QDateTime> transitions;

  // Read the observance data for this standard/daylight savings phase
  QList<QByteArray> abbrevs;
  QString comment;
  prevOffset = 0;
  int utcOffset = 0;
  bool recurs = false;
  bool found_dtstart = false;
  bool found_tzoffsetfrom = false;
  bool found_tzoffsetto = false;
  icaltimetype dtstart = icaltime_null_time();

  // Now do the ical reading.
  icalproperty *p = icalcomponent_get_first_property( c, ICAL_ANY_PROPERTY );
  while ( p ) {
    icalproperty_kind kind = icalproperty_isa( p );
    switch ( kind ) {

    case ICAL_TZNAME_PROPERTY:     // abbreviated name for this time offset
    {
      // TZNAME can appear multiple times in order to provide language
      // translations of the time zone offset name.

      // TODO: Does this cope with multiple language specifications?
      QByteArray tzname = icalproperty_get_tzname( p );
      // Outlook (2000) places "Standard Time" and "Daylight Time" in the TZNAME
      // strings, which is totally useless. So ignore those.
      if ( ( !daylight && tzname == "Standard Time" ) ||
           ( daylight && tzname == "Daylight Time" ) ) {
        break;
      }
      if ( !abbrevs.contains( tzname ) ) {
        abbrevs += tzname;
      }
      break;
    }
    case ICAL_DTSTART_PROPERTY:      // local time at which phase starts
      dtstart = icalproperty_get_dtstart( p );
      found_dtstart = true;
      break;

    case ICAL_TZOFFSETFROM_PROPERTY:    // UTC offset immediately before start of phase
      prevOffset = icalproperty_get_tzoffsetfrom( p );
      found_tzoffsetfrom = true;
      break;

    case ICAL_TZOFFSETTO_PROPERTY:
      utcOffset = icalproperty_get_tzoffsetto( p );
      found_tzoffsetto = true;
      break;

    case ICAL_COMMENT_PROPERTY:
      comment = QString::fromUtf8( icalproperty_get_comment( p ) );
      break;

    case ICAL_RDATE_PROPERTY:
    case ICAL_RRULE_PROPERTY:
      recurs = true;
      break;

    default:
      kDebug() << "Unknown property:" << int( kind );
      break;
    }
    p = icalcomponent_get_next_property( c, ICAL_ANY_PROPERTY );
  }

  // Validate the phase data
  if ( !found_dtstart || !found_tzoffsetfrom || !found_tzoffsetto ) {
    kDebug() << "DTSTART/TZOFFSETFROM/TZOFFSETTO missing";
    return transitions;
  }

  // Convert DTSTART to QDateTime, and from local time to UTC
  QDateTime localStart = toQDateTime( dtstart );   // local time
  dtstart.second -= prevOffset;
  dtstart.is_utc = 1;
  QDateTime utcStart = toQDateTime( icaltime_normalize( dtstart ) );   // UTC

  transitions += utcStart;
  if ( recurs ) {
    /* RDATE or RRULE is specified. There should only be one or the other, but
     * it doesn't really matter - the code can cope with both.
     * Note that we had to get DTSTART, TZOFFSETFROM, TZOFFSETTO before reading
     * recurrences.
     */
    KDateTime klocalStart( localStart, KDateTime::Spec::ClockTime() );
    KDateTime maxTime( MAX_DATE(), KDateTime::Spec::ClockTime() );
    Recurrence recur;
    icalproperty *p = icalcomponent_get_first_property( c, ICAL_ANY_PROPERTY );
    while ( p ) {
      icalproperty_kind kind = icalproperty_isa( p );
      switch ( kind ) {

      case ICAL_RDATE_PROPERTY:
      {
        icaltimetype t = icalproperty_get_rdate(p).time;
        if ( icaltime_is_date( t ) ) {
          // RDATE with a DATE value inherits the (local) time from DTSTART
          t.hour = dtstart.hour;
          t.minute = dtstart.minute;
          t.second = dtstart.second;
          t.is_date = 0;
          t.is_utc = 0;    // dtstart is in local time
        }
        // RFC2445 states that RDATE must be in local time,
        // but we support UTC as well to be safe.
        if ( !t.is_utc ) {
          t.second -= prevOffset;    // convert to UTC
          t.is_utc = 1;
          t = icaltime_normalize( t );
        }
        transitions += toQDateTime( t );
        break;
      }
      case ICAL_RRULE_PROPERTY:
      {
        RecurrenceRule r;
        ICalFormat icf;
        ICalFormatImpl impl( &icf );
        impl.readRecurrence( icalproperty_get_rrule( p ), &r );
        r.setStartDt( klocalStart );
        // The end date time specified in an RRULE should be in UTC.
        // Convert to local time to avoid timesInInterval() getting things wrong.
        if ( r.duration() == 0 ) {
          KDateTime end( r.endDt() );
          if ( end.timeSpec() == KDateTime::Spec::UTC() ) {
            end.setTimeSpec( KDateTime::Spec::ClockTime() );
            r.setEndDt( end.addSecs( prevOffset ) );
          }
        }
        DateTimeList dts = r.timesInInterval( klocalStart, maxTime );
        for ( int i = 0, end = dts.count();  i < end;  ++i ) {
          QDateTime utc = dts[i].dateTime();
          utc.setTimeSpec( Qt::UTC );
          transitions += utc.addSecs( -prevOffset );
        }
        break;
      }
      default:
        break;
      }
      p = icalcomponent_get_next_property( c, ICAL_ANY_PROPERTY );
    }
    qSortUnique( transitions );
  }

  phase = KTimeZone::Phase( utcOffset, abbrevs, daylight, comment );
  return transitions;
}
//@endcond

ICalTimeZone ICalTimeZoneSource::standardZone( const QString &zone, bool icalBuiltIn )
{
  if ( !icalBuiltIn ) {
    // Try to fetch a system time zone in preference, on the grounds
    // that system time zones are more likely to be up to date than
    // built-in libical ones.
    QString tzid = zone;
    QString prefix = QString::fromUtf8( icalTzidPrefix() );
    if ( zone.startsWith( prefix ) ) {
      int i = zone.indexOf( '/', prefix.length() );
      if ( i > 0 ) {
        tzid = zone.mid( i + 1 );   // strip off the libical prefix
      }
    }
    KTimeZone ktz = KSystemTimeZones::readZone( tzid );
    if ( ktz.isValid() ) {
      if ( ktz.data( true ) ) {
        ICalTimeZone icaltz( ktz );
        //kDebug() << zone << " read from system database";
        return icaltz;
      }
    }
  }
  // Try to fetch a built-in libical time zone.
  // First try to look it up as a geographical location (e.g. Europe/London)
  QByteArray zoneName = zone.toUtf8();
  icaltimezone *icaltz = icaltimezone_get_builtin_timezone( zoneName );
  if ( !icaltz ) {
    // This will find it if it includes the libical prefix
    icaltz = icaltimezone_get_builtin_timezone_from_tzid( zoneName );
    if ( !icaltz ) {
      return ICalTimeZone();
    }
  }
  return parse( icaltz );
}

QByteArray ICalTimeZoneSource::icalTzidPrefix()
{
  if ( ICalTimeZoneSourcePrivate::icalTzidPrefix.isEmpty() ) {
    icaltimezone *icaltz = icaltimezone_get_builtin_timezone( "Europe/London" );
    QByteArray tzid = icaltimezone_get_tzid( icaltz );
    if ( tzid.right( 13 ) == "Europe/London" ) {
      int i = tzid.indexOf( '/', 1 );
      if ( i > 0 ) {
        ICalTimeZoneSourcePrivate::icalTzidPrefix = tzid.left( i + 1 );
        return ICalTimeZoneSourcePrivate::icalTzidPrefix;
      }
    }
    kError() << "failed to get libical TZID prefix";
  }
  return ICalTimeZoneSourcePrivate::icalTzidPrefix;
}

void ICalTimeZoneSource::virtual_hook( int id, void *data )
{
  Q_UNUSED( id );
  Q_UNUSED( data );
  Q_ASSERT( false );
}

}  // namespace KCalCore
