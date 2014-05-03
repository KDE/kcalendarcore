/*
  This file is part of the kcalcore library.
  Copyright (c) 2006,2008 Allen Winter <winter@kde.org>
  Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>
  Copyright (C) 2009-2010 Klaralvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>

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

#ifndef TESTATTENDEE_H
#define TESTATTENDEE_H

#include <QtCore/QObject>

class AttendeeTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testValidity();
    void testType();
    void testCompare();
    void testCompareType();
    void testAssign();
    void testDataStreamOut();
    void testDataStreamIn();
};

#endif
