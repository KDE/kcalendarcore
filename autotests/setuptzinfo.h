/*
  This file is part of the kcalcore library.

  Copyright (c) 2017 Pino Toscano <pino@kde.org>

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

#ifndef SETUPTZINFO_H
#define SETUPTZINFO_H

#include <QStandardPaths>

#include <KConfig>
#include <KConfigGroup>

class SetupTzinfo
{
public:
    SetupTzinfo()
    {
        QStandardPaths::setTestModeEnabled(true);
#ifdef Q_OS_UNIX
        KConfig conf(QLatin1String("ktimezonedrc"));
        KConfigGroup group(&conf, QLatin1String("TimeZones"));
        group.writeEntry("ZoneinfoDir", "/usr/share/zoneinfo");
        group.writeEntry("LocalZone", "Europe/Vienna");
        group.writeEntry("Zonetab", "/usr/share/zoneinfo/zone.tab");
#endif
    }
};

#endif
