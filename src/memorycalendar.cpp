/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 1998 Preston Brown <pbrown@kde.org>
  SPDX-FileCopyrightText: 2001, 2003, 2004 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
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
#include "calformat.h"
#include "kcalendarcore_debug.h"

#include <QDate>

#include <functional>

using namespace KCalendarCore;

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class Q_DECL_HIDDEN KCalendarCore::MemoryCalendar::Private
{
private:
    static constexpr int incidenceTypeCount = 4;

public:
    Private(MemoryCalendar *qq)
        : q(qq)
        , mFormat(nullptr)
        , mUpdateLastModified(true)
    {
    }
    ~Private()
    {
    }

    MemoryCalendar *q;
    CalFormat *mFormat; // calendar format
    QString mIncidenceBeingUpdated; //  Instance identifier of Incidence currently being updated
    QDateTime mDtStartBeingUpdated; // dtStart field of the incidence being updated
    bool mUpdateLastModified; // Call setLastModified() on incidence modific ations

    /**
     * List of all incidences.
     * First indexed by incidence->type(), then by incidence->uid();
     */
    QMultiHash<QString, QSharedPointer<Incidence>> mIncidences[incidenceTypeCount];

    /**
     * Has all incidences, indexed by identifier.
     */
    QHash<QString, QSharedPointer<KCalendarCore::Incidence>> mIncidencesByIdentifier;

    /**
     * Contains incidences ( to-dos; non-recurring, non-multiday events; journals; )
     * indexed by start/due date.
     *
     * The QMap key is the incidence->type().
     * The QMultiHash key is the dtStart/dtDue() converted to calendar's timezone
     *
     * Note: We had 3 variables, mJournalsForDate, mTodosForDate and mEventsForDate
     * but i merged them into one (indexed by type) because it simplifies code using
     * it. No need to if else based on type.
     */
    QMultiHash<QDate, QSharedPointer<Incidence>> mIncidencesForDate[incidenceTypeCount];

    void insertIncidence(const QSharedPointer<Incidence> &incidence);

    QSharedPointer<Incidence> incidence(const QString &uid, IncidenceBase::IncidenceType type, const QDateTime &recurrenceId = {}) const;

    bool deleteIncidence(const QString &uid, IncidenceBase::IncidenceType type, const QDateTime &recurrenceId = {});

    void deleteAllIncidences(IncidenceBase::IncidenceType type);

    template<typename IncidenceType, typename Key>
    void forIncidences(const QMultiHash<Key, QSharedPointer<Incidence>> &incidences,
                       const Key &key,
                       std::function<void(const QSharedPointer<IncidenceType> &)> &&op) const
    {
        for (auto it = incidences.constFind(key), end = incidences.cend(); it != end && it.key() == key; ++it) {
            op(it.value().template staticCast<IncidenceType>());
        }
    }

    template<typename IncidenceType, typename Key>
    void forIncidences(const QMultiHash<Key, QSharedPointer<Incidence>> &incidences, std::function<void(const QSharedPointer<IncidenceType> &)> &&op) const
    {
        for (const auto &incidence : incidences) {
            op(incidence.template staticCast<IncidenceType>());
        }
    }

    template<typename IncidenceType>
    QList<QSharedPointer<IncidenceType>> castIncidenceList(const QMultiHash<QString, QSharedPointer<Incidence>> &incidences) const
    {
        QList<QSharedPointer<IncidenceType>> list;
        list.reserve(incidences.size());
        std::transform(incidences.cbegin(), incidences.cend(), std::back_inserter(list), [](const QSharedPointer<Incidence> &inc) {
            return inc.staticCast<IncidenceType>();
        });
        return list;
    }

    template<typename IncidenceType>
    QList<QSharedPointer<IncidenceType>> incidenceInstances(IncidenceBase::IncidenceType type, const QSharedPointer<Incidence> &incidence) const
    {
        QList<QSharedPointer<IncidenceType>> list;
        forIncidences<IncidenceType, QString>(mIncidences[type], incidence->uid(), [&list](const QSharedPointer<IncidenceType> &incidence) {
            if (incidence->hasRecurrenceId()) {
                list.push_back(incidence);
            }
        });
        return list;
    }

    QSharedPointer<Incidence>
    findIncidence(const QMultiHash<QString, QSharedPointer<Incidence>> &incidences, const QString &uid, const QDateTime &recurrenceId) const
    {
        for (auto it = incidences.constFind(uid), end = incidences.cend(); it != end && it.key() == uid; ++it) {
            const auto &incidence = it.value();
            if (recurrenceId.isNull() && !incidence->hasRecurrenceId()) {
                return incidence;
            } else if (!recurrenceId.isNull() && incidence->hasRecurrenceId() && recurrenceId == incidence->recurrenceId()) {
                return incidence;
            }
        }
        return {};
    }
};
//@endcond

