/*
  This file is part of the kcalcore library.

  Copyright (c) 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2004 Reinhold Kainhofer <reinhold@kainhofer.com>

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
  defines the FreeBusy class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
*/

#ifndef KCALCORE_FREEBUSY_H
#define KCALCORE_FREEBUSY_H

#include "kcalcore_export.h"
#include "event.h"
#include "freebusyperiod.h"
#include "incidencebase.h"
#include "period.h"

#include <QtCore/QMetaType>

namespace KCalCore {

class FreeBusy;

/**
  @brief
  Provides information about the free/busy time of a calendar.

  A free/busy is a collection of Periods (@see Period).
*/
class KCALCORE_EXPORT FreeBusy : public IncidenceBase
{
  friend KCALCORE_EXPORT QDataStream &operator<<( QDataStream &s,
                                                  const KCalCore::FreeBusy::Ptr &freebusy );
  friend KCALCORE_EXPORT QDataStream &operator>>( QDataStream &s,
                                                  KCalCore::FreeBusy::Ptr &freebusy );

  public:

    /**
      A shared pointer to a FreeBusy object.
    */
    typedef QSharedPointer<FreeBusy> Ptr;

    /**
      List of FreeBusy objects.
    */
    typedef QVector<Ptr> List;

    /**
      Constructs an free/busy without any periods.
    */
    FreeBusy();

    /**
      Copy constructor.
      @param other is the free/busy to copy.
    */
    FreeBusy( const FreeBusy &other );

    /**
      Constructs a free/busy from a list of periods.
      @param busyPeriods is a list of periods.
    */
    explicit FreeBusy( const Period::List &busyPeriods );

    /**
      Constructs a free/busy from a list of periods.
      @param busyPeriods is a list of periods.
    */
    explicit FreeBusy( const FreeBusyPeriod::List &busyPeriods );

    /**
      Constructs a free/busy from a single period.

      @param start is the start datetime of the period.
      @param end is the end datetime of the period.
    */
    FreeBusy( const KDateTime &start, const KDateTime &end );

    /**
      Constructs a freebusy for a specified list of events given a single period.

      @param events list of events.
      @param start is the start datetime of the period.
      @param end is the end datetime of the period.
    */
    FreeBusy( const Event::List &events, const KDateTime &start, const KDateTime &end );

    /**
      Destroys a free/busy.
    */
    ~FreeBusy();

    /**
      @copydoc
      IncidenceBase::type()
    */
    IncidenceType type() const;

    /**
      @copydoc
      IncidenceBase::typeStr()
    */
    QByteArray typeStr() const;

    /**
      Sets the start datetime for the free/busy. Note that this datetime
      may be later or earlier than all periods within the free/busy.

      @param start is a KDateTime specifying an start datetime.
      @see IncidenceBase::dtStart(), setDtEnd().
    */
    virtual void setDtStart( const KDateTime &start );

    /**
      Sets the end datetime for the free/busy. Note that this datetime
      may be later or earlier than all periods within the free/busy.

      @param end is a KDateTime specifying an end datetime.
      @see dtEnd(), setDtStart().
    */
    void setDtEnd( const KDateTime &end );

    /**
      Returns the end datetime for the free/busy.
      FIXME: calling addPeriod() does not change mDtEnd. Is that incorrect?
      @see setDtEnd().
    */
    virtual KDateTime dtEnd() const;

    /**
      @copydoc
      IncidenceBase::shiftTimes()
    */
    virtual void shiftTimes( const KDateTime::Spec &oldSpec,
                             const KDateTime::Spec &newSpec );

    /**
      Returns the list of all periods within the free/busy.
    */
    Period::List busyPeriods() const;

    /**
      Returns the list of all periods within the free/busy.
    */
    FreeBusyPeriod::List fullBusyPeriods() const;

    /**
      Adds a period to the freebusy list and sorts the list.

      @param start is the start datetime of the period.
      @param end is the end datetime of the period.
    */
    void addPeriod( const KDateTime &start, const KDateTime &end );

    /**
      Adds a period to the freebusy list and sorts the list.

      @param start is the start datetime of the period.
      @param duration is the Duration of the period.
    */
    void addPeriod( const KDateTime &start, const Duration &duration );

    /**
      Adds a list of periods to the freebusy object and then sorts that list.
      Use this if you are adding many items, instead of the addPeriod method,
      to avoid sorting repeatedly.

      @param list is a list of Period objects.
    */
    void addPeriods( const Period::List &list );

    /**
      Adds a list of periods to the freebusy object and then sorts that list.
      Use this if you are adding many items, instead of the addPeriod method,
      to avoid sorting repeatedly.

      @param list is a list of FreeBusyPeriod objects.
    */
    void addPeriods( const FreeBusyPeriod::List &list );

    /**
      Sorts the list of free/busy periods into ascending order.
    */
    void sortList();

    /**
      Merges another free/busy into this free/busy.

      @param freebusy is a pointer to a valid FreeBusy object.
    */
    void merge( FreeBusy::Ptr freebusy );

    /**
      @copydoc
      IncidenceBase::dateTime()
    */
    KDateTime dateTime( DateTimeRole role ) const;

    /**
      @copydoc
      IncidenceBase::setDateTime()
    */
    void setDateTime( const KDateTime &dateTime, DateTimeRole role );

    /**
       @copydoc
       IncidenceBase::mimeType()
    */
    QLatin1String mimeType() const;

    /**
       Returns the Akonadi specific sub MIME type of a KCalCore::FreeBusy.
    */
    static QLatin1String freeBusyMimeType();

  protected:
    /**
      Compare this with @p freebusy for equality.
      @param freebusy is the FreeBusy to compare.
    */
    virtual bool equals( const IncidenceBase &freebusy ) const;

    /**
      @copydoc
      IncidenceBase::assign()
    */
    virtual IncidenceBase &assign( const IncidenceBase &other );

    /**
      @copydoc
      IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook( int id, void *data );

  private:
    /**
     @copydoc
     IncidenceBase::accept()
    */
    bool accept( Visitor &v, IncidenceBase::Ptr incidence );

    /**
      Disabled, otherwise could be dangerous if you subclass FreeBusy.
      Use IncidenceBase::operator= which is safe because it calls
      virtual function assign().
      @param other is another FreeBusy object to assign to this one.
     */
    FreeBusy &operator=( const FreeBusy &other );

    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond
};

/**
  Serializes the @p fb object into the @p stream.
*/
KCALCORE_EXPORT QDataStream &operator<<( QDataStream &stream,
                                         const KCalCore::FreeBusy::Ptr &freebusy );
/**
  Initializes the @p fb object from the @p stream.
*/
KCALCORE_EXPORT QDataStream &operator>>( QDataStream &stream,
                                         KCalCore::FreeBusy::Ptr &freebusy );
}

//@cond PRIVATE
Q_DECLARE_TYPEINFO( KCalCore::FreeBusy::Ptr, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( KCalCore::FreeBusy::Ptr )
//@endcond

#endif
