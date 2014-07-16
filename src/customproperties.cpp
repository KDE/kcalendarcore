/*
  This file is part of the kcalcore library.

  Copyright (c) 2002,2006,2010 David Jarvie <djarvie@kde.org>

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
  defines the CustomProperties class.

  @brief
  A class to manage custom calendar properties.

  @author David Jarvie \<djarvie@kde.org\>
*/

#include "customproperties.h"

#include <QDataStream>
#include <QDebug>

using namespace KCalCore;

//@cond PRIVATE
static bool checkName(const QByteArray &name);

class CustomProperties::Private
{
public:
    bool operator==(const Private &other) const;
    QMap<QByteArray, QString> mProperties;   // custom calendar properties
    QMap<QByteArray, QString> mPropertyParameters;

    // Volatile properties are not written back to the serialized format and are not compared in operator==
    // They are only used for runtime purposes and are not part of the payload.
    QMap<QByteArray, QString> mVolatileProperties;


    bool isVolatileProperty(const QString &name) const
    {
        return name.startsWith(QStringLiteral("X-KDE-VOLATILE"));
    }
};

bool CustomProperties::Private::operator==(const CustomProperties::Private &other) const
{
    if (mProperties.count() != other.mProperties.count()) {
        // qDebug() << "Property count is different:" << mProperties << other.mProperties;
        return false;
    }
    for (QMap<QByteArray, QString>::ConstIterator it = mProperties.begin();
            it != mProperties.end(); ++it) {
        QMap<QByteArray, QString>::ConstIterator itOther =
            other.mProperties.find(it.key());
        if (itOther == other.mProperties.end() || itOther.value() != it.value()) {
            return false;
        }
    }
    for (QMap<QByteArray, QString>::ConstIterator it = mPropertyParameters.begin();
            it != mPropertyParameters.end(); ++it) {
        QMap<QByteArray, QString>::ConstIterator itOther =
            other.mPropertyParameters.find(it.key());
        if (itOther == other.mPropertyParameters.end() || itOther.value() != it.value()) {
            return false;
        }
    }
    return true;
}
//@endcond

CustomProperties::CustomProperties()
    : d(new Private)
{
}

CustomProperties::CustomProperties(const CustomProperties &cp)
    : d(new Private(*cp.d))
{
}

CustomProperties &CustomProperties::operator=(const CustomProperties &other)
{
    // check for self assignment
    if (&other == this) {
        return *this;
    }

    *d = *other.d;
    return *this;
}

CustomProperties::~CustomProperties()
{
    delete d;
}

bool CustomProperties::operator==(const CustomProperties &other) const
{
    return *d == *other.d;
}

void CustomProperties::setCustomProperty(const QByteArray &app, const QByteArray &key,
        const QString &value)
{
    if (value.isNull() || key.isEmpty() || app.isEmpty()) {
        return;
    }
    QByteArray property = "X-KDE-" + app + '-' + key;
    if (!checkName(property)) {
        return;
    }
    customPropertyUpdate();

    if (d->isVolatileProperty(QLatin1String(property)))  {
        d->mVolatileProperties[property] = value;
    } else {
        d->mProperties[property] = value;
    }

    customPropertyUpdated();
}

void CustomProperties::removeCustomProperty(const QByteArray &app, const QByteArray &key)
{
    removeNonKDECustomProperty(QByteArray("X-KDE-" + app + '-' + key));
}

QString CustomProperties::customProperty(const QByteArray &app, const QByteArray &key) const
{
    return nonKDECustomProperty(QByteArray("X-KDE-" + app + '-' + key));
}

QByteArray CustomProperties::customPropertyName(const QByteArray &app, const QByteArray &key)
{
    QByteArray property("X-KDE-" + app + '-' + key);
    if (!checkName(property)) {
        return QByteArray();
    }
    return property;
}

void CustomProperties::setNonKDECustomProperty(const QByteArray &name, const QString &value,
        const QString &parameters)
{
    if (value.isNull() || !checkName(name)) {
        return;
    }
    customPropertyUpdate();
    d->mProperties[name] = value;
    d->mPropertyParameters[name] = parameters;
    customPropertyUpdated();
}
void CustomProperties::removeNonKDECustomProperty(const QByteArray &name)
{
    if (d->mProperties.contains(name)) {
        customPropertyUpdate();
        d->mProperties.remove(name);
        d->mPropertyParameters.remove(name);
        customPropertyUpdated();
    } else if (d->mVolatileProperties.contains(name)) {
        customPropertyUpdate();
        d->mVolatileProperties.remove(name);
        customPropertyUpdated();
    }
}

QString CustomProperties::nonKDECustomProperty(const QByteArray &name) const
{
    return d->isVolatileProperty(QLatin1String(name)) ? d->mVolatileProperties.value(name) : d->mProperties.value(name);
}

QString CustomProperties::nonKDECustomPropertyParameters(const QByteArray &name) const
{
    return d->mPropertyParameters.value(name);
}

void CustomProperties::setCustomProperties(const QMap<QByteArray, QString> &properties)
{
    bool changed = false;
    for (QMap<QByteArray, QString>::ConstIterator it = properties.begin();
            it != properties.end();  ++it) {
        // Validate the property name and convert any null string to empty string
        if (checkName(it.key())) {
            if (d->isVolatileProperty(QLatin1String(it.key()))) {
                d->mVolatileProperties[it.key()] = it.value().isNull() ? QStringLiteral("") : it.value();
            } else {
                d->mProperties[it.key()] = it.value().isNull() ? QStringLiteral("") : it.value();
            }
            if (!changed) {
                customPropertyUpdate();
            }
            changed = true;
        }
    }
    if (changed) {
        customPropertyUpdated();
    }
}

QMap<QByteArray, QString> CustomProperties::customProperties() const
{
    QMap<QByteArray, QString> result;
    result.unite(d->mProperties);
    result.unite(d->mVolatileProperties);

    return result;
}

void CustomProperties::customPropertyUpdate()
{
}

void CustomProperties::customPropertyUpdated()
{
}

void CustomProperties::virtual_hook(int id, void *data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);
    Q_ASSERT(false);
}

//@cond PRIVATE
bool checkName(const QByteArray &name)
{
    // Check that the property name starts with 'X-' and contains
    // only the permitted characters
    const char *n = name;
    int len = name.length();
    if (len < 2 ||  n[0] != 'X' || n[1] != '-') {
        return false;
    }
    for (int i = 2;  i < len;  ++i) {
        char ch = n[i];
        if ((ch >= 'A' && ch <= 'Z') ||
                (ch >= 'a' && ch <= 'z') ||
                (ch >= '0' && ch <= '9') ||
                ch == '-') {
            continue;
        }
        return false;   // invalid character found
    }
    return true;
}
//@endcond

QDataStream &KCalCore::operator<<(QDataStream &stream,
                                  const KCalCore::CustomProperties &properties)
{
    return stream << properties.d->mProperties
           << properties.d->mPropertyParameters;
}

QDataStream &KCalCore::operator>>(QDataStream &stream,
                                  KCalCore::CustomProperties &properties)
{
    properties.d->mVolatileProperties.clear();
    return stream >> properties.d->mProperties
           >> properties.d->mPropertyParameters;
}

