/*
  This file is part of the kcalcore library.

  Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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
  defines the FileStorage class.

  @brief
  This class provides a calendar storage as a local file.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/
#include "filestorage.h"
#include "exceptions.h"
#include "icalformat.h"
#include "memorycalendar.h"
#include "vcalformat.h"

#include <KDebug>

using namespace KCalCore;

/*
  Private class that helps to provide binary compatibility between releases.
*/
//@cond PRIVATE
class KCalCore::FileStorage::Private
{
  public:
    Private( const QString &fileName, CalFormat *format )
      : mFileName( fileName ),
        mSaveFormat( format )
    {}
    ~Private() { delete mSaveFormat; }

    QString mFileName;
    CalFormat *mSaveFormat;
};
//@endcond

FileStorage::FileStorage( const Calendar::Ptr &cal, const QString &fileName,
                          CalFormat *format )
  : CalStorage( cal ),
    d( new Private( fileName, format ) )
{
}

FileStorage::~FileStorage()
{
  delete d;
}

void FileStorage::setFileName( const QString &fileName )
{
  d->mFileName = fileName;
}

QString FileStorage::fileName() const
{
  return d->mFileName;
}

void FileStorage::setSaveFormat( CalFormat *format )
{
  delete d->mSaveFormat;
  d->mSaveFormat = format;
}

CalFormat *FileStorage::saveFormat() const
{
  return d->mSaveFormat;
}

bool FileStorage::open()
{
  return true;
}

bool FileStorage::load()
{
  // do we want to silently accept this, or make some noise?  Dunno...
  // it is a semantical thing vs. a practical thing.
  if ( d->mFileName.isEmpty() ) {
    return false;
  }

  // Always try to load with iCalendar. It will detect, if it is actually a
  // vCalendar file.
  bool success;
  // First try the supplied format. Otherwise fall through to iCalendar, then
  // to vCalendar
  success = saveFormat() && saveFormat()->load( calendar(), d->mFileName );
  if ( !success ) {
    ICalFormat iCal;

    success = iCal.load( calendar(), d->mFileName );

    if ( !success ) {
      if ( iCal.exception() ) {
        if ( iCal.exception()->code() == Exception::CalVersion1 ) {
          // Expected non vCalendar file, but detected vCalendar
          kDebug() << "Fallback to VCalFormat";
          VCalFormat vCal;
          success = vCal.load( calendar(), d->mFileName );
          calendar()->setProductId( vCal.productId() );
        } else {
          return false;
        }
      } else {
        kDebug() << "Warning! There should be an exception set.";
        return false;
      }
    } else {
      calendar()->setProductId( iCal.loadedProductId() );
    }
  }

  calendar()->setModified( false );

  return true;
}

bool FileStorage::save()
{
  kDebug();
  if ( d->mFileName.isEmpty() ) {
    return false;
  }

  CalFormat *format = d->mSaveFormat ? d->mSaveFormat : new ICalFormat;

  bool success = format->save( calendar(), d->mFileName );

  if ( success ) {
    calendar()->setModified( false );
  } else {
    if ( !format->exception() ) {
      kDebug() << "Error. There should be an expection set.";
    } else {
      kDebug() << int( format->exception()->code() );
    }
  }

  if ( !d->mSaveFormat ) {
    delete format;
  }

  return success;
}

bool FileStorage::close()
{
  return true;
}
