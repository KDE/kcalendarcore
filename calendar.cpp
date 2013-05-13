/*
  This file is part of the kcalcore library.

  Copyright (c) 1998 Preston Brown <pbrown@kde.org>
  Copyright (c) 2000-2004 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  Copyright (c) 2006 David Jarvie <software@astrojar.org.uk>

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
  defines the Calendar class.

  @brief
  Represents the main calendar class.

  @author Preston Brown \<pbrown@kde.org\>
  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
  @author David Jarvie \<software@astrojar.org.uk\>
*/
#include "calendar.h"
#include "calfilter.h"
#include "icaltimezones.h"
#include "sorting.h"
#include "visitor.h"

#include <KDebug>

extern "C" {
  #include <icaltimezone.h>
}

#include <algorithm>  // for std::remove()

using namespace KCalCore;

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class KCalCore::Calendar::Private
{
  public:
    Private()
      : mTimeZones( new ICalTimeZones ),
        mModified( false ),
        mNewObserver( false ),
        mObserversEnabled( true ),
        mDefaultFilter( new CalFilter ),
        batchAddingInProgress( false )
    {
      // Setup default filter, which does nothing
      mFilter = mDefaultFilter;
      mFilter->setEnabled( false );

      mOwner = Person::Ptr( new Person() );
      mOwner->setName( "Unknown Name" );
      mOwner->setEmail( "unknown@nowhere" );
    }

    ~Private()
    {
      delete mTimeZones;
      mTimeZones = 0;
      if ( mFilter != mDefaultFilter ) {
        delete mFilter;
      }
      delete mDefaultFilter;
    }
    KDateTime::Spec timeZoneIdSpec( const QString &timeZoneId, bool view );

    QString mProductId;
    Person::Ptr mOwner;
    ICalTimeZones *mTimeZones; // collection of time zones used in this calendar
    ICalTimeZone mBuiltInTimeZone;   // cached time zone lookup
    ICalTimeZone mBuiltInViewTimeZone;   // cached viewing time zone lookup
    KDateTime::Spec mTimeSpec;
    mutable KDateTime::Spec mViewTimeSpec;
    bool mModified;
    bool mNewObserver;
    bool mObserversEnabled;
    QList<CalendarObserver*> mObservers;

    CalFilter *mDefaultFilter;
    CalFilter *mFilter;

    // These lists are used to put together related To-dos
    QMultiHash<QString, Incidence::Ptr> mOrphans;
    QMultiHash<QString, Incidence::Ptr> mOrphanUids;

    // Lists for associating incidences to notebooks
    QMultiHash<QString, Incidence::Ptr >mNotebookIncidences;
    QHash<QString, QString>mUidToNotebook;
    QHash<QString, bool>mNotebooks; // name to visibility
    QHash<Incidence::Ptr, bool>mIncidenceVisibility; // incidence -> visibility
    QString mDefaultNotebook; // uid of default notebook
    QMap<QString, Incidence::List > mIncidenceRelations;
    bool batchAddingInProgress;

};

/**
  Make a QHash::value that returns a QVector.
*/
template <typename K, typename V>
QVector<V> values( const QMultiHash<K,V> &c )
{
  QVector<V> v;
  v.reserve( c.size() );
  for ( typename QMultiHash<K,V>::const_iterator it = c.begin(), end = c.end(); it != end; ++it ) {
    v.push_back( it.value() );
  }
  return v;
}

template <typename K, typename V>
QVector<V> values( const QMultiHash<K,V> &c, const K &x )
{
  QVector<V> v;
  typename QMultiHash<K,V>::const_iterator it = c.find( x );
  while ( it != c.end() && it.key() == x ) {
    v.push_back( it.value() );
    ++it;
  }
  return v;
}

/**
  Template for a class that implements a visitor for adding an Incidence
  to a resource supporting addEvent(), addTodo() and addJournal() calls.
*/
template<class T>
class AddVisitor : public Visitor
{
  public:
    AddVisitor( T *r ) : mResource( r ) {}

    bool visit( Event::Ptr e )
    {
      return mResource->addEvent( e );
    }
    bool visit( Todo::Ptr t )
    {
      return mResource->addTodo( t );
    }
    bool visit( Journal::Ptr j )
    {
      return mResource->addJournal( j );
    }
    bool visit( FreeBusy::Ptr )
    {
      return false;
    }

  private:
    T *mResource;
};

/**
  Template for a class that implements a visitor for deleting an Incidence
  from a resource supporting deleteEvent(), deleteTodo() and deleteJournal()
  calls.
*/
template<class T>
class DeleteVisitor : public Visitor
{
  public:
    DeleteVisitor( T *r ) : mResource( r ) {}

    bool visit( Event::Ptr e )
    {
      mResource->deleteEvent( e );
      return true;
    }
    bool visit( Todo::Ptr t )
    {
      mResource->deleteTodo( t );
      return true;
    }
    bool visit( Journal::Ptr j )
    {
      mResource->deleteJournal( j );
      return true;
    }
    bool visit( FreeBusy::Ptr )
    {
      return false;
    }

  private:
    T *mResource;
};
//@endcond

Calendar::Calendar( const KDateTime::Spec &timeSpec )
  : d( new KCalCore::Calendar::Private )
{
  d->mTimeSpec = timeSpec;
  d->mViewTimeSpec = timeSpec;
}

Calendar::Calendar( const QString &timeZoneId )
  : d( new KCalCore::Calendar::Private )
{
  setTimeZoneId( timeZoneId );
}

