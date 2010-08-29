/*
  This file is part of the kcalcore library.

  Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2004 Reinhold Kainhofer <reinhold@kainhofer.com>

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
/**
  @file
  This file is part of the API for handling calendar data and
  defines the FreeBusy class.

  @brief
  Provides information about the free/busy time of a calendar user.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
*/
#include "freebusy.h"
#include "visitor.h"

#include "icalformat.h"

#include <KDebug>

using namespace KCalCore;

//@cond PRIVATE
class KCalCore::FreeBusy::Private
{
  private:
  FreeBusy *q;
  public:
    Private( FreeBusy *qq ) : q( qq )
    {}

    Private( const KCalCore::FreeBusy::Private &other, FreeBusy *qq ) : q( qq )
    {
      init( other );
    }

    Private( const FreeBusyPeriod::List &busyPeriods, FreeBusy *qq )
      : q( qq ), mBusyPeriods( busyPeriods )
    {}

    void init( const KCalCore::FreeBusy::Private &other );
    void init( const Event::List &events, const KDateTime &start, const KDateTime &end );

    KDateTime mDtEnd;                  // end datetime
    FreeBusyPeriod::List mBusyPeriods; // list of periods

    // This is used for creating a freebusy object for the current user
    bool addLocalPeriod( FreeBusy *fb, const KDateTime &start, const KDateTime &end );
};

void KCalCore::FreeBusy::Private::init( const KCalCore::FreeBusy::Private &other )
{
  mDtEnd = other.mDtEnd;
  mBusyPeriods = other.mBusyPeriods;
}
//@endcond

FreeBusy::FreeBusy()
  : d( new KCalCore::FreeBusy::Private( this ) )
{
}

FreeBusy::FreeBusy( const FreeBusy &other )
  : IncidenceBase( other ),
    d( new KCalCore::FreeBusy::Private( *other.d, this ) )
{
}

FreeBusy::FreeBusy( const KDateTime &start, const KDateTime &end )
  : d( new KCalCore::FreeBusy::Private( this ) )
{
  setDtStart( start );
  setDtEnd( end );
}

FreeBusy::FreeBusy( const Event::List &events, const KDateTime &start, const KDateTime &end )
  : d( new KCalCore::FreeBusy::Private( this ) )
{
  setDtStart( start );
  setDtEnd( end );

  d->init( events, start, end );
}

//@cond PRIVATE
void FreeBusy::Private::init( const Event::List &eventList,
                              const KDateTime &start, const KDateTime &end )
{
  int extraDays, i, x, duration;
  duration = start.daysTo( end );
  QDate day;
  KDateTime tmpStart;
  KDateTime tmpEnd;

  // Loops through every event in the calendar
  Event::List::ConstIterator it;
  for ( it = eventList.constBegin(); it != eventList.constEnd(); ++it ) {
    Event::Ptr event = *it;

    // If this event is transparent it shouldn't be in the freebusy list.
    if ( event->transparency() == Event::Transparent ) {
      continue;
    }

    // The code below can not handle all-day events. Fixing this resulted
    // in a lot of duplicated code. Instead, make a copy of the event and
    // set the period to the full day(s). This trick works for recurring,
    // multiday, and single day all-day events.
    Event::Ptr allDayEvent;
    if ( event->allDay() ) {
      // addDay event. Do the hack
      kDebug() << "All-day event";
      allDayEvent = Event::Ptr( new Event( *event ) );

      // Set the start and end times to be on midnight
      KDateTime st = allDayEvent->dtStart();
      st.setTime( QTime( 0, 0 ) );
      KDateTime nd = allDayEvent->dtEnd();
      nd.setTime( QTime( 23, 59, 59, 999 ) );
      allDayEvent->setAllDay( false );
      allDayEvent->setDtStart( st );
      allDayEvent->setDtEnd( nd );

      kDebug() << "Use:" << st.toString() << "to" << nd.toString();
      // Finally, use this event for the setting below
      event = allDayEvent;
    }

    // This whole for loop is for recurring events, it loops through
    // each of the days of the freebusy request

    for ( i = 0; i <= duration; ++i ) {
      day = start.addDays(i).date();
      tmpStart.setDate( day );
      tmpEnd.setDate( day );

      if ( event->recurs() ) {
        if ( event->isMultiDay() ) {
  // FIXME: This doesn't work for sub-daily recurrences or recurrences with
  //        a different time than the original event.
          extraDays = event->dtStart().daysTo( event->dtEnd() );
          for ( x = 0; x <= extraDays; ++x ) {
            if ( event->recursOn( day.addDays(-x), start.timeSpec() ) ) {
              tmpStart.setDate( day.addDays(-x) );
              tmpStart.setTime( event->dtStart().time() );
              tmpEnd = event->duration().end( tmpStart );

              addLocalPeriod( q, tmpStart, tmpEnd );
              break;
            }
          }
        } else {
          if ( event->recursOn( day, start.timeSpec() ) ) {
            tmpStart.setTime( event->dtStart().time() );
            tmpEnd.setTime( event->dtEnd().time() );

            addLocalPeriod ( q, tmpStart, tmpEnd );
          }
        }
      }

    }
    // Non-recurring events
    addLocalPeriod( q, event->dtStart(), event->dtEnd() );
  }

  q->sortList();
}
//@endcond

