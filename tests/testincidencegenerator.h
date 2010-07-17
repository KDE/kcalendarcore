/*
  This file is part of the kcalcore library.

  Copyright (C) 2004 Till Adam <adam@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License version 2 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef TESTINCIDENCEGENERATOR_H
#define TESTINCIDENCEGENERATOR_H

#include "../event.h"
#include "../todo.h"
#include "../journal.h"
using namespace KCalCore;

static Event *makeTestEvent()
{
  Event *event = new Event();
  event->setSummary( "Test Event" );
  event->recurrence()->setDaily( 2 );
  event->recurrence()->setDuration( 3 );
  return event;
}

static Todo *makeTestTodo()
{
  Todo *todo = new Todo();
  todo->setSummary( "Test Todo" );
  todo->setPriority( 5 );
  return todo;
}

static Journal *makeTestJournal()
{
  Journal *journal = new Journal();
  journal->setSummary( "Test Journal" );
  return journal;
}

#endif
