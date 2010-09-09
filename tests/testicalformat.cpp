/*
  This file is part of the kcalcore library.

  Copyright (C) 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
  Author: Sergio Martins <sergio.martins@kdab.com>

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

#include "testicalformat.h"
#include "testicalformat.moc"
#include "../event.h"
#include "../icalformat.h"
#include "../memorycalendar.h"

#include <KDebug>
#include <kdatetime.h>

#include <qtest_kde.h>

#include <unistd.h>

QTEST_KDEMAIN( ICalFormatTest, NoGUI )

using namespace KCalCore;

void ICalFormatTest::testCharsets()
{
  ICalFormat format;
  const QDate currentDate = QDate::currentDate();
  Event::Ptr event = Event::Ptr( new Event() );
  event->setUid( "12345" );
  event->setDtStart( KDateTime( currentDate ) );
  event->setDtEnd( KDateTime( currentDate.addDays( 1 ) ) );

  // ü
  const char latin1_umlaut[] = { 0xFC, '\0' };
  event->setSummary( latin1_umlaut );

  // Test if toString( Incidence ) didn't mess charsets
  const QString serialized = format.toString( event.staticCast<Incidence>() );
  const char utf_umlaut[] = { 0xC3, 0XBC, '\0' };
  QVERIFY( serialized.toUtf8().contains( utf_umlaut ) );
  QVERIFY( !serialized.toUtf8().contains( latin1_umlaut ) );
  QVERIFY( serialized.toLatin1().contains( latin1_umlaut ) );
  QVERIFY( !serialized.toLatin1().contains( utf_umlaut ) );

  // test fromString( QString )
  const QString serializedCalendar = "BEGIN:VCALENDAR\nPRODID:-//K Desktop Environment//NONSGML libkcal 3.2//EN\nVERSION:2.0\n"
                                     + serialized + "\nEND:VCALENDAR";

  Incidence::Ptr event2 = format.fromString( serializedCalendar );
  QVERIFY( event->summary() == event2->summary() );
  QVERIFY( event2->summary().toUtf8() == QByteArray( utf_umlaut ) );

  // test save()
  MemoryCalendar::Ptr calendar( new MemoryCalendar( "UTC" ) );
  calendar->addIncidence( event );
  QVERIFY( format.save( calendar, "hommer.ics" ) );

  // Make sure hommer.ics is in UTF-8
  QFile file( "hommer.ics" );
  Q_ASSERT( file.open( QIODevice::ReadOnly | QIODevice::Text ) );

  const QByteArray bytesFromFile = file.readAll();
  QVERIFY( bytesFromFile.contains( utf_umlaut ) );
  QVERIFY( !bytesFromFile.contains( latin1_umlaut ) );
  file.close();

  // Test load:
  MemoryCalendar::Ptr calendar2( new MemoryCalendar( "UTC" ) );
  QVERIFY( format.load( calendar2, "hommer.ics" ) );
  QVERIFY( calendar2->incidences().count() == 1 );

  // kDebug() << format.toString( event.staticCast<Incidence>() );
  // kDebug() << format.toString( calendar2->incidences().first() );

  Event::Ptr loadedEvent = calendar2->incidences().first().staticCast<Event>();
  QVERIFY( loadedEvent->summary().toUtf8() == QByteArray( utf_umlaut ) );
  QVERIFY( *loadedEvent == *event );


  // Test fromRawString()
  MemoryCalendar::Ptr calendar3( new MemoryCalendar( "UTC" ) );
  format.fromRawString( calendar3, bytesFromFile );
  QVERIFY( calendar3->incidences().count() == 1 );
  QVERIFY( *calendar3->incidences().first() == *event );

  unlink( "hommer.ics" );
}
