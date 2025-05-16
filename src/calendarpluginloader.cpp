/*
    SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "calendarpluginloader.h"

#include <QCoreApplication>
#include <QDirIterator>
#include <QPluginLoader>
#include <QPointer>

using namespace KCalendarCore;

struct PluginLoader {
    PluginLoader();
    ~PluginLoader();
    QPointer<KCalendarCore::CalendarPlugin> plugin;
};

PluginLoader::PluginLoader()
{
    // static plugins
    const auto staticPluginData = QPluginLoader::staticPlugins();
    for (const auto &data : staticPluginData) {
        if (data.metaData().value(QLatin1String("IID")).toString() == QLatin1String("org.kde.kcalendarcore.CalendarPlugin")) {
            plugin = qobject_cast<KCalendarCore::CalendarPlugin *>(data.instance());
        }
        if (plugin) {
            break;
        }
    }

    // dynamic plugins
    if (!plugin) {
        QStringList searchPaths(QCoreApplication::applicationDirPath());
        searchPaths += QCoreApplication::libraryPaths();

        for (const auto &searchPath : std::as_const(searchPaths)) {
            const QString pluginPath = searchPath + QLatin1String("/kf6/org.kde.kcalendarcore.calendars");
            for (QDirIterator it(pluginPath, QDir::Files); it.hasNext() && !plugin;) {
                it.next();
                QPluginLoader loader(it.fileInfo().absoluteFilePath());
                if (loader.load()) {
                    plugin = qobject_cast<KCalendarCore::CalendarPlugin *>(loader.instance());
                } else {
                    qDebug() << loader.errorString();
                }
            }
        }
    }

    // Delete the plugin while QCoreApplication still exists
    // Without this destruction happens as part of the global static destruction
    // which can be after QCoreApplication is gone. Plugins with still pending KJobs
    // require that still to be present to destruct correctly.
    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, plugin, &QObject::deleteLater);
}

PluginLoader::~PluginLoader()
{
    delete plugin.get();
}

Q_GLOBAL_STATIC(PluginLoader, s_pluginLoader)

bool CalendarPluginLoader::hasPlugin()
{
    return (bool)s_pluginLoader->plugin;
}

KCalendarCore::CalendarPlugin *CalendarPluginLoader::plugin()
{
    return s_pluginLoader->plugin.get();
}

#include "moc_calendarpluginloader.cpp"
