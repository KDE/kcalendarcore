/*
  This file is part of the kcalcore library.

  Copyright (c) 2002,2003 Cornelius Schumacher <schumacher@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the CalStorage abstract base class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/

#ifndef KCALCORE_CALSTORAGE_H
#define KCALCORE_CALSTORAGE_H

#include "kcalcore_export.h"
#include "calendar.h"

#include <QtCore/QObject>

namespace KCalCore {

/**
  @brief
  An abstract base class that provides a calendar storage interface.

  This is the base class for calendar storage. It provides an interface for the
  loading and saving of calendars.
*/
class KCALCORE_EXPORT CalStorage : public QObject
{
  Q_OBJECT

  public:
    /**
      Construcst a new storage object for a calendar.
      @param calendar is a pointer to a valid Calendar object.
    */
    explicit CalStorage( const Calendar::Ptr &calendar );

    /**
      Destuctor.
    */
    virtual ~CalStorage();

    /**
      Returns a pointer to the calendar whose storage is being managed.
    */
    Calendar::Ptr calendar() const;

    /**
      Opens the calendar for storage.
      @return true if the open was successful; false otherwise.
    */
    virtual bool open() = 0;

    /**
      Loads the calendar into memory.
      @return true if the load was successful; false otherwise.
    */
    virtual bool load() = 0;

    /**
      Saves the calendar.
      @return true if the save was successful; false otherwise.
    */
    virtual bool save() = 0;

    /**
      Closes the calendar storage.
      @return true if the close was successful; false otherwise.
    */
    virtual bool close() = 0;

  private:
    //@cond PRIVATE
    Q_DISABLE_COPY( CalStorage )
    class Private;
    Private *const d;
    //@endcond
};

}

#endif