Calendar::~Calendar()
{
  delete d;
}

Person::Ptr Calendar::owner() const
{
  return d->mOwner;
}

void Calendar::setOwner( const Person::Ptr &owner )
{
  Q_ASSERT( owner );
  d->mOwner = owner;
  setModified( true );
}

void Calendar::setTimeSpec( const KDateTime::Spec &timeSpec )
{
  d->mTimeSpec = timeSpec;
  d->mBuiltInTimeZone = ICalTimeZone();
  setViewTimeSpec( timeSpec );

  doSetTimeSpec( d->mTimeSpec );
}

KDateTime::Spec Calendar::timeSpec() const
{
  return d->mTimeSpec;
}

void Calendar::setTimeZoneId( const QString &timeZoneId )
{
  d->mTimeSpec = d->timeZoneIdSpec( timeZoneId, false );
  d->mViewTimeSpec = d->mTimeSpec;
  d->mBuiltInViewTimeZone = d->mBuiltInTimeZone;

  doSetTimeSpec( d->mTimeSpec );
}

//@cond PRIVATE
KDateTime::Spec Calendar::Private::timeZoneIdSpec( const QString &timeZoneId,
                                                   bool view )
{
  if ( view ) {
    mBuiltInViewTimeZone = ICalTimeZone();
  } else {
    mBuiltInTimeZone = ICalTimeZone();
  }
  if ( timeZoneId == QLatin1String( "UTC" ) ) {
    return KDateTime::UTC;
  }
  ICalTimeZone tz = mTimeZones->zone( timeZoneId );
  if ( !tz.isValid() ) {
    ICalTimeZoneSource tzsrc;
    tz = tzsrc.parse( icaltimezone_get_builtin_timezone( timeZoneId.toLatin1() ) );
    if ( view ) {
      mBuiltInViewTimeZone = tz;
    } else {
      mBuiltInTimeZone = tz;
    }
  }
  if ( tz.isValid() ) {
    return tz;
  } else {
    return KDateTime::ClockTime;
  }
}
//@endcond

QString Calendar::timeZoneId() const
{
  KTimeZone tz = d->mTimeSpec.timeZone();
  return tz.isValid() ? tz.name() : QString();
}

void Calendar::setViewTimeSpec( const KDateTime::Spec &timeSpec ) const
{
  d->mViewTimeSpec = timeSpec;
  d->mBuiltInViewTimeZone = ICalTimeZone();
}

void Calendar::setViewTimeZoneId( const QString &timeZoneId ) const
{
  d->mViewTimeSpec = d->timeZoneIdSpec( timeZoneId, true );
}

KDateTime::Spec Calendar::viewTimeSpec() const
{
  return d->mViewTimeSpec;
}

QString Calendar::viewTimeZoneId() const
{
  KTimeZone tz = d->mViewTimeSpec.timeZone();
  return tz.isValid() ? tz.name() : QString();
}

ICalTimeZones *Calendar::timeZones() const
{
  return d->mTimeZones;
}

void Calendar::setTimeZones( ICalTimeZones *zones )
{
  if ( !zones ) {
    return;
  }

  if ( d->mTimeZones && ( d->mTimeZones != zones ) ) {
    delete d->mTimeZones;
    d->mTimeZones = 0;
  }
  d->mTimeZones = zones;
}

void Calendar::shiftTimes( const KDateTime::Spec &oldSpec, const KDateTime::Spec &newSpec )
{
  setTimeSpec( newSpec );

  int i, end;
  Event::List ev = events();
  for ( i = 0, end = ev.count();  i < end;  ++i ) {
    ev[i]->shiftTimes( oldSpec, newSpec );
  }

  Todo::List to = todos();
  for ( i = 0, end = to.count();  i < end;  ++i ) {
    to[i]->shiftTimes( oldSpec, newSpec );
  }

  Journal::List jo = journals();
  for ( i = 0, end = jo.count();  i < end;  ++i ) {
    jo[i]->shiftTimes( oldSpec, newSpec );
  }
}

void Calendar::setFilter( CalFilter *filter )
{
  if ( filter ) {
    d->mFilter = filter;
  } else {
    d->mFilter = d->mDefaultFilter;
  }
  emit filterChanged();
}

CalFilter *Calendar::filter() const
{
  return d->mFilter;
}

QStringList Calendar::categories() const
{
  Incidence::List rawInc( rawIncidences() );
  QStringList cats, thisCats;
  // @TODO: For now just iterate over all incidences. In the future,
  // the list of categories should be built when reading the file.
  for ( Incidence::List::ConstIterator i = rawInc.constBegin();
        i != rawInc.constEnd(); ++i ) {
    thisCats = ( *i )->categories();
    for ( QStringList::ConstIterator si = thisCats.constBegin();
          si != thisCats.constEnd(); ++si ) {
      if ( !cats.contains( *si ) ) {
        cats.append( *si );
      }
    }
  }
  return cats;
}

Incidence::List Calendar::incidences( const QDate &date ) const
{
  return mergeIncidenceList( events( date ), todos( date ), journals( date ) );
}

Incidence::List Calendar::incidences() const
{
  return mergeIncidenceList( events(), todos(), journals() );
}

Incidence::List Calendar::rawIncidences() const
{
  return mergeIncidenceList( rawEvents(), rawTodos(), rawJournals() );
}

