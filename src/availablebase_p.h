/*
  This file is part of the kcalendarcore library.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the AvailableBase class.

*/

#include "availablebase.h"
#include "duration.h"
#include "kcalendarcore_debug.h"

#include <QDateTime>

using namespace KCalendarCore;

//@cond PRIVATE
class Q_DECL_HIDDEN KCalendarCore::AvailableBase::AvailableBasePrivate
{
public:
    AvailableBasePrivate() = default;

    AvailableBasePrivate(const AvailableBasePrivate &other)
    {
        init(other);
    }

    void init(const AvailableBasePrivate &other);

    virtual ~AvailableBasePrivate() = default;

    QString mUid; // unique id
    QDateTime mCreated; // creation datetime
    QDateTime mDtStamp;
    QDateTime mDtStart; // start time
    QDateTime mDtEnd; // end time -> only one of dtEnd/duration allowed, so opting to always saving dtEnd.
    Duration mDuration; // duration
    QDateTime mLastModified; // last modified date

    QString mSummary;
    bool mSummaryIsRich = false; // summary string is richtext.

    bool mLocationIsRich = false; // location string is richtext.
    QString mLocation; // location string

    QString mDescription; // description string
    bool mDescriptionIsRich = false; // description string is richtext.

    QStringList mCategories; // category list

    QStringList mComments; // list of incidence comments

    QStringList mContacts; // list of incidence contacts
};

//@endcond
