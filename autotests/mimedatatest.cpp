/*
  SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <KCalendarCore/Incidence>
#include <KCalendarCore/MimeData>

#include <QDebug>
#include <QMimeData>
#include <QTest>

using namespace Qt::Literals;
using namespace KCalendarCore;

class MimeDataTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    static void testMimeData()
    {
        const Event::Ptr ev(new Event());
        ev->setSummary(u"Summary"_s);
        ev->setDtStart(QDateTime(QDate(2010, 8, 8), {}));
        ev->setDtEnd(QDateTime(QDate(2010, 8, 9), {}));
        ev->setAllDay(true);
        const auto originalUid = ev->uid();
        QVERIFY(!originalUid.isEmpty());

        Incidence::List incidencesToCopy;
        incidencesToCopy.append(ev);

        QMimeData mimeData;
        QVERIFY(!MimeData::canDecode(&mimeData));

        MimeData::populate(&mimeData, incidencesToCopy);
        QVERIFY(MimeData::canDecode(&mimeData));

        const auto &cal = MimeData::decodeCalendar(&mimeData);
        QVERIFY(cal);
        QCOMPARE(cal->incidences().size(), 1);
        QCOMPARE(cal->events().size(), 1);
        QVERIFY(!cal->events()[0]->uid().isEmpty());
        QCOMPARE(cal->events()[0]->uid(), originalUid);

        QVERIFY(MimeData::decodeEvent(&mimeData));
        QVERIFY(!MimeData::decodeTodo(&mimeData));

        QCOMPARE(MimeData::decodeIncidences(&mimeData).size(), 1);
    }
};

QTEST_APPLESS_MAIN(MimeDataTest)

#include "mimedatatest.moc"