Incidence::List Calendar::instances( const Incidence::Ptr &incidence ) const
{
  if ( incidence ) {
    Event::List elist;
    Todo::List tlist;
    Journal::List jlist;

    if ( incidence->type() == Incidence::TypeEvent ) {
      elist = eventInstances( incidence );
    } else if ( incidence->type() == Incidence::TypeTodo ) {
      tlist = todoInstances( incidence );
    } else if ( incidence->type() == Incidence::TypeJournal ) {
      jlist = journalInstances( incidence );
    }
    return mergeIncidenceList( elist, tlist, jlist );
  } else {
    return Incidence::List();
  }
}

Incidence::List Calendar::duplicates( const Incidence::Ptr &incidence )
{
  if ( incidence ) {
    Incidence::List list;
    Incidence::List vals = values( d->mNotebookIncidences );
    Incidence::List::const_iterator it;
    for ( it = vals.constBegin(); it != vals.constEnd(); ++it ) {
      if ( ( ( incidence->dtStart() == ( *it )->dtStart() ) ||
             ( !incidence->dtStart().isValid() && !( *it )->dtStart().isValid() ) ) &&
           ( incidence->summary() == ( *it )->summary() ) ) {
        list.append( *it );
      }
    }
    return list;
  } else {
    return Incidence::List();
  }
}

bool Calendar::addNotebook( const QString &notebook, bool isVisible )
{
  if ( d->mNotebooks.contains( notebook ) ) {
    return false;
  } else {
    d->mNotebooks.insert( notebook, isVisible );
    return true;
  }
}

bool Calendar::updateNotebook( const QString &notebook, bool isVisible )
{
  if ( !d->mNotebooks.contains( notebook ) ) {
    return false;
  } else {
    d->mNotebooks.insert( notebook, isVisible );
    return true;
  }
}

bool Calendar::deleteNotebook( const QString &notebook )
{
  if ( !d->mNotebooks.contains( notebook ) ) {
    return false;
  } else {
    return d->mNotebooks.remove( notebook );
  }
}

bool Calendar::setDefaultNotebook( const QString &notebook )
{
  if ( !d->mNotebooks.contains( notebook ) ) {
    return false;
  } else {
    d->mDefaultNotebook = notebook;
    return true;
  }
}

QString Calendar::defaultNotebook() const
{
  return d->mDefaultNotebook;
}

bool Calendar::hasValidNotebook( const QString &notebook ) const
{
  return d->mNotebooks.contains( notebook );
}

bool Calendar::isVisible( const Incidence::Ptr &incidence ) const
{
  if ( d->mIncidenceVisibility.contains( incidence ) ) {
    return d->mIncidenceVisibility[incidence];
  }
  const QString nuid = notebook( incidence );
  bool rv;
  if ( d->mNotebooks.contains( nuid ) ) {
    rv = d->mNotebooks.value( nuid );
  } else {
    // NOTE returns true also for nonexisting notebooks for compatibility
    rv = true;
  }
  d->mIncidenceVisibility[incidence] = rv;
  return rv;
}

void Calendar::clearNotebookAssociations()
{
  d->mNotebookIncidences.clear();
  d->mUidToNotebook.clear();
  d->mIncidenceVisibility.clear();
}

bool Calendar::setNotebook( const Incidence::Ptr &inc, const QString &notebook )
{
  if ( !inc ) {
    return false;
  }

  if ( !notebook.isEmpty() &&
       !incidence( inc->uid(), inc->recurrenceId() ) ) {
    kWarning() << "cannot set notebook until incidence has been added";
    return false;
  }

  if ( d->mUidToNotebook.contains( inc->uid() ) ) {
    QString old = d->mUidToNotebook.value( inc->uid() );
    if ( !old.isEmpty() && notebook != old ) {
      if ( inc->hasRecurrenceId() ) {
        kWarning() << "cannot set notebook for child incidences";
        return false;
      }
      // Move all possible children also.
      Incidence::List list = instances( inc );
      Incidence::List::Iterator it;
      for ( it = list.begin(); it != list.end(); ++it ) {
        d->mNotebookIncidences.remove( old, *it );
        d->mNotebookIncidences.insert( notebook, *it );
      }
      notifyIncidenceChanged( inc ); // for removing from old notebook
      // don not remove from mUidToNotebook to keep deleted incidences
      d->mNotebookIncidences.remove( old, inc );
    }
  }
  if ( !notebook.isEmpty() ) {
    d->mUidToNotebook.insert( inc->uid(), notebook );
    d->mNotebookIncidences.insert( notebook, inc );
    kDebug() << "setting notebook" << notebook << "for" << inc->uid();
    notifyIncidenceChanged( inc ); // for inserting into new notebook
  }

  return true;
}

QString Calendar::notebook( const Incidence::Ptr &incidence ) const
{
  if ( incidence ) {
    return d->mUidToNotebook.value( incidence->uid() );
  } else {
    return QString();
  }
}

QString Calendar::notebook( const QString &uid ) const
{
  return d->mUidToNotebook.value( uid );
}

QStringList Calendar::notebooks() const
{
  return d->mNotebookIncidences.uniqueKeys();
}

Incidence::List Calendar::incidences( const QString &notebook ) const
{
  if ( notebook.isEmpty() ) {
    return values( d->mNotebookIncidences );
  } else {
    return values( d->mNotebookIncidences, notebook );
  }
}

