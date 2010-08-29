/*
  This file is part of the kcalcore library.

  Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

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
  This file is part of the API for handling calendar data and defines
  classes for managing compatibility between different calendar formats.

  @brief
  Classes that provide compatibility to older or "broken" calendar formats.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
*/

#include "compat.h"
#include "incidence.h"

#include <KDebug>

#include <QtCore/QRegExp>
#include <QtCore/QString>

using namespace KCalCore;

Compat *CompatFactory::createCompat( const QString &productId )
{
  Compat *compat = 0;

  int korg = productId.indexOf( "KOrganizer" );
  int outl9 = productId.indexOf( "Outlook 9.0" );

  // TODO: Use the version of LibKCal to determine the compat class...
  if ( korg >= 0 ) {
    int versionStart = productId.indexOf( " ", korg );
    if ( versionStart >= 0 ) {
      int versionStop = productId.indexOf( QRegExp( "[ /]" ), versionStart + 1 );
      if ( versionStop >= 0 ) {
        QString version = productId.mid( versionStart + 1,
                                         versionStop - versionStart - 1 );

        int versionNum = version.section( '.', 0, 0 ).toInt() * 10000 +
                         version.section( '.', 1, 1 ).toInt() * 100 +
                         version.section( '.', 2, 2 ).toInt();
        int releaseStop = productId.indexOf( "/", versionStop );
        QString release;
        if ( releaseStop > versionStop ) {
          release = productId.mid( versionStop+1, releaseStop-versionStop-1 );
        }
        if ( versionNum < 30100 ) {
          compat = new CompatPre31;
        } else if ( versionNum < 30200 ) {
          compat = new CompatPre32;
        } else if ( versionNum == 30200 && release == "pre" ) {
          kDebug() << "Generating compat for KOrganizer 3.2 pre";
          compat = new Compat32PrereleaseVersions;
        } else if ( versionNum < 30400 ) {
          compat = new CompatPre34;
        } else if ( versionNum < 30500 ) {
          compat = new CompatPre35;
        }
      }
    }
  } else if ( outl9 >= 0 ) {
    kDebug() << "Generating compat for Outlook < 2000 (Outlook 9.0)";
    compat = new CompatOutlook9;
  }

  if ( !compat ) {
    compat = new Compat;
  }

  return compat;
}

Compat::Compat()
{
}

Compat::~Compat()
{
}

void Compat::fixEmptySummary( const Incidence::Ptr &incidence )
{
  // some stupid vCal exporters ignore the standard and use Description
  // instead of Summary for the default field. Correct for this: Copy the
  // first line of the description to the summary (if summary is just one
  // line, move it)
  if ( incidence->summary().isEmpty() && !( incidence->description().isEmpty() ) ) {
    QString oldDescription = incidence->description().trimmed();
    QString newSummary( oldDescription );
    newSummary.remove( QRegExp( "\n.*" ) );
    incidence->setSummary( newSummary );
    if ( oldDescription == newSummary ) {
      incidence->setDescription( "" );
    }
  }
}

void Compat::fixAlarms( const Incidence::Ptr &incidence )
{
  Q_UNUSED( incidence );
}

void Compat::fixFloatingEnd( QDate &date )
{
  Q_UNUSED( date );
}

void Compat::fixRecurrence( const Incidence::Ptr &incidence )
{
  Q_UNUSED( incidence );
  // Prevent use of compatibility mode during subsequent changes by the application
  // incidence->recurrence()->setCompatVersion();
}

int Compat::fixPriority( int priority )
{
  return priority;
}

bool Compat::useTimeZoneShift()
{
  return true;
}

void CompatPre35::fixRecurrence( const Incidence::Ptr &incidence )
{
  Recurrence *recurrence = incidence->recurrence();
  if ( recurrence ) {
    KDateTime start( incidence->dtStart() );
    // kde < 3.5 only had one rrule, so no need to loop over all RRULEs.
    RecurrenceRule *r = recurrence->defaultRRule();
    if ( r && !r->dateMatchesRules( start ) ) {
      recurrence->addExDateTime( start );
    }
  }

  // Call base class method now that everything else is done
  Compat::fixRecurrence( incidence );
}

