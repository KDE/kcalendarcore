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

#include "../filestorage.h"
#include "../memorycalendar.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <kdebug.h>

using namespace KCalCore;

int main( int argc, char **argv )
{
  KAboutData aboutData( "testcalendar", 0, ki18n( "Test Calendar" ), "0.1" );
  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add( "verbose", ki18n( "Verbose output" ) );
  KCmdLineArgs::addCmdLineOptions( options );

  KComponentData componentData( &aboutData );
  //QCoreApplication app( KCmdLineArgs::qtArgc(), KCmdLineArgs::qtArgv() );

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  Q_UNUSED( args );

  MemoryCalendar::Ptr cal( new MemoryCalendar( KDateTime::UTC ) );
  FileStorage store( cal, "cal" );
  store.load();

  Todo::List todoList;
  Todo::List::ConstIterator todo;

  // Build dictionary to look up Task object from Todo uid.  Each task is a
  // QListViewItem, and is initially added with the view as the parent.
  todoList = cal->rawTodos();
  kDebug() << ( *todoList.begin() )->uid();
  QString result = ( *todoList.begin() )->customProperty( QByteArray( "karm" ),
                                                          QByteArray( "totalTaskTime" ) );
  kDebug() << result;
  if ( result != QString( "a,b" ) ) {
    kDebug() << "The string a,b was expected, but given was" << result;
    return 1;
  } else {
    kDebug() << "Test passed";
  }
}
