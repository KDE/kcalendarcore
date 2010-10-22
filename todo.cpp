/*
  This file is part of the kcalcore library.

  Copyright (c) 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2009 Allen Winter <winter@kde.org>

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
  defines the Todo class.

  @brief
  Provides a To-do in the sense of RFC2445.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Allen Winter \<winter@kde.org\>
*/

#include "todo.h"
#include "visitor.h"

using namespace KCalCore;

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class KCalCore::Todo::Private
{
  public:
    Private()
      : mPercentComplete( 0 ),
        mHasDueDate( false ),
        mHasStartDate( false ),
        mHasCompletedDate( false )
    {}
    Private( const KCalCore::Todo::Private &other )
    { init( other ); }

    void init( const KCalCore::Todo::Private &other );

    KDateTime mDtDue;        // to-do due date (if there is one)
                             // ALSO the first occurrence of a recurring to-do
    KDateTime mDtRecurrence; // next occurrence (for recurring to-dos)
    KDateTime mCompleted;    // to-do completion date (if it has been completed)
    int mPercentComplete;    // to-do percent complete [0,100]
    bool mHasDueDate;        // true if the to-do has a due date
    bool mHasStartDate;      // true if the to-do has a starting date
    bool mHasCompletedDate;  // true if the to-do has a completion date

    /**
      Returns true if the todo got a new date, else false will be returned.
    */
   bool recurTodo( Todo *todo );
};

void KCalCore::Todo::Private::init( const KCalCore::Todo::Private &other )
{
  mDtDue = other.mDtDue;
  mDtRecurrence = other.mDtRecurrence;
  mCompleted = other.mCompleted;
  mPercentComplete = other.mPercentComplete;
  mHasDueDate = other.mHasDueDate;
  mHasStartDate = other.mHasStartDate;
  mHasCompletedDate = other.mHasCompletedDate;
}

//@endcond

Todo::Todo()
  : d( new KCalCore::Todo::Private )
{
}

Todo::Todo( const Todo &other )
  : Incidence( other ),
    d( new KCalCore::Todo::Private( *other.d ) )
{
}

Todo::~Todo()
{
  delete d;
}

Todo *Todo::clone() const
{
  return new Todo( *this );
}

IncidenceBase &Todo::assign( const IncidenceBase &other )
{
  if ( &other != this ) {
    Incidence::assign( other );
    const Todo *t = static_cast<const Todo*>( &other );
    d->init( *( t->d ) );
  }
  return *this;
}

bool Todo::equals( const IncidenceBase &todo ) const
{
  if ( !Incidence::equals( todo ) ) {
    return false;
  } else {
    // If they weren't the same type IncidenceBase::equals would had returned false already
    const Todo *t = static_cast<const Todo*>( &todo );
    return ( ( dtDue() == t->dtDue() ) ||
             ( !dtDue().isValid() && !t->dtDue().isValid() ) ) &&
      hasDueDate() == t->hasDueDate() &&
      hasStartDate() == t->hasStartDate() &&
      ( ( completed() == t->completed() ) ||
        ( !completed().isValid() && !t->completed().isValid() ) ) &&
      hasCompletedDate() == t->hasCompletedDate() &&
      percentComplete() == t->percentComplete();
  }
}

Incidence::IncidenceType Todo::type() const
{
  return TypeTodo;
}

QByteArray Todo::typeStr() const
{
  return "Todo";
}
void Todo::setDtDue( const KDateTime &dtDue, bool first )
{
  startUpdates();

  //int diffsecs = d->mDtDue.secsTo(dtDue);

  /*if (mReadOnly) return;
  const Alarm::List& alarms = alarms();
  for (Alarm *alarm = alarms.first(); alarm; alarm = alarms.next()) {
    if (alarm->enabled()) {
      alarm->setTime(alarm->time().addSecs(diffsecs));
    }
  }*/
  if ( dtDue.isValid() ) {
      d->mHasDueDate = true;
  } else {
      d->mHasDueDate = false;
  }

  if ( recurs() && !first ) {
    d->mDtRecurrence = dtDue;
  } else {
    d->mDtDue = dtDue;
    // TODO: This doesn't seem right...
    recurrence()->setStartDateTime( dtDue );
    recurrence()->setAllDay( allDay() );
  }

  if ( recurs() && dtDue < recurrence()->startDateTime() ) {
    setDtStart( dtDue );
  }

  /*const Alarm::List& alarms = alarms();
  for (Alarm *alarm = alarms.first(); alarm; alarm = alarms.next())
    alarm->setAlarmStart(d->mDtDue);*/
  setFieldDirty( FieldDtDue );
  endUpdates();
}

KDateTime Todo::dtDue( bool first ) const
{
  if ( !hasDueDate() ) {
    return KDateTime();
  }
  if ( recurs() && !first && d->mDtRecurrence.isValid() ) {
    return d->mDtRecurrence;
  }

  return d->mDtDue;
}

bool Todo::hasDueDate() const
{
  return d->mHasDueDate;
}

