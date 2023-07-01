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
