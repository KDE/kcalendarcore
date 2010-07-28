/*
  This file is part of the kcalcore library.

  Copyright (c) 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (c) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  Copyright (c) 2005 Rafal Rzepecki <divide@users.sourceforge.net>
  Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
  Contact: Alvaro Manera <alvaro.manera@nokia.com>

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

#ifndef KCALCORE_VISITOR_P_H
#define KCALCORE_VISITOR_P_H

#include "event.h"
#include "journal.h"
#include "todo.h"
#include "freebusy.h"

namespace KCalCore {

/**
  This class provides the interface for a visitor of calendar components.
  It serves as base class for concrete visitors, which implement certain
  actions on calendar components. It allows to add functions, which operate
  on the concrete types of calendar components, without changing the
  calendar component classes.
*/
class KCALCORE_EXPORT Visitor //krazy:exclude=dpointer
{
  public:
    /** Destruct Incidence::Visitor */
    virtual ~Visitor();

    /**
      Reimplement this function in your concrete subclass of
      IncidenceBase::Visitor to perform actions on an Event object.
      @param event is a pointer to a valid Event object.
    */
    virtual bool visit( Event::Ptr event );

    /**
      Reimplement this function in your concrete subclass of
      IncidenceBase::Visitor to perform actions on a Todo object.
      @param todo is a pointer to a valid Todo object.
    */
    virtual bool visit( Todo::Ptr todo );

    /**
      Reimplement this function in your concrete subclass of
      IncidenceBase::Visitor to perform actions on an Journal object.
      @param journal is a pointer to a valid Journal object.
    */
    virtual bool visit( Journal::Ptr journal );

    /**
      Reimplement this function in your concrete subclass of
      IncidenceBase::Visitor to perform actions on a FreeBusy object.
      @param freebusy is a pointer to a valid FreeBusy object.
    */
    virtual bool visit( FreeBusy::Ptr freebusy );

  protected:
    /**
      Constructor is protected to prevent direct creation of visitor
      base class.
    */
    Visitor();
};

} // end namespace

#endif