void Todo::setHasDueDate( bool f )
{
  if ( mReadOnly ) {
    return;
  }
  update();
  d->mHasDueDate = f;
  setFieldDirty( FieldDtDue );
  updated();
}

bool Todo::hasStartDate() const
{
  return d->mHasStartDate;
}

void Todo::setHasStartDate( bool f )
{
  if ( mReadOnly ) {
    return;
  }

  update();
  if ( recurs() && !f ) {
    if ( !comments().filter( "NoStartDate" ).count() ) {
      addComment( "NoStartDate" ); //TODO: --> custom flag?
    }
  } else {
    QString s( "NoStartDate" );
    removeComment( s );
  }
  d->mHasStartDate = f;
  setFieldDirty( FieldDtStart );
  updated();
}

KDateTime Todo::dtStart() const
{
  return dtStart( false );
}

KDateTime Todo::dtStart( bool first ) const
{
  if ( !hasStartDate() ) {
    return KDateTime();
  }
  if ( recurs() && !first ) {
    KDateTime dt = d->mDtRecurrence.addDays( dtDue( true ).daysTo( IncidenceBase::dtStart() ) );
    dt.setTime( IncidenceBase::dtStart().time() );
    return dt;
  } else {
    return IncidenceBase::dtStart();
  }
}

void Todo::setDtStart( const KDateTime &dtStart )
{
  // TODO: This doesn't seem right (rfc 2445/6 says, recurrence is calculated from the dtstart...)

  d->mHasStartDate = dtStart.isValid();

  if ( recurs() ) {
    recurrence()->setStartDateTime( d->mDtDue );
    recurrence()->setAllDay( allDay() );
  }
  IncidenceBase::setDtStart( dtStart );
}

bool Todo::isCompleted() const
{
  return d->mPercentComplete == 100;
}

void Todo::setCompleted( bool completed )
{
  update();
  if ( completed ) {
    d->mPercentComplete = 100;
  } else {
    d->mPercentComplete = 0;
    d->mHasCompletedDate = false;
    d->mCompleted = KDateTime();
  }
  setFieldDirty( FieldCompleted );
  updated();
}

KDateTime Todo::completed() const
{
  if ( hasCompletedDate() ) {
    return d->mCompleted;
  } else {
    return KDateTime();
  }
}

void Todo::setCompleted( const KDateTime &completed )
{
  update();
  if ( !d->recurTodo( this ) ) {
    d->mHasCompletedDate = true;
    d->mPercentComplete = 100;
    d->mCompleted = completed.toUtc();
    setFieldDirty( FieldCompleted );
  }
  updated();
}

bool Todo::hasCompletedDate() const
{
  return d->mHasCompletedDate;
}

int Todo::percentComplete() const
{
  return d->mPercentComplete;
}

void Todo::setPercentComplete( int percent )
{
  if ( percent > 100 ) {
    percent = 100;
  } else if ( percent < 0 ) {
    percent = 0;
  }

  update();
  d->mPercentComplete = percent;
  if ( percent != 100 ) {
    d->mHasCompletedDate = false;
  }
  setFieldDirty( FieldPercentComplete );
  updated();
}

bool Todo::isInProgress( bool first ) const
{
  if ( isOverdue() ) {
    return false;
  }

  if ( d->mPercentComplete > 0 ) {
    return true;
  }

  if ( d->mHasStartDate && d->mHasDueDate ) {
    if ( allDay() ) {
      QDate currDate = QDate::currentDate();
      if ( dtStart( first ).date() <= currDate && currDate < dtDue( first ).date() ) {
        return true;
      }
    } else {
      KDateTime currDate = KDateTime::currentUtcDateTime();
      if ( dtStart( first ) <= currDate && currDate < dtDue( first ) ) {
        return true;
      }
    }
  }

  return false;
}

bool Todo::isOpenEnded() const
{
  if ( !d->mHasDueDate && !isCompleted() ) {
    return true;
  }
  return false;

}

bool Todo::isNotStarted( bool first ) const
{
  if ( d->mPercentComplete > 0 ) {
    return false;
  }

  if ( !d->mHasStartDate ) {
    return false;
  }

  if ( allDay() ) {
    if ( dtStart( first ).date() >= QDate::currentDate() ) {
      return false;
    }
  } else {
    if ( dtStart( first ) >= KDateTime::currentUtcDateTime() ) {
      return false;
    }
  }
  return true;
}

void Todo::shiftTimes( const KDateTime::Spec &oldSpec,
                       const KDateTime::Spec &newSpec )
{
  Incidence::shiftTimes( oldSpec, newSpec );
  d->mDtDue = d->mDtDue.toTimeSpec( oldSpec );
  d->mDtDue.setTimeSpec( newSpec );
  if ( recurs() ) {
    d->mDtRecurrence = d->mDtRecurrence.toTimeSpec( oldSpec );
    d->mDtRecurrence.setTimeSpec( newSpec );
  }
  if ( d->mHasCompletedDate ) {
    d->mCompleted = d->mCompleted.toTimeSpec( oldSpec );
    d->mCompleted.setTimeSpec( newSpec );
  }
}

