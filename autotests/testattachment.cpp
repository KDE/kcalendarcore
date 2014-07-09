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

#include "testattachment.h"
#include "event.h"
#include "attachment.h"

#include <qtest.h>
QTEST_MAIN(AttachmentTest)

using namespace KCalCore;

void AttachmentTest::testValidity()
{
    Attachment attachment(QString("http://www.kde.org"));
    QCOMPARE(attachment.uri(), QString::fromLatin1("http://www.kde.org"));
    QCOMPARE(attachment.data(), QByteArray());
    QVERIFY(attachment.decodedData().isEmpty());
    QVERIFY(!attachment.isBinary());

    attachment.setDecodedData("foo");
    QVERIFY(attachment.isBinary());
    QCOMPARE(attachment.decodedData(), QByteArray("foo"));
    QCOMPARE(attachment.data(), QByteArray("Zm9v"));
    QCOMPARE(attachment.size(), 3U);

    Attachment attachment2 = Attachment(QByteArray("Zm9v"));
    QCOMPARE(attachment2.size(), 3U);
    QCOMPARE(attachment2.decodedData(), QByteArray("foo"));
    attachment2.setDecodedData("123456");
    QCOMPARE(attachment2.size(), 6U);

    Attachment attachment3(attachment2);
    QCOMPARE(attachment3.size(), attachment2.size());

    QByteArray fred("jkajskldfasjfklasjfaskfaskfasfkasfjdasfkasjf");
    Attachment attachment4(fred, QByteArray("image/nonsense"));
    QCOMPARE(fred, attachment4.data());
    QVERIFY(attachment4.isBinary());
    QByteArray ethel("a9fafafjafkasmfasfasffksjklfjau");
    attachment4.setData(ethel);
    QCOMPARE(ethel, attachment4.data());

    Attachment attachment5(QString("http://www.kde.org"));
    Attachment attachment6(QString("http://www.kde.org"));
    QVERIFY(attachment5 == attachment6);
    attachment5.setUri("http://bugs.kde.org");
    QVERIFY(attachment5 != attachment6);
    attachment5.setDecodedData("123456");
    attachment6.setDecodedData("123456");
    QVERIFY(attachment5 == attachment6);
    attachment6.setDecodedData("12345");
    QVERIFY(attachment5 != attachment6);
}


void AttachmentTest::testSerializer_data()
{
    QTest::addColumn<KCalCore::Attachment::Ptr>("attachment");

    Attachment::Ptr nonInline = Attachment::Ptr(new Attachment(QString("http://www.kde.org")));
    Attachment::Ptr inlineAttachment = Attachment::Ptr(new Attachment(QByteArray("foo"), QByteArray("image/nonsense")));

    QTest::newRow("inline") << inlineAttachment;
    QTest::newRow("not inline") << nonInline;
}


void AttachmentTest::testSerializer()
{
    QFETCH(KCalCore::Attachment::Ptr , attachment);

    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << attachment; // Serialize

    Attachment::Ptr attachment2 = Attachment::Ptr(new Attachment(QString("foo")));
    QVERIFY(*attachment != *attachment2);
    QDataStream stream2(&array, QIODevice::ReadOnly);
    stream2 >> attachment2; // deserialize
    QVERIFY(*attachment == *attachment2);
}

