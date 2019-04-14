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

#ifndef KCALCORE_UTILS_H
#define KCALCORE_UTILS_H

#include "kcalcore_export.h"

#include <QDateTime>

class QDataStream;

namespace KCalCore {

/**
 * Helpers to retain backwards compatibility of binary serialization.
 */
KCALCORE_EXPORT void serializeQDateTimeAsKDateTime(QDataStream &out, const QDateTime &dt);
KCALCORE_EXPORT void deserializeKDateTimeAsQDateTime(QDataStream &in, QDateTime &dt);

void serializeQDateTimeList(QDataStream &out, const QList<QDateTime> &list);
void deserializeQDateTimeList(QDataStream &in, QList<QDateTime> &list);

void serializeQTimeZoneAsSpec(QDataStream &out, const QTimeZone &tz);
void deserializeSpecAsQTimeZone(QDataStream &in, QTimeZone &tz);

}

#endif