void Todo::setDtRecurrence( const KDateTime &dt )
{
  d->mDtRecurrence = dt;
  setFieldDirty( FieldRecurrence );
}

KDateTime Todo::dtRecurrence() const
{
  return d->mDtRecurrence.isValid() ? d->mDtRecurrence : d->mDtDue;
}

bool Todo::recursOn( const QDate &date, const KDateTime::Spec &timeSpec ) const
{
  QDate today = QDate::currentDate();
  return
    Incidence::recursOn( date, timeSpec ) &&
    !( date < today && d->mDtRecurrence.date() < today &&
       d->mDtRecurrence > recurrence()->startDateTime() );
}

bool Todo::isOverdue() const
{
  if ( !dtDue().isValid() ) {
    return false; // if it's never due, it can't be overdue
  }

  bool inPast = allDay() ?
                dtDue().date() < QDate::currentDate() :
                dtDue() < KDateTime::currentUtcDateTime();
  return inPast && !isCompleted();
}

void Todo::setAllDay( bool allday )
{
  if ( allday != allDay() && !mReadOnly ) {
    if ( hasDueDate() && dtDue().isValid() ) {
      setFieldDirty( FieldDtDue );
    }
    Incidence::setAllDay( allday );
  }
}

//@cond PRIVATE
bool Todo::Private::recurTodo( Todo *todo )
{
  if ( todo && todo->recurs() ) {
    Recurrence *r = todo->recurrence();
    KDateTime endDateTime = r->endDateTime();
    KDateTime nextDate = r->getNextDateTime( todo->dtDue() );

    if ( ( r->duration() == -1 ||
           ( nextDate.isValid() && endDateTime.isValid() &&
             nextDate <= endDateTime ) ) ) {

      while ( !todo->recursAt( nextDate ) ||
              nextDate <= KDateTime::currentUtcDateTime() ) {

        if ( !nextDate.isValid() ||
             ( nextDate > endDateTime && r->duration() != -1 ) ) {

          return false;
        }

        nextDate = r->getNextDateTime( nextDate );
      }

      todo->setDtDue( nextDate );
      todo->setCompleted( false );
      todo->setRevision( todo->revision() + 1 );

      return true;
    }
  }

  return false;
}
//@endcond

bool Todo::accept( Visitor &v, IncidenceBase::Ptr incidence )
{
  return v.visit( incidence.staticCast<Todo>() );
}

KDateTime Todo::dateTime( DateTimeRole role ) const
{
  switch ( role ) {
  case RoleAlarmStartOffset:
    return dtStart();
  case RoleAlarmEndOffset:
    return dtDue();
  case RoleSort:
    // Sorting to-dos first compares dtDue, then dtStart if
    // dtDue doesn't exist
    return hasDueDate() ? dtDue() : dtStart();
  case RoleCalendarHashing:
    return dtDue();
  case RoleStartTimeZone:
    return dtStart();
  case RoleEndTimeZone:
    return dtDue();
  case RoleEndRecurrenceBase:
    return dtDue();
  case RoleDisplayEnd:
    return dtDue();
  case RoleAlarm:
    if ( alarms().isEmpty() ) {
      return KDateTime();
    } else {
      Alarm::Ptr alarm = alarms().first();
      if ( alarm->hasStartOffset() && hasStartDate() ) {
        return dtStart();
      } else if ( alarm->hasEndOffset() && hasDueDate() ) {
        return dtDue();
      } else {
        // The application shouldn't add alarms on to-dos without dates.
        return KDateTime();
      }
    }
  case RoleRecurrenceStart:
    return dtDue();
    break;
  case RoleEnd:
    // todos don't have dtEnd
  default:
    return KDateTime();
  }
}

void Todo::setDateTime( const KDateTime &dateTime, DateTimeRole role )
{
  Q_UNUSED( dateTime );
  Q_UNUSED( role );
}

void Todo::virtual_hook( int id, void *data )
{
  Q_UNUSED( id );
  Q_UNUSED( data );
  Q_ASSERT( false );
}

QLatin1String Todo::mimeType() const
{
  return Todo::todoMimeType();
}

QLatin1String Todo::todoMimeType()
{
  return QLatin1String( "application/x-vnd.akonadi.calendar.todo" );
}

QLatin1String Todo::iconName( const KDateTime &recurrenceId ) const
{
  KDateTime occurrenceDT = recurrenceId;

  if ( recurs() && occurrenceDT.isDateOnly() ) {
    occurrenceDT.setTime( QTime( 0, 0 ) );
  }

  const bool usesCompletedTaskPixmap = isCompleted() ||
                                       ( recurs() && occurrenceDT.isValid() &&
                                         occurrenceDT < dtDue( false ) );

  if ( usesCompletedTaskPixmap ) {
    return QLatin1String( "task-complete" );
  } else {
    return QLatin1String( "view-calendar-tasks" );
  }
}
