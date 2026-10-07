/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2015 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testcalendarobserver.h"
#include "calendar.h"
#include "filestorage.h"
#include "memorycalendar.h"

#include <QSignalSpy>
#include <QTest>
#include <QTimeZone>

QTEST_MAIN(CalendarObserverTest)

using namespace KCalendarCore;
Q_DECLARE_METATYPE(QSharedPointer<KCalendarCore::Incidence>)
Q_DECLARE_METATYPE(const Calendar *)

class SimpleObserver : public QObject, public Calendar::CalendarObserver
{
    Q_OBJECT
public:
    SimpleObserver(Calendar *cal, QObject *parent = nullptr)
        : QObject(parent)
        , mCal(cal)
    {
    }

    Calendar *mCal = nullptr;
Q_SIGNALS:
    void incidenceAdded(const QSharedPointer<KCalendarCore::Incidence> &incidence);
    void incidenceChanged(const QSharedPointer<KCalendarCore::Incidence> &incidence);
    void incidenceAboutToBeDeleted(const QSharedPointer<KCalendarCore::Incidence> &incidence);
    void incidenceDeleted(const QSharedPointer<KCalendarCore::Incidence> &incidence, const KCalendarCore::Calendar *calendar);

protected:
    void calendarIncidenceAdded(const QSharedPointer<KCalendarCore::Incidence> &incidence) override
    {
        Q_EMIT incidenceAdded(incidence);
    }

    void calendarIncidenceChanged(const QSharedPointer<KCalendarCore::Incidence> &incidence) override
    {
        Q_EMIT incidenceChanged(incidence);
    }

    void calendarIncidenceAboutToBeDeleted(const QSharedPointer<KCalendarCore::Incidence> &incidence) override
    {
        QVERIFY(mCal->incidences().contains(incidence));
        Q_EMIT incidenceAboutToBeDeleted(incidence);
    }

    void calendarIncidenceDeleted(const QSharedPointer<KCalendarCore::Incidence> &incidence, const Calendar *calendar) override
    {
        QCOMPARE(calendar, mCal);
        QVERIFY(!calendar->incidences().contains(incidence));
        Q_EMIT incidenceDeleted(incidence, calendar);
    }
};

void CalendarObserverTest::testAdd()
{
    qRegisterMetaType<QSharedPointer<KCalendarCore::Incidence>>();
    QSharedPointer<MemoryCalendar> cal(new MemoryCalendar(QTimeZone::utc()));
    SimpleObserver ob(cal.data());
    QSignalSpy spy(&ob, &SimpleObserver::incidenceAdded);
    cal->registerObserver(&ob);
    auto event1 = QSharedPointer<Event>(new Event());
    event1->setUid(QStringLiteral("1"));

    cal->addEvent(event1);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).value<QSharedPointer<KCalendarCore::Incidence>>(), static_cast<QSharedPointer<KCalendarCore::Incidence>>(event1));
}

void CalendarObserverTest::testChange()
{
    qRegisterMetaType<QSharedPointer<KCalendarCore::Incidence>>();
    QSharedPointer<MemoryCalendar> cal(new MemoryCalendar(QTimeZone::utc()));
    SimpleObserver ob(cal.data());
    QSignalSpy spy(&ob, &SimpleObserver::incidenceChanged);
    cal->registerObserver(&ob);
    auto event1 = QSharedPointer<Event>(new Event());
    event1->setUid(QStringLiteral("1"));
    cal->addEvent(event1);
    QCOMPARE(spy.count(), 0);

    event1->setDescription(QStringLiteral("desc"));
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).value<QSharedPointer<KCalendarCore::Incidence>>(), static_cast<QSharedPointer<KCalendarCore::Incidence>>(event1));
}

void CalendarObserverTest::testDelete()
{
    qRegisterMetaType<QSharedPointer<KCalendarCore::Incidence>>();
    qRegisterMetaType<const Calendar *>();
    QSharedPointer<MemoryCalendar> cal(new MemoryCalendar(QTimeZone::utc()));
    SimpleObserver ob(cal.data());
    QSignalSpy spy1(&ob, &SimpleObserver::incidenceAboutToBeDeleted);
    QSignalSpy spy2(&ob, &SimpleObserver::incidenceDeleted);
    cal->registerObserver(&ob);
    auto event1 = QSharedPointer<Event>(new Event());
    event1->setUid(QStringLiteral("1"));
    cal->addEvent(event1);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);

    cal->deleteEvent(event1);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QList<QVariant> arguments = spy1.takeFirst();
    QCOMPARE(arguments.at(0).value<QSharedPointer<KCalendarCore::Incidence>>(), static_cast<QSharedPointer<KCalendarCore::Incidence>>(event1));
    arguments = spy2.takeFirst();
    QCOMPARE(arguments.at(0).value<QSharedPointer<KCalendarCore::Incidence>>(), static_cast<QSharedPointer<KCalendarCore::Incidence>>(event1));
    QCOMPARE(arguments.at(1).value<const Calendar *>(), cal.data());
}

#include "moc_testcalendarobserver.cpp"
#include "testcalendarobserver.moc"
