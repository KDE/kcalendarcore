/*
  This file is part of the kcalcore library.

  Copyright (c) 2006 Allen Winter <winter@kde.org>

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

#include "testcalfilter.h"
#include "calfilter.h"

#include <QStringList>

#include <qtest.h>
QTEST_MAIN(CalFilterTest)

using namespace KCalCore;

void CalFilterTest::testValidity()
{
    CalFilter *f = new CalFilter;
    f->setName("testfilter");
    QVERIFY(f->name() == "testfilter");
    delete f;
    CalFilter g("fredfilter");
    QVERIFY(g.name() == "fredfilter");
    CalFilter f1, f2;
    QVERIFY(f1 == f2);
}

void CalFilterTest::testCats()
{
    CalFilter f1, f2;
    QStringList cats;
    cats << "a" << "b" << "c";
    f1.setCategoryList(cats);
    f2.setCategoryList(cats);
    QVERIFY(f1.categoryList() == f2.categoryList());
}
