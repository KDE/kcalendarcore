/*
  This file is part of the kcalcore library.

  Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

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

#include "../icalformat.h"
#include "../event.h"
#include "../todo.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <kdebug.h>
#include <klocale.h>

#include <QtCore/QCoreApplication>

using namespace KCalCore;

int main( int argc, char **argv )
{
  KAboutData aboutData( "testincidence", 0, ki18n( "Test Incidence" ), "0.1" );
  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add( "verbose", ki18n( "Verbose output" ) );
  KCmdLineArgs::addCmdLineOptions( options );

  KComponentData componentData( &aboutData );
  //QCoreApplication app( KCmdLineArgs::qtArgc(), KCmdLineArgs::qtArgv() );

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  bool verbose = false;
  if ( args->isSet( "verbose" ) ) {
    verbose = true;
  }

  ICalFormat f;

  Event::Ptr event1 = Event::Ptr( new Event );
  event1->setSummary( "Test Event" );
  event1->recurrence()->setDaily( 2 );
  event1->recurrence()->setDuration( 3 );

  QString eventString1 = f.toString( event1.staticCast<Incidence>() );
  if ( verbose ) {
    kDebug() << "EVENT1 START:" << eventString1 << "EVENT1 END";
  }

  Incidence::Ptr event2 = Incidence::Ptr( event1->clone() );

  QString eventString2 = f.toString( event2.staticCast<Incidence>() );
  if( verbose ) {
    kDebug() << "EVENT2 START:" << eventString2 << "EVENT2 END";
  }

  if ( eventString1 != eventString2 ) {
    kDebug() << "Clone Event FAILED.";
  } else {
    kDebug() << "Clone Event SUCCEEDED.";
  }

  Todo::Ptr todo1 = Todo::Ptr( new Todo );
  todo1->setSummary( "Test todo" );
  QString todoString1 = f.toString( todo1.staticCast<Incidence>() );
  if( verbose ) {
    kDebug() << "todo1 START:" << todoString1 << "todo1 END";
  }

  Incidence::Ptr todo2 = Incidence::Ptr( todo1->clone() );
  QString todoString2 = f.toString( todo2 );
  if( verbose ) {
    kDebug() << "todo2 START:" << todoString2 << "todo2 END";
  }

  if ( todoString1 != todoString2 ) {
    kDebug() << "Clone Todo FAILED.";
  } else {
    kDebug() << "Clone Todo SUCCEEDED.";
  }
}