MemoryCalendar::MemoryCalendar(const QTimeZone &timeZone)
    : Calendar(timeZone)
    , d(new KCalendarCore::MemoryCalendar::Private(this))
{
}

MemoryCalendar::MemoryCalendar(const QByteArray &timeZoneId)
    : Calendar(timeZoneId)
    , d(new KCalendarCore::MemoryCalendar::Private(this))
{
}

MemoryCalendar::~MemoryCalendar()
{
    setObserversEnabled(false);

    // Don't call the virtual function deleteEvents() etc, the base class might have
    // other ways of deleting the data.
    d->deleteAllIncidences(Incidence::TypeEvent);
    d->deleteAllIncidences(Incidence::TypeTodo);
    d->deleteAllIncidences(Incidence::TypeJournal);

    d->mIncidencesByIdentifier.clear();

    setModified(false);

    setObserversEnabled(true);

    delete d;
}

void MemoryCalendar::doSetTimeZone(const QTimeZone &timeZone)
{
    // Reset date based hashes before storing for the new zone.
    for (auto &table : d->mIncidencesForDate) {
        table.clear();
    }

    for (auto &table : d->mIncidences) {
        for (const auto &incidence : table) {
            const QDateTime dt = incidence->dateTime(Incidence::RoleCalendarHashing);
            if (dt.isValid()) {
                d->mIncidencesForDate[incidence->type()].insert(dt.toTimeZone(timeZone).date(), incidence);
            }
        }
    }
}

bool MemoryCalendar::deleteIncidence(const QSharedPointer<Incidence> &incidence)
{
    // Notify while the incidence is still available,
    // this is necessary so korganizer still has time to query for exceptions
    notifyIncidenceAboutToBeDeleted(incidence);
    incidence->unRegisterObserver(this);
    const Incidence::IncidenceType type = incidence->type();
    const QString &uid = incidence->uid();
    bool deleted = d->deleteIncidence(uid, type, incidence->recurrenceId());
    if (deleted) {
        setModified(true);

        // Delete child-incidences.
        if (!incidence->hasRecurrenceId() && incidence->recurs()) {
            deleteIncidenceInstances(incidence);
        }
    } else {
        qCWarning(KCALCORE_LOG) << incidence->typeStr() << " not found. uid=" << uid;
    }
    notifyIncidenceDeleted(incidence);
    return deleted;
}

bool MemoryCalendar::deleteIncidenceInstances(const QSharedPointer<Incidence> &incidence)
{
    QList<QSharedPointer<Incidence>> instances;
    for (auto it = d->mIncidences[incidence->type()].constFind(incidence->uid()), end = d->mIncidences[incidence->type()].constEnd();
         it != end && it.key() == incidence->uid();
         ++it) {
        if (it.value()->hasRecurrenceId()) {
            qCDebug(KCALCORE_LOG) << "deleting child"
                                  << ", type=" << int(incidence->type()) << ", uid="
                                  << incidence->uid()
                                  //                   << ", start=" << i->dtStart()
                                  << " from calendar";
            // Don't call deleteIncidence() now since it's modifying the
            // mIncidences map we're iterating over.
            instances.append(it.value());
        }
    }
    for (QSharedPointer<Incidence> &instance : instances) {
        deleteIncidence(instance);
    }

    return true;
}

