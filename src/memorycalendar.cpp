/*
  This file is part of the kcalcore library.

  Copyright (c) 1998 Preston Brown <pbrown@kde.org>
  Copyright (c) 2001,2003,2004 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

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
/**
  @file
  This file is part of the API for handling calendar data and
  defines the MemoryCalendar class.

  @brief
  This class provides a calendar stored as a local file.

  @author Preston Brown \<pbrown@kde.org\>
  @author Cornelius Schumacher \<schumacher@kde.org\>
 */

#include "memorycalendar.h"
#include "kcalcore_debug.h"
#include "utils.h"

#include <QDate>
#include <KDateTime>

template <typename K, typename V>
static QVector<V> values(const QMultiHash<K, V> &c)
{
    QVector<V> v;
    v.reserve(c.size());
    for (typename QMultiHash<K, V>::const_iterator it = c.begin(), end = c.end(); it != end; ++it) {
        v.push_back(it.value());
    }
    return v;
}

template <typename K, typename V>
static  QVector<V> values(const QMultiHash<K, V> &c, const K &x)
{
    QVector<V> v;
    typename QMultiHash<K, V>::const_iterator it = c.find(x);
    while (it != c.end() && it.key() == x) {
        v.push_back(it.value());
        ++it;
    }
    return v;
}

using namespace KCalCore;

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class Q_DECL_HIDDEN KCalCore::MemoryCalendar::Private
{
public:
    Private(MemoryCalendar *qq)
        : q(qq), mFormat(nullptr)
    {
    }
    ~Private()
    {
    }

    MemoryCalendar *q;
    CalFormat *mFormat;                    // calendar format
    QString mIncidenceBeingUpdated;        //  Instance identifier of Incidence currently being updated

    /**
     * List of all incidences.
     * First indexed by incidence->type(), then by incidence->uid();
     */
    QMap<IncidenceBase::IncidenceType, QMultiHash<QString, Incidence::Ptr> > mIncidences;

    /**
     * Has all incidences, indexed by identifier.
     */
    QHash<QString, KCalCore::Incidence::Ptr> mIncidencesByIdentifier;

    /**
     * List of all deleted incidences.
     * First indexed by incidence->type(), then by incidence->uid();
     */
    QMap<IncidenceBase::IncidenceType, QMultiHash<QString, Incidence::Ptr> > mDeletedIncidences;

    /**
     * Contains incidences ( to-dos; non-recurring, non-multiday events; journals; )
     * indexed by start/due date.
     *
     * The QMap key is the incidence->type().
     * The QMultiHash key is the dtStart/dtDue().toString()
     *
     * Note: We had 3 variables, mJournalsForDate, mTodosForDate and mEventsForDate
     * but i merged them into one (indexed by type) because it simplifies code using
     * it. No need to if else based on type.
     */
    QMap<IncidenceBase::IncidenceType, QMultiHash<QString, IncidenceBase::Ptr> > mIncidencesForDate;

    void insertIncidence(const Incidence::Ptr &incidence);

    Incidence::Ptr incidence(const QString &uid,
                             const IncidenceBase::IncidenceType type,
                             const QDateTime &recurrenceId = {}) const;

    Incidence::Ptr deletedIncidence(const QString &uid,
                                    const QDateTime &recurrenceId,
                                    const IncidenceBase::IncidenceType type) const;

    void deleteAllIncidences(const IncidenceBase::IncidenceType type);

};
//@endcond

MemoryCalendar::MemoryCalendar(const QTimeZone &timeZone)
    : Calendar(timeZone),
      d(new KCalCore::MemoryCalendar::Private(this))
{
}

MemoryCalendar::MemoryCalendar(const QByteArray &timeZoneId)
    : Calendar(timeZoneId),
      d(new KCalCore::MemoryCalendar::Private(this))
{
}

MemoryCalendar::~MemoryCalendar()
{
    close();
    delete d;
}

