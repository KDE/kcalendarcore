/*
  This file is part of the kcalcore library.

  Copyright (c) 2002 Michael Brade <brade@kde.org>

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
/**
  @file
  This file is part of the API for handling calendar data and
  defines the Attachment class.

  @brief
  Represents information related to an attachment for a Calendar Incidence.

  @author Michael Brade \<brade@kde.org\>
*/

#include "attachment.h"
#include <QDataStream>

using namespace KCalCore;

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class Q_DECL_HIDDEN KCalCore::Attachment::Private : public QSharedData
{
public:
    Private() = default;
    Private(const QString &mime, bool binary)
        : mMimeType(mime),
          mBinary(binary)
    {}
    Private(const Private &other) = default;

    ~Private()
    {
    }

    mutable uint mSize = 0;
    mutable QByteArray mDecodedDataCache;
    QString mMimeType;
    QString mUri;
    QByteArray mEncodedData;
    QString mLabel;
    bool mBinary = false;
    bool mLocal = false;
    bool mShowInline = false;
};
//@endcond

Attachment::Attachment()
    : d(new Attachment::Private)
{
}

Attachment::Attachment(const Attachment &attachment) = default;

Attachment::Attachment(const QString &uri, const QString &mime)
    : d(new Attachment::Private(mime, false))
{
    d->mUri = uri;
}

Attachment::Attachment(const QByteArray &base64, const QString &mime)
    : d(new Attachment::Private(mime, true))
{
    d->mEncodedData = base64;
}

Attachment::~Attachment() = default;

bool Attachment::isEmpty() const
{
    return d->mMimeType.isEmpty() && d->mUri.isEmpty() && d->mEncodedData.isEmpty();
}

bool Attachment::isUri() const
{
    return !d->mBinary;
}

QString Attachment::uri() const
{
    if (!d->mBinary) {
        return d->mUri;
    } else {
        return QString();
    }
}

void Attachment::setUri(const QString &uri)
{
    d->mUri = uri;
    d->mBinary = false;
}

bool Attachment::isBinary() const
{
    return d->mBinary;
}

QByteArray Attachment::data() const
{
    if (d->mBinary) {
        return d->mEncodedData;
    } else {
        return QByteArray();
    }
}

QByteArray Attachment::decodedData() const
{
    if (d->mDecodedDataCache.isNull()) {
        d->mDecodedDataCache = QByteArray::fromBase64(d->mEncodedData);
    }

    return d->mDecodedDataCache;
}

void Attachment::setDecodedData(const QByteArray &data)
{
    setData(data.toBase64());
    d->mDecodedDataCache = data;
    d->mSize = d->mDecodedDataCache.size();
}

void Attachment::setData(const QByteArray &base64)
{
    d->mEncodedData = base64;
    d->mBinary = true;
    d->mDecodedDataCache = QByteArray();
    d->mSize = 0;
}

uint Attachment::size() const
{
    if (isUri()) {
        return 0;
    }
    if (!d->mSize) {
        d->mSize = decodedData().size();
    }

    return d->mSize;
}

QString Attachment::mimeType() const
{
    return d->mMimeType;
}

void Attachment::setMimeType(const QString &mime)
{
    d->mMimeType = mime;
}

bool Attachment::showInline() const
{
    return d->mShowInline;
}

void Attachment::setShowInline(bool showinline)
{
    d->mShowInline = showinline;
}

QString Attachment::label() const
{
    return d->mLabel;
}

void Attachment::setLabel(const QString &label)
{
    d->mLabel = label;
}

bool Attachment::isLocal() const
{
    return d->mLocal;
}

void Attachment::setLocal(bool local)
{
    d->mLocal = local;
}

Attachment &Attachment::operator=(const Attachment &other) = default;

bool Attachment::operator==(const Attachment &a2) const
{
    return uri()          == a2.uri() &&
           d->mLabel      == a2.label() &&
           d->mLocal      == a2.isLocal() &&
           d->mBinary     == a2.isBinary() &&
           d->mShowInline == a2.showInline() &&
           size()         == a2.size() &&
           decodedData()  == a2.decodedData();
}

bool Attachment::operator!=(const Attachment &a2) const
{
    return !(*this == a2);
}

QDataStream &KCalCore::operator<<(QDataStream &out, const KCalCore::Attachment &a)
{
    out << a.d->mSize
        << a.d->mMimeType
        << a.d->mUri
        << a.d->mEncodedData
        << a.d->mLabel
        << a.d->mBinary
        << a.d->mLocal
        << a.d->mShowInline;
    return out;
}

QDataStream &KCalCore::operator>>(QDataStream &in, KCalCore::Attachment &a)
{
    in >> a.d->mSize
        >> a.d->mMimeType
        >> a.d->mUri
        >> a.d->mEncodedData
        >> a.d->mLabel
        >> a.d->mBinary
        >> a.d->mLocal
        >> a.d->mShowInline;
    return in;
}