int CompatPre34::fixPriority( int priority )
{
  if ( 0 < priority && priority < 6 ) {
    // adjust 1->1, 2->3, 3->5, 4->7, 5->9
    return 2 * priority - 1;
  } else {
    return priority;
  }
}

void CompatPre32::fixRecurrence( const Incidence::Ptr &incidence )
{
  Recurrence *recurrence = incidence->recurrence();
  if ( recurrence->recurs() &&  recurrence->duration() > 0 ) {
    recurrence->setDuration( recurrence->duration() + incidence->recurrence()->exDates().count() );
  }
  // Call base class method now that everything else is done
  CompatPre35::fixRecurrence( incidence );
}

void CompatPre31::fixFloatingEnd( QDate &endDate )
{
  endDate = endDate.addDays( 1 );
}

void CompatPre31::fixRecurrence( const Incidence::Ptr &incidence )
{
  CompatPre32::fixRecurrence( incidence );

  Recurrence *recur = incidence->recurrence();
  RecurrenceRule *r = 0;
  if ( recur ) {
    r = recur->defaultRRule();
  }
  if ( recur && r ) {
    int duration = r->duration();
    if ( duration > 0 ) {
      // Backwards compatibility for KDE < 3.1.
      // rDuration was set to the number of time periods to recur,
      // with week start always on a Monday.
      // Convert this to the number of occurrences.
      r->setDuration( -1 );
      QDate end( r->startDt().date() );
      bool doNothing = false;
      // # of periods:
      int tmp = ( duration - 1 ) * r->frequency();
      switch ( r->recurrenceType() ) {
      case RecurrenceRule::rWeekly:
      {
        end = end.addDays( tmp * 7 + 7 - end.dayOfWeek() );
        break;
      }
      case RecurrenceRule::rMonthly:
      {
        int month = end.month() - 1 + tmp;
        end.setYMD( end.year() + month / 12, month % 12 + 1, 31 );
        break;
      }
      case RecurrenceRule::rYearly:
      {
        end.setYMD( end.year() + tmp, 12, 31 );
        break;
      }
      default:
        doNothing = true;
        break;
      }
      if ( !doNothing ) {
        duration = r->durationTo(
          KDateTime( end, QTime( 0, 0, 0 ), incidence->dtStart().timeSpec() ) );
        r->setDuration( duration );
      }
    }

    /* addYearlyNum */
    // Dates were stored as day numbers, with a fiddle to take account of
    // leap years. Convert the day number to a month.
    QList<int> days = r->byYearDays();
    if ( !days.isEmpty() ) {
      QList<int> months = r->byMonths();
      for ( int i = 0; i < months.size(); ++i ) {
        int newmonth =
          QDate( r->startDt().date().year(), 1, 1 ).addDays( months.at( i ) - 1 ).month();
        if ( !months.contains( newmonth ) ) {
          months.append( newmonth );
        }
      }

      r->setByMonths( months );
      days.clear();
      r->setByYearDays( days );
    }
  }
}

void CompatOutlook9::fixAlarms( const Incidence::Ptr &incidence )
{
  if ( !incidence ) {
    return;
  }
  Alarm::List alarms = incidence->alarms();
  Alarm::List::Iterator it;
  for ( it = alarms.begin(); it != alarms.end(); ++it ) {
    Alarm::Ptr al = *it;
    if ( al && al->hasStartOffset() ) {
      Duration offsetDuration = al->startOffset();
      int offs = offsetDuration.asSeconds();
      if ( offs > 0 ) {
        offsetDuration = Duration( -offs );
      }
      al->setStartOffset( offsetDuration );
    }
  }
}

bool Compat32PrereleaseVersions::useTimeZoneShift()
{
  return false;
}
