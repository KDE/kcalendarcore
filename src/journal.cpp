/*
  This file is part of the kcalcore library.

  Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

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
  defines the Journal class.

  @brief
  Provides a Journal in the sense of RFC2445.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/

#include "journal.h"
#include "visitor.h"

#include "kcalendarcore_debug.h"

using namespace KCalendarCore;

Journal::Journal() : d(nullptr)
{
}

Journal::Journal(const Journal&) = default;
Journal::~Journal() = default;

Incidence::IncidenceType Journal::type() const
{
    return TypeJournal;
}

QByteArray Journal::typeStr() const
{
    return QByteArrayLiteral("Journal");
}

Journal *Journal::clone() const
{
    return new Journal(*this);
}

IncidenceBase &Journal::assign(const IncidenceBase &other)
{
    Incidence::assign(other);
    return *this;
}

bool Journal::equals(const IncidenceBase &journal) const
{
    return Incidence::equals(journal);
}

bool Journal::accept(Visitor &v, const IncidenceBase::Ptr &incidence)
{
    return v.visit(incidence.staticCast<Journal>());
}

QDateTime Journal::dateTime(DateTimeRole role) const
{
    switch (role) {
    case RoleEnd:
    case RoleEndTimeZone:
        return QDateTime();
    case RoleDisplayStart:
    case RoleDisplayEnd:
        return dtStart();
    default:
        return dtStart();
    }
}

void Journal::setDateTime(const QDateTime &dateTime, DateTimeRole role)
{
    switch (role) {
    case RoleDnD: {
        setDtStart(dateTime);
        break;
    }
    default:
        qCDebug(KCALCORE_LOG) << "Unhandled role" << role;
    }
}

void Journal::virtual_hook(VirtualHook id, void *data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);
}

QLatin1String Journal::mimeType() const
{
    return Journal::journalMimeType();
}

/* static */
QLatin1String Journal::journalMimeType()
{
    return QLatin1String("application/x-vnd.akonadi.calendar.journal");
}

QLatin1String Journal::iconName(const QDateTime &) const
{
    return QLatin1String("view-pim-journal");
}

void Journal::serialize(QDataStream &out) const
{
    Incidence::serialize(out);
}

void Journal::deserialize(QDataStream &in)
{
    Incidence::deserialize(in);
}

bool Journal::supportsGroupwareCommunication() const
{
    return false;
}
