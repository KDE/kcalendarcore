/*
  This file is part of the kcalcore library.

  Copyright (C) 2015 Sandro Knauß <sknauss@kde.org>

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

#include "testcalendarobserver.h"
#include "filestorage.h"
#include "calendar.h"
#include "memorycalendar.h"

#include <QDebug>
#include <QSignalSpy>

#include <QTest>
QTEST_MAIN(CalendarObserverTest)

using namespace KCalCore;
Q_DECLARE_METATYPE(KCalCore::Incidence::Ptr)
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
    void incidenceAdded(const KCalCore::Incidence::Ptr &incidence);
    void incidenceChanged(const KCalCore::Incidence::Ptr &incidence);
    void incidenceAboutToBeDeleted(const KCalCore::Incidence::Ptr &incidence);
    void incidenceDeletedDeprecated(const KCalCore::Incidence::Ptr &incidence);
    void incidenceDeleted(const KCalCore::Incidence::Ptr &incidence, const Calendar *calendar);
protected:
    void calendarIncidenceAdded(const KCalCore::Incidence::Ptr &incidence) override {
        Q_EMIT incidenceAdded(incidence);
    }
    void calendarIncidenceChanged(const KCalCore::Incidence::Ptr &incidence) override {
        Q_EMIT incidenceChanged(incidence);
    }
    void calendarIncidenceAboutToBeDeleted(const KCalCore::Incidence::Ptr &incidence) override {
        QVERIFY(mCal->incidences().contains(incidence));
        Q_EMIT incidenceAboutToBeDeleted(incidence);
    }
    void calendarIncidenceDeleted(const KCalCore::Incidence::Ptr &incidence) override {
        QVERIFY(!mCal->incidences().contains(incidence));
        Q_EMIT incidenceDeletedDeprecated(incidence);
    }
    void calendarIncidenceDeleted(const KCalCore::Incidence::Ptr &incidence, const Calendar *calendar) override {
        QCOMPARE(calendar, mCal);
        QVERIFY(!calendar->incidences().contains(incidence));
        Q_EMIT incidenceDeleted(incidence, calendar);
    }
};

void CalendarObserverTest::testAdd()
{
    qRegisterMetaType<KCalCore::Incidence::Ptr>();
    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    SimpleObserver ob(cal.data());
    QSignalSpy spy(&ob, &SimpleObserver::incidenceAdded);
    cal->registerObserver(&ob);
    Event::Ptr event1 = Event::Ptr(new Event());
    event1->setUid(QStringLiteral("1"));

    cal->addEvent(event1);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).value<KCalCore::Incidence::Ptr>(), static_cast<KCalCore::Incidence::Ptr>(event1));
}

void CalendarObserverTest::testChange()
{
    qRegisterMetaType<KCalCore::Incidence::Ptr>();
    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    SimpleObserver ob(cal.data());
    QSignalSpy spy(&ob, &SimpleObserver::incidenceChanged);
    cal->registerObserver(&ob);
    Event::Ptr event1 = Event::Ptr(new Event());
    event1->setUid(QStringLiteral("1"));
    cal->addEvent(event1);
    QCOMPARE(spy.count(), 0);

    event1->setDescription(QStringLiteral("desc"));
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).value<KCalCore::Incidence::Ptr>(), static_cast<KCalCore::Incidence::Ptr>(event1));
}

void CalendarObserverTest::testDelete()
{
    qRegisterMetaType<KCalCore::Incidence::Ptr>();
    qRegisterMetaType<const Calendar *>();
    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    SimpleObserver ob(cal.data());
    QSignalSpy spy1(&ob, &SimpleObserver::incidenceAboutToBeDeleted);
    QSignalSpy spy2(&ob, &SimpleObserver::incidenceDeleted);
    QSignalSpy spy3(&ob, &SimpleObserver::incidenceDeletedDeprecated);
    cal->registerObserver(&ob);
    Event::Ptr event1 = Event::Ptr(new Event());
    event1->setUid(QStringLiteral("1"));
    cal->addEvent(event1);
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);

    cal->deleteEvent(event1);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(spy3.count(), 1);
    QList<QVariant> arguments = spy1.takeFirst();
    QCOMPARE(arguments.at(0).value<KCalCore::Incidence::Ptr>(), static_cast<KCalCore::Incidence::Ptr>(event1));
    arguments = spy2.takeFirst();
    QCOMPARE(arguments.at(0).value<KCalCore::Incidence::Ptr>(), static_cast<KCalCore::Incidence::Ptr>(event1));
    QCOMPARE(arguments.at(1).value<const Calendar *>(), cal.data());
    arguments = spy3.takeFirst();
    QCOMPARE(arguments.at(0).value<KCalCore::Incidence::Ptr>(), static_cast<KCalCore::Incidence::Ptr>(event1));
}
#include "testcalendarobserver.moc"
