/*
  This file is part of the kcalendarcore library.

  SPDX-FileCopyrightText: 2006, 2008 Allen Winter <winter@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTAVAILABILITY_H
#define TESTAVAILABILITY_H

#include <QObject>

class AvailabilityTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void parseAvailability();
    void parseAvailability2();
    void testValidity();
};

#endif
