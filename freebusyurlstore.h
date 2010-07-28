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

#ifndef KCALCORE_FREEBUSYURLSTORE_H
#define KCALCORE_FREEBUSYURLSTORE_H

#include "kcalcore_export.h"
#include <QtCore/QString>

namespace KCalCore {

/**
  A class for reading and writing Free/Busy URLs to a cache file.
  Each URL is keyed by an email address.
*/
class KCALCORE_EXPORT FreeBusyUrlStore
{
  public:
    static FreeBusyUrlStore *self();
    ~FreeBusyUrlStore();

    void writeUrl( const QString &email, const QString &url );

    QString readUrl( const QString &email );

    void sync();

  private:
    FreeBusyUrlStore();

    //@cond PRIVATE
    Q_DISABLE_COPY( FreeBusyUrlStore )
    class Private;
    Private *const d;
    //@endcond
};

}

#endif