//@cond PRIVATE
bool MemoryCalendar::Private::deleteIncidence(const QString &uid, IncidenceBase::IncidenceType type, const QDateTime &recurrenceId)
{
    for (auto it = mIncidences[type].find(uid), end = mIncidences[type].end(); it != end && it.key() == uid; ++it) {
        QSharedPointer<Incidence> incidence = it.value();
        if (recurrenceId.isNull() && incidence->hasRecurrenceId()) {
            continue;
        } else if (!recurrenceId.isNull() && (!incidence->hasRecurrenceId() || recurrenceId != incidence->recurrenceId())) {
            continue;
        }
        mIncidences[type].erase(it);
        mIncidencesByIdentifier.remove(incidence->instanceIdentifier());
        const QDateTime dt = incidence->dateTime(Incidence::RoleCalendarHashing);
        if (dt.isValid()) {
            mIncidencesForDate[type].remove(dt.toTimeZone(q->timeZone()).date(), incidence);
        }
        return true;
    }
    return false;
}

void MemoryCalendar::Private::deleteAllIncidences(Incidence::IncidenceType incidenceType)
{
    for (auto &incidence : mIncidences[incidenceType]) {
        q->notifyIncidenceAboutToBeDeleted(incidence);
        incidence->unRegisterObserver(q);
    }
    mIncidences[incidenceType].clear();
    mIncidencesForDate[incidenceType].clear();
}

QSharedPointer<Incidence> MemoryCalendar::Private::incidence(const QString &uid, Incidence::IncidenceType type, const QDateTime &recurrenceId) const
{
    return findIncidence(mIncidences[type], uid, recurrenceId);
}

void MemoryCalendar::Private::insertIncidence(const QSharedPointer<Incidence> &incidence)
{
    const QString uid = incidence->uid();
    const Incidence::IncidenceType type = incidence->type();
    if (!mIncidences[type].contains(uid, incidence)) {
        mIncidences[type].insert(uid, incidence);
        mIncidencesByIdentifier.insert(incidence->instanceIdentifier(), incidence);
        const QDateTime dt = incidence->dateTime(Incidence::RoleCalendarHashing);
        if (dt.isValid()) {
            mIncidencesForDate[type].insert(dt.toTimeZone(q->timeZone()).date(), incidence);
        }

    } else {
        // if we already have an to-do with this UID, it must be the same incidence,
        // otherwise something's really broken
        qCWarning(KCALCORE_LOG) << "Calendar already contains an incidence of type" << type << "with UID" << uid << ", not inserting it again";
        const auto existing = mIncidences[type].value(uid);
        if (existing != incidence) {
            qCWarning(KCALCORE_LOG) << "The new incidence is not the same as the existing incidence!";
            qCWarning(KCALCORE_LOG) << "The existing incidence is summary=" << existing->summary() << ", start=" << existing->dtStart();
            qCWarning(KCALCORE_LOG) << "The new incidence is summary=" << incidence->summary() << ", start=" << incidence->dtStart();
        }
        Q_ASSERT(existing == incidence);
    }
}
//@endcond

bool MemoryCalendar::addIncidence(const QSharedPointer<Incidence> &incidence)
{
    d->insertIncidence(incidence);

    notifyIncidenceAdded(incidence);

    incidence->registerObserver(this);

    setModified(true);

    return true;
}

bool MemoryCalendar::addEvent(const QSharedPointer<Event> &event)
{
    return addIncidence(event);
}

bool MemoryCalendar::deleteEvent(const QSharedPointer<Event> &event)
{
    return deleteIncidence(event);
}

bool MemoryCalendar::deleteEventInstances(const QSharedPointer<Event> &event)
{
    return deleteIncidenceInstances(event);
}

QSharedPointer<Event> MemoryCalendar::event(const QString &uid, const QDateTime &recurrenceId) const
{
    return d->incidence(uid, Incidence::TypeEvent, recurrenceId).staticCast<Event>();
}

bool MemoryCalendar::addTodo(const QSharedPointer<Todo> &todo)
{
    return addIncidence(todo);
}

bool MemoryCalendar::deleteTodo(const QSharedPointer<Todo> &todo)
{
    return deleteIncidence(todo);
}

bool MemoryCalendar::deleteTodoInstances(const QSharedPointer<Todo> &todo)
{
    return deleteIncidenceInstances(todo);
}

