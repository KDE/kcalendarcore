/*
    SPDX-FileCopyrightText: 2024 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCALENDARCORE_QML_TYPES
#define KCALENDARCORE_QML_TYPES

#include <KCalendarCore/Calendar>
#include <KCalendarCore/CalendarListModel>

#include <QQmlEngine>

struct CalendarListModelForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(CalendarListModel)
    QML_FOREIGN(KCalendarCore::CalendarListModel)
};

namespace KCalendarCoreForeign
{
Q_NAMESPACE
QML_NAMED_ELEMENT(KCalendarCore)
QML_FOREIGN_NAMESPACE(KCalendarCore)
};

#endif