/** static */
Event::List Calendar::sortEvents( const Event::List &eventList,
                                  EventSortField sortField,
                                  SortDirection sortDirection )
{

  if ( eventList.isEmpty() ) {
    return Event::List();
  }

  Event::List eventListSorted;

  // Notice we alphabetically presort Summaries first.
  // We do this so comparison "ties" stay in a nice order.
  eventListSorted = eventList;
  switch ( sortField ) {
  case EventSortUnsorted:
    break;

  case EventSortStartDate:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( eventListSorted.begin(), eventListSorted.end(), Events::startDateLessThan );
    } else {
      qSort( eventListSorted.begin(), eventListSorted.end(), Events::startDateMoreThan );
    }
    break;

  case EventSortEndDate:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( eventListSorted.begin(), eventListSorted.end(), Events::endDateLessThan );
    } else {
      qSort( eventListSorted.begin(), eventListSorted.end(), Events::endDateMoreThan );
    }
    break;

  case EventSortSummary:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( eventListSorted.begin(), eventListSorted.end(), Events::summaryLessThan );
    } else {
      qSort( eventListSorted.begin(), eventListSorted.end(), Events::summaryMoreThan );
    }
    break;
  }

  return eventListSorted;

}

Event::List Calendar::events( const QDate &date,
                              const KDateTime::Spec &timeSpec,
                              EventSortField sortField,
                              SortDirection sortDirection ) const
{
  Event::List el = rawEventsForDate( date, timeSpec, sortField, sortDirection );
  d->mFilter->apply( &el );
  return el;
}

Event::List Calendar::events( const KDateTime &dt ) const
{
  Event::List el = rawEventsForDate( dt );
  d->mFilter->apply( &el );
  return el;
}

Event::List Calendar::events( const QDate &start, const QDate &end,
                              const KDateTime::Spec &timeSpec,
                              bool inclusive ) const
{
  Event::List el = rawEvents( start, end, timeSpec, inclusive );
  d->mFilter->apply( &el );
  return el;
}

Event::List Calendar::events( EventSortField sortField,
                              SortDirection sortDirection ) const
{
  Event::List el = rawEvents( sortField, sortDirection );
  d->mFilter->apply( &el );
  return el;
}

bool Calendar::addIncidence( const Incidence::Ptr &incidence )
{
  if ( !incidence ) {
    return false;
  }

  AddVisitor<Calendar> v( this );
  return incidence->accept( v, incidence );
}

bool Calendar::deleteIncidence( const Incidence::Ptr &incidence )
{
  if ( !incidence ) {
    return false;
  }

  if ( beginChange( incidence ) ) {
    DeleteVisitor<Calendar> v( this );
    const bool result = incidence->accept( v, incidence );
    endChange( incidence );
    return result;
  } else {
    return false;
  }
}

Incidence::Ptr Calendar::createException( const Incidence::Ptr &incidence,
                                          const KDateTime &recurrenceId,
                                          bool thisAndFuture )
{
  Q_UNUSED(thisAndFuture);
  if ( !incidence || !incidence->recurs() ) {
    return Incidence::Ptr();
  }

  Incidence::Ptr newInc( incidence->clone() );
  newInc->setCreated( KDateTime::currentUtcDateTime() );
  newInc->setRevision(0);
  //Recurring exceptions are not support for now
  newInc->clearRecurrence();

  //FIXME thisAndFuture
  newInc->setRecurrenceId( recurrenceId );
  newInc->setDtStart(recurrenceId);

  // Calculate and set the new end of the incidence
  KDateTime end = incidence->dateTime( IncidenceBase::RoleEnd );

  if ( end.isValid() ) {
    if ( incidence->dtStart().isDateOnly() ) {
      int offset = incidence->dtStart().daysTo( recurrenceId );
      end = end.addDays( offset );
    } else {
      qint64 offset = incidence->dtStart().secsTo_long( recurrenceId );
      end = end.addSecs( offset );
    }
    newInc->setDateTime( end, IncidenceBase::RoleEnd );
  }
  return newInc;
}

// Dissociate a single occurrence or all future occurrences from a recurring
// sequence. The new incidence is returned, but not automatically inserted
// into the calendar, which is left to the calling application.
Incidence::Ptr Calendar::dissociateOccurrence( const Incidence::Ptr &incidence,
                                               const QDate &date,
                                               const KDateTime::Spec &spec,
                                               bool single )
{
  if ( !incidence || !incidence->recurs() ) {
    return Incidence::Ptr();
  }

  Incidence::Ptr newInc( incidence->clone() );
  newInc->recreate();
  // Do not call setRelatedTo() when dissociating recurring to-dos, otherwise the new to-do
  // will appear as a child.  Originally, we planned to set a relation with reltype SIBLING
  // when dissociating to-dos, but currently kcalcore only supports reltype PARENT.
  // We can uncomment the following line when we support the PARENT reltype.
  //newInc->setRelatedTo( incidence );
  Recurrence *recur = newInc->recurrence();
  if ( single ) {
    recur->clear();
  } else {
    // Adjust the recurrence for the future incidences. In particular adjust
    // the "end after n occurrences" rules! "No end date" and "end by ..."
    // don't need to be modified.
    int duration = recur->duration();
    if ( duration > 0 ) {
      int doneduration = recur->durationTo( date.addDays( -1 ) );
      if ( doneduration >= duration ) {
        kDebug() << "The dissociated event already occurred more often"
                 << "than it was supposed to ever occur. ERROR!";
        recur->clear();
      } else {
        recur->setDuration( duration - doneduration );
      }
    }
  }
  // Adjust the date of the incidence
  if ( incidence->type() == Incidence::TypeEvent ) {
    Event::Ptr ev = newInc.staticCast<Event>();
    KDateTime start( ev->dtStart() );
    int daysTo = start.toTimeSpec( spec ).date().daysTo( date );
    ev->setDtStart( start.addDays( daysTo ) );
    ev->setDtEnd( ev->dtEnd().addDays( daysTo ) );
  } else if ( incidence->type() == Incidence::TypeTodo ) {
    Todo::Ptr td = newInc.staticCast<Todo>();
    bool haveOffset = false;
    int daysTo = 0;
    if ( td->hasDueDate() ) {
      KDateTime due( td->dtDue() );
      daysTo = due.toTimeSpec( spec ).date().daysTo( date );
      td->setDtDue( due.addDays( daysTo ), true );
      haveOffset = true;
    }
    if ( td->hasStartDate() ) {
      KDateTime start( td->dtStart() );
      if ( !haveOffset ) {
        daysTo = start.toTimeSpec( spec ).date().daysTo( date );
      }
      td->setDtStart( start.addDays( daysTo ) );
      haveOffset = true;
    }
  }
  recur = incidence->recurrence();
  if ( recur ) {
    if ( single ) {
      recur->addExDate( date );
    } else {
      // Make sure the recurrence of the past events ends
      // at the corresponding day
      recur->setEndDate( date.addDays(-1) );
    }
  }
  return newInc;
}

