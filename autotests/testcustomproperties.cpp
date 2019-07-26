/*
  This file is part of the kcalcore library.

  Copyright (c) 2009 Allen Winter <winter@kde.org>
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
#include "testcustomproperties.h"
#include "customproperties.h"

#include <QDataStream>
#include <QTest>

QTEST_MAIN(CustomPropertiesTest)

using namespace KCalendarCore;

void CustomPropertiesTest::testValidity()
{
    CustomProperties cp;

    QByteArray app("KORG");
    QByteArray key("TEXT");

    QByteArray name("X-KDE-KORG-TEXT");
    QCOMPARE(cp.customPropertyName(app, key), name);
    cp.setCustomProperty(app, key, QStringLiteral("rich"));
    QCOMPARE(cp.customProperty(app, key), QLatin1String("rich"));
    QCOMPARE(cp.nonKDECustomProperty(name), QLatin1String("rich"));

    cp.removeCustomProperty(app, key);
    cp.setCustomProperty(app, key, QStringLiteral("foo"));
    cp.setCustomProperty(app, key, QStringLiteral("rich"));
    QCOMPARE(cp.customProperty(app, key), QLatin1String("rich"));

    key = "X-TEXT";
    cp.setNonKDECustomProperty(key, QStringLiteral("rich"));
    QCOMPARE(cp.nonKDECustomProperty(key), QLatin1String("rich"));

    cp.removeNonKDECustomProperty(key);
    cp.setNonKDECustomProperty(key, QStringLiteral("foo"));
    cp.setNonKDECustomProperty(key, QStringLiteral("rich"));
    QCOMPARE(cp.nonKDECustomProperty(key), QLatin1String("rich"));
}

void CustomPropertiesTest::testCompare()
{
    CustomProperties cp1, cp2;

    QByteArray app("KORG");
    QByteArray key("TEXT");

    cp1.setCustomProperty(app, key, QStringLiteral("rich"));
    cp2 = cp1;
    QVERIFY(cp1 == cp2);

    CustomProperties cp3;
    cp3.setCustomProperty(app, key, cp1.customProperty(app, key));
    QVERIFY(cp1 == cp3);

    QVERIFY(cp1.customProperty(app, key) == QLatin1String("rich"));
    QVERIFY(cp1.customProperty(app, "foo").isEmpty());
    QVERIFY(cp1.customProperty(app, QByteArray()).isEmpty());

    CustomProperties cp4;
    QVERIFY(cp4.customProperty(app, key).isEmpty());
    QVERIFY(cp4.customProperty(app, "foo").isEmpty());
    QVERIFY(cp4.customProperty(app, QByteArray()).isEmpty());

    key = "X-TEXT";
    cp1.setNonKDECustomProperty(key, QStringLiteral("rich"));
    cp2 = cp1;
    QVERIFY(cp1 == cp2);

    cp3.setNonKDECustomProperty(key, cp1.nonKDECustomProperty(key));
    QVERIFY(cp1 == cp3);

    QVERIFY(cp1.nonKDECustomProperty(key) == QLatin1String("rich"));
    QVERIFY(cp1.nonKDECustomProperty("foo").isEmpty());
    QVERIFY(cp1.nonKDECustomProperty(QByteArray()).isEmpty());

    CustomProperties cp5;
    QVERIFY(cp5.nonKDECustomProperty(key).isEmpty());
    QVERIFY(cp5.nonKDECustomProperty("foo").isEmpty());
    QVERIFY(cp5.nonKDECustomProperty(QByteArray()).isEmpty());
}

void CustomPropertiesTest::testMapValidity()
{
    QMap<QByteArray, QString> cpmap;
    cpmap.insert("X-key1", QStringLiteral("val1"));
    cpmap.insert("X-key2", QStringLiteral("val2"));
    cpmap.insert("X-key3", QStringLiteral("val3"));
    cpmap.insert("X-key4", QStringLiteral("val4"));
    cpmap.insert("X-key5", QStringLiteral("val5"));

    CustomProperties cp;
    cp.setCustomProperties(cpmap);

    QVERIFY(cp.customProperties().value("X-key3") == QLatin1String("val3"));
}

void CustomPropertiesTest::testMapCompare()
{
    QMap<QByteArray, QString> cpmap;
    cpmap.insert("X-key1", QStringLiteral("val1"));
    cpmap.insert("X-key2", QStringLiteral("val2"));
    cpmap.insert("X-key3", QStringLiteral("val3"));
    cpmap.insert("X-key4", QStringLiteral("val4"));
    cpmap.insert("X-key5", QStringLiteral("val5"));

    CustomProperties cp1, cp2;
    cp1.setCustomProperties(cpmap);
    cp1 = cp2;
    QVERIFY(cp1 == cp2);

    CustomProperties cp3;
    cp3.setCustomProperties(cp1.customProperties());
    QVERIFY(cp1 == cp3);
}

void CustomPropertiesTest::testEmpty()
{
    CustomProperties cp;

    QByteArray app("KORG");
    QByteArray key("TEXT");
    QString empty;

    cp.setCustomProperty(app, key, empty);
    QCOMPARE(cp.customProperty(app, key), empty);

    cp.removeCustomProperty(app, key);
    cp.setCustomProperty(app, key, empty);
    QCOMPARE(cp.customProperty(app, key), empty);

    key = "X-TEXT";
    cp.setNonKDECustomProperty(key, empty);
    QCOMPARE(cp.nonKDECustomProperty(key), empty);

    cp.removeNonKDECustomProperty(key);
    cp.setNonKDECustomProperty(key, empty);
    QCOMPARE(cp.nonKDECustomProperty(key), empty);
}

void CustomPropertiesTest::testDataStreamIn()
{
    QMap<QByteArray, QString> cpmap;
    cpmap.insert("X-key1", QStringLiteral("val1"));
    cpmap.insert("X-key2", QStringLiteral("val2"));
    cpmap.insert("X-key3", QStringLiteral("val3"));
    cpmap.insert("X-key4", QStringLiteral("val4"));
    cpmap.insert("X-key5", QStringLiteral("val5"));

    CustomProperties cp;
    cp.setCustomProperties(cpmap);
    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << cp;

    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);

    QMap<QByteArray, QString> cpmap2;

    in_stream >> cpmap2;

    QVERIFY(cpmap == cpmap2);
}

void CustomPropertiesTest::testVolatile()
{
    QMap<QByteArray, QString> cpmap;
    cpmap.insert("X-key1", QStringLiteral("val1"));
    cpmap.insert("X-KDE-VOLATILE-FOO", QStringLiteral("val2"));

    CustomProperties cp;
    cp.setCustomProperties(cpmap);

    QCOMPARE(cp.customProperties().count(), 2);

    QMap<QByteArray, QString> cpmap2;
    cpmap2.insert("X-key1", QStringLiteral("val1"));
    CustomProperties cp2;
    cp2.setCustomProperties(cpmap2);
    QCOMPARE(cp, cp2);

    cp.removeCustomProperty("VOLATILE", "FOO");
    QCOMPARE(cp.customProperties().count(), 1);

    cp.setCustomProperty("VOLATILE", "FOO", QStringLiteral("BAR"));
    QCOMPARE(cp.customProperties().count(), 2);

    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << cp;
    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);
    in_stream >> cp;

    QCOMPARE(cp.customProperties().count(), 1);
}

void CustomPropertiesTest::testDataStreamOut()
{
    QMap<QByteArray, QString> cpmap;
    cpmap.insert("X-key1", QStringLiteral("val1"));
    cpmap.insert("X-key2", QStringLiteral("val2"));
    cpmap.insert("X-key3", QStringLiteral("val3"));
    cpmap.insert("X-key4", QStringLiteral("val4"));
    cpmap.insert("X-key5", QStringLiteral("val5"));

    CustomProperties cp, cp2;
    cp.setCustomProperties(cpmap);
    QByteArray byteArray;
    QDataStream out_stream(&byteArray, QIODevice::WriteOnly);

    out_stream << cp;

    QDataStream in_stream(&byteArray, QIODevice::ReadOnly);

    in_stream >> cp2;

    QVERIFY(cp2 == cp);
}