void MemoryCalendar::close()
{
    setObserversEnabled(false);

    // Don't call the virtual function deleteEvents() etc, the base class might have
    // other ways of deleting the data.
    d->deleteAllIncidences(Incidence::TypeEvent);
    d->deleteAllIncidences(Incidence::TypeTodo);
    d->deleteAllIncidences(Incidence::TypeJournal);

    d->mIncidencesByIdentifier.clear();
    d->mDeletedIncidences.clear();

    setModified(false);

    setObserversEnabled(true);
}

bool MemoryCalendar::deleteIncidence(const Incidence::Ptr &incidence)
{
    // Handle orphaned children
    // relations is an Incidence's property, not a Todo's, so
    // we remove relations in deleteIncidence, not in deleteTodo.
    removeRelations(incidence);
    const Incidence::IncidenceType type = incidence->type();
    const QString uid = incidence->uid();
    if (d->mIncidences[type].contains(uid, incidence)) {
        // Notify while the incidence is still available,
        // this is necessary so korganizer still has time to query for exceptions
        notifyIncidenceAboutToBeDeleted(incidence);

        d->mIncidences[type].remove(uid, incidence);
        d->mIncidencesByIdentifier.remove(incidence->instanceIdentifier());
        setModified(true);
        notifyIncidenceDeletedOld(incidence);
        if (deletionTracking()) {
            d->mDeletedIncidences[type].insert(uid, incidence);
        }

        const KDateTime dt = incidence->dateTime(Incidence::RoleCalendarHashing);
        if (dt.isValid()) {
            d->mIncidencesForDate[type].remove(dt.date().toString(), incidence);
        }
        // Delete child-incidences.
        if (!incidence->hasRecurrenceId()) {
            deleteIncidenceInstances(incidence);
        }
        notifyIncidenceDeleted(incidence);
        return true;
    } else {
        qCWarning(KCALCORE_LOG) << incidence->typeStr() << " not found. uid=" << uid;
        return false;
    }
}

bool MemoryCalendar::deleteIncidenceInstances(const Incidence::Ptr &incidence)
{
    const Incidence::IncidenceType type = incidence->type();
    Incidence::List values = ::values(d->mIncidences[type], incidence->uid());
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        Incidence::Ptr i = *it;
        if (i->hasRecurrenceId()) {
            qCDebug(KCALCORE_LOG) << "deleting child"
                                  << ", type=" << int(type)
                                  << ", uid=" << i->uid()
//                   << ", start=" << i->dtStart()
                                  << " from calendar";
            deleteIncidence(i);
        }
    }

    return true;
}

//@cond PRIVATE
void MemoryCalendar::Private::deleteAllIncidences(const Incidence::IncidenceType incidenceType)
{
    QHashIterator<QString, Incidence::Ptr>i(mIncidences[incidenceType]);
    while (i.hasNext()) {
        i.next();
        q->notifyIncidenceAboutToBeDeleted(i.value());
        i.value()->unRegisterObserver(q);
    }
    mIncidences[incidenceType].clear();
    mIncidencesForDate[incidenceType].clear();
}

Incidence::Ptr MemoryCalendar::Private::incidence(const QString &uid,
        const Incidence::IncidenceType type,
        const QDateTime &recurrenceId) const
{
    Incidence::List values = ::values(mIncidences[type], uid);
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        Incidence::Ptr i = *it;
        if (recurrenceId.isNull()) {
            if (!i->hasRecurrenceId()) {
                return i;
            }
        } else {
            if (i->hasRecurrenceId() && i->recurrenceId() == recurrenceId) {
                return i;
            }
        }
    }
    return Incidence::Ptr();
}

Incidence::Ptr
MemoryCalendar::Private::deletedIncidence(const QString &uid,
    const QDateTime &recurrenceId,
        const IncidenceBase::IncidenceType type) const
{
    if (!q->deletionTracking()) {
        return Incidence::Ptr();
    }

    Incidence::List values = ::values(mDeletedIncidences[type], uid);
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        Incidence::Ptr i = *it;
        if (recurrenceId.isNull()) {
            if (!i->hasRecurrenceId()) {
                return i;
            }
        } else {
            if (i->hasRecurrenceId() && i->recurrenceId() == recurrenceId) {
                return i;
            }
        }
    }
    return Incidence::Ptr();
}

