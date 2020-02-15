/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2015 Sandro Knauß <knauss@kolabsys.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTINCIDENCE_H
#define TESTINCIDENCE_H

#include <QObject>

class IncidenceTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDtStartChange();
    void testSummaryChange();
    void testLocationChange();

    void testRecurrenceTypeChange();
    void testRecurrenceEndTimeChange();
    void testRecurrenceEndTimeDurationChange();
    void testRecurrenceDurationChange();
    void testRecurrenceExDatesChange();
    void testRecurrenceMonthlyPos();
    void testRecurrenceMonthlyDate();
    void testRecurrenceYearlyDay();
    void testRecurrenceYearlyMonth();
};

#endif
