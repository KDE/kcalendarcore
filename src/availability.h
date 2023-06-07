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
      A shared pointer to a Availability object.
      */
    typedef QSharedPointer<Availability> Ptr;

    /**
      List of to-dos.
      */
    typedef QVector<Ptr> List;

    ///@cond PRIVATE
    // needed for Akonadi polymorphic payload support
    typedef Incidence SuperClass;
    ///@endcond

    /**
      Constructs an empty availability.
      */
    Availability();

    /**
      Copy constructor.
      @param other is the to-do to copy.
      */
    Availability(const Availability &other);

    /**
      Costructs a availability out of an incidence
      This constructs allows to make it easy to create a availability from an event.
      @param other is the incidence to copy.
      @since 4.14
      */
    Availability(const Incidence &other); // krazy:exclude=explicit (copy ctor)

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

    /**
      Returns an exact copy of this availability. The returned object is owned by the caller.
      @return A pointer to a Availability containing an exact copy of this object.
      */
    Availability *clone() const override;

    /**
      Returns if the availability has a start datetime.
      @return true if the availability has a start datetime; false otherwise.
      */
    Q_REQUIRED_RESULT bool hasStartDate() const;

    /**
      @copydoc IncidenceBase::dateTime()
    */
    Q_REQUIRED_RESULT QDateTime dateTime(DateTimeRole role) const override;

    /**
      @copydoc IncidenceBase::dtStart()
      */
    Q_REQUIRED_RESULT QDateTime dtStart() const override;

    /**
      @copydoc IncidenceBase::setDateTime()
    */
    void setDateTime(const QDateTime &dateTime, DateTimeRole role) override;

    /**
       @copydoc IncidenceBase::mimeType()
    */
    Q_REQUIRED_RESULT QLatin1String mimeType() const override;

    /**
       Returns the Akonadi specific sub MIME type of a KCalendarCore::Availability.
    */
    Q_REQUIRED_RESULT static QLatin1String availabilityMimeType();

    /**
      @copydoc IncidenceBase::virtual_hook()
    */
    void virtual_hook(VirtualHook id, void *data) override;

    /**
       @copydoc Incidence::iconName()
    */
    Q_REQUIRED_RESULT QLatin1String iconName(const QDateTime &recurrenceId = {}) const override;

    /**
       @copydoc
       Incidence::supportsGroupwareCommunication()
    */
    bool supportsGroupwareCommunication() const override;
};
};

#endif
