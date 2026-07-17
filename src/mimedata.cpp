/*
  SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mimedata.h"

#include "icalformat.h"
#include "memorycalendar.h"
#include "vcalformat.h"

#include <QMimeData>

using namespace Qt::Literals;
using namespace KCalendarCore;

QString MimeData::mimeType()
{
    return u"text/calendar"_s;
}

[[nodiscard]] static QString vcalMimeType()
{
    return u"text/x-vCalendar"_s;
}

QStringList MimeData::supportedMimeTypes()
{
    static const QStringList l{mimeType(), vcalMimeType()};
    return l;
}

bool MimeData::canDecode(const QMimeData *mimeData)
{
    if (!mimeData) {
        return false;
    }
    const auto supportedTypes = supportedMimeTypes();
    return std::ranges::any_of(supportedTypes, [mimeData](const QString &mt) {
        return mimeData->hasFormat(mt);
    });
}

void MimeData::populate(QMimeData *mimeData, const KCalendarCore::Calendar::Ptr &calendar)
{
    Q_ASSERT(mimeData);
    ICalFormat format;
    const auto encoded = format.toString(calendar);
    if (!encoded.isEmpty()) {
        mimeData->setData(mimeType(), encoded.toUtf8());
    }
}

void MimeData::populate(QMimeData *mimeData, const KCalendarCore::Incidence::List &incidences)
{
    if (incidences.isEmpty()) {
        return;
    }

    const Calendar::Ptr calendar(new MemoryCalendar(QTimeZone::systemTimeZone()));
    std::ranges::for_each(incidences, [&calendar](const auto &i) {
        calendar->addIncidence(Incidence::Ptr(i->clone()));
    });
    populate(mimeData, calendar);
}

Calendar::Ptr MimeData::decodeCalendar(const QMimeData *mimeData)
{
    if (!mimeData) {
        return {};
    }

    Calendar::Ptr calendar(new MemoryCalendar(QTimeZone::systemTimeZone()));
    if (mimeData->hasFormat(mimeType())) {
        const QByteArray payload = mimeData->data(mimeType());
        ICalFormat format;
        if (!format.fromRawString(calendar, payload)) {
            return {};
        }
        return calendar;
    }

    if (mimeData->hasFormat(vcalMimeType())) {
        const QByteArray payload = mimeData->data(mimeType());
        VCalFormat format;
        if (!format.fromRawString(calendar, payload)) {
            return {};
        }
        return calendar;
    }

    return {};
}

Incidence::List MimeData::decodeIncidences(const QMimeData *mimeData)
{
    const auto cal = decodeCalendar(mimeData);
    if (!cal) {
        return {};
    }

    KCalendarCore::Incidence::List result;
    const KCalendarCore::Incidence::List decodedIncidences = cal->incidences();
    result.reserve(decodedIncidences.size());
    for (const KCalendarCore::Incidence::Ptr &i : decodedIncidences) {
        result.push_back(KCalendarCore::Incidence::Ptr(i->clone()));
    }

    return result;
}

Event::Ptr MimeData::decodeEvent(const QMimeData *mimeData)
{
    const auto cal = decodeCalendar(mimeData);
    if (!cal) {
        return nullptr;
    }
    const auto events = cal->events();
    return events.size() == 1 ? Event::Ptr(events.front()->clone()) : nullptr;
}

Todo::Ptr MimeData::decodeTodo(const QMimeData *mimeData)
{
    const auto cal = decodeCalendar(mimeData);
    if (!cal) {
        return nullptr;
    }
    const auto todos = cal->todos();
    return todos.size() == 1 ? Todo::Ptr(todos.front()->clone()) : nullptr;
}
