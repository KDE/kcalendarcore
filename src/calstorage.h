/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2002, 2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the CalStorage abstract base class.
*/

#ifndef KCALCORE_CALSTORAGE_H
#define KCALCORE_CALSTORAGE_H

#include "calendar.h"
#include "kcalendarcore_export.h"

#include <QObject>

namespace KCalendarCore
{
/*!
  \class KCalendarCore::CalStorage
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/CalStorage
  \brief
  An abstract base class that provides a calendar storage interface.

  This is the base class for calendar storage. It provides an interface for the
  loading and saving of calendars.
*/
class KCALENDARCORE_EXPORT CalStorage : public QObject
{
    Q_OBJECT

public:
    /*!
      Constructs a new storage object for a calendar.
      \a calendar is a pointer to a valid Calendar object.
    */
    explicit CalStorage(const Calendar::Ptr &calendar);

    ~CalStorage() override;

    /*!
      Returns the calendar for this storage object.
      Returns A pointer to the calendar whose storage is being managed.
    */
    Calendar::Ptr calendar() const;

    /*!
      Opens the calendar for storage.
      Returns true if the open was successful; false otherwise.
    */
    virtual bool open() = 0;

    /*!
      Loads the calendar into memory.
      Returns true if the load was successful; false otherwise.
    */
    virtual bool load() = 0;

    /*!
      Saves the calendar.
      Returns true if the save was successful; false otherwise.
    */
    virtual bool save() = 0;

    /*!
      Closes the calendar storage.
      Returns true if the close was successful; false otherwise.
    */
    virtual bool close() = 0;

private:
    //@cond PRIVATE
    Q_DISABLE_COPY(CalStorage)
    class Private;
    Private *const d;
    //@endcond
};

}

#endif
