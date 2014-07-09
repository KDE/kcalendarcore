/*
  This file is part of the kcalcore library.

  Copyright (C) 2007-2008 Allen Winter <winter@kde.org>

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
#include "testjournal.h"
#include "journal.h"

#include <qtest.h>
QTEST_MAIN(JournalTest)

using namespace KCalCore;

void JournalTest::testValidity()
{
    QDate dt = QDate::currentDate();
    Journal *journal = new Journal();
    journal->setDtStart(KDateTime(dt));
    journal->setSummary("Journal Summary");
    journal->setDescription("This is a description of my journal");
    journal->setLocation("the place");
    //KDE5: QVERIFY( journal->typeStr() == i18n( "journal" ) );
    QVERIFY(journal->summary() == "Journal Summary");
    QVERIFY(journal->location() == "the place");
}

void JournalTest::testCompare()
{
    QDate dt = QDate::currentDate();
    Journal journal1;
    journal1.setDtStart(KDateTime(dt));
    journal1.setSummary("Journal Summary");
    journal1.setDescription("This is a description of my journal");
    journal1.setLocation("the place");

    Journal journal2;
    journal2.setDtStart(KDateTime(dt).addDays(1));
    journal2.setSummary("Journal2 Summary");
    journal2.setDescription("This is a description of another journal");
    journal2.setLocation("the other place");

    QVERIFY(!(journal1 == journal2));
    QVERIFY(journal2.summary() == "Journal2 Summary");
}

void JournalTest::testClone()
{
    QDate dt = QDate::currentDate();
    Journal journal1;
    journal1.setDtStart(KDateTime(dt));
    journal1.setSummary("Journal1 Summary");
    journal1.setDescription("This is a description of the first journal");
    journal1.setLocation("the place");

    Journal *journal2 = journal1.clone();
    QVERIFY(journal1.summary() == journal2->summary());
    QVERIFY(journal1.dtStart() == journal2->dtStart());
    QVERIFY(journal1.description() == journal2->description());
    QVERIFY(journal1.location() == journal2->location());
}

void JournalTest::testRich()
{
    QDate dt = QDate::currentDate();
    Journal journal1;
    journal1.setDtStart(KDateTime(dt));
    journal1.setSummary("<html><b><i>Journal1 Summary</i></b></html>", true);
    journal1.setDescription("<html>This is a of the <b>first</b> journal</html>", true);
    journal1.setLocation("<qt><h1>the place</h1></qt>", true);
    QVERIFY(journal1.summaryIsRich());
    QVERIFY(journal1.descriptionIsRich());
    QVERIFY(journal1.locationIsRich());
}

void JournalTest::testAssign()
{
    QDate dt = QDate::currentDate();
    Journal journal1;
    journal1.setDtStart(KDateTime(dt));
    journal1.setSummary("Journal1 Summary");
    journal1.setDescription("This is a description of the first journal");
    journal1.setLocation("the place");

    Journal journal2 = journal1;
    QVERIFY(journal1 == journal2);
}


void JournalTest::testSerializer_data()
{
    QTest::addColumn<KCalCore::Journal::Ptr>("journal");

    Journal::Ptr journal1 = Journal::Ptr(new Journal());

    QTest::newRow("journal") << journal1;
}

void JournalTest::testSerializer()
{
    QFETCH(KCalCore::Journal::Ptr, journal);
    IncidenceBase::Ptr incidenceBase = journal.staticCast<KCalCore::IncidenceBase>();

    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << incidenceBase;

    Journal::Ptr journal2 = Journal::Ptr(new Journal());
    IncidenceBase::Ptr incidenceBase2 = journal2.staticCast<KCalCore::IncidenceBase>();
    QVERIFY(*journal != *journal2);
    QDataStream stream2(&array, QIODevice::ReadOnly);
    stream2 >> incidenceBase2;
    QVERIFY(*journal == *journal2);
}
