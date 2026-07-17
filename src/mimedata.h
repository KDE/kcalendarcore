/*
  SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCALENDARCORE_MIMEDATA_H
#define KCALENDARCORE_MIMEDATA_H

#include "kcalendarcore_export.h"

#include "calendar.h"

#include <QStringList>

class QMimeData;

namespace KCalendarCore
{
/*!
 * \namespace MimeData
 *
 * Methods to interface between iCal data and QMimeData.
 *
 * \since 6.29
 */
namespace MimeData
{

/*!
 * Mimetype for generated QMimeData objects with iCal content.
 */
[[nodiscard]] KCALENDARCORE_EXPORT QString mimeType();

/*!
 * Mimetypes for iCal content that can be decoded.
 * For use in QAbstractItemModel::mimeTypes() for example.
 */
[[nodiscard]] KCALENDARCORE_EXPORT QStringList supportedMimeTypes();

/*!
 * Returns whether \p mimeData contains iCal content we can decode.
 */
[[nodiscard]] KCALENDARCORE_EXPORT bool canDecode(const QMimeData *mimeData);

/*!
 * Populate \p mimeData with the content of \p calendar.
 */
KCALENDARCORE_EXPORT void populate(QMimeData *mimeData, const KCalendarCore::Calendar::Ptr &cal);

/*!
 * Populate \p mimeData with \p incidences.
 */
KCALENDARCORE_EXPORT void populate(QMimeData *mimeData, const KCalendarCore::Incidence::List &incidences);

/*!
 * Create a calendar from \p mimeData.
 * If \p mimeData cannot be decoded, \c nullptr is returned.
 */
[[nodiscard]] KCALENDARCORE_EXPORT Calendar::Ptr decodeCalendar(const QMimeData *mimeData);

/*!
 * Create a list of incidences from \p mimeData.
 * If \p mimeData cannot be decoded an empty list is returned.
 */
[[nodiscard]] KCALENDARCORE_EXPORT Incidence::List decodeIncidences(const QMimeData *mimeData);

/*!
 * Create an event from \p mimeData.
 * If \p mimeData contains iCal data other than exactly one event, \c nullptr is returned.
 */
[[nodiscard]] KCALENDARCORE_EXPORT Event::Ptr decodeEvent(const QMimeData *mimeData);

/*!
 * Create a todo from \p mimeData.
 * If \p mimeData contains iCal data other than exactly one todo, \c nullptr is returned.
 */
[[nodiscard]] KCALENDARCORE_EXPORT Todo::Ptr decodeTodo(const QMimeData *mimeData);

}
}

#endif
