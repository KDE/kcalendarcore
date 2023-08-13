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
#include "availablebase.h"
#include "freebusyperiod.h"
#include "kcalendarcore_export.h"
#include "person.h"

namespace KCalendarCore
{

/**
  @brief
  Provides a Availability component in the sense of RFC7953.
  */
class KCALENDARCORE_EXPORT Availability : public AvailableBase
{
    // Q_PROPERTY(QString uid READ uid WRITE setUid) // TODO is this needed
public:
    /**
      A shared pointer to a Availability object.
    */
    typedef QSharedPointer<Availability> Ptr;

    Availability();

    ~Availability() override;

    /**
      Copy constructor
      @param other is the Available obj to copy
    */
    Availability(const Availability &other);

    /**
      @copydoc AvailableBase::type()
     */
    Q_REQUIRED_RESULT AvailableType type() const override;

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
      Sets the busytype.
      @param type is the busytype enum value.
      @see busyType().
    */
    void setBusyType(const FreeBusyPeriod::FreeBusyType *type);

    /**
      Returns the busytype .
      @see setBusyType()
    */
    Q_REQUIRED_RESULT FreeBusyPeriod::FreeBusyType busyType() const;

    /**
      Adds new available entry in list
      @param TODO
     */
    void addNewAvailable(const QSharedPointer<Available> available);

    Q_REQUIRED_RESULT QVector<QSharedPointer<Available>> &getAvailables() const;

    /**
       @copydoc IncidenceBase::mimeType()
     */
    Q_REQUIRED_RESULT QLatin1String mimeType() const;

    Q_REQUIRED_RESULT static QLatin1String availabilityMimeType();

private:
    /**
      Disabled, otherwise could be dangerous if you subclass Availability.
      Use AvailableBase::operator= which is safe because it calls
      virtual function assign().
      @param other is another Availability object to assign to this one.
     */
    Availability &operator=(const Availability &other);

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
};
};

#endif
