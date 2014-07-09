/*
 * Copyright (C) 2013  Christian Mollekopf <mollekopf@kolabsys.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "testrecurrenceexception.h"
#include "memorycalendar.h"

#include <qtest.h>
#include <qdebug.h>
QTEST_MAIN(TestRecurrenceException)

void TestRecurrenceException::testCreateTodoException()
{
    const KDateTime dtstart(QDate(2013, 03, 10), QTime(10, 0, 0), KDateTime::UTC);
    const KDateTime dtdue(QDate(2013, 03, 10), QTime(11, 0, 0), KDateTime::UTC);
    const KDateTime recurrenceId(KDateTime(dtstart).addDays(1));

    KCalCore::Todo::Ptr todo(new KCalCore::Todo());
    todo->setUid("todo");
    todo->setDtStart(dtstart);
    todo->setDtDue(dtdue);
    todo->recurrence()->setDaily(1);
    todo->recurrence()->setDuration(3);

    const KCalCore::Todo::Ptr exception = KCalCore::MemoryCalendar::createException(todo, recurrenceId, false).staticCast<KCalCore::Todo>();
    QCOMPARE(exception->dtStart(), recurrenceId);
    QCOMPARE(exception->dtDue(), KDateTime(dtdue).addDays(1));
    //FIXME should be done on clearing the recurrence, but we can't due to BC. Probably not that important as long as dtRecurrence is ignored if the todo is not recurring
    //QCOMPARE(exception->dtRecurrence(), KDateTime());
    //TODO dtCompleted
}
