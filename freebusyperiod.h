/*
  This file is part of the kcalcore library.

  Copyright (c) 2001-2003 Cornelius Schumacher <schumacher@kde.org>

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
  defines the Period class.

  @brief
  Represents a period of time.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/

#ifndef KCALCORE_FREEBUSYPERIOD_H
#define KCALCORE_FREEBUSYPERIOD_H

#include "kcalcore_export.h"
#include "period.h"

#include <QtCore/QMetaType>

namespace KCalCore {

/**
  The period can be defined by either a start time and an end time or
  by a start time and a duration.
*/
class KCALCORE_EXPORT FreeBusyPeriod : public Period
{
  public:
   /**
      List of periods.
    */
    typedef QVector<FreeBusyPeriod> List;

    /**
      Constructs a period without a duration.
    */
    FreeBusyPeriod();

    /**
      Constructs a period from @p start to @p end.

      @param start the time the period begins.
      @param end the time the period ends.
    */
    FreeBusyPeriod( const KDateTime &start, const KDateTime &end );

    /**
      Constructs a period from @p start and lasting @p duration.

      @param start the time when the period starts.
      @param duration how long the period lasts.
    */
    FreeBusyPeriod( const KDateTime &start, const Duration &duration );

    /**
      Constructs a period by copying another period object

      @param period the period to copy
     */

    FreeBusyPeriod( const FreeBusyPeriod &period );

    /**
      Constructs a period by copying another period object

      @param period the period to copy
     */

    FreeBusyPeriod( const Period &period );

    /**
      Destroys a period.
    */
    ~FreeBusyPeriod();

    /**
      Sets this period equal to the @p other one.

      @param other is the other period to compare.
    */
    FreeBusyPeriod &operator=( const FreeBusyPeriod &other );

    /**
      Sets the period summary.
      @param summary is the period summary string.
      @see summary().
    */
    void setSummary( const QString &summary );

    /**
      Returns the period summary.
      @see setSummary()
    */
    QString summary() const;

    /**
      Sets the period location.
      @param location is the period location string.
      @see location().
    */
    void setLocation( const QString &location );

    /**
      Returns the period location.
      @see setLocation()
    */
    QString location() const;

  private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond

    friend KCALCORE_EXPORT QDataStream &operator<<( QDataStream &stream,
                                                    const KCalCore::FreeBusyPeriod &period );
    friend KCALCORE_EXPORT QDataStream &operator>>( QDataStream &stream,
                                                    KCalCore::FreeBusyPeriod &period );
};

/** Write @p period to the datastream @p stream, in binary format. */
KCALCORE_EXPORT QDataStream &operator<<( QDataStream &stream,
                                         const KCalCore::FreeBusyPeriod &period );

/** Read a Period object into @p period from @p stream, in binary format. */
KCALCORE_EXPORT QDataStream &operator>>( QDataStream &stream, KCalCore::FreeBusyPeriod &period );
}

//@cond PRIVATE
Q_DECLARE_METATYPE( KCalCore::FreeBusyPeriod )
//@endcond

#endif