void MemoryCalendar::Private::insertIncidence(const Incidence::Ptr &incidence)
{
    const QString uid = incidence->uid();
    const Incidence::IncidenceType type = incidence->type();
    if (!mIncidences[type].contains(uid, incidence)) {
        mIncidences[type].insert(uid, incidence);
        mIncidencesByIdentifier.insert(incidence->instanceIdentifier(), incidence);
        const KDateTime dt = incidence->dateTime(Incidence::RoleCalendarHashing);
        if (dt.isValid()) {
            mIncidencesForDate[type].insert(dt.date().toString(), incidence);
        }

    } else {
#ifndef NDEBUG
        // if we already have an to-do with this UID, it must be the same incidence,
        // otherwise something's really broken
        Q_ASSERT(mIncidences[type].value(uid) == incidence);
#endif
    }
}
//@endcond

bool MemoryCalendar::addIncidence(const Incidence::Ptr &incidence)
{
    d->insertIncidence(incidence);

    notifyIncidenceAdded(incidence);

    incidence->registerObserver(this);

    setupRelations(incidence);

    setModified(true);

    return true;
}

bool MemoryCalendar::addEvent(const Event::Ptr &event)
{
    return addIncidence(event);
}

bool MemoryCalendar::deleteEvent(const Event::Ptr &event)
{
    return deleteIncidence(event);
}

bool MemoryCalendar::deleteEventInstances(const Event::Ptr &event)
{
    return deleteIncidenceInstances(event);
}

Event::Ptr MemoryCalendar::event(const QString &uid,
                                 const QDateTime &recurrenceId) const
{
    return d->incidence(uid, Incidence::TypeEvent, recurrenceId).staticCast<Event>();
}

Event::Ptr MemoryCalendar::deletedEvent(const QString &uid, const QDateTime &recurrenceId) const
{
    return d->deletedIncidence(uid, recurrenceId, Incidence::TypeEvent).staticCast<Event>();
}

bool MemoryCalendar::addTodo(const Todo::Ptr &todo)
{
    return addIncidence(todo);
}

bool MemoryCalendar::deleteTodo(const Todo::Ptr &todo)
{
    return deleteIncidence(todo);
}

bool MemoryCalendar::deleteTodoInstances(const Todo::Ptr &todo)
{
    return deleteIncidenceInstances(todo);
}

Todo::Ptr MemoryCalendar::todo(const QString &uid,
                               const QDateTime &recurrenceId) const
{
    return d->incidence(uid, Incidence::TypeTodo, recurrenceId).staticCast<Todo>();
}

Todo::Ptr MemoryCalendar::deletedTodo(const QString &uid,
                                      const QDateTime &recurrenceId) const
{
    return d->deletedIncidence(uid, recurrenceId, Incidence::TypeTodo).staticCast<Todo>();
}

Todo::List MemoryCalendar::rawTodos(TodoSortField sortField,
                                    SortDirection sortDirection) const
{
    Todo::List todoList;
    todoList.reserve(d->mIncidences[Incidence::TypeTodo].count());
    QHashIterator<QString, Incidence::Ptr>i(d->mIncidences[Incidence::TypeTodo]);
    while (i.hasNext()) {
        i.next();
        todoList.append(i.value().staticCast<Todo>());
    }
    return Calendar::sortTodos(todoList, sortField, sortDirection);
}

Todo::List MemoryCalendar::deletedTodos(TodoSortField sortField,
                                        SortDirection sortDirection) const
{
    if (!deletionTracking()) {
        return Todo::List();
    }

    Todo::List todoList;
    todoList.reserve(d->mDeletedIncidences[Incidence::TypeTodo].count());
    QHashIterator<QString, Incidence::Ptr >i(d->mDeletedIncidences[Incidence::TypeTodo]);
    while (i.hasNext()) {
        i.next();
        todoList.append(i.value().staticCast<Todo>());
    }
    return Calendar::sortTodos(todoList, sortField, sortDirection);
}

