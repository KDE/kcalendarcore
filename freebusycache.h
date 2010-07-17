/*
  This file is part of the kcalcore library.

  Copyright (c) 2004 Cornelius Schumacher <schumacher@kde.org>

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
  defines the FreeBusyCache abstract base class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/

#ifndef KCALCORE_FREEBUSYCACHE_H
#define KCALCORE_FREEBUSYCACHE_H

#include "kcalcore_export.h"

#include "freebusy.h"

class QString;

namespace KCalCore {

class Person;

/**
  @brief
  An abstract base class to allow different implementations of storing
  free busy information, e.g. local storage or storage on a Kolab server.
*/
class KCALCORE_EXPORT FreeBusyCache
{
  public:
    /**
      Destructor.
    */
    virtual ~FreeBusyCache();

    /**
      Save freebusy information belonging to an email.

      @param freebusy is a pointer to a valid FreeBusy instance.
      @param person is a valid Person instance.
    */
    virtual bool saveFreeBusy( const FreeBusy::Ptr &freebusy, const Person::Ptr &person ) = 0;

    /**
      Load freebusy information belonging to an email.

      @param email is a QString containing a email string in the
      "FirstName LastName <emailaddress>" format.
    */
    virtual FreeBusy::Ptr loadFreeBusy( const QString &email ) = 0;

  protected:
    /**
      @copydoc
      IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook( int id, void *data );
};

}

#endif
