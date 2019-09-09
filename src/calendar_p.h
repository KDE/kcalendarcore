/*
  This file is part of the kcalcore library.

  Copyright (c) 1998 Preston Brown <pbrown@kde.org>
  Copyright (c) 2000-2004 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  Copyright (c) 2006 David Jarvie <djarvie@kde.org>

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

#ifndef KCALCORE_CALENDAR_P_H
#define KCALCORE_CALENDAR_P_H

#include "calendar.h"
#include "calfilter.h"

namespace KCalendarCore {

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class Q_DECL_HIDDEN Calendar::Private
{
public:
    Private() :
          mModified(false),
          mNewObserver(false),
          mObserversEnabled(true),
          mDefaultFilter(new CalFilter),
          batchAddingInProgress(false),
          mDeletionTracking(true)
    {
        // Setup default filter, which does nothing
        mFilter = mDefaultFilter;
        mFilter->setEnabled(false);

        mOwner.setName(QStringLiteral("Unknown Name"));
        mOwner.setEmail(QStringLiteral("unknown@nowhere"));
    }

    ~Private()
    {
        if (mFilter != mDefaultFilter) {
            delete mFilter;
        }
        delete mDefaultFilter;
    }
    QTimeZone timeZoneIdSpec(const QByteArray &timeZoneId);

    QString mProductId;
    Person mOwner;
    QTimeZone mTimeZone;
    QVector<QTimeZone> mTimeZones;
    bool mModified = false;
    bool mNewObserver = false;
    bool mObserversEnabled = false;
    QList<CalendarObserver *> mObservers;

    CalFilter *mDefaultFilter = nullptr;
    CalFilter *mFilter = nullptr;

    // These lists are used to put together related To-dos
    QMultiHash<QString, Incidence::Ptr> mOrphans;
    QMultiHash<QString, Incidence::Ptr> mOrphanUids;

    // Lists for associating incidences to notebooks
    QMultiHash<QString, Incidence::Ptr > mNotebookIncidences;
    QHash<QString, QString> mUidToNotebook;
    QHash<QString, bool> mNotebooks; // name to visibility
    QHash<Incidence::Ptr, bool> mIncidenceVisibility; // incidence -> visibility
    QString mDefaultNotebook; // uid of default notebook
    QMap<QString, Incidence::List > mIncidenceRelations;
    bool batchAddingInProgress = false;
    bool mDeletionTracking = false;
};

}

#endif
