/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "kcalendarcore_export.h"

#include <KCalendarCore/Calendar>
#include <KCalendarCore/CalendarMetadata>

namespace KCalendarCore
{
/**
  @brief
  A plugin that provides calendar data.

  It allows calendar applications to consume data provided by multiple
  sources, e.g. local ical files or remote calendars.

  @since 5.83

*/
class KCALENDARCORE_EXPORT CalendarPlugin : public QObject
{
    Q_OBJECT
public:
    CalendarPlugin(QObject *parent, const QVariantList &args);

    /**
     * The set of calendars defined by this plugin.
     *
     * @return QHash of calendars and their respective metadata.
     */
    virtual QHash<Calendar::Ptr, CalendarMetadata::ConstPtr> calendars() const = 0;
    
    /**
     * Fetch metadata for an incidence inside a calendar provided by the
     * plugin. This is usefull when the CalendarMetadata provided calendars()
     * are null.
     * @param incidence The incidence for which we want more information.
     * @param calendar The calendar containing the incidence.
     */
    virtual CalendarMetadata::ConstPtr fetchMetadata(const Incidence::Ptr &incidence,
                                                     const Calendar::Ptr &calendar) = 0;

Q_SIGNALS:
    /**
     * Emitted when the set of calendars changed.
     */
    void calendarsChanged();

private:
    void *d;
};

}
