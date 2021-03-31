// This file is part of the kcalcore library.
// SPDX-FileCopyrightText: 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
// Contact: Alvaro Manera <alvaro.manera@nokia.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>

// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "kcalendarcore_export.h"

#include <KCalendarCore/Calendar>
#include <memory>

namespace KCalendarCore
{
/// @brief Additional metadata for calendar made available by plugins.
///
/// @sa CalendarPlugin
/// @since 5.83
class KCALENDARCORE_EXPORT CalendarMetadata
{
public:
    /// A shared pointer to a CalendarMetadata.
    typedef QSharedPointer<CalendarMetadata> Ptr;

    /// A shared pointer to a non-mutable CalendarMetadata.
    typedef QSharedPointer<const CalendarMetadata> ConstPtr;

    /// Constructs the metadata related to a calendar.
    explicit CalendarMetadata();

    /// Constructs the metadata related to a calendar with a name and description.
    explicit CalendarMetadata(const QString &name, const QString &description);

    /// Returns true if the calendar metadata is empty. This generally
    /// means that the metadata need to be fetched in a per event basis
    /// from the plugin.
    bool isNull() const;

    /// Returns the uid of the calendar.
    /// @see setUid().
    QString uid() const;

    /// Set the uid of the calendar.
    /// Typically called internally by the storage.
    /// @param uid unique identifier.
    void setUid(const QString &uid);

    /// Returns the name of the calendar.
    /// @see setName()
    QString name() const;

    /// Set the name of the calendar.
    /// @param name calendar name
    void setName(const QString &name);

    /// Returns the calendar description.
    /// @see setDescription().
    QString description() const;

    /// Set the description of the calendar.
    /// @param description calendar description.
    void setDescription(const QString &description);

    /// Returns the calendar color in the form of #RRGGBB.
    /// @see setColor().
    QString color() const;

    /// Set calendar color.
    /// @param color calendar color.
    void setColor(const QString &color);

    /// Returns true if calendar is read-only.
    /// @see setIsReadOnly().
    bool isReadOnly() const;

    /// Set calendar into read-only mode.
    /// This means that storages will not save any notes for the calendar.
    /// Typically used for showing shared calendars without write permission.

    /// @param isReadOnly true to set read-only mode
    void setIsReadOnly(bool isReadOnly);

    /// Gets the name of the plugin that created the calendar.
    /// @return The name of the plugin that owns the calendar.
    /// @see setPluginName().
    QString pluginName() const;

    /// Sets the plugin name that created the calendar (if any).
    /// @param pluginName The name of the plugin.
    /// @see pluginName();
    void setPluginName(const QString &pluginName);

    /// Gets the account associated with the calendar.
    /// @return The account.
    /// @see setAccount().
    QString account() const;

    /// Sets the account associated with the calendar (if any).
    /// @param account The account.
    /// @see account().
    void setAccount(const QString &account);

    /// Set a key/value property. Setting the value to the empty string
    /// will remove the property.
    /// @param key The name of the property.
    /// @param value The value of the property.
    void setCustomProperty(const QByteArray &key, const QString &value);

    /// A getter function for a custom property, see setCustomProperty().
    /// @param key The name of the property.
    /// @param default A default value if the property does not exists.
    QString customProperty(const QByteArray &key, const QString &defaultValue = QString()) const;

    /// List the keys of all stored custom properties.
    QList<QByteArray> customPropertyKeys() const;

    /// Assignment operator.
    CalendarMetadata &operator=(const CalendarMetadata &other);

    /// Compare this with calendar for equality.
    bool operator==(const CalendarMetadata &calendar) const;

private:
    class Private;
    std::unique_ptr<Private> d;
};
}
