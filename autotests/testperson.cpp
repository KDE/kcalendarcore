/*
  This file is part of the kcalcore library.

  Copyright (C) 2006-2009 Allen Winter <winter@kde.org>
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

#include "testperson.h"
#include "person.h"

#include <qdebug.h>

#include <qtest.h>
QTEST_MAIN(PersonTest)

using namespace KCalCore;

void PersonTest::testValidity()
{
    Person person("fred", "fred@flintstone.com");
    QVERIFY(person.name() == "fred");
}

void PersonTest::testCompare()
{
    Person person1("fred", "fred@flintstone.com");
    Person person2("wilma", "wilma@flintstone.com");
    Person::Ptr person3 = Person::fromFullName("fred <fred@flintstone.com>");
    Person person1copy(person1);   // test copy constructor
    Person person1assign = person1; // test operator=

    QVERIFY(!(person1 == person2));
    QVERIFY(person1 == *person3.data());
    QVERIFY(person1 == person1copy);
    QVERIFY(person1 == person1assign);
    QVERIFY(person1.name() == "fred");
    QVERIFY(person2.email() == "wilma@flintstone.com");
    QVERIFY(person3->name() == "fred");
    QVERIFY(person3->email() == "fred@flintstone.com");
}

void PersonTest::testStringify()
{
    Person person1("fred", "fred@flintstone.com");
    Person person2("wilma", "wilma@flintstone.com");
    QVERIFY(person1.fullName() == "fred <fred@flintstone.com>");
    QVERIFY(person2.fullName() == "wilma <wilma@flintstone.com>");

    person1.setName("");
    QVERIFY(person1.fullName() == "fred@flintstone.com");
    person1.setEmail(QString());
    QVERIFY(person1.fullName().isEmpty());
}

void PersonTest::testDataStreamIn()
{
    Person::Ptr person1(new Person("fred", "fred@flintstone.com"));
    int initial_count = person1->count();

    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << person1;

    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);

    QString name, email;
    int count;

    in_stream >> name;
    QVERIFY(name == "fred");

    in_stream >> email;
    QVERIFY(email == "fred@flintstone.com");

    in_stream >> count;
    QVERIFY(count == initial_count);
}

void PersonTest::testDataStreamOut()
{
    Person::Ptr person1(new Person("fred", "fred@flintstone.com"));

    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << person1;

    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);
    Person::Ptr person2;

    in_stream >> person2;

    QVERIFY(person2->name() == person1->name());
    QVERIFY(person2->email() == person1->email());
    QVERIFY(person2->count() == person1->count());
}