Todo::List MemoryCalendar::todoInstances(const Incidence::Ptr &todo,
        TodoSortField sortField,
        SortDirection sortDirection) const
{
    Todo::List list;

    Incidence::List values = ::values(d->mIncidences[Incidence::TypeTodo], todo->uid());
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        Todo::Ptr t = (*it).staticCast<Todo>();
        if (t->hasRecurrenceId()) {
            list.append(t);
        }
    }
    return Calendar::sortTodos(list, sortField, sortDirection);
}

Todo::List MemoryCalendar::rawTodosForDate(const QDate &date) const
{
    Todo::List todoList;
    Todo::Ptr t;

    const QString dateStr = date.toString();
    QMultiHash<QString, IncidenceBase::Ptr >::const_iterator it =
        d->mIncidencesForDate[Incidence::TypeTodo].constFind(dateStr);
    while (it != d->mIncidencesForDate[Incidence::TypeTodo].constEnd() && it.key() == dateStr) {
        t = it.value().staticCast<Todo>();
        todoList.append(t);
        ++it;
    }

    // Iterate over all todos. Look for recurring todoss that occur on this date
    QHashIterator<QString, Incidence::Ptr >i(d->mIncidences[Incidence::TypeTodo]);
    while (i.hasNext()) {
        i.next();
        t = i.value().staticCast<Todo>();
        if (t->recurs()) {
            if (t->recursOn(date, zoneToSpec(timeZone()))) {
                todoList.append(t);
            }
        }
    }

    return todoList;
}

Todo::List MemoryCalendar::rawTodos(const QDate &start,
                                    const QDate &end,
                                    const QTimeZone &timeZone,
                                    bool inclusive) const
{
    Q_UNUSED(inclusive);   // use only exact dtDue/dtStart, not dtStart and dtEnd

    Todo::List todoList;
    const auto ts = timeZone.isValid() ? timeZone : this->timeZone();
    QDateTime st(start, QTime(0, 0, 0), ts);
    QDateTime nd(end, QTime(23, 59, 59, 999), ts);

    // Get todos
    QHashIterator<QString, Incidence::Ptr >i(d->mIncidences[Incidence::TypeTodo]);
    Todo::Ptr todo;
    while (i.hasNext()) {
        i.next();
        todo = i.value().staticCast<Todo>();
        if (!isVisible(todo)) {
            continue;
        }

        QDateTime rStart = todo->hasDueDate() ? k2q(todo->dtDue()) :
                           todo->hasStartDate() ? k2q(todo->dtStart()) : QDateTime();
        if (!rStart.isValid()) {
            continue;
        }

        if (!todo->recurs()) {   // non-recurring todos
            if (nd.isValid() && nd < rStart) {
                continue;
            }
            if (st.isValid() && rStart < st) {
                continue;
            }
        } else { // recurring events
            switch (todo->recurrence()->duration()) {
            case -1: // infinite
                break;
            case 0: // end date given
            default: // count given
                QDateTime rEnd(todo->recurrence()->endDate(), QTime(23, 59, 59, 999), ts);
                if (!rEnd.isValid()) {
                    continue;
                }
                if (st.isValid() && rEnd < st) {
                    continue;
                }
                break;
            } // switch(duration)
        } //if(recurs)

        todoList.append(todo);
    }

    return todoList;
}

Alarm::List MemoryCalendar::alarmsTo(const QDateTime &to) const
{
    return alarms(QDateTime(QDate(1900, 1, 1), QTime(0, 0, 0)), to);
}