Incidence::Ptr Calendar::incidence( const QString &uid,
                                    const KDateTime &recurrenceId ) const
{
  Incidence::Ptr i = event( uid, recurrenceId );
  if ( i ) {
    return i;
  }

  i = todo( uid, recurrenceId );
  if ( i ) {
    return i;
  }

  i = journal( uid, recurrenceId );
  return i;
}

Incidence::Ptr Calendar::deleted( const QString &uid, const KDateTime &recurrenceId ) const
{
  Incidence::Ptr i = deletedEvent( uid, recurrenceId );
  if ( i ) {
    return i;
  }

  i = deletedTodo( uid, recurrenceId );
  if ( i ) {
    return i;
  }

  i = deletedJournal( uid, recurrenceId );
  return i;
}

Incidence::List Calendar::incidencesFromSchedulingID( const QString &sid ) const
{
  Incidence::List result;
  const Incidence::List incidences = rawIncidences();
  Incidence::List::const_iterator it = incidences.begin();
  for ( ; it != incidences.end(); ++it ) {
    if ( (*it)->schedulingID() == sid ) {
      result.append( *it );
    }
  }
  return result;
}

Incidence::Ptr Calendar::incidenceFromSchedulingID( const QString &uid ) const
{
  const Incidence::List incidences = rawIncidences();
  Incidence::List::const_iterator it = incidences.begin();
  for ( ; it != incidences.end(); ++it ) {
    if ( (*it)->schedulingID() == uid ) {
      // Touchdown, and the crowd goes wild
      return *it;
    }
  }
  // Not found
  return Incidence::Ptr();
}

/** static */
Todo::List Calendar::sortTodos( const Todo::List &todoList,
                                TodoSortField sortField,
                                SortDirection sortDirection )
{
  if ( todoList.isEmpty() ) {
    return Todo::List();
  }

  Todo::List todoListSorted;

  // Notice we alphabetically presort Summaries first.
  // We do this so comparison "ties" stay in a nice order.

  // Note that To-dos may not have Start DateTimes nor due DateTimes.

  todoListSorted = todoList;
  switch ( sortField ) {
  case TodoSortUnsorted:
    break;

  case TodoSortStartDate:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::startDateLessThan );
    } else {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::startDateMoreThan );
    }
    break;

  case TodoSortDueDate:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::dueDateLessThan );
    } else {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::dueDateMoreThan );
    }
    break;

  case TodoSortPriority:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::priorityLessThan );
    } else {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::priorityMoreThan );
    }
    break;

  case TodoSortPercentComplete:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::percentLessThan );
    } else {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::percentMoreThan );
    }
    break;

  case TodoSortSummary:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::summaryLessThan );
    } else {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::summaryMoreThan );
    }
    break;

  case TodoSortCreated:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::createdLessThan );
    } else {
      qSort( todoListSorted.begin(), todoListSorted.end(), Todos::createdMoreThan );
    }
    break;
  }

  return todoListSorted;
}

Todo::List Calendar::todos( TodoSortField sortField,
                            SortDirection sortDirection ) const
{
  Todo::List tl = rawTodos( sortField, sortDirection );
  d->mFilter->apply( &tl );
  return tl;
}

Todo::List Calendar::todos( const QDate &date ) const
{
  Todo::List el = rawTodosForDate( date );
  d->mFilter->apply( &el );
  return el;
}

Todo::List Calendar::todos( const QDate &start, const QDate &end,
                            const KDateTime::Spec &timespec, bool inclusive ) const
{
  Todo::List tl = rawTodos( start, end, timespec, inclusive );
  d->mFilter->apply( &tl );
  return tl;
}

/** static */
Journal::List Calendar::sortJournals( const Journal::List &journalList,
                                      JournalSortField sortField,
                                      SortDirection sortDirection )
{
  if ( journalList.isEmpty() ) {
    return Journal::List();
  }

  Journal::List journalListSorted = journalList;

  switch ( sortField ) {
  case JournalSortUnsorted:
    break;

  case JournalSortDate:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( journalListSorted.begin(), journalListSorted.end(), Journals::dateLessThan );
    } else {
      qSort( journalListSorted.begin(), journalListSorted.end(), Journals::dateMoreThan );
    }
    break;

  case JournalSortSummary:
    if ( sortDirection == SortDirectionAscending ) {
      qSort( journalListSorted.begin(), journalListSorted.end(), Journals::summaryLessThan );
    } else {
      qSort( journalListSorted.begin(), journalListSorted.end(), Journals::summaryMoreThan );
    }
    break;
  }

  return journalListSorted;
}

