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
#include "recurrence.h"
//#include "kalendarcore_export.h" TODO is this needed?

namespace KCalendarCore
{

/**
  @brief
  Provides a Available component in the sense of RFC7953.
  */
class KCALENDARCORE_EXPORT Available : public AvailableBase
{
private:
    /**
      Disabled, otherwise could be dangerous if you subclass Available.
      Use AvailableBase::operator= which is safe because it calls
      virtual function assign().
      @param other is another Available object to assign to this one.
     */
    Available &operator=(const Available &other);

    class Private;
    std::unique_ptr<Private> d;

protected:
    /**
      Compare this with @p available for equality.
      @param available is what to compare against.
     */
    bool equals(const AvailableBase &available) const override;

    /**
      @copydoc
      IncidenceBase::assign()
    */
    AvailableBase &assign(const AvailableBase &other) override;

public:
    /**
      A shared pointer to a Availability object.
    */
    typedef QSharedPointer<Available> Ptr;

    Available();

    ~Available() override;

    /**
      Copy constructor
      @param other is the Available obj to copy
    */
    Available(const Available &other);

    /**
      @copydoc AvailableBase::type()
     */
    Q_REQUIRED_RESULT AvailableType type() const override;

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
};
};

#endif