QSharedPointer<Todo> MemoryCalendar::todo(const QString &uid, const QDateTime &recurrenceId) const
{
    return d->incidence(uid, Incidence::TypeTodo, recurrenceId).staticCast<Todo>();
}

QList<QSharedPointer<Todo>> MemoryCalendar::rawTodos(TodoSortField sortField, SortDirection sortDirection) const
{
    return Calendar::sortTodos(d->castIncidenceList<Todo>(d->mIncidences[Incidence::TypeTodo]), sortField, sortDirection);
}

QList<QSharedPointer<Todo>> MemoryCalendar::todoInstances(const QSharedPointer<Incidence> &todo, TodoSortField sortField, SortDirection sortDirection) const
{
    return Calendar::sortTodos(d->incidenceInstances<Todo>(Incidence::TypeTodo, todo), sortField, sortDirection);
}

QList<QSharedPointer<Todo>> MemoryCalendar::rawTodosForDate(const QDate &date) const
{
    QList<QSharedPointer<Todo>> todoList;

    d->forIncidences<Todo>(d->mIncidencesForDate[Incidence::TypeTodo], date, [&todoList](const QSharedPointer<Todo> &todo) {
        todoList.append(todo);
    });

    // Iterate over all todos. Look for recurring todoss that occur on this date
    d->forIncidences<Todo>(d->mIncidences[Incidence::TypeTodo], [this, &todoList, &date](const QSharedPointer<Todo> &todo) {
        if (todo->recurs() && todo->recursOn(date, timeZone())) {
            todoList.append(todo);
        }
    });

    return todoList;
}

QList<QSharedPointer<Todo>> MemoryCalendar::rawTodos(const QDate &start, const QDate &end, const QTimeZone &timeZone, bool inclusive) const
{
    Q_UNUSED(inclusive); // use only exact dtDue/dtStart, not dtStart and dtEnd

    QList<QSharedPointer<Todo>> todoList;
    const auto ts = timeZone.isValid() ? timeZone : this->timeZone();
    QDateTime st(start, QTime(0, 0, 0), ts);
    QDateTime nd(end, QTime(23, 59, 59, 999), ts);

    // Get todos
    for (const auto &incidence : std::as_const(d->mIncidences[Incidence::TypeTodo])) {
        const auto todo = incidence.staticCast<Todo>();

        QDateTime rStart = todo->hasDueDate() ? todo->dtDue() : todo->hasStartDate() ? todo->dtStart() : QDateTime();
        if (!rStart.isValid()) {
            continue;
        }

        if (!todo->recurs()) { // non-recurring todos
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
        } // if(recurs)

        todoList.append(todo);
    }

    return todoList;
}

QList<QSharedPointer<Alarm>> MemoryCalendar::alarms(const QDateTime &from, const QDateTime &to, bool excludeBlockedAlarms) const
{
    Q_UNUSED(excludeBlockedAlarms);
    QList<QSharedPointer<Alarm>> alarmList;

    d->forIncidences<Event>(d->mIncidences[Incidence::TypeEvent], [this, &alarmList, &from, &to](const QSharedPointer<Event> &e) {
        if (e->recurs()) {
            appendRecurringAlarms(alarmList, e, from, to);
        } else {
            appendAlarms(alarmList, e, from, to);
        }
    });

    d->forIncidences<Todo>(d->mIncidences[IncidenceBase::TypeTodo], [this, &alarmList, &from, &to](const QSharedPointer<Todo> &t) {
        if (!t->isCompleted()) {
            appendAlarms(alarmList, t, from, to);
            if (t->recurs()) {
                appendRecurringAlarms(alarmList, t, from, to);
            } else {
                appendAlarms(alarmList, t, from, to);
            }
        }
    });

    return alarmList;
}

bool MemoryCalendar::updateLastModifiedOnChange() const
{
    return d->mUpdateLastModified;
}

void MemoryCalendar::setUpdateLastModifiedOnChange(bool update)
{
    d->mUpdateLastModified = update;
}

