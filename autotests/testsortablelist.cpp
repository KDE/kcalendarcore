/*
  This file is part of the kcalcore library.

  Copyright (c) 2006 David Jarvie <software@astrojar.org.uk>

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

#include "testsortablelist.h"
#include "sortablelist.h"

#include <stdlib.h>

#include <qtest.h>
QTEST_MAIN(SortableListTest)

using namespace KCalCore;

void SortableListTest::general()
{
    SortableList<int> list;
    list << 10 << 8 << 6 << 4 << 6 << 3 << 10 << 6 << 1;
    list.sortUnique();
    QCOMPARE(list.count(), 6);
    QCOMPARE(list[0], 1);
    QCOMPARE(list[1], 3);
    QCOMPARE(list[2], 4);
    QCOMPARE(list[3], 6);
    QCOMPARE(list[4], 8);
    QCOMPARE(list[5], 10);

    QVERIFY(!list.containsSorted(0));
    QVERIFY(list.containsSorted(1));
    QVERIFY(!list.containsSorted(2));
    QVERIFY(list.containsSorted(3));
    QVERIFY(list.containsSorted(4));
    QVERIFY(!list.containsSorted(5));
    QVERIFY(list.containsSorted(6));
    QVERIFY(!list.containsSorted(7));
    QVERIFY(list.containsSorted(8));
    QVERIFY(!list.containsSorted(9));
    QVERIFY(list.containsSorted(10));
    QVERIFY(!list.containsSorted(11));

    QCOMPARE(list.findSorted(0), -1);
    QCOMPARE(list.findSorted(1), 0);
    QCOMPARE(list.findSorted(2), -1);
    QCOMPARE(list.findSorted(3), 1);
    QCOMPARE(list.findSorted(4), 2);
    QCOMPARE(list.findSorted(5), -1);
    QCOMPARE(list.findSorted(6), 3);
    QCOMPARE(list.findSorted(7), -1);
    QCOMPARE(list.findSorted(8), 4);
    QCOMPARE(list.findSorted(9), -1);
    QCOMPARE(list.findSorted(10), 5);
    QCOMPARE(list.findSorted(11), -1);

    QCOMPARE(list.findLT(0), -1);
    QCOMPARE(list.findLT(1), -1);
    QCOMPARE(list.findLT(2), 0);
    QCOMPARE(list.findLT(3), 0);
    QCOMPARE(list.findLT(4), 1);
    QCOMPARE(list.findLT(5), 2);
    QCOMPARE(list.findLT(6), 2);
    QCOMPARE(list.findLT(7), 3);
    QCOMPARE(list.findLT(8), 3);
    QCOMPARE(list.findLT(9), 4);
    QCOMPARE(list.findLT(10), 4);
    QCOMPARE(list.findLT(11), 5);

    QCOMPARE(list.findLE(0), -1);
    QCOMPARE(list.findLE(1), 0);
    QCOMPARE(list.findLE(2), 0);
    QCOMPARE(list.findLE(3), 1);
    QCOMPARE(list.findLE(4), 2);
    QCOMPARE(list.findLE(5), 2);
    QCOMPARE(list.findLE(6), 3);
    QCOMPARE(list.findLE(7), 3);
    QCOMPARE(list.findLE(8), 4);
    QCOMPARE(list.findLE(9), 4);
    QCOMPARE(list.findLE(10), 5);
    QCOMPARE(list.findLE(11), 5);

    QCOMPARE(list.findGE(0), 0);
    QCOMPARE(list.findGE(1), 0);
    QCOMPARE(list.findGE(2), 1);
    QCOMPARE(list.findGE(3), 1);
    QCOMPARE(list.findGE(4), 2);
    QCOMPARE(list.findGE(5), 3);
    QCOMPARE(list.findGE(6), 3);
    QCOMPARE(list.findGE(7), 4);
    QCOMPARE(list.findGE(8), 4);
    QCOMPARE(list.findGE(9), 5);
    QCOMPARE(list.findGE(10), 5);
    QCOMPARE(list.findGE(11), -1);

    QCOMPARE(list.findGT(0), 0);
    QCOMPARE(list.findGT(1), 1);
    QCOMPARE(list.findGT(2), 1);
    QCOMPARE(list.findGT(3), 2);
    QCOMPARE(list.findGT(4), 3);
    QCOMPARE(list.findGT(5), 3);
    QCOMPARE(list.findGT(6), 4);
    QCOMPARE(list.findGT(7), 4);
    QCOMPARE(list.findGT(8), 5);
    QCOMPARE(list.findGT(9), 5);
    QCOMPARE(list.findGT(10), -1);
    QCOMPARE(list.findGT(11), -1);

    QCOMPARE(list.insertSorted(4), 2);
    QCOMPARE(list.count(), 6);
    QCOMPARE(list[2], 4);
    QCOMPARE(list.insertSorted(0), 0);
    QCOMPARE(list.count(), 7);
    QCOMPARE(list[0], 0);
    QCOMPARE(list.insertSorted(7), 5);
    QCOMPARE(list.count(), 8);
    QCOMPARE(list[5], 7);
    QCOMPARE(list.insertSorted(14), 8);
    QCOMPARE(list.count(), 9);
    QCOMPARE(list[8], 14);

    QCOMPARE(list.removeSorted(7), 5);
    QCOMPARE(list.count(), 8);
    QCOMPARE(list[5], 8);
    QCOMPARE(list.removeSorted(5), -1);
    QCOMPARE(list.count(), 8);
    QCOMPARE(list.removeSorted(0), 0);
    QCOMPARE(list.count(), 7);
    QCOMPARE(list[0], 1);
}
