/*
    SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCALENDARCORE_ANDROIDCALENDAR_H
#define KCALENDARCORE_ANDROIDCALENDAR_H

#include "incidencekey_p.h"
#include "jnicalendar.h"

#include "jni.h"

#include <KCalendarCore/Calendar>

#include <unordered_map>

/** Access to an Android system calendar. */
class AndroidCalendar : public KCalendarCore::Calendar
{
public:
    explicit AndroidCalendar(const QTimeZone &tz, const QString &owner, jlong id);
    ~AndroidCalendar();

    // KCalendarCore::Calendar interface
    bool deleteIncidenceInstances(const QSharedPointer<KCalendarCore::Incidence> &incidence) override;

    bool addEvent(const QSharedPointer<KCalendarCore::Event> &event) override;
    bool deleteEvent(const QSharedPointer<KCalendarCore::Event> &event) override;
    bool deleteEventInstances(const QSharedPointer<KCalendarCore::Event> &event) override;
    QList<QSharedPointer<KCalendarCore::Event>> rawEvents(KCalendarCore::EventSortField sortField = KCalendarCore::EventSortUnsorted,
                                                          KCalendarCore::SortDirection sortDirection = KCalendarCore::SortDirectionAscending) const override;
    QList<QSharedPointer<KCalendarCore::Event>>
    rawEvents(const QDate &start, const QDate &end, const QTimeZone &timeZone = {}, bool inclusive = false) const override;
    QList<QSharedPointer<KCalendarCore::Event>>
    rawEventsForDate(const QDate &date,
                     const QTimeZone &timeZone = {},
                     KCalendarCore::EventSortField sortField = KCalendarCore::EventSortUnsorted,
                     KCalendarCore::SortDirection sortDirection = KCalendarCore::SortDirectionAscending) const override;
    QSharedPointer<KCalendarCore::Event> event(const QString &uid, const QDateTime &recurrenceId = {}) const override;
    QList<QSharedPointer<KCalendarCore::Event>>
    eventInstances(const QSharedPointer<KCalendarCore::Incidence> &event,
                   KCalendarCore::EventSortField sortField = KCalendarCore::EventSortUnsorted,
                   KCalendarCore::SortDirection sortDirection = KCalendarCore::SortDirectionAscending) const override;

    bool addTodo(const QSharedPointer<KCalendarCore::Todo> &todo) override;
    bool deleteTodo(const QSharedPointer<KCalendarCore::Todo> &todo) override;
    bool deleteTodoInstances(const QSharedPointer<KCalendarCore::Todo> &todo) override;
    QList<QSharedPointer<KCalendarCore::Todo>> rawTodos(KCalendarCore::TodoSortField sortField = KCalendarCore::TodoSortUnsorted,
                                                        KCalendarCore::SortDirection sortDirection = KCalendarCore::SortDirectionAscending) const override;
    QList<QSharedPointer<KCalendarCore::Todo>> rawTodosForDate(const QDate &date) const override;
    QList<QSharedPointer<KCalendarCore::Todo>>
    rawTodos(const QDate &start, const QDate &end, const QTimeZone &timeZone = {}, bool inclusive = false) const override;
    QSharedPointer<KCalendarCore::Todo> todo(const QString &uid, const QDateTime &recurrenceId = {}) const override;
    QList<QSharedPointer<KCalendarCore::Todo>> todoInstances(const QSharedPointer<KCalendarCore::Incidence> &todo,
                                                             KCalendarCore::TodoSortField sortField = KCalendarCore::TodoSortUnsorted,
                                                             KCalendarCore::SortDirection sortDirection = KCalendarCore::SortDirectionAscending) const override;

    bool addJournal(const QSharedPointer<KCalendarCore::Journal> &journal) override;
    bool deleteJournal(const QSharedPointer<KCalendarCore::Journal> &journal) override;
    bool deleteJournalInstances(const QSharedPointer<KCalendarCore::Journal> &journal) override;
    QList<QSharedPointer<KCalendarCore::Journal>>
    rawJournals(KCalendarCore::JournalSortField sortField = KCalendarCore::JournalSortUnsorted,
                KCalendarCore::SortDirection sortDirection = KCalendarCore::SortDirectionAscending) const override;
    QList<QSharedPointer<KCalendarCore::Journal>> rawJournalsForDate(const QDate &date) const override;
    QSharedPointer<KCalendarCore::Journal> journal(const QString &uid, const QDateTime &recurrenceId = {}) const override;
    QList<QSharedPointer<KCalendarCore::Journal>>
    journalInstances(const QSharedPointer<KCalendarCore::Incidence> &journal,
                     KCalendarCore::JournalSortField sortField = KCalendarCore::JournalSortUnsorted,
                     KCalendarCore::SortDirection sortDirection = KCalendarCore::SortDirectionAscending) const override;

    QList<QSharedPointer<KCalendarCore::Alarm>> alarms(const QDateTime &from, const QDateTime &to, bool excludeBlockedAlarms = false) const override;

    // KCalendarCore::IncidenceObserver interface
    void incidenceUpdate(const QString &uid, const QDateTime &recurrenceId) override;
    void incidenceUpdated(const QString &uid, const QDateTime &recurrenceId) override;

private:
    void registerEvents(const QList<QSharedPointer<KCalendarCore::Event>> &events) const;
    void registerEvent(const QSharedPointer<KCalendarCore::Event> &event) const;

    JniCalendar m_calendar;
    const QString m_owner;
    mutable std::unordered_map<IncidenceKey, QSharedPointer<KCalendarCore::Event>> m_incidences;
};

#endif // KCALENDARCORE_ANDROIDCALENDAR_H
