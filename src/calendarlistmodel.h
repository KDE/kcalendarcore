/*
    SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCALENDARCORE_CALENDARLISTMODEL_H
#define KCALENDARCORE_CALENDARLISTMODEL_H

#include "kcalendarcore_export.h"

#include <QAbstractListModel>

#include <memory>

namespace KCalendarCore
{

class CalendarListModelPrivate;

/*!
 * \class KCalendarCore::CalendarListModel
 * \inmodule KCalendarCore
 * \inheaderfile KCalendarCore/CalendarListModel
 * \brief Model adaptor for KCalendarCore::CalendarPlugin::calendars().
 *
 * \since 6.4
 */
class KCALENDARCORE_EXPORT CalendarListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CalendarListModel(QObject *parent = nullptr);
    ~CalendarListModel();

    /*!
      \enum KCalendarCore::CalendarListModel::Role

      \value NameRole = Qt::DisplayRole
      Display name of the calendar.
      \value IconRole = Qt::DecorationRole
      The calendar icon, when available.
      \value CalendarRole = Qt::UserRole
      The KCalendarCore::Calendar calendar.
      \value AccessModeRole
      The access mode of the calendar (see KCalendarCore::AccessMode).
      \value IdRole
      The internal calendar id.
     */
    enum Role {
        NameRole = Qt::DisplayRole,
        IconRole = Qt::DecorationRole,
        CalendarRole = Qt::UserRole,
        AccessModeRole,
        IdRole,
    };

    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

private:
    std::unique_ptr<CalendarListModelPrivate> d;
};

}

#endif // KCALENDARCORE_CALENDARLISTMODEL_H
