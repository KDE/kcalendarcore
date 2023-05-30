/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText:

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the Availability class.

  @author
*/

#ifndef KCALCORE_AVAILABILITY_H
#define KCALCORE_AVAILABILITY_H

#include "incidence.h"
//#include "kalendarcore_export.h"

namespace KCalendarCore
{

/**
  @brief
  Provides a Availability component in the sense of RFC7953.
*/
class KCALCORE_AVAILABILITY_H Availability : public Incidence
{
public:
    /**
      Constructs an empty availability.
     */
    Availability();

    /**
      Destroys an availability.
     */
    ~Availability() override;

    /**
      @copydoc IncidenceBase::type()
    */
    Q_REQUIRED_RESULT IncidenceType type() const override;

    /**
      @copydoc IncidenceBase::typeStr()
    */
    Q_REQUIRED_RESULT QByteArray typeStr() const override;
};
};

#endif
