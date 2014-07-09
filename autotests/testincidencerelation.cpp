/*
  This file is part of the kcalcore library.

  Copyright (c) 2009 Thomas McGuire <mcguire@kde.org>

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
#include "testincidencerelation.h"
#include "todo.h"

#include <qtest.h>
QTEST_MAIN(IncidenceRelationTest)

using namespace KCalCore;

void IncidenceRelationTest::testRelations()
{
    // Build the following tree:
    // todo1
    // \- todo2
    //    \- todo3

    // Then make todo3 independent:
    // todo3
    // todo1
    // \- todo2

    Todo::Ptr todo1 = Todo::Ptr(new Todo());
    todo1->setSummary("todo");

    Todo::Ptr todo2 = Todo::Ptr(new Todo());
    todo2->setSummary("sub-todo");

    Todo::Ptr todo3 = Todo::Ptr(new Todo());
    todo3->setSummary("sub-sub-todo");

    todo3->setRelatedTo(todo2->uid());
    todo2->setRelatedTo(todo1->uid());

    QCOMPARE(todo3->relatedTo(), todo2->uid());
    QCOMPARE(todo2->relatedTo(), todo1->uid());
    QCOMPARE(todo1->relatedTo(), QString());

    todo3->setRelatedTo(QString());

    QCOMPARE(todo3->relatedTo(), QString());
    QCOMPARE(todo2->relatedTo(), todo1->uid());
    QCOMPARE(todo1->relatedTo(), QString());
}
