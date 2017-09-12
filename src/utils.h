/*
  This file is part of the kcalcore library.

  Copyright (c) 2017  Daniel Vrátil <dvratil@kde.org>

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

#ifndef KCALCORE_UTILS_H_
#define KCALCORE_UTILS_H_

#include "kcalcore_export.h"

#include <QDateTime>
#include <KDateTime>

class QDataStream;

namespace KCalCore {

// TODO: PORTING-helper only, remove once KDateTime is gone
/**
 * Applies KDateTime::Spec to given QDateTime and returns a modified QDateTime
 */
KCALCORE_EXPORT QDateTime applySpec(const QDateTime &dt, const KDateTime::Spec &spec, bool isAllDay);

/**
 * Helpers to retain backwards compatibility of binary serialization.
 */
void serializeQDateTimeAsKDateTime(QDataStream &out, const QDateTime &dt);
void deserializeKDateTimeAsQDateTime(QDataStream &in, QDateTime &dt);

void serializeQDateTimeSortableList(QDataStream &out, const SortableList<QDateTime> &list);
void deserializeQDateTimeSortableList(QDataStream &in, SortableList<QDateTime> &list);

void serializeQTimeZoneAsSpec(QDataStream &out, const QTimeZone &tz);
void deserializeSpecAsQTimeZone(QDataStream &in, QTimeZone &tz);

/** Convert a QTimeZone to a KDateTime::Spec */
KCALCORE_EXPORT KDateTime::Spec zoneToSpec(const QTimeZone &zone);

/** Convert a QTimeZone to a KDateTime::Spec */
KCALCORE_EXPORT QTimeZone specToZone(const KDateTime::Spec &spec);

/** Convert KDateTime to QDateTime, correctly preserves timespec */
KCALCORE_EXPORT QDateTime k2q(const KDateTime &kdt);
KCALCORE_EXPORT KDateTime q2k(const QDateTime &qdt);

}

#endif
