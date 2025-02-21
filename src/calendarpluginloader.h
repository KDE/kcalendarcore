/*
    SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCALENDARCORE_CALENDARPLUGINLOADER_H
#define KCALENDARCORE_CALENDARPLUGINLOADER_H

#include "kcalendarcore_export.h"

#include <KCalendarCore/CalendarPlugin>

namespace KCalendarCore
{

/*!
 * \qmlvaluetype calendarPluginLoader
 * \inqmlmodule org.kde.kcalendarcore
 * \nativetype KCalendarCore::CalendarPluginLoader
 * \brief
 * Provides access to a KCalendarCore::CalendarPlugin instance, if available.
 */

/*!
 * \class KCalendarCore::CalendarPluginLoader
 * \inmodule KCalendarCore
 * \inheaderfile KCalendarCore/CalendarPluginLoader
 * \brief
 * Provides access to a KCalendarCore::CalendarPlugin instance, if available.
 *
 * \since 5.97
 */
class KCALENDARCORE_EXPORT CalendarPluginLoader
{
    Q_GADGET

    /*!
     * \qmlproperty bool calendarPluginLoader::hasPlugin
     */

    /*!
     * \property KCalendarCore::CalendarPluginLoader::hasPlugin
     */
    Q_PROPERTY(bool hasPlugin READ hasPlugin)

    /*!
     * \qmlproperty KCalendarCore::CalendarPlugin calendarPluginLoader::plugin
     */

    /*!
     * \property KCalendarCore::CalendarPluginLoader::plugin
     */
    Q_PROPERTY(KCalendarCore::CalendarPlugin *plugin READ plugin)

public:
    /*! Returns \c true if there is a platform calendar available. */
    static bool hasPlugin();

    /*! Returns the platform calendar plugin. */
    static KCalendarCore::CalendarPlugin *plugin();
};

}

#endif // KCALENDARCORE_CALENDARPLUGINLOADER_H