Alarm::List MemoryCalendar::alarms(const QDateTime &from, const QDateTime &to, bool excludeBlockedAlarms) const
{
    Q_UNUSED(excludeBlockedAlarms);
    Alarm::List alarmList;
    QHashIterator<QString, Incidence::Ptr>ie(d->mIncidences[Incidence::TypeEvent]);
    Event::Ptr e;
    while (ie.hasNext()) {
        ie.next();
        e = ie.value().staticCast<Event>();
        if (e->recurs()) {
            appendRecurringAlarms(alarmList, e, from, to);
        } else {
            appendAlarms(alarmList, e, from, to);
        }
    }

    QHashIterator<QString, Incidence::Ptr>it(d->mIncidences[Incidence::TypeTodo]);
    Todo::Ptr t;
    while (it.hasNext()) {
        it.next();
        t = it.value().staticCast<Todo>();

        if (!t->isCompleted()) {
            appendAlarms(alarmList, t, from, to);
            if (t->recurs()) {
                appendRecurringAlarms(alarmList, t, from, to);
            } else {
                appendAlarms(alarmList, t, from, to);
            }
        }
    }

    return alarmList;
}

void MemoryCalendar::incidenceUpdate(const QString &uid, const QDateTime &recurrenceId)
{
    Incidence::Ptr inc = incidence(uid, recurrenceId);

    if (inc) {
        if (!d->mIncidenceBeingUpdated.isEmpty()) {
            qCWarning(KCALCORE_LOG) << "Incidence::update() called twice without an updated() call in between.";
        }

        // Save it so we can detect changes to uid or recurringId.
        d->mIncidenceBeingUpdated = inc->instanceIdentifier();

        const KDateTime dt = inc->dateTime(Incidence::RoleCalendarHashing);
        if (dt.isValid()) {
            const Incidence::IncidenceType type = inc->type();
            d->mIncidencesForDate[type].remove(dt.date().toString(), inc);
        }
    }
}

void MemoryCalendar::incidenceUpdated(const QString &uid, const QDateTime &recurrenceId)
{
    Incidence::Ptr inc = incidence(uid, recurrenceId);

    if (inc) {

        if (d->mIncidenceBeingUpdated.isEmpty()) {
            qCWarning(KCALCORE_LOG) << "Incidence::updated() called twice without an update() call in between.";
        } else if (inc->instanceIdentifier() != d->mIncidenceBeingUpdated) {
            // Instance identifier changed, update our hash table
            d->mIncidencesByIdentifier.remove(d->mIncidenceBeingUpdated);
            d->mIncidencesByIdentifier.insert(inc->instanceIdentifier(), inc);
        }

        d->mIncidenceBeingUpdated = QString();

        inc->setLastModified(KDateTime::currentUtcDateTime());
        // we should probably update the revision number here,
        // or internally in the Event itself when certain things change.
        // need to verify with ical documentation.

        const KDateTime dt = inc->dateTime(Incidence::RoleCalendarHashing);
        if (dt.isValid()) {
            const Incidence::IncidenceType type = inc->type();
            d->mIncidencesForDate[type].insert(dt.date().toString(), inc);
        }

        notifyIncidenceChanged(inc);

        setModified(true);
    }
}

