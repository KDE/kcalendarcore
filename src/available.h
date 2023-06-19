/*
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

#include "incidence.h"
//#include "kalendarcore_export.h"

namespace KCalendarCore
{

/**
  @brief
  Provides a Available component in the sense of RFC7953.
  */
class KCALENDARCORE_EXPORT Available : public CustomProperties
{
public:
    void setUid(const QString &uid);

    // uuid
    Q_REQUIRED_RESULT QString uid() const;

private:
    mutable QString mUid;
};
};

#endif
