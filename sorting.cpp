/*
  This file is part of the kcalcore library.

  Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
  Contact: Alvaro Manera <alvaro.manera@nokia.com>

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
#include "sorting.h"
#include "event.h"
#include "journal.h"
#include "todo.h"

#include <KDateTime>

// PENDING(kdab) Review
// The QString::compare() need to be replace by a DUI string comparisons.
// See http://qt.gitorious.org/maemo-6-ui-framework/libdui
// If not compiled in "meego-mode" should we be using locale compares?

using namespace KCalCore;

bool KCalCore::Events::startDateLessThan( const Event::Ptr &e1, const Event::Ptr &e2 )
{
  const KDateTime d1= e1->dtStart();
  KDateTime::Comparison res = d1.compare( e2->dtStart() );
  if ( res == KDateTime::Equal ) {
    return Events::summaryLessThan( e1, e2 );
  } else {
    return ( res & KDateTime::Before || res & KDateTime::AtStart );
  }
}

bool KCalCore::Events::startDateMoreThan( const Event::Ptr &e1, const Event::Ptr &e2 )
{
  const KDateTime d1= e1->dtStart();
  KDateTime::Comparison res = d1.compare( e2->dtStart() );
  if ( res == KDateTime::Equal ) {
    return Events::summaryMoreThan( e1, e2 );
  } else {
    return ( res & KDateTime::After || res & KDateTime::AtEnd );
  }
}

bool KCalCore::Events::summaryLessThan( const Event::Ptr &e1, const Event::Ptr &e2 )
{
  return QString::compare( e1->summary(), e2->summary(), Qt::CaseInsensitive ) < 0;
}

bool KCalCore::Events::summaryMoreThan( const Event::Ptr &e1, const Event::Ptr &e2 )
{
  return QString::compare( e1->summary(), e2->summary(), Qt::CaseInsensitive ) > 0;
}

bool KCalCore::Events::endDateLessThan( const Event::Ptr &e1, const Event::Ptr &e2 )
{
  const KDateTime d1= e1->dtEnd();
  KDateTime::Comparison res = d1.compare( e2->dtEnd() );
  if ( res == KDateTime::Equal ) {
    return Events::summaryLessThan( e1, e2 );
  } else {
    return ( res & KDateTime::Before || res & KDateTime::AtStart );
  }
}

bool KCalCore::Events::endDateMoreThan( const Event::Ptr &e1, const Event::Ptr &e2 )
{
  const KDateTime d1= e1->dtEnd();
  KDateTime::Comparison res = d1.compare( e2->dtEnd() );
  if ( res == KDateTime::Equal ) {
    return Events::summaryMoreThan( e1, e2 );
  } else {
    return ( res & KDateTime::After || res & KDateTime::AtEnd );
  }
}

bool KCalCore::Journals::dateLessThan( const Journal::Ptr &j1, const Journal::Ptr &j2 )
{
  const KDateTime d1 = j1->dtStart();
  KDateTime::Comparison res = d1.compare( j2->dtStart() );
  return ( res & KDateTime::Before || res & KDateTime::AtStart );
}

bool KCalCore::Journals::dateMoreThan( const Journal::Ptr &j1, const Journal::Ptr &j2 )
{
  const KDateTime d1= j1->dtStart();
  KDateTime::Comparison res = d1.compare( j2->dtStart() );
  return ( res & KDateTime::After || res & KDateTime::AtEnd );
}

bool KCalCore::Journals::summaryLessThan( const Journal::Ptr &j1, const Journal::Ptr &j2 )
{

  return QString::compare( j1->summary(), j2->summary(), Qt::CaseInsensitive ) < 0;
}

bool KCalCore::Journals::summaryMoreThan( const Journal::Ptr &j1, const Journal::Ptr &j2 )
{
  return QString::compare( j1->summary(), j2->summary(), Qt::CaseInsensitive ) > 0;
}

bool KCalCore::Todos::startDateLessThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  const KDateTime d1= t1->dtStart();
  KDateTime::Comparison res = d1.compare( t2->dtStart() );
  if ( res == KDateTime::Equal ) {
    return Todos::summaryLessThan( t1, t2 );
  } else {
    return ( res & KDateTime::Before || res & KDateTime::AtStart );
  }
}

bool KCalCore::Todos::startDateMoreThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  const KDateTime d1= t1->dtStart();
  KDateTime::Comparison res = d1.compare( t2->dtStart() );
  if ( res == KDateTime::Equal ) {
    return Todos::summaryMoreThan( t1, t2 );
  } else {
    return ( res & KDateTime::After || res & KDateTime::AtEnd );
  }
}

bool KCalCore::Todos::dueDateLessThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  const KDateTime d1= t1->dtDue();
  KDateTime::Comparison res = d1.compare( t2->dtDue() );
  if ( res == KDateTime::Equal ) {
    return Todos::summaryLessThan( t1, t2 );
  } else {
    return ( res & KDateTime::Before || res & KDateTime::AtStart );
  }
}

bool KCalCore::Todos::dueDateMoreThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  const KDateTime d1= t1->dtDue();
  KDateTime::Comparison res = d1.compare( t2->dtDue() );
  if ( res == KDateTime::Equal ) {
    return Todos::summaryMoreThan( t1, t2 );
  } else {
    return ( res & KDateTime::After || res & KDateTime::AtEnd );
  }
}

bool KCalCore::Todos::priorityLessThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  if ( t1->priority() < t2->priority() ) {
    return true;
  } else if ( t1->priority() == t2->priority() ) {
    return Todos::summaryLessThan( t1, t2 );
  } else {
    return false;
  }
}

bool KCalCore::Todos::priorityMoreThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  if ( t1->priority() > t2->priority() ) {
    return true;
  } else if ( t1->priority() == t2->priority() ) {
    return Todos::summaryMoreThan( t1, t2 );
  } else {
    return false;
  }
}

bool KCalCore::Todos::percentLessThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  if ( t1->percentComplete() < t2->percentComplete() ) {
    return true;
  } else if ( t1->percentComplete() == t2->percentComplete() ) {
    return Todos::summaryLessThan( t1, t2 );
  } else {
    return false;
  }
}

bool KCalCore::Todos::percentMoreThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  if ( t1->percentComplete() > t2->percentComplete() ) {
    return true;
  } else if ( t1->percentComplete() == t2->percentComplete() ) {
    return Todos::summaryMoreThan( t1, t2 );
  } else {
    return false;
  }
}

bool KCalCore::Todos::summaryLessThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  return QString::compare( t1->summary(), t2->summary(), Qt::CaseInsensitive ) < 0;
}

bool KCalCore::Todos::summaryMoreThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  return QString::compare( t1->summary(), t2->summary(), Qt::CaseInsensitive ) > 0;
}

bool KCalCore::Todos::createdLessThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  const KDateTime d1= t1->created();
  KDateTime::Comparison res = d1.compare( t2->created() );
  if ( res == KDateTime::Equal ) {
    return Todos::summaryLessThan( t1, t2 );
  } else {
    return ( res & KDateTime::Before || res & KDateTime::AtStart );
  }
}

bool KCalCore::Todos::createdMoreThan( const Todo::Ptr &t1, const Todo::Ptr &t2 )
{
  const KDateTime d1= t1->created();
  KDateTime::Comparison res = d1.compare( t2->created() );
  if ( res == KDateTime::Equal ) {
    return Todos::summaryMoreThan( t1, t2 );
  } else {
    return ( res & KDateTime::After || res & KDateTime::AtEnd );
  }
}

bool KCalCore::Incidences::dateLessThan( const Incidence::Ptr &i1,
                                         const Incidence::Ptr &i2 )
{
  const KDateTime d1 = i1->dateTime( Incidence::RoleSort );
  const KDateTime d2 = i2->dateTime( Incidence::RoleSort );

  KDateTime::Comparison res = d1.compare( d2 );
  if ( res == KDateTime::Equal ) {
    return Incidences::summaryLessThan( i1, i2 );
  } else {
    return ( res & KDateTime::Before || res & KDateTime::AtStart );
  }
}

bool KCalCore::Incidences::dateMoreThan( const Incidence::Ptr &i1,
                                         const Incidence::Ptr &i2 )
{
  const KDateTime d1 = i1->dateTime( Incidence::RoleSort );
  const KDateTime d2 = i2->dateTime( Incidence::RoleSort );

  KDateTime::Comparison res = d1.compare( d2 );
  if ( res == KDateTime::Equal ) {
    return Incidences::summaryMoreThan( i1, i2 );
  } else {
    return ( res & KDateTime::After || res & KDateTime::AtEnd );
  }
}

bool KCalCore::Incidences::createdLessThan( const Incidence::Ptr &i1,
                                            const Incidence::Ptr &i2 )
{
  const KDateTime d1= i1->created();
  KDateTime::Comparison res = d1.compare( i2->created() );
  if ( res == KDateTime::Equal ) {
    return Incidences::summaryLessThan( i1, i2 );
  } else {
    return ( res & KDateTime::Before || res & KDateTime::AtStart );
  }
}

bool KCalCore::Incidences::createdMoreThan( const Incidence::Ptr &i1,
                                            const Incidence::Ptr &i2 )
{
  const KDateTime d1= i1->created();
  KDateTime::Comparison res = d1.compare( i2->created() );
  if ( res == KDateTime::Equal ) {
    return Incidences::summaryMoreThan( i1, i2 );
  } else {
    return ( res & KDateTime::After || res & KDateTime::AtEnd );
  }
}

bool KCalCore::Incidences::summaryLessThan( const Incidence::Ptr &i1,
                                            const Incidence::Ptr &i2 )
{
  return QString::compare( i1->summary(), i2->summary(), Qt::CaseInsensitive ) < 0;
}

bool KCalCore::Incidences::summaryMoreThan( const Incidence::Ptr &i1,
                                            const Incidence::Ptr &i2 )
{
  return QString::compare( i1->summary(), i2->summary(), Qt::CaseInsensitive ) > 0;
}

bool KCalCore::Persons::countMoreThan( const Person::Ptr &p1, const Person::Ptr &p2 )
{
  return p1->count() > p2->count();
}