Event::List MemoryCalendar::rawEventsForDate(const QDate &date,
        const QTimeZone &timeZone,
        EventSortField sortField,
        SortDirection sortDirection) const
{
    Event::List eventList;

    if (!date.isValid()) {
        // There can't be events on invalid dates
        return eventList;
    }

    Event::Ptr ev;

    // Find the hash for the specified date
    const QString dateStr = date.toString();
    QMultiHash<QString, IncidenceBase::Ptr >::const_iterator it =
        d->mIncidencesForDate[Incidence::TypeEvent].constFind(dateStr);
    // Iterate over all non-recurring, single-day events that start on this date
    const auto ts = timeZone.isValid() ? timeZone : this->timeZone();
    while (it != d->mIncidencesForDate[Incidence::TypeEvent].constEnd() && it.key() == dateStr) {
        ev = it.value().staticCast<Event>();
        KDateTime end(ev->dtEnd().toTimeSpec(ev->dtStart()));
        if (ev->allDay()) {
            end.setDateOnly(true);
        } else {
            end = end.addSecs(-1);
        }
        if (end.date() >= date) {
            eventList.append(ev);
        }
        ++it;
    }

    // Iterate over all events. Look for recurring events that occur on this date
    QHashIterator<QString, Incidence::Ptr>i(d->mIncidences[Incidence::TypeEvent]);
    while (i.hasNext()) {
        i.next();
        ev = i.value().staticCast<Event>();
        if (ev->recurs()) {
            if (ev->isMultiDay()) {
                int extraDays = ev->dtStart().date().daysTo(ev->dtEnd().date());
                for (int i = 0; i <= extraDays; ++i) {
                    if (ev->recursOn(date.addDays(-i), zoneToSpec(ts))) {
                        eventList.append(ev);
                        break;
                    }
                }
            } else {
                if (ev->recursOn(date, zoneToSpec(ts))) {
                    eventList.append(ev);
                }
            }
        } else {
            if (ev->isMultiDay()) {
                if (ev->dtStart().date() <= date && ev->dtEnd().date() >= date) {
                    eventList.append(ev);
                }
            }
        }
    }

    return Calendar::sortEvents(eventList, sortField, sortDirection);
}

Event::List MemoryCalendar::rawEvents(const QDate &start,
                                      const QDate &end,
                                      const QTimeZone &timeZone,
                                      bool inclusive) const
{
    Event::List eventList;
    const auto ts = timeZone.isValid() ? timeZone : this->timeZone();
    QDateTime st(start, QTime(0, 0, 0), ts);
    QDateTime nd(end, QTime(23, 59, 59, 999), ts);
    QDateTime yesterStart = st.addDays(-1);

    // Get non-recurring events
    QHashIterator<QString, Incidence::Ptr>i(d->mIncidences[Incidence::TypeEvent]);
    Event::Ptr event;
    while (i.hasNext()) {
        i.next();
        event = i.value().staticCast<Event>();
        QDateTime rStart = k2q(event->dtStart());
        if (nd < rStart) {
            continue;
        }
        if (inclusive && rStart < st) {
            continue;
        }

        if (!event->recurs()) {   // non-recurring events
            QDateTime rEnd = k2q(event->dtEnd());
            if (rEnd < st) {
                continue;
            }
            if (inclusive && nd < rEnd) {
                continue;
            }
        } else { // recurring events
            switch (event->recurrence()->duration()) {
            case -1: // infinite
                if (inclusive) {
                    continue;
                }
                break;
            case 0: // end date given
            default: // count given
                QDateTime rEnd(event->recurrence()->endDate(), QTime(23, 59, 59, 999), ts);
                if (!rEnd.isValid()) {
                    continue;
                }
                if (rEnd < st) {
                    continue;
                }
                if (inclusive && nd < rEnd) {
                    continue;
                }
                break;
            } // switch(duration)
        } //if(recurs)

        eventList.append(event);
    }

    return eventList;
}

Event::List MemoryCalendar::rawEventsForDate(const QDateTime &kdt) const
{
    return rawEventsForDate(kdt.date(), kdt.timeZone());
}

Event::List MemoryCalendar::rawEvents(EventSortField sortField,
                                      SortDirection sortDirection) const
{
    Event::List eventList;
    eventList.reserve(d->mIncidences[Incidence::TypeEvent].count());
    QHashIterator<QString, Incidence::Ptr> i(d->mIncidences[Incidence::TypeEvent]);
    while (i.hasNext()) {
        i.next();
        eventList.append(i.value().staticCast<Event>());
    }
    return Calendar::sortEvents(eventList, sortField, sortDirection);
}

Event::List MemoryCalendar::deletedEvents(EventSortField sortField,
        SortDirection sortDirection) const
{
    if (!deletionTracking()) {
        return Event::List();
    }

    Event::List eventList;
    eventList.reserve(d->mDeletedIncidences[Incidence::TypeEvent].count());
    QHashIterator<QString, Incidence::Ptr>i(d->mDeletedIncidences[Incidence::TypeEvent]);
    while (i.hasNext()) {
        i.next();
        eventList.append(i.value().staticCast<Event>());
    }
    return Calendar::sortEvents(eventList, sortField, sortDirection);
}

