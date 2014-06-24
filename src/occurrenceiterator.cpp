/*
  This file is part of the kcalcore library.

  Copyright (C) 2013 Christian Mollekopf <mollekopf@kolabsys.com>

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
  defines the OccurrenceIterator class.

  @brief
  This class provides an iterator to iterate over all occurrences of incidences.

  @author Christian Mollekopf \<mollekopf@kolabsys.com\>
 */

#include "occurrenceiterator.h"
#include "calendar.h"
#include "calfilter.h"

#include <QDate>

using namespace KCalCore;

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class KCalCore::OccurrenceIterator::Private
{
public:
    Private(OccurrenceIterator *qq)
        : q(qq),
          occurrenceIt(occurrenceList)
    {
    }

    OccurrenceIterator *q;
    KDateTime start;
    KDateTime end;

    struct Occurrence
    {
        Occurrence()
        {
        }

        Occurrence(const Incidence::Ptr &i, const KDateTime &d)
            : incidence(i), date(d)
        {
        }

        Incidence::Ptr incidence;
        KDateTime date;
    };
    QList<Occurrence> occurrenceList;
    QListIterator<Occurrence> occurrenceIt;
    Occurrence current;

    /*
     * KCalCore::CalFilter can't handle individual occurrences.
     * When filtering completed to-dos, the CalFilter doesn't hide
     * them if it's a recurring to-do.
     */
    bool occurrenceIsHidden(const Calendar &calendar,
                            const Incidence::Ptr &inc,
                            const KDateTime &occurrenceDate)
    {
        if ((inc->type() == Incidence::TypeTodo) &&
                calendar.filter() &&
                (calendar.filter()->criteria() & KCalCore::CalFilter::HideCompletedTodos)) {
            if (inc->recurs()) {
                const Todo::Ptr todo = inc.staticCast<Todo>();
                if (todo && (occurrenceDate < todo->dtDue())) {
                    return true;
                }
            } else if (inc->hasRecurrenceId()) {
                const Todo::Ptr mainTodo = calendar.todo(inc->uid());
                if (mainTodo && mainTodo->isCompleted()) {
                    return true;
                }
            }
        }
        return false;
    }

    void setupIterator(const Calendar &calendar, const Incidence::List &incidences)
    {
        foreach(const Incidence::Ptr &inc, incidences) {
            if (inc->hasRecurrenceId()) {
                continue;
            }
            if (inc->recurs()) {
                QHash<KDateTime, Incidence::Ptr> recurrenceIds;
                KDateTime incidenceRecStart = inc->dateTime(Incidence::RoleRecurrenceStart);
                foreach(const Incidence::Ptr &exception, calendar.instances(inc)) {
                    if (incidenceRecStart.isValid())
                        recurrenceIds.insert(exception->recurrenceId().toTimeSpec(incidenceRecStart.timeSpec()), exception);
                }
                const bool isAllDay = inc->allDay();
                const DateTimeList occurrences = inc->recurrence()->timesInInterval(start, end);
                Incidence::Ptr incidence(inc);
                qint64 offset(0);
                foreach(KDateTime occurrenceDate, occurrences) {    //krazy:exclude=foreach
                    //timesInInterval generates always date-times,
                    //which is not what we want for all-day events
                    occurrenceDate.setDateOnly(isAllDay);

                    bool resetIncidence = false;
                    if (recurrenceIds.contains(occurrenceDate)) {
                        // TODO: exclude exceptions where the start/end is not within
                        // (so the occurrence of the recurrence is omitted, but no exception is added)
                        if (recurrenceIds.value(occurrenceDate)->status() == Incidence::StatusCanceled)
                            continue;

                        incidence = recurrenceIds.value(occurrenceDate);
                        occurrenceDate = incidence->dtStart();
                        resetIncidence = !incidence->thisAndFuture();
                        offset = incidence->recurrenceId().secsTo_long(incidence->dtStart());
                    } else if (inc != incidence) {   //thisAndFuture exception is active
                        occurrenceDate = occurrenceDate.addSecs(offset);
                    }
                    if (!occurrenceIsHidden(calendar, incidence, occurrenceDate)) {
                        occurrenceList << Private::Occurrence(incidence, occurrenceDate);
                    }
                    if (resetIncidence) {
                        incidence = inc;
                        offset = 0;
                    }
                }
            } else {
                occurrenceList << Private::Occurrence(inc, inc->dtStart());
            }
        }
        occurrenceIt = QListIterator<Private::Occurrence>(occurrenceList);
    }
};
//@endcond

static uint qHash(const KDateTime &dt)
{
    return qHash(dt.toString());
}

/**
 * Right now there is little point in the iterator, but:
 * With an iterator it should be possible to solve this more memory efficiently
 * and with immediate results at the beginning of the selected timeframe.
 * Either all events are iterated simoulatneously, resulting in occurrences
 * of all events in parallel in the correct time-order, or incidence after
 * incidence, which would be even more efficient.
 *
 * By making this class a friend of calendar, we could also use the internally
 * available data structures.
 */
OccurrenceIterator::OccurrenceIterator(const Calendar &calendar,
                                       const KDateTime &start,
                                       const KDateTime &end)
    : d(new KCalCore::OccurrenceIterator::Private(this))
{
    d->start = start;
    d->end = end;

    Event::List events = calendar.rawEvents(start.date(), end.date(), start.timeSpec());
    if (calendar.filter()) {
        calendar.filter()->apply(&events);
    }

    Todo::List todos = calendar.rawTodos(start.date(), end.date(), start.timeSpec());
    if (calendar.filter()) {
        calendar.filter()->apply(&todos);
    }

    Journal::List journals;
    const Journal::List allJournals = calendar.rawJournals();
    foreach(const KCalCore::Journal::Ptr &journal, allJournals) {
        const QDate journalStart = journal->dtStart().toTimeSpec(start.timeSpec()).date();
        if (journal->dtStart().isValid() &&
                journalStart >= start.date() &&
                journalStart <= end.date())
            journals << journal;
    }

    if (calendar.filter()) {
        calendar.filter()->apply(&journals);
    }

    const Incidence::List incidences =
        KCalCore::Calendar::mergeIncidenceList(events, todos, journals);
    d->setupIterator(calendar, incidences);
}

OccurrenceIterator::OccurrenceIterator(const Calendar &calendar,
                                       const Incidence::Ptr &incidence,
                                       const KDateTime &start,
                                       const KDateTime &end)
    : d(new KCalCore::OccurrenceIterator::Private(this))
{
    Q_ASSERT(incidence);
    d->start = start;
    d->end = end;
    d->setupIterator(calendar, Incidence::List() << incidence);
}

OccurrenceIterator::~OccurrenceIterator()
{
}

bool OccurrenceIterator::hasNext() const
{
    return d->occurrenceIt.hasNext();
}

void OccurrenceIterator::next()
{
    d->current = d->occurrenceIt.next();
}

Incidence::Ptr OccurrenceIterator::incidence() const
{
    return d->current.incidence;
}

KDateTime OccurrenceIterator::occurrenceStartDate() const
{
    return d->current.date;
}
