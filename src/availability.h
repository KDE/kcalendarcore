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
class KCALENDARCORE_EXPORT Availability : public CustomProperties
{
    // Q_PROPERTY(QString uid READ uid WRITE setUid) // TODO is this needed
public:
    explicit Availability();

    ~Availability();

    /**
      Sets the @acronym UID of the availability to @p uid.

      @param uid is the @acronym UID to use for the availability component.

      @see uid()
    */
    void setUid(const QString &uid);

    /**
      Returns the @acronym UID of the availability.

      @see setUid()
    */
    Q_REQUIRED_RESULT QString uid() const;

    /**
      Sets the incidence starting date/time.

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
      Sets the event end date and time.
      @param dtEnd is a QDateTime specifying when the event ends.
      @see dtEnd().
    */
    void setDtEnd(const QDateTime &dtEnd);

    /**
      Returns the event end date and time.
      @see setDtEnd().
    */
    virtual QDateTime dtEnd() const;

    /**
      Sets the organizer for the incidence.

      @param organizer is a non-null Person to use as the incidence @ref organizer.
      @see organizer(), setOrganizer(const QString &)
    */
    void setOrganizer(const Person &organizer);

    /**
      Sets the incidence organizer to any string @p organizer.

      @param organizer is a string to use as the incidence @ref organizer.
      @see organizer(), setOrganizer(const Person &)
    */
    void setOrganizer(const QString &organizer);

    /**
      Returns the Person associated with this incidence.
      If no Person was set through setOrganizer(), a default Person()
      is returned. // TODO default part not done
      @see setOrganizer(const QString &), setOrganizer(const Person &)
    */
    Person organizer() const;

    /**
      Sets the period summary.
      @param summary is the period summary string.
      @see summary().
    */
    void setSummary(const QString &summary);

    /**
      Returns the period summary.
      @see setSummary()
    */
    Q_REQUIRED_RESULT QString summary() const;

    // dtstamp
    // QDateTime lastModified() const;

private:
    class Private;
    std::unique_ptr<Private> d;
};
};

#endif
