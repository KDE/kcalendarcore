/*
  This file is part of the kcalcore library.


  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the AvailableBase class.

*/

#ifndef KCALCORE_AVAILABLEBASE_H
#define KCALCORE_AVAILABLEBASE_H

#include "customproperties.h"

#include "kcalendarcore_debug.h"

#include <QSharedPointer>

namespace KCalendarCore
{

/**
  @brief
  An abstract class that provides a common base for Availability/Available classes.

  AvailableBase
  + Availability
  + Available

  AvailableBase contains the properties and their getter/setter methods that are common
  to Available and Availability classes. The two derived classes then has some more
  members that are not common to both.
*/
class KCALENDARCORE_EXPORT AvailableBase : public CustomProperties
{
protected:
    class AvailableBasePrivate;
    std::unique_ptr<AvailableBasePrivate> d;

public:
    AvailableBase() = delete;

    /**
      Constructs an empty AvailableBase.
      @param p (non-null) a Private data object provided by the instantiated
      class (Available, Availability).  It takes ownership of the object.
    */
    KCALENDARCORE_NO_EXPORT explicit AvailableBase(AvailableBasePrivate *p);

    /**
      Destroys the AvailableBase.
    */
    ~AvailableBase() override;

    AvailableBase(const AvailableBase &);

    /**
      A shared pointer to an AvailableBase.
    */
    typedef QSharedPointer<AvailableBase> Ptr;

    // TODO add constructor/destructor

    /**
      Sets the unique id for the incidence to @p uid.
      @param uid is the string containing the incidence @ref uid.
      @see uid()
    */
    void setUid(const QString &uid);

    /**
      Returns the unique id (@ref uid) for the incidence.
      @see setUid()
    */
    Q_REQUIRED_RESULT QString uid() const;

    /**
      Sets the timestamp of creation.

      @param dt is the dtstamp.
      @see dtStart().
    */
    void setDtStamp(const QDateTime &dt);

    /**
      Returns dtStamp as a QDateTime.
      @see setDtStart().
    */
    Q_REQUIRED_RESULT QDateTime dtStamp() const;

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
      Sets thesummary.

      @param summary is the summary string.
      @see summary().
    */
    void setSummary(const QString &summary);

    /**
      Returns the summary.
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
};
}

#endif
