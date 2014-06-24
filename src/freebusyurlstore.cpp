/*
  This file is part of the kcal library.

  Copyright (c) 2004 Cornelius Schumacher <schumacher@kde.org>

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
  defines the FreeBusyUrlStore class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/
#include "freebusyurlstore.h"

#include <KConfig>
#include <KConfigGroup>

#include <QtCore/QCoreApplication>
#include <QStandardPaths>

using namespace KCalCore;

//@cond PRIVATE
class FreeBusyUrlStore::Private
{
public:
    Private()
        : mConfig(0)
    {}
    ~Private()
    {
        qRemovePostRoutine(cleanupFreeBusyUrlStore);
    }
    KConfig *mConfig;

    static FreeBusyUrlStore *sSelf;
    static void cleanupFreeBusyUrlStore()
    {
        delete sSelf;
        sSelf = 0;
    }
};
FreeBusyUrlStore *FreeBusyUrlStore::Private::sSelf = 0;
//@endcond

FreeBusyUrlStore *FreeBusyUrlStore::self()
{
    static Private p;
    if (!p.sSelf) {
        p.sSelf = new FreeBusyUrlStore();
        qAddPostRoutine(Private::cleanupFreeBusyUrlStore);
    }
    return p.sSelf;
}

FreeBusyUrlStore::FreeBusyUrlStore() : d(new Private())
{
    QString configFile =
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1Char('/') + QStringLiteral("korganizer/freebusyurls");
    d->mConfig = new KConfig(configFile);
}

FreeBusyUrlStore::~FreeBusyUrlStore()
{
    delete d->mConfig;
    delete d;
}

void FreeBusyUrlStore::writeUrl(const QString &email, const QString &url)
{
    KConfigGroup group = d->mConfig->group(email);
    group.writeEntry("url", url);
}

QString FreeBusyUrlStore::readUrl(const QString &email)
{
    KConfigGroup group = d->mConfig->group(email);
    return group.readEntry("url");
}

void FreeBusyUrlStore::sync()
{
    d->mConfig->sync();
}
