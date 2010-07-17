/*
  This file is part of the kcalcore library.

  Copyright (c) 2001,2004 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2004 Reinhold Kainhofer <reinhold@kainhofer.com>

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

#include "schedulemessage.h"
#include "incidencebase.h"

#include <QtCore/QString>

using namespace KCalCore;

//@cond PRIVATE
class KCalCore::ScheduleMessage::Private
{
  public:
    Private() {}

    IncidenceBase::Ptr mIncidence;
    iTIPMethod mMethod;
    Status mStatus;
    QString mError;

    ~Private() {}
};
//@endcond

ScheduleMessage::ScheduleMessage( IncidenceBase::Ptr incidence,
                                  iTIPMethod method,
                                  ScheduleMessage::Status status )
  : d( new KCalCore::ScheduleMessage::Private )
{
  d->mIncidence = incidence;
  d->mMethod = method;
  d->mStatus = status;
}

ScheduleMessage::~ScheduleMessage()
{
  delete d;
}

IncidenceBase::Ptr ScheduleMessage::event() const
{
  return d->mIncidence;
}

iTIPMethod ScheduleMessage::method() const
{
  return d->mMethod;
}

QString ScheduleMessage::methodName( iTIPMethod method )
{
  switch ( method ) {
  case iTIPPublish:
    return QLatin1String( "Publish" );
  case iTIPRequest:
    return QLatin1String( "Request" );
  case iTIPRefresh:
    return QLatin1String( "Refresh" );
  case iTIPCancel:
    return QLatin1String( "Cancel" );
  case iTIPAdd:
    return QLatin1String( "Add" );
  case iTIPReply:
    return QLatin1String( "Reply" );
  case iTIPCounter:
    return QLatin1String( "Counter" );
  case iTIPDeclineCounter:
    return QLatin1String( "Decline Counter" );
  default:
    return QLatin1String( "Unknown" );
  }
}

ScheduleMessage::Status ScheduleMessage::status() const
{
  return d->mStatus;
}

QString ScheduleMessage::error() const
{
  return d->mError;
}
