/*
  This file is part of the kcalcore library.

  Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2005 Reinhold Kainhofer <reinhold@kainhofer.com>

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
#include "../vcalformat.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <kdebug.h>
#include <klocale.h>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

using namespace KCalCore;

int main( int argc, char **argv )
{
  KAboutData aboutData( "testvcalexport", 0,
                        ki18n( "Part of LibKCal's test suite. Checks if export "
                               "to vCalendar still works correctly."), "0.1" );
  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add( "verbose", ki18n( "Verbose output" ) );
  options.add( "+input", ki18n( "Name of input file" ) );
  options.add( "+output", ki18n( "Name of output file" ) );
  KCmdLineArgs::addCmdLineOptions( options );

  KComponentData componentData( &aboutData );
  //QCoreApplication app( KCmdLineArgs::qtArgc(), KCmdLineArgs::qtArgv() );

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if ( args->count() != 2 ) {
    args->usage( "Wrong number of arguments." );
  }

  QString input = args->arg( 0 );
  QString output = args->arg( 1 );

  QFileInfo outputFileInfo( output );
  output = outputFileInfo.absoluteFilePath();

  kDebug() << "Input file:" << input;
  kDebug() << "Output file:" << output;

  MemoryCalendar::Ptr cal( new MemoryCalendar( KDateTime::UTC ) );
  FileStorage instore( cal, input );

  if ( !instore.load() ) {
    return 1;
  }
  QString tz = cal->nonKDECustomProperty( "X-LibKCal-Testsuite-OutTZ" );
  if ( !tz.isEmpty() ) {
    cal->setViewTimeZoneId( tz );
  }

  FileStorage outstore( cal, output, new VCalFormat );
  if ( !outstore.save() ) {
    return 1;
  }

  return 0;
}
