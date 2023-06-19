/*
  This file is part of the kcalendarcore library.

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

#include "available.h"
#include "incidence.h"
#include "kcalendarcore_export.h"
#include "person.h"

namespace KCalendarCore
{

/**
  @brief
  Provides a Availability component in the sense of RFC7953.
  */
class KCALCORE_AVAILABILITY_H Availability : public CustomProperties
{
    // Q_PROPERTY(QString uid READ uid WRITE setUid) // TODO is this needed
public:
    void setUid(const QString &uid);

    // uuid
    Q_REQUIRED_RESULT QString uid() const;

    void setDtStart(const QDateTime &dt);

    Q_REQUIRED_RESULT QDateTime dtStart() const;

    void setDtEnd(const QDateTime &dtEnd);

    virtual QDateTime dtEnd() const;

    void setOrganizer(const Person &organizer);

    void setOrganizer(const QString &organizer);

    Person organizer() const;

    // dtstamp
    // QDateTime lastModified() const;

private:
    QVector<Available> availables() const;

    QDateTime mDtStart; // start time
    QDateTime mDtEnd; // end time

    Person mOrganizer; // person (owner)

    mutable QString mUid;
};
};

#endif
