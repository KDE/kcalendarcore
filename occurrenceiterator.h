/*
  This file is part of the kcalcore library.

  Copyright (c) 2013 Christian Mollekopf <mollekopf@kolabsys.com>

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
  defines the OccurrenceIterator class.

  @author Christian Mollekopf \<mollekopf@kolabsys.com\>
 */

#ifndef KCALCORE_OCCURRENCEITERATOR_H
#define KCALCORE_OCCURRENCEITERATOR_H

#include "kcalcore_export.h"
#include "incidence.h"

namespace KCalCore {

class Calendar;
/**
 * Iterate over calendar items in a calendar.
 *
 * The iterator takes recurrences and exceptions to recurrences into account
 *
 * The iterator does not iterate the occurrences of all incidences chronologically.
 * @since 4.11
 */
class KCALCORE_EXPORT OccurrenceIterator
{
  public:
    /**
     * Creates iterator that iterates over all occurrences of all incidences
     * between @param start and @param end (inclusive)
     */
    explicit OccurrenceIterator( const Calendar &calendar,
                                 const KDateTime &start = KDateTime(),
                                 const KDateTime &end = KDateTime() );

    /**
     * Creates iterator that iterates over all occurrences
     * of @param incidence between @param start and @param end (inclusive)
     */
    OccurrenceIterator( const Calendar &calendar,
                        const KCalCore::Incidence::Ptr &incidence,
                        const KDateTime &start = KDateTime(),
                        const KDateTime &end = KDateTime() );
    ~OccurrenceIterator();
    bool hasNext() const;

    /**
     * Advance iterator to the next occurrence.
     */
    void next();

    /**
     * Returns either main incidence or exception, depending on occurrence.
     */
    Incidence::Ptr incidence() const;

    /**
     * Returns the start date of the occurrence
     *
     * This is either the occurrence date, or the start date of an exception
     * which overrides that occurrence.
     */
    KDateTime occurrenceStartDate() const;

  private:
     Q_DISABLE_COPY(OccurrenceIterator)
    //@cond PRIVATE
    class Private;
    QScopedPointer<Private> d;
    //@endcond
};

} //namespace

#endif

