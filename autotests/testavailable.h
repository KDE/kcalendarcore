/*
  This file is part of the kcalendarcore library.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTAVAILABLE_H
#define TESTAVAILABLE_H

#include <QObject>

class AvailableTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testAvailable();
    void testCompare();
};

#endif