Journal::List Calendar::journals( JournalSortField sortField,
                                  SortDirection sortDirection ) const
{
  Journal::List jl = rawJournals( sortField, sortDirection );
  d->mFilter->apply( &jl );
  return jl;
}

Journal::List Calendar::journals( const QDate &date ) const
{
  Journal::List el = rawJournalsForDate( date );
  d->mFilter->apply( &el );
  return el;
}

// When this is called, the to-dos have already been added to the calendar.
// This method is only about linking related to-dos.
void Calendar::setupRelations( const Incidence::Ptr &forincidence )
{
  if ( !forincidence ) {
    return;
  }

  const QString uid = forincidence->uid();

  // First, go over the list of orphans and see if this is their parent
  Incidence::List l = values( d->mOrphans, uid );
  d->mOrphans.remove( uid );
  for ( int i = 0, end = l.count();  i < end;  ++i ) {
    d->mIncidenceRelations[uid].append( l[i] );
    d->mOrphanUids.remove( l[i]->uid() );
  }

  // Now see about this incidences parent
  if ( forincidence->relatedTo().isEmpty() && !forincidence->relatedTo().isEmpty() ) {
    // Incidence has a uid it is related to but is not registered to it yet.
    // Try to find it
    Incidence::Ptr parent = incidence( forincidence->relatedTo() );
    if ( parent ) {
      // Found it

      // look for hierarchy loops
      if ( isAncestorOf( forincidence, parent ) ) {
        forincidence->setRelatedTo( QString() );
        kWarning() << "hierarchy loop beetween " << forincidence->uid() << " and " << parent->uid();
      } else {
        d->mIncidenceRelations[parent->uid()].append( forincidence );
      }
    } else {
      // Not found, put this in the mOrphans list
      // Note that the mOrphans dict might contain multiple entries with the
      // same key! which are multiple children that wait for the parent
      // incidence to be inserted.
      d->mOrphans.insert( forincidence->relatedTo(), forincidence );
      d->mOrphanUids.insert( forincidence->uid(), forincidence );
    }
  }
}

// If a to-do with sub-to-dos is deleted, move it's sub-to-dos to the orphan list
void Calendar::removeRelations( const Incidence::Ptr &incidence )
{
  if ( !incidence ) {
    kDebug() << "Warning: incidence is 0";
    return;
  }

  const QString uid = incidence->uid();

  foreach ( Incidence::Ptr i, d->mIncidenceRelations[uid] ) {
    if ( !d->mOrphanUids.contains( i->uid() ) ) {
      d->mOrphans.insert( uid, i );
      d->mOrphanUids.insert( i->uid(), i );
      i->setRelatedTo( uid );
    }
  }

  const QString parentUid = incidence->relatedTo();

  // If this incidence is related to something else, tell that about it
  if ( !parentUid.isEmpty() ) {
    d->mIncidenceRelations[parentUid].erase(
      std::remove( d->mIncidenceRelations[parentUid].begin(),
                   d->mIncidenceRelations[parentUid].end(), incidence ),
      d->mIncidenceRelations[parentUid].end() );
  }

  // Remove this one from the orphans list
  if ( d->mOrphanUids.remove( uid ) ) {
    // This incidence is located in the orphans list - it should be removed
    // Since the mOrphans dict might contain the same key (with different
    // child incidence pointers!) multiple times, take care that we remove
    // the correct one. So we need to remove all items with the given
    // parent UID, and readd those that are not for this item. Also, there
    // might be other entries with differnet UID that point to this
    // incidence (this might happen when the relatedTo of the item is
    // changed before its parent is inserted. This might happen with
    // groupware servers....). Remove them, too
    QStringList relatedToUids;

    // First, create a list of all keys in the mOrphans list which point
    // to the removed item
    relatedToUids << incidence->relatedTo();
    for ( QMultiHash<QString, Incidence::Ptr>::Iterator it = d->mOrphans.begin();
          it != d->mOrphans.end(); ++it ) {
      if ( it.value()->uid() == uid ) {
        relatedToUids << it.key();
      }
    }

    // now go through all uids that have one entry that point to the incidence
    for ( QStringList::const_iterator uidit = relatedToUids.constBegin();
          uidit != relatedToUids.constEnd(); ++uidit ) {
      Incidence::List tempList;
      // Remove all to get access to the remaining entries
      QList<Incidence::Ptr> l = d->mOrphans.values( *uidit );
      d->mOrphans.remove( *uidit );
      foreach ( Incidence::Ptr i, l ) {
        if ( i != incidence ) {
          tempList.append( i );
        }
      }
      // Readd those that point to a different orphan incidence
      for ( Incidence::List::Iterator incit = tempList.begin();
            incit != tempList.end(); ++incit ) {
        d->mOrphans.insert( *uidit, *incit );
      }
    }
  }

  // Make sure the deleted incidence doesn't relate to a non-deleted incidence,
  // since that would cause trouble in MemoryCalendar::close(), as the deleted
  // incidences are destroyed after the non-deleted incidences. The destructor
  // of the deleted incidences would then try to access the already destroyed
  // non-deleted incidence, which would segfault.
  //
  // So in short: Make sure dead incidences don't point to alive incidences
  // via the relation.
  //
  // This crash is tested in MemoryCalendarTest::testRelationsCrash().
//  incidence->setRelatedTo( Incidence::Ptr() );
}