Event::List MemoryCalendar::eventInstances(const Incidence::Ptr &event,
        EventSortField sortField,
        SortDirection sortDirection) const
{
    Event::List list;

    Incidence::List values = ::values(d->mIncidences[Incidence::TypeEvent], event->uid());
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        Event::Ptr ev = (*it).staticCast<Event>();
        if (ev->hasRecurrenceId()) {
            list.append(ev);
        }
    }
    return Calendar::sortEvents(list, sortField, sortDirection);
}

bool MemoryCalendar::addJournal(const Journal::Ptr &journal)
{
    return addIncidence(journal);
}

bool MemoryCalendar::deleteJournal(const Journal::Ptr &journal)
{
    return deleteIncidence(journal);
}

bool MemoryCalendar::deleteJournalInstances(const Journal::Ptr &journal)
{
    return deleteIncidenceInstances(journal);
}

Journal::Ptr MemoryCalendar::journal(const QString &uid,
                                     const QDateTime &recurrenceId) const
{
    return d->incidence(uid, Incidence::TypeJournal, recurrenceId).staticCast<Journal>();
}

Journal::Ptr MemoryCalendar::deletedJournal(const QString &uid, const QDateTime &recurrenceId) const
{
    return d->deletedIncidence(uid, recurrenceId, Incidence::TypeJournal).staticCast<Journal>();
}

Journal::List MemoryCalendar::rawJournals(JournalSortField sortField,
        SortDirection sortDirection) const
{
    Journal::List journalList;
    QHashIterator<QString, Incidence::Ptr>i(d->mIncidences[Incidence::TypeJournal]);
    while (i.hasNext()) {
        i.next();
        journalList.append(i.value().staticCast<Journal>());
    }
    return Calendar::sortJournals(journalList, sortField, sortDirection);
}

Journal::List MemoryCalendar::deletedJournals(JournalSortField sortField,
        SortDirection sortDirection) const
{
    if (!deletionTracking()) {
        return Journal::List();
    }

    Journal::List journalList;
    journalList.reserve(d->mDeletedIncidences[Incidence::TypeJournal].count());
    QHashIterator<QString, Incidence::Ptr>i(d->mDeletedIncidences[Incidence::TypeJournal]);
    while (i.hasNext()) {
        i.next();
        journalList.append(i.value().staticCast<Journal>());
    }
    return Calendar::sortJournals(journalList, sortField, sortDirection);
}

Journal::List MemoryCalendar::journalInstances(const Incidence::Ptr &journal,
        JournalSortField sortField,
        SortDirection sortDirection) const
{
    Journal::List list;

    Incidence::List values = ::values(d->mIncidences[Incidence::TypeJournal], journal->uid());
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        Journal::Ptr j = (*it).staticCast<Journal>();
        if (j->hasRecurrenceId()) {
            list.append(j);
        }
    }
    return Calendar::sortJournals(list, sortField, sortDirection);
}

Journal::List MemoryCalendar::rawJournalsForDate(const QDate &date) const
{
    Journal::List journalList;
    Journal::Ptr j;

    QString dateStr = date.toString();
    QMultiHash<QString, IncidenceBase::Ptr >::const_iterator it =
        d->mIncidencesForDate[Incidence::TypeJournal].constFind(dateStr);

    while (it != d->mIncidencesForDate[Incidence::TypeJournal].constEnd() && it.key() == dateStr) {
        j = it.value().staticCast<Journal>();
        journalList.append(j);
        ++it;
    }
    return journalList;
}

Incidence::Ptr MemoryCalendar::instance(const QString &identifier) const
{
    return d->mIncidencesByIdentifier.value(identifier);
}

void MemoryCalendar::virtual_hook(int id, void *data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);
    Q_ASSERT(false);
}
