/*
  This file is part of the kcalcore library.

  Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

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
  defines the Journal class.

  @brief
  Provides a Journal in the sense of RFC2445.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/

#include "journal.h"
#include "visitor.h"

using namespace KCalCore;

Journal::Journal() : d( 0 )
{
}

Journal::~Journal()
{
}

Incidence::IncidenceType Journal::type() const
{
  return TypeJournal;
}

QByteArray Journal::typeStr() const
{
  return "Journal";
}

Journal *Journal::clone() const
{
  return new Journal( *this );
}

IncidenceBase &Journal::assign( const IncidenceBase &other )
{
  Incidence::assign( other );
  return *this;
}

bool Journal::equals( const IncidenceBase &journal ) const
{
  return Incidence::equals( journal );
}

bool Journal::accept( Visitor &v, IncidenceBase::Ptr incidence )
{
  return v.visit( incidence.staticCast<Journal>() );
}

KDateTime Journal::dateTime( DateTimeRole role ) const
{
  switch ( role ) {
    case RoleEnd:
    case RoleEndTimeZone:
      return KDateTime();
    case RoleDisplayEnd:
      return dtStart();
    default:
      return dtStart();
  }
}

void Journal::setDateTime( const KDateTime &dateTime, DateTimeRole role )
{
  Q_UNUSED( dateTime );
  Q_UNUSED( role );
}

void Journal::virtual_hook( int id, void *data )
{
  Q_UNUSED( id );
  Q_UNUSED( data );
  Q_ASSERT( false );
}

QLatin1String Journal::mimeType() const
{
  return Journal::journalMimeType();
}

/* static */
QLatin1String Journal::journalMimeType()
{
  return QLatin1String( "application/x-vnd.akonadi.calendar.journal" );
}

QLatin1String Journal::iconName( const KDateTime & ) const
{
  return QLatin1String( "view-pim-journal" );
}
