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

#include "visitor.h"
#include "event.h"
#include "todo.h"
#include "journal.h"
#include "freebusy.h"

using namespace KCalCore;

Visitor::Visitor()
{
}

Visitor::~Visitor()
{
}

bool Visitor::visit( Event::Ptr event )
{
  Q_UNUSED( event );
  return false;
}

bool Visitor::visit( Todo::Ptr todo )
{
  Q_UNUSED( todo );
  return false;
}

bool Visitor::visit( Journal::Ptr journal )
{
  Q_UNUSED( journal );
  return false;
}

bool Visitor::visit( FreeBusy::Ptr freebusy )
{
  Q_UNUSED( freebusy );
  return false;
}