FreeBusy::FreeBusy( const Period::List &busyPeriods )
  : d( new KCalCore::FreeBusy::Private( this ) )
{
  addPeriods(busyPeriods);
}

FreeBusy::FreeBusy( const FreeBusyPeriod::List &busyPeriods )
  : d( new KCalCore::FreeBusy::Private( busyPeriods, this ) )
{
}

FreeBusy::~FreeBusy()
{
  delete d;
}

IncidenceBase::IncidenceType FreeBusy::type() const
{
  return TypeFreeBusy;
}

QByteArray FreeBusy::typeStr() const
{
  return "FreeBusy";
}

void FreeBusy::setDtStart( const KDateTime &start )
{
  IncidenceBase::setDtStart( start.toUtc() );
  updated();
}

void FreeBusy::setDtEnd( const KDateTime &end )
{
  d->mDtEnd = end;
}

KDateTime FreeBusy::dtEnd() const
{
  return d->mDtEnd;
}

Period::List FreeBusy::busyPeriods() const
{
  Period::List res;

  foreach ( const FreeBusyPeriod &p, d->mBusyPeriods ) {
    res << p;
  }

  return res;
}

FreeBusyPeriod::List FreeBusy::fullBusyPeriods() const
{
  return d->mBusyPeriods;
}

void FreeBusy::sortList()
{
  qSort( d->mBusyPeriods );
  return;
}

void FreeBusy::addPeriods( const Period::List &list )
{
  foreach ( const Period &p, list ) {
    d->mBusyPeriods << FreeBusyPeriod( p );
  }
  sortList();
}

void FreeBusy::addPeriods( const FreeBusyPeriod::List &list )
{
  d->mBusyPeriods += list;
  sortList();
}

void FreeBusy::addPeriod( const KDateTime &start, const KDateTime &end )
{
  d->mBusyPeriods.append( FreeBusyPeriod( start, end ) );
  sortList();
}

void FreeBusy::addPeriod( const KDateTime &start, const Duration &duration )
{
  d->mBusyPeriods.append( FreeBusyPeriod( start, duration ) );
  sortList();
}

void FreeBusy::merge( FreeBusy::Ptr freeBusy )
{
  if ( freeBusy->dtStart() < dtStart() ) {
    setDtStart( freeBusy->dtStart() );
  }

  if ( freeBusy->dtEnd() > dtEnd() ) {
    setDtEnd( freeBusy->dtEnd() );
  }

  Period::List periods = freeBusy->busyPeriods();
  Period::List::ConstIterator it;
  for ( it = periods.constBegin(); it != periods.constEnd(); ++it ) {
    d->mBusyPeriods.append( FreeBusyPeriod( (*it).start(), (*it).end() ) );
  }
  sortList();
}

