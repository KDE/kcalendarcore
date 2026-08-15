/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "event.h"
#include "icalformat.h"
#include "todo.h"

#include <QDebug>
#include <QTest>

using namespace Qt::Literals;
using namespace KCalendarCore;

static void initLocale()
{
    qputenv("LC_ALL", "en_US.utf-8");
    QLocale::setDefault(QLocale(u"en_US"_s));
}

Q_CONSTRUCTOR_FUNCTION(initLocale)

class IncidencesTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testClone()
    {
        ICalFormat f;

        Event::Ptr event1 = Event::Ptr(new Event);
        event1->setSummary(QStringLiteral("Test Event"));
        event1->recurrence()->setDaily(2);
        event1->recurrence()->setDuration(3);
        event1->setSchedulingID(QStringLiteral("foo"));
        QString eventString1 = f.toString(event1.staticCast<Incidence>());

        Incidence::Ptr event2 = Incidence::Ptr(event1->clone());
        QCOMPARE(event1->uid(), event2->uid());
        QCOMPARE(event1->schedulingID(), event2->schedulingID());

        QString eventString2 = f.toString(event2.staticCast<Incidence>());
        QCOMPARE(eventString1, eventString2);

        Todo::Ptr todo1 = Todo::Ptr(new Todo);
        todo1->setSummary(QStringLiteral("Test todo"));
        QString todoString1 = f.toString(todo1.staticCast<Incidence>());

        Incidence::Ptr todo2 = Incidence::Ptr(todo1->clone());
        QString todoString2 = f.toString(todo2);
        QCOMPARE(todoString1, todoString2);
    }

    void testRecurrenceDescription()
    {
        // TEST: A daily recurrence with date exclusions //
        const Event::Ptr e1 = Event::Ptr(new Event());

        const QDate day(2010, 10, 3);
        const QTime tim(12, 0, 0);
        const QDateTime kdt(day, tim, QTimeZone::utc());
        e1->setDtStart(kdt);
        e1->setDtEnd(kdt.addSecs(60 * 60)); // 1hr event

        QCOMPARE(e1->recurrenceDescription(), "No recurrence"_L1);

        Recurrence *r1 = e1->recurrence();

        r1->setDaily(1);
        r1->setEndDateTime(kdt.addDays(5)); // ends 5 days from now
        QString endDateStr = QLocale().toString(kdt.addDays(5).toLocalTime(), QLocale::ShortFormat);
        QEXPECT_FAIL("", "missing en_US plural translation catalog", Continue);
        QCOMPARE(e1->recurrenceDescription(), "Recurs daily until %1"_L1.arg(endDateStr));

        r1->setFrequency(2);

        QCOMPARE(e1->recurrenceDescription(), "Recurs every 2 days until "_L1 + endDateStr);

        r1->addExDate(kdt.addDays(1).date());
        const QString exDateStr = QLocale().toString(kdt.addDays(1).date(), QLocale::ShortFormat);
        QCOMPARE(e1->recurrenceDescription(), "Recurs every 2 days until %1 (excluding %2)"_L1.arg(endDateStr, exDateStr));

        r1->addExDate(kdt.addDays(3).date());
        const QString exDateStr2 = QLocale().toString(kdt.addDays(3).date(), QLocale::ShortFormat);
        QCOMPARE(e1->recurrenceDescription(), "Recurs every 2 days until %1 (excluding %2,%3)"_L1.arg(endDateStr, exDateStr, exDateStr2));

        // TEST: An daily recurrence, with datetime exclusions //
        const Event::Ptr e2 = Event::Ptr(new Event());
        e2->setDtStart(kdt);
        e2->setDtEnd(kdt.addSecs(60 * 60)); // 1hr event

        Recurrence *r2 = e2->recurrence();

        r2->setDaily(1);
        r2->setEndDate(kdt.addDays(5).date()); // ends 5 days from now
        QEXPECT_FAIL("", "missing en_US plural translation catalog", Continue);
        QCOMPARE(e2->recurrenceDescription(), "Recurs daily until %1"_L1.arg(endDateStr));

        r2->setFrequency(2);

        QCOMPARE(e2->recurrenceDescription(), "Recurs every 2 days until %1"_L1.arg(endDateStr));

        r2->addExDateTime(kdt.addDays(1));
        QCOMPARE(e2->recurrenceDescription(), "Recurs every 2 days until %1 (excluding %2)"_L1.arg(endDateStr, exDateStr));

        r2->addExDate(kdt.addDays(3).date());
        QCOMPARE(e2->recurrenceDescription(), "Recurs every 2 days until %1 (excluding %2,%3)"_L1.arg(endDateStr, exDateStr, exDateStr2));

        // TEST: An hourly recurrence, with exclusions //
        const Event::Ptr e3 = Event::Ptr(new Event());
        e3->setDtStart(kdt);
        e3->setDtEnd(kdt.addSecs(60 * 60)); // 1hr event

        Recurrence *r3 = e3->recurrence();

        r3->setHourly(1);
        r3->setEndDateTime(kdt.addSecs(5 * 60 * 60)); // ends 5 hrs from now
        endDateStr = QLocale().toString(r3->endDateTime().toLocalTime(), QLocale::ShortFormat);
        QEXPECT_FAIL("", "missing en_US plural translation catalog", Continue);
        QCOMPARE(e3->recurrenceDescription(), "Recurs hourly until %1"_L1.arg(endDateStr));

        r3->setFrequency(2);

        QCOMPARE(e3->recurrenceDescription(), "Recurs every 2 hours until %1"_L1.arg(endDateStr));

        r3->addExDateTime(kdt.addSecs(1 * 60 * 60));
        const QString hourStr = QLocale().toString(QTime(13, 0), QLocale::ShortFormat);
        QCOMPARE(e3->recurrenceDescription(), "Recurs every 2 hours until %1 (excluding %2)"_L1.arg(endDateStr, hourStr));

        r3->addExDateTime(kdt.addSecs(3 * 60 * 60));
        const QString hourStr2 = QLocale().toString(QTime(15, 0), QLocale::ShortFormat);
        QCOMPARE(e3->recurrenceDescription(), "Recurs every 2 hours until %1 (excluding %2,%3)"_L1.arg(endDateStr, hourStr, hourStr2));
    }
};

QTEST_APPLESS_MAIN(IncidencesTest)

#include "incidencestest.moc"