bool Calendar::isAncestorOf( const Incidence::Ptr &ancestor,
                             const Incidence::Ptr &incidence ) const
{
  if ( !incidence || incidence->relatedTo().isEmpty() ) {
    return false;
  } else if ( incidence->relatedTo() == ancestor->uid() ) {
    return true;
  } else {
    return isAncestorOf( ancestor, this->incidence( incidence->relatedTo() ) );
  }
}

Incidence::List Calendar::relations( const QString &uid ) const
{
  return d->mIncidenceRelations[uid];
}

Calendar::CalendarObserver::~CalendarObserver()
{
}

void Calendar::CalendarObserver::calendarModified( bool modified, Calendar *calendar )
{
  Q_UNUSED( modified );
  Q_UNUSED( calendar );
}

void Calendar::CalendarObserver::calendarIncidenceAdded( const Incidence::Ptr &incidence )
{
  Q_UNUSED( incidence );
}

void Calendar::CalendarObserver::calendarIncidenceChanged( const Incidence::Ptr &incidence )
{
  Q_UNUSED( incidence );
}

void Calendar::CalendarObserver::calendarIncidenceDeleted( const Incidence::Ptr &incidence )
{
  Q_UNUSED( incidence );
}

void
Calendar::CalendarObserver::calendarIncidenceAdditionCanceled( const Incidence::Ptr &incidence )
{
  Q_UNUSED( incidence );
}

void Calendar::registerObserver( CalendarObserver *observer )
{
  if ( !observer ) {
    return;
  }

  if ( !d->mObservers.contains( observer ) ) {
    d->mObservers.append( observer );
  } else {
    d->mNewObserver = true;
  }
}

void Calendar::unregisterObserver( CalendarObserver *observer )
{
  if ( !observer ) {
    return;
  } else {
    d->mObservers.removeAll( observer );
  }
}

bool Calendar::isSaving() const
{
  return false;
}

void Calendar::setModified( bool modified )
{
  if ( modified != d->mModified || d->mNewObserver ) {
    d->mNewObserver = false;
    foreach ( CalendarObserver *observer, d->mObservers ) {
      observer->calendarModified( modified, this );
    }
    d->mModified = modified;
  }
}

bool Calendar::isModified() const
{
  return d->mModified;
}

bool Calendar::save()
{
  return true;
}

bool Calendar::reload()
{
  return true;
}

void Calendar::incidenceUpdated( const QString &uid, const KDateTime &recurrenceId )
{

  Incidence::Ptr inc = incidence( uid, recurrenceId );

  if ( !inc ) {
    return;
  }

  inc->setLastModified( KDateTime::currentUtcDateTime() );
  // we should probably update the revision number here,
  // or internally in the Event itself when certain things change.
  // need to verify with ical documentation.

  notifyIncidenceChanged( inc );

  setModified( true );
}

void Calendar::doSetTimeSpec( const KDateTime::Spec &timeSpec )
{
  Q_UNUSED( timeSpec );
}

void Calendar::notifyIncidenceAdded( const Incidence::Ptr &incidence )
{
  if ( !incidence ) {
    return;
  }

  if ( !d->mObserversEnabled ) {
    return;
  }

  foreach ( CalendarObserver *observer, d->mObservers ) {
    observer->calendarIncidenceAdded( incidence );
  }
}

void Calendar::notifyIncidenceChanged( const Incidence::Ptr &incidence )
{
  if ( !incidence ) {
    return;
  }

  if ( !d->mObserversEnabled ) {
    return;
  }

  foreach ( CalendarObserver *observer, d->mObservers ) {
    observer->calendarIncidenceChanged( incidence );
  }
}

void Calendar::notifyIncidenceDeleted( const Incidence::Ptr &incidence )
{
  if ( !incidence ) {
    return;
  }

  if ( !d->mObserversEnabled ) {
    return;
  }

  foreach ( CalendarObserver *observer, d->mObservers ) {
    observer->calendarIncidenceDeleted( incidence );
  }
}

void Calendar::notifyIncidenceAdditionCanceled( const Incidence::Ptr &incidence )
{
  if ( !incidence ) {
    return;
  }

  if ( !d->mObserversEnabled ) {
    return;
  }

  foreach ( CalendarObserver *observer, d->mObservers ) {
    observer->calendarIncidenceAdditionCanceled( incidence );
  }
}

void Calendar::customPropertyUpdated()
{
  setModified( true );
}

void Calendar::setProductId( const QString &id )
{
  d->mProductId = id;
}

QString Calendar::productId() const
{
  return d->mProductId;
}

/** static */
Incidence::List Calendar::mergeIncidenceList( const Event::List &events,
                                              const Todo::List &todos,
                                              const Journal::List &journals )
{
  Incidence::List incidences;

  int i, end;
  for ( i = 0, end = events.count();  i < end;  ++i ) {
    incidences.append( events[i] );
  }

  for ( i = 0, end = todos.count();  i < end;  ++i ) {
    incidences.append( todos[i] );
  }

  for ( i = 0, end = journals.count();  i < end;  ++i ) {
    incidences.append( journals[i] );
  }

  return incidences;
}

bool Calendar::beginChange( const Incidence::Ptr &incidence )
{
  Q_UNUSED( incidence );
  return true;
}

bool Calendar::endChange( const Incidence::Ptr &incidence )
{
  Q_UNUSED( incidence );
  return true;
}

