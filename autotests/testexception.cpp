/*
  This file is part of the kcalcore library.

  Copyright (C) 2006 Allen Winter <winter@kde.org>

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

#include "testexception.h"
#include "exceptions.h"

#include <qtest.h>
QTEST_MAIN(ExceptionTest)

using namespace KCalCore;

void ExceptionTest::testValidity()
{
    //KDAB_TODO: getting undefined reference while compiling
//  Exception ef( Exception::LoadError );
//  QVERIFY( ef.code() == Exception::LoadError );

}
