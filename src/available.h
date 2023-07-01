/*json
  This file is part of the kcalendarcore library.

  SPDX-FileCopyrightText:

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the Available class.

  @author
*/

#ifndef KCALCORE_AVAILABILE_H
#define KCALCORE_AVAILABILE_H

#include "availablebase.h"
#include "incidence.h" // TODO do we need this?
#include "incidencebase.h"
#include "recurrence.h"
//#include "kalendarcore_export.h"

namespace KCalendarCore
{

/**
  @brief
  Provides a Available component in the sense of RFC7953.
  */
class KCALENDARCORE_EXPORT Available : public AvailableBase
{
public:
    /**
      A shared pointer to a Availability object.
    */
    typedef QSharedPointer<Available> Ptr;

    explicit Available();

    ~Available();

    /**
      Copy constructor
      @param other is the Available obj to copy
    */
    Available(const Available &other);

    Available &operator=(const Available &other);

    /**
      Sets the @acronym UID of the availability to @p uid.

      @param uid is the @acronym UID to use for the availability component.

      @see uid()
    */
    //    void setUid(const QString &uid); in base

    /**
      Returns the @acronym UID of the availability.

      @see setUid()
    */
    //    Q_REQUIRED_RESULT QString uid() const; in base

    /**
      Sets the starting date/time.

      @param dt is the starting date/time.
      @see dtStart().
    */
    void setDtStart(const QDateTime &dt);

    /**
      Returns starting date/time as a QDateTime.
      @see setDtStart().
    */
    Q_REQUIRED_RESULT QDateTime dtStart() const;

    /**
      Sets the ending date/time.

      @param dt is the ending date/time.
      @see dtStart().
    */
    void setDtEnd(const QDateTime &dt);

    /**
      Returns ending date/time as a QDateTime.
      @see setDtStart().
    */
    Q_REQUIRED_RESULT QDateTime dtEnd() const;

    /**
      Sets the incidence summary.

      @param summary is the incidence summary string.
      @see summary().
    */
    void setSummary(const QString &summary);

    /**
      Returns the incidence summary.
    */
    Q_REQUIRED_RESULT QString summary() const;

    /**
      Returns true if summary contains RichText; false otherwise.
      @see setSummary(), summary().
    */
    Q_REQUIRED_RESULT bool summaryIsRich() const;

    /**
      Sets the location. Do _not_ use with journals.

      @param location is the location string.
      @param isRich if true indicates the location string contains richtext.
      @see location().
    */
    void setLocation(const QString &location, bool isRich);

    /**
      Returns the location. Do _not_ use with journals.
      @see setLocation().
      @see richLocation().
    */
    Q_REQUIRED_RESULT QString location() const;

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%  Recurrence-related methods
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Returns the recurrence rule associated with this incidence. If there is
      none, returns an appropriate (non-0) object.
    */
    Recurrence *recurrence() const;

    /**
      Removes all recurrence and exception rules and dates.
    */
    void clearRecurrence();

private:
    class Private;
    std::unique_ptr<Private> d;
};
};

#endif
