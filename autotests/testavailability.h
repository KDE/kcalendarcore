/*
  This file is part of the kcalendarcore library.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTAVAILABILITY_H
#define TESTAVAILABILITY_H

#include "availability.h"

#include <QObject>

class AvailabilityTest : public QObject
{
    void prettyPrint(QVector<KCalendarCore::Availability::Ptr> availability);

    Q_OBJECT
private Q_SLOTS:
    void parseAvailability();
    void parseAvailability2();
    void parseAvailability3();
    void testValidity();
};

#endif
