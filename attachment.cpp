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

using namespace KCalCore;

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class KCalCore::Attachment::Private
{
  public:
    Private( const QString &mime, bool binary )
      : mSize( 0 ),
        mMimeType( mime ),
        mBinary( binary ),
        mLocal( false ),
        mShowInline( false )
    {}
    Private( const Private &other )
      : mSize( other.mSize ),
        mMimeType( other.mMimeType ),
        mUri( other.mUri ),
        mEncodedData( other.mEncodedData ),
        mLabel( other.mLabel ),
        mBinary( other.mBinary ),
        mLocal( other.mLocal ),
        mShowInline( other.mShowInline )
    {}

    ~Private()
    {
    }

    QByteArray mDecodedDataCache;
    uint mSize;
    QString mMimeType;
    QString mUri;
    QByteArray mEncodedData;
    QString mLabel;
    bool mBinary;
    bool mLocal;
    bool mShowInline;
};
//@endcond

Attachment::Attachment( const Attachment &attachment )
  : d( new Attachment::Private( *attachment.d ) )
{
}

Attachment::Attachment( const QString &uri, const QString &mime )
  : d( new Attachment::Private( mime, false ) )
{
  d->mUri = uri;
}

Attachment::Attachment( const QByteArray &base64, const QString &mime )
  : d( new Attachment::Private( mime, true ) )
{
  d->mEncodedData = base64;
}

Attachment::~Attachment()
{
  delete d;
}

bool Attachment::isUri() const
{
  return !d->mBinary;
}

QString Attachment::uri() const
{
  if ( !d->mBinary ) {
    return d->mUri;
  } else {
    return QString();
  }
}

void Attachment::setUri( const QString &uri )
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
  if ( d->mBinary ) {
    return d->mEncodedData;
  } else {
    return QByteArray();
  }
}

QByteArray Attachment::decodedData() const
{
  if ( d->mDecodedDataCache.isNull() ) {
    d->mDecodedDataCache = QByteArray::fromBase64( d->mEncodedData );
  }

  return d->mDecodedDataCache;
}

void Attachment::setDecodedData( const QByteArray &data )
{
  setData( data.toBase64() );
  d->mDecodedDataCache = data;
  d->mSize = d->mDecodedDataCache.size();
}

void Attachment::setData( const QByteArray &base64 )
{
  d->mEncodedData = base64;
  d->mBinary = true;
  d->mDecodedDataCache = QByteArray();
  d->mSize = 0;
}

uint Attachment::size() const
{
  if ( isUri() ) {
    return 0;
  }
  if ( !d->mSize ) {
    d->mSize = decodedData().size();
  }

  return d->mSize;
}

QString Attachment::mimeType() const
{
  return d->mMimeType;
}

void Attachment::setMimeType( const QString &mime )
{
  d->mMimeType = mime;
}

bool Attachment::showInline() const
{
  return d->mShowInline;
}

void Attachment::setShowInline( bool showinline )
{
  d->mShowInline = showinline;
}

QString Attachment::label() const
{
  return d->mLabel;
}

void Attachment::setLabel( const QString &label )
{
  d->mLabel = label;
}

bool Attachment::isLocal() const
{
  return d->mLocal;
}

void Attachment::setLocal( bool local )
{
  d->mLocal = local;
}

Attachment &Attachment::operator=( const Attachment &other )
{
  if ( this != &other ) {
    d->mSize = other.d->mSize;
    d->mMimeType = other.d->mMimeType;
    d->mUri = other.d->mUri;
    d->mEncodedData = other.d->mEncodedData;
    d->mLabel = other.d->mLabel;
    d->mBinary = other.d->mBinary;
    d->mLocal  = other.d->mLocal;
    d->mShowInline = other.d->mShowInline;
  }

  return *this;
}

bool Attachment::operator==( const Attachment &a2 ) const
{
  return uri()          == a2.uri() &&
         d->mLabel      == a2.label() &&
         d->mLocal      == a2.isLocal() &&
         d->mBinary     == a2.isBinary() &&
         d->mShowInline == a2.showInline() &&
         size()         == a2.size() &&
         decodedData()  == a2.decodedData();
}

bool Attachment::operator!=( const Attachment &a2 ) const
{
  return !( *this == a2 );
}