void Calendar::setObserversEnabled( bool enabled )
{
  d->mObserversEnabled = enabled;
}

void Calendar::appendAlarms( Alarm::List &alarms, const Incidence::Ptr &incidence,
                             const KDateTime &from, const KDateTime &to ) const
{
  KDateTime preTime = from.addSecs( -1 );

  Alarm::List alarmlist = incidence->alarms();
  for ( int i = 0, iend = alarmlist.count();  i < iend;  ++i ) {
    if ( alarmlist[i]->enabled() ) {
      KDateTime dt = alarmlist[i]->nextRepetition( preTime );
      if ( dt.isValid() && dt <= to ) {
        kDebug() << incidence->summary() << "':" << dt.toString();
        alarms.append( alarmlist[i] );
      }
    }
  }
}

void Calendar::appendRecurringAlarms( Alarm::List &alarms,
                                      const Incidence::Ptr &incidence,
                                      const KDateTime &from,
                                      const KDateTime &to ) const
{
  KDateTime dt;
  bool endOffsetValid = false;
  Duration endOffset( 0 );
  Duration period( from, to );

  Alarm::List alarmlist = incidence->alarms();
  for ( int i = 0, iend = alarmlist.count();  i < iend;  ++i ) {
    Alarm::Ptr a = alarmlist[i];
    if ( a->enabled() ) {
      if ( a->hasTime() ) {
        // The alarm time is defined as an absolute date/time
        dt = a->nextRepetition( from.addSecs( -1 ) );
        if ( !dt.isValid() || dt > to ) {
          continue;
        }
      } else {
        // Alarm time is defined by an offset from the event start or end time.
        // Find the offset from the event start time, which is also used as the
        // offset from the recurrence time.
        Duration offset( 0 );
        if ( a->hasStartOffset() ) {
          offset = a->startOffset();
        } else if ( a->hasEndOffset() ) {
          offset = a->endOffset();
          if ( !endOffsetValid ) {
            endOffset = Duration( incidence->dtStart(),
                                  incidence->dateTime( Incidence::RoleAlarmEndOffset ) );
            endOffsetValid = true;
          }
        }

        // Find the incidence's earliest alarm
        KDateTime alarmStart =
          offset.end( a->hasEndOffset() ? incidence->dateTime( Incidence::RoleAlarmEndOffset ) :
                                          incidence->dtStart() );
//        KDateTime alarmStart = incidence->dtStart().addSecs( offset );
        if ( alarmStart > to ) {
          continue;
        }
        KDateTime baseStart = incidence->dtStart();
        if ( from > alarmStart ) {
          alarmStart = from;   // don't look earlier than the earliest alarm
          baseStart = ( -offset ).end( ( -endOffset ).end( alarmStart ) );
        }

        // Adjust the 'alarmStart' date/time and find the next recurrence at or after it.
        // Treate the two offsets separately in case one is daily and the other not.
        dt = incidence->recurrence()->getNextDateTime( baseStart.addSecs( -1 ) );
        if ( !dt.isValid() ||
             ( dt = endOffset.end( offset.end( dt ) ) ) > to ) // adjust 'dt' to get the alarm time
        {
          // The next recurrence is too late.
          if ( !a->repeatCount() ) {
            continue;
          }

          // The alarm has repetitions, so check whether repetitions of previous
          // recurrences fall within the time period.
          bool found = false;
          Duration alarmDuration = a->duration();
          for ( KDateTime base = baseStart;
                ( dt = incidence->recurrence()->getPreviousDateTime( base ) ).isValid();
                base = dt ) {
            if ( a->duration().end( dt ) < base ) {
              break;  // this recurrence's last repetition is too early, so give up
            }

            // The last repetition of this recurrence is at or after 'alarmStart' time.
            // Check if a repetition occurs between 'alarmStart' and 'to'.
            int snooze = a->snoozeTime().value();   // in seconds or days
            if ( a->snoozeTime().isDaily() ) {
              Duration toFromDuration( dt, base );
              int toFrom = toFromDuration.asDays();
              if ( a->snoozeTime().end( from ) <= to ||
                   ( toFromDuration.isDaily() && toFrom % snooze == 0 ) ||
                   ( toFrom / snooze + 1 ) * snooze <= toFrom + period.asDays() ) {
                found = true;
#ifndef NDEBUG
                // for debug output
                dt = offset.end( dt ).addDays( ( ( toFrom - 1 ) / snooze + 1 ) * snooze );
#endif
                break;
              }
            } else {
              int toFrom = dt.secsTo( base );
              if ( period.asSeconds() >= snooze ||
                   toFrom % snooze == 0 ||
                   ( toFrom / snooze + 1 ) * snooze <= toFrom + period.asSeconds() )
              {
                found = true;
#ifndef NDEBUG
                // for debug output
                dt = offset.end( dt ).addSecs( ( ( toFrom - 1 ) / snooze + 1 ) * snooze );
#endif
                break;
              }
            }
          }
          if ( !found ) {
            continue;
          }
        }
      }
      kDebug() << incidence->summary() << "':" << dt.toString();
      alarms.append( a );
    }
  }
}

void Calendar::startBatchAdding()
{
  d->batchAddingInProgress = true;
}

void Calendar::endBatchAdding()
{
  d->batchAddingInProgress = false;
}

bool Calendar::batchAdding() const
{
  return d->batchAddingInProgress;
}

void Calendar::virtual_hook( int id, void *data )
{
  Q_UNUSED( id );
  Q_UNUSED( data );
  Q_ASSERT( false );
}

