/*
  This file is part of the kcalcore library.

  Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

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
  defines the Exception class.

  We don't use actual C++ exceptions right now. These classes are currently
  returned by an error function; but we can build upon them, if/when we start
  to use C++ exceptions.

  @brief
  Exception base class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/

#include "exceptions.h"
#include "calformat.h"

using namespace KCalCore;

Exception::Exception( const ErrorCode code, const QStringList &arguments )
  : mCode( code ), mArguments( arguments ), d( 0 )
{
}

Exception::~Exception()
{
}

Exception::ErrorCode Exception::code() const
{
  return mCode;
}

QStringList Exception::arguments() const
{
  return mArguments;
}

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class KCalCore::Exception::Private
{
};
//@endcond
