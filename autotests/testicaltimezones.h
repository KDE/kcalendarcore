/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2005 David Jarvie <djarvie@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TESTICALTIMEZONES_H
#define TESTICALTIMEZONES_H

#include <QObject>

class ICalTimeZonesTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void parse_data();
    void parse();
    void write();
};

#endif
