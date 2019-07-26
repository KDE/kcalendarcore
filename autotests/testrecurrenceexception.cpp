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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "testrecurrenceexception.h"
#include "memorycalendar.h"

#include <QTest>
QTEST_MAIN(TestRecurrenceException)

void TestRecurrenceException::testCreateTodoException()
{
    const QDateTime dtstart(QDate(2013, 03, 10), QTime(10, 0, 0), Qt::UTC);
    const QDateTime dtdue(QDate(2013, 03, 10), QTime(11, 0, 0), Qt::UTC);
    const QDateTime recurrenceId(QDateTime(dtstart).addDays(1));

    KCalendarCore::Todo::Ptr todo(new KCalendarCore::Todo());
    todo->setUid(QStringLiteral("todo"));
    todo->setDtStart(dtstart);
    todo->setDtDue(dtdue);
    todo->recurrence()->setDaily(1);
    todo->recurrence()->setDuration(3);

    const KCalendarCore::Todo::Ptr exception
        = KCalendarCore::MemoryCalendar::createException(todo, recurrenceId, false).staticCast<KCalendarCore::Todo>();
    QCOMPARE(exception->dtStart(), recurrenceId);
    QCOMPARE(exception->dtDue(), QDateTime(dtdue).addDays(1));
    //FIXME should be done on clearing the recurrence, but we can't due to BC.
    //Probably not that important as long as dtRecurrence is ignored if the todo is not recurring
    //QCOMPARE(exception->dtRecurrence(), QDateTime());
    //TODO dtCompleted
}
