/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
  SPDX-FileContributor: Alvaro Manera <alvaro.manera@nokia.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KCALCORE_SORTING_H
#define KCALCORE_SORTING_H

#include "event.h"
#include "freebusy.h"
#include "journal.h"
#include "person.h"
#include "todo.h"

#include "kcalendarcore_export.h"

namespace KCalendarCore
{
namespace Events
{
KCALENDARCORE_EXPORT bool startDateLessThan(const QSharedPointer<Event> &e1, const QSharedPointer<Event> &e2);

KCALENDARCORE_EXPORT bool summaryLessThan(const QSharedPointer<Event> &e1, const QSharedPointer<Event> &e2);

KCALENDARCORE_EXPORT bool summaryMoreThan(const QSharedPointer<Event> &e1, const QSharedPointer<Event> &e2);

KCALENDARCORE_EXPORT bool startDateMoreThan(const QSharedPointer<Event> &e1, const QSharedPointer<Event> &e2);

KCALENDARCORE_EXPORT bool endDateLessThan(const QSharedPointer<Event> &e1, const QSharedPointer<Event> &e2);

KCALENDARCORE_EXPORT bool endDateMoreThan(const QSharedPointer<Event> &e1, const QSharedPointer<Event> &e2);
}

namespace Todos
{
KCALENDARCORE_EXPORT bool startDateLessThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool startDateMoreThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool dueDateLessThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool dueDateMoreThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool priorityLessThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool priorityMoreThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool percentLessThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool percentMoreThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool summaryLessThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool summaryMoreThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool createdLessThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);

KCALENDARCORE_EXPORT bool createdMoreThan(const QSharedPointer<Todo> &t1, const QSharedPointer<Todo> &t2);
}

namespace Journals
{
KCALENDARCORE_EXPORT bool dateLessThan(const QSharedPointer<Journal> &j1, const QSharedPointer<Journal> &j2);

KCALENDARCORE_EXPORT bool dateMoreThan(const QSharedPointer<Journal> &j1, const QSharedPointer<Journal> &j2);

KCALENDARCORE_EXPORT bool summaryLessThan(const QSharedPointer<Journal> &j1, const QSharedPointer<Journal> &j2);

KCALENDARCORE_EXPORT bool summaryMoreThan(const QSharedPointer<Journal> &j1, const QSharedPointer<Journal> &j2);
}

namespace Incidences
{
KCALENDARCORE_EXPORT bool dateLessThan(const QSharedPointer<Incidence> &i1, const QSharedPointer<Incidence> &i2);

KCALENDARCORE_EXPORT bool dateMoreThan(const QSharedPointer<Incidence> &i1, const QSharedPointer<Incidence> &i2);

KCALENDARCORE_EXPORT bool createdLessThan(const QSharedPointer<Incidence> &i1, const QSharedPointer<Incidence> &i2);

KCALENDARCORE_EXPORT bool createdMoreThan(const QSharedPointer<Incidence> &i1, const QSharedPointer<Incidence> &i2);

KCALENDARCORE_EXPORT bool summaryLessThan(const QSharedPointer<Incidence> &i1, const QSharedPointer<Incidence> &i2);

KCALENDARCORE_EXPORT bool summaryMoreThan(const QSharedPointer<Incidence> &i1, const QSharedPointer<Incidence> &i2);

/**
 * Compare the categories (tags) of two incidences, as returned by categoriesStr().
 * If they are equal, return summaryLessThan().
 * @since 5.83
 */
KCALENDARCORE_EXPORT bool categoriesLessThan(const QSharedPointer<Incidence> &i1, const QSharedPointer<Incidence> &i2);

/**
 * Compare the categories (tags) of two incidences, as returned by categoriesStr().
 * If they are equal, return summaryMoreThan().
 * @since 5.83
 */
KCALENDARCORE_EXPORT bool categoriesMoreThan(const QSharedPointer<Incidence> &i1, const QSharedPointer<Incidence> &i2);
}

}

#endif