void FreeBusy::shiftTimes( const KDateTime::Spec &oldSpec,
                           const KDateTime::Spec &newSpec )
{
  if ( oldSpec.isValid() && newSpec.isValid() && oldSpec != newSpec ) {
    IncidenceBase::shiftTimes( oldSpec, newSpec );
    d->mDtEnd = d->mDtEnd.toTimeSpec( oldSpec );
    d->mDtEnd.setTimeSpec( newSpec );
    foreach ( FreeBusyPeriod p, d->mBusyPeriods ) { //krazy:exclude=foreach
      p.shiftTimes( oldSpec, newSpec );
    }
  }
}

IncidenceBase &FreeBusy::assign( const IncidenceBase &other )
{
  if ( &other != this ) {
    IncidenceBase::assign( other );
    const FreeBusy *f = static_cast<const FreeBusy*>( &other );
    d->init( *( f->d ) );
  }
  return *this;
}

bool FreeBusy::equals( const IncidenceBase &freeBusy ) const
{
  if ( !IncidenceBase::equals( freeBusy ) ) {
    return false;
  } else {
    // If they weren't the same type IncidenceBase::equals would had returned false already
    const FreeBusy *fb = static_cast<const FreeBusy*>( &freeBusy );
    return
      dtEnd() == fb->dtEnd() &&
      d->mBusyPeriods == fb->d->mBusyPeriods;
  }
}

bool FreeBusy::accept( Visitor &v, IncidenceBase::Ptr incidence )
{
  return v.visit( incidence.staticCast<FreeBusy>() );
}

KDateTime FreeBusy::dateTime( DateTimeRole role ) const
{
  Q_UNUSED( role );
  // No roles affecting freeBusy yet
  return KDateTime();
}

void FreeBusy::setDateTime( const KDateTime &dateTime, DateTimeRole role )
{
  Q_UNUSED( dateTime );
  Q_UNUSED( role );
}

void FreeBusy::virtual_hook( int id, void *data )
{
  Q_UNUSED( id );
  Q_UNUSED( data );
  Q_ASSERT( false );
}

//@cond PRIVATE
bool FreeBusy::Private::addLocalPeriod( FreeBusy *fb,
                                        const KDateTime &eventStart,
                                        const KDateTime &eventEnd )
{
  KDateTime tmpStart;
  KDateTime tmpEnd;

  //Check to see if the start *or* end of the event is
  //between the start and end of the freebusy dates.
  KDateTime start = fb->dtStart();
  if ( !( ( ( start.secsTo( eventStart ) >= 0 ) &&
            ( eventStart.secsTo( mDtEnd ) >= 0 ) ) ||
          ( ( start.secsTo( eventEnd ) >= 0 ) &&
            ( eventEnd.secsTo( mDtEnd ) >= 0 ) ) ) ) {
    return false;
  }

  if ( eventStart.secsTo( start ) >= 0 ) {
    tmpStart = start;
  } else {
    tmpStart = eventStart;
  }

  if ( eventEnd.secsTo( mDtEnd ) <= 0 ) {
    tmpEnd = mDtEnd;
  } else {
    tmpEnd = eventEnd;
  }

  FreeBusyPeriod p( tmpStart, tmpEnd );
  mBusyPeriods.append( p );

  return true;
}
//@endcond

QLatin1String FreeBusy::mimeType() const
{
  return FreeBusy::freeBusyMimeType();
}

QLatin1String KCalCore::FreeBusy::freeBusyMimeType()
{
  return QLatin1String( "application/x-vnd.akonadi.calendar.freebusy" );
}

QDataStream &KCalCore::operator<<( QDataStream &stream, const KCalCore::FreeBusy::Ptr &freebusy )
{
  KCalCore::ICalFormat format;
  QString data = format.createScheduleMessage( freebusy, iTIPPublish );
  return stream << data;
}

QDataStream &KCalCore::operator>>( QDataStream &stream, KCalCore::FreeBusy::Ptr &freebusy )
{
  QString freeBusyVCal;
  stream >> freeBusyVCal;

  KCalCore::ICalFormat format;
  freebusy = format.parseFreeBusy( freeBusyVCal );

  if ( !freebusy ) {
    kDebug() << "Error parsing free/busy";
    kDebug() << freeBusyVCal;
  }

  return stream;
}

