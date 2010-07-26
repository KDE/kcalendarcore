/*
  This file is part of the kcalcore library.

  Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2005 Reinhold Kainhofer <reinhold@kainhofe.com>

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

#include "../filestorage.h"
#include "../memorycalendar.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <kdatetime.h>
#include <kdebug.h>
#include <ksystemtimezone.h>

#include <QtCore/QFile>
#include <QtCore/QTextStream>

using namespace KCalCore;

static QString dumpTime( const KDateTime &dt, const KDateTime::Spec &viewSpec );

int main( int argc, char **argv )
{
  KAboutData aboutData(
    "testrecurrencenew", 0,
    ki18n( "Load recurrence rules with the new class and print out debug messages" ), "0.1" );
  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add( "verbose", ki18n( "Verbose output" ) );
  options.add( "+input", ki18n( "Name of input file" ) );
  options.add( "[+output]", ki18n( "optional name of output file for the recurrence dates" ) );
  KCmdLineArgs::addCmdLineOptions( options );

  KComponentData componentData( &aboutData );
  //QCoreApplication app( KCmdLineArgs::qtArgc(), KCmdLineArgs::qtArgv() );

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if ( args->count() < 1 ) {
    args->usage( "Wrong number of arguments." );
  }

  QString input = args->arg( 0 );
  kDebug() << "Input file:" << input;

  QTextStream *outstream;
  outstream = 0;
  QString fn( "" );
  if ( args->count() > 1 ) {
    fn = args->arg( 1 );
    kDebug() << "We have a file name given:" << fn;
  }
  QFile outfile( fn );
  if ( !fn.isEmpty() && outfile.open( QIODevice::WriteOnly ) ) {
    kDebug() << "Opened output file!!!";
    outstream = new QTextStream( &outfile );
  }

  MemoryCalendar::Ptr cal( new MemoryCalendar( KDateTime::UTC ) );

  KDateTime::Spec viewSpec;
  FileStorage store( cal, input );
  if ( !store.load() ) return 1;
  QString tz = cal->nonKDECustomProperty( "X-LibKCal-Testsuite-OutTZ" );
  if ( !tz.isEmpty() ) {
    viewSpec = KDateTime::Spec( KSystemTimeZones::zone( tz ) );
  }

  Incidence::List inc = cal->incidences();

  for ( Incidence::List::Iterator it = inc.begin(); it != inc.end(); ++it ) {
    Incidence::Ptr incidence = *it;
    kDebug() << "*+*+*+*+*+*+*+*+*+*";
    kDebug() << " ->" << incidence->summary() << "<-";

    incidence->recurrence()->dump();

    KDateTime dt;
    if ( incidence->allDay() ) {
      dt = incidence->dtStart().addDays( -1 );
    } else {
      dt = incidence->dtStart().addSecs( -1 );
    }
    int i=0;
    if ( outstream ) {
      // Output to file for testing purposes
      while ( dt.isValid() && i < 500 ) {
        ++i;
        dt = incidence->recurrence()->getNextDateTime( dt );
        if ( dt.isValid() ) {
          (*outstream) << dumpTime( dt, viewSpec ) << endl;
        }
      }
    } else {
      incidence->recurrence()->dump();
      // Output to konsole
      while ( dt.isValid() && i < 10 ) {
        ++i;
        kDebug() << "-------------------------------------------";
        dt = incidence->recurrence()->getNextDateTime( dt );
        if ( dt.isValid() ) {
          kDebug() << " *~*~*~*~ Next date is:" << dumpTime( dt, viewSpec );
        }
      }
    }
  }

  delete outstream;
  outfile.close();
  return 0;
}

QString dumpTime( const KDateTime &dt, const KDateTime::Spec &viewSpec )
{
  if ( !dt.isValid() ) {
    return QString();
  }
  KDateTime vdt = viewSpec.isValid() ? dt.toTimeSpec( viewSpec ) : dt;
  QString format;
#ifdef FLOAT_IS_DATE_ONLY
  if ( vdt.isDateOnly() ) {
    format = QLatin1String( "%Y-%m-%d" );
  } else
#endif
    format = QLatin1String( "%Y-%m-%dT%H:%M:%S" );
  if ( vdt.isSecondOccurrence() ) {
    format += QLatin1String( " %Z" );
  }
  if ( vdt.timeSpec() != KDateTime::ClockTime ) {
    format += QLatin1String( " %:Z" );
  }
  return vdt.toString( format );
}