void MemoryCalendar::incidenceUpdate(const QString &uid, const QDateTime &recurrenceId)
{
    QSharedPointer<Incidence> inc = incidence(uid, recurrenceId);

    if (inc) {
        if (!d->mIncidenceBeingUpdated.isEmpty()) {
            qCWarning(KCALCORE_LOG) << "Incidence::update() called twice without an updated() call in between.";
        }

        // Save it so we can detect changes to uid or recurringId.
        d->mIncidenceBeingUpdated = inc->instanceIdentifier();

        // Save dtStart so we can detect a change and apply it to exceptions.
        d->mDtStartBeingUpdated = inc->dtStart();

        const QDateTime dt = inc->dateTime(Incidence::RoleCalendarHashing);
        if (dt.isValid()) {
            d->mIncidencesForDate[inc->type()].remove(dt.toTimeZone(timeZone()).date(), inc);
        }
    }
}

void MemoryCalendar::incidenceUpdated(const QString &uid, const QDateTime &recurrenceId)
{
    QSharedPointer<Incidence> inc = incidence(uid, recurrenceId);

    if (inc) {
        if (d->mIncidenceBeingUpdated.isEmpty()) {
            qCWarning(KCALCORE_LOG) << "Incidence::updated() called twice without an update() call in between.";
        } else if (inc->instanceIdentifier() != d->mIncidenceBeingUpdated) {
            // Instance identifier changed, update our hash table
            d->mIncidencesByIdentifier.remove(d->mIncidenceBeingUpdated);
            d->mIncidencesByIdentifier.insert(inc->instanceIdentifier(), inc);
        }

        d->mIncidenceBeingUpdated = QString();

        if (d->mUpdateLastModified) {
            inc->setLastModified(QDateTime::currentDateTimeUtc());
        }
        // we should probably update the revision number here,
        // or internally in the Event itself when certain things change.
        // need to verify with ical documentation.

        const QDateTime dt = inc->dateTime(Incidence::RoleCalendarHashing);
        if (dt.isValid()) {
            d->mIncidencesForDate[inc->type()].insert(dt.toTimeZone(timeZone()).date(), inc);
        }

        // When dstart changes, move recurrence ids of exception accordingly.
        if (inc->recurs() && inc->dtStart() != d->mDtStartBeingUpdated) {
            const Duration delta(d->mDtStartBeingUpdated, inc->dtStart());
            for (QSharedPointer<Incidence> &exception : instances(inc)) {
                exception->setRecurrenceId(delta.end(exception->recurrenceId()));
            }
        }
        d->mDtStartBeingUpdated = QDateTime();

        notifyIncidenceChanged(inc);

        setModified(true);
    }
}

QList<QSharedPointer<Event>>
MemoryCalendar::rawEventsForDate(const QDate &date, const QTimeZone &timeZone, EventSortField sortField, SortDirection sortDirection) const
{
    QList<QSharedPointer<Event>> eventList;

    if (!date.isValid()) {
        // There can't be events on invalid dates
        return eventList;
    }

    if (timeZone.isValid() && timeZone != this->timeZone()) {
        // We cannot use the hash table on date, since time zone is different.
        eventList = rawEvents(date, date, timeZone, false);
        return Calendar::sortEvents(std::move(eventList), sortField, sortDirection);
    }

    // Iterate over all non-recurring, single-day events that start on this date
    d->forIncidences<Event>(d->mIncidencesForDate[Incidence::TypeEvent], date, [&eventList](const QSharedPointer<Event> &event) {
        eventList.append(event);
    });

    // Iterate over all events. Look for recurring events that occur on this date
    const auto ts = timeZone.isValid() ? timeZone : this->timeZone();
    for (const auto &event : std::as_const(d->mIncidences[Incidence::TypeEvent])) {
        const auto ev = event.staticCast<Event>();
        if (ev->recurs()) {
            if (ev->isMultiDay()) {
                int extraDays = ev->dtStart().date().daysTo(ev->dtEnd().date());
                for (int i = 0; i <= extraDays; ++i) {
                    if (ev->recursOn(date.addDays(-i), ts)) {
                        eventList.append(ev);
                        break;
                    }
                }
            } else {
                if (ev->recursOn(date, ts)) {
                    eventList.append(ev);
                }
            }
        } else {
            if (ev->isMultiDay()) {
                if (ev->dtStart().toTimeZone(ts).date() <= date && ev->dtEnd().toTimeZone(ts).date() >= date) {
                    eventList.append(ev);
                }
            }
        }
    }

    return Calendar::sortEvents(std::move(eventList), sortField, sortDirection);
}

