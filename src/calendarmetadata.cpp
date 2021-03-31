// This file is part of the kcalcore library.
// SPDX-FileCopyrightText: 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
// Contact: Alvaro Manera <alvaro.manera@nokia.com>
// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>

#include "calendarmetadata.h"

using namespace KCalendarCore;

class CalendarMetadata::Private
{
public:
    Private()
        : mUid(QString())
        , mPluginName(QString())
        , mAccount(QString())
    {}

    Private(const Private &other)
        : mUid(other.mUid)
        , mName(other.mName)
        , mDescription(other.mDescription)
        , mColor(other.mColor)
        , mReadOnly(false)
        , mNull(other.mNull)
        , mPluginName(other.mPluginName)
        , mAccount(other.mAccount),
        , mCustomProperties(other.mCustomProperties)
    {}

    QString mUid;
    QString mName;
    QString mDescription;
    QString mColor;
    bool mReadOnly;
    bool mNull;
    QString mPluginName;
    QString mAccount;
    QHash<QByteArray, QString> mCustomProperties;
};


CalendarMetadata::CalendarMetadata()
    : d(std::unique_ptr<CalendarMetadata::Private>(new CalendarMetadata::Private()))
{
}

CalendarMetadata::CalendarMetadata(const QString &name, const QString &description)
    : d(std::unique_ptr<CalendarMetadata::Private>(new CalendarMetadata::Private()))
{
    setName(name);
    setDescription(description);
}

QString CalendarMetadata::uid() const
{
    return d->mUid;
}

void CalendarMetadata::setUid(const QString &uid)
{
    d->mUid = uid;
}

QString CalendarMetadata::name() const
{
    return d->mName;
}

void CalendarMetadata::setName(const QString &name)
{
    d->mName = name;
}

QString CalendarMetadata::description() const
{
    return d->mDescription;
}

void CalendarMetadata::setDescription(const QString &description)
{
    d->mDescription = description;
}

QString CalendarMetadata::color() const
{
    return d->mColor;
}

void CalendarMetadata::setColor(const QString &color)
{
    d->mColor = color;
}


bool CalendarMetadata::isNull() const
{
    return d->mNull;
}

bool CalendarMetadata::isReadOnly() const
{
    return d->mReadOnly;
}

void CalendarMetadata::setIsReadOnly(bool isReadOnly)
{
    d->mReadOnly = isReadOnly;
}

QString CalendarMetadata::pluginName() const
{
    return d->mPluginName;
}

void CalendarMetadata::setPluginName(const QString &pluginName)
{
    d->mPluginName = pluginName;
}

QString CalendarMetadata::account() const
{
    return d->mAccount;
}

void CalendarMetadata::setAccount(const QString &account)
{
    d->mAccount = account;
}


void CalendarMetadata::setCustomProperty(const QByteArray &key, const QString &value)
{
    if (value.isEmpty()) {
        d->mCustomProperties.remove(key);
    } else {
        d->mCustomProperties.insert(key, value);
    }
}

QString CalendarMetadata::customProperty(const QByteArray &key, const QString &defaultValue) const
{
    return d->mCustomProperties.value(key, defaultValue);
}

QList<QByteArray> CalendarMetadata::customPropertyKeys() const
{
    return d->mCustomProperties.keys();
}

CalendarMetadata &CalendarMetadata::operator=(const CalendarMetadata &other)
{
    // check for self assignment
    if (&other == this) {
        return *this;
    }
    *d = *other.d;
    return *this;
}

bool CalendarMetadata::operator==(const CalendarMetadata &i2) const
{
    return
        d->mUid == i2.uid() &&
        d->mName == i2.name() &&
        d->mDescription == i2.description() &&
        d->mColor == i2.color() &&
        d->mNull == i2.isNull() &&
        d->mReadOnly == i2.isReadOnly() &&
        d->mPluginName == i2.pluginName();
}