QList<QSharedPointer<Event>> MemoryCalendar::rawEvents(const QDate &start, const QDate &end, const QTimeZone &timeZone, bool inclusive) const
{
    QList<QSharedPointer<Event>> eventList;
    const auto ts = timeZone.isValid() ? timeZone : this->timeZone();
    QDateTime st(start, QTime(0, 0, 0), ts);
    QDateTime nd(end, QTime(23, 59, 59, 999), ts);

    // Get non-recurring events
    for (const auto &e : std::as_const(d->mIncidences[Incidence::TypeEvent])) {
        const auto event = e.staticCast<Event>();
        QDateTime rStart = event->dtStart();
        if (nd.isValid() && nd < rStart) {
            continue;
        }
        if (inclusive && st.isValid() && rStart < st) {
            continue;
        }

        if (!event->recurs()) { // non-recurring events
            QDateTime rEnd = event->dtEnd();
            if (st.isValid() && rEnd < st) {
                continue;
            }
            if (inclusive && nd.isValid() && nd < rEnd) {
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
                if (st.isValid() && rEnd < st) {
                    continue;
                }
                if (inclusive && nd.isValid() && nd < rEnd) {
                    continue;
                }
                break;
            } // switch(duration)
        } // if(recurs)

        eventList.append(event);
    }

    return eventList;
}

QList<QSharedPointer<Event>> MemoryCalendar::rawEvents(EventSortField sortField, SortDirection sortDirection) const
{
    return Calendar::sortEvents(d->castIncidenceList<Event>(d->mIncidences[Incidence::TypeEvent]), sortField, sortDirection);
}

QList<QSharedPointer<Event>> MemoryCalendar::eventInstances(const QSharedPointer<Incidence> &event, EventSortField sortField, SortDirection sortDirection) const
{
    return Calendar::sortEvents(d->incidenceInstances<Event>(Incidence::TypeEvent, event), sortField, sortDirection);
}

bool MemoryCalendar::addJournal(const QSharedPointer<Journal> &journal)
{
    return addIncidence(journal);
}

bool MemoryCalendar::deleteJournal(const QSharedPointer<Journal> &journal)
{
    return deleteIncidence(journal);
}

bool MemoryCalendar::deleteJournalInstances(const QSharedPointer<Journal> &journal)
{
    return deleteIncidenceInstances(journal);
}

QSharedPointer<Journal> MemoryCalendar::journal(const QString &uid, const QDateTime &recurrenceId) const
{
    return d->incidence(uid, Incidence::TypeJournal, recurrenceId).staticCast<Journal>();
}

QList<QSharedPointer<Journal>> MemoryCalendar::rawJournals(JournalSortField sortField, SortDirection sortDirection) const
{
    return Calendar::sortJournals(d->castIncidenceList<Journal>(d->mIncidences[Incidence::TypeJournal]), sortField, sortDirection);
}

QList<QSharedPointer<Journal>>
MemoryCalendar::journalInstances(const QSharedPointer<Incidence> &journal, JournalSortField sortField, SortDirection sortDirection) const
{
    return Calendar::sortJournals(d->incidenceInstances<Journal>(Incidence::TypeJournal, journal), sortField, sortDirection);
}

QList<QSharedPointer<Journal>> MemoryCalendar::rawJournalsForDate(const QDate &date) const
{
    QList<QSharedPointer<Journal>> journalList;

    d->forIncidences<Journal>(d->mIncidencesForDate[Incidence::TypeJournal], date, [&journalList](const QSharedPointer<Journal> &journal) {
        journalList.append(journal);
    });

    return journalList;
}

QSharedPointer<Incidence> MemoryCalendar::instance(const QString &identifier) const
{
    return d->mIncidencesByIdentifier.value(identifier);
}

void MemoryCalendar::virtual_hook(int id, void *data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);
    Q_ASSERT(false);
}

#include "moc_memorycalendar.cpp"
