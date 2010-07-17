/*
  This file is part of the kcalcore library.

  Copyright (c) 2001-2003 Cornelius Schumacher <schumacher@kde.org>

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

#ifndef KCALCORE_EXCEPTIONS_H
#define KCALCORE_EXCEPTIONS_H

#include "kcalcore_export.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

namespace KCalCore {

/**
  Exception base class, currently used as a fancy kind of error code
  and not as an C++ exception.
*/
class Exception
{
  public:

    /**
      The different types of error codes
    */
    //KDAB_TODO: give decent names here
    enum ErrorCode {
      LoadError,         /**< Load error */
      SaveError,         /**< Save error */
      ParseErrorIcal,    /**< Parse error in libical */
      ParseErrorKcal,    /**< Parse error in libkcal */
      NoCalendar,        /**< No calendar component found */
      CalVersion1,       /**< vCalendar v1.0 detected */
      CalVersion2,       /**< iCalendar v2.0 detected */
      CalVersionUnknown, /**< Unknown calendar format detected */
      Restriction,       /**< Restriction violation */
      UserCancel,        /**< User canceled the operation */
      NoWritableFound,   /**< No writable resource is available */
      SaveErrorOpenFile,
      SaveErrorSaveFile,
      LibICalError,
      VersionPropertyMissing,
      ExpectedCalVersion2,
      ExpectedCalVersion2Unknown,
      ParseErrorNotIncidence,
      ParseErrorEmptyMessage,
      ParseErrorUnableToParse,
      ParseErrorMethodProperty
    };

    /**
      Construct an exception.
      @param code is the error code.
      @param arguments is a list of arguments that can be passed
             to an i18n engine to help build a descriptive message for the user, a common
             argument is for example the filename where the error occurred.
    */
    explicit Exception( const ErrorCode code,
                        const QStringList &arguments = QStringList() );

    /**
      Destructor.
    */
    virtual ~Exception();

    /**
      Returns the error code
    */
    virtual ErrorCode code() const;

    /**
       Returns the arguments.
    */
    virtual QStringList arguments() const;

  protected:
    /** The current exception code. */
    ErrorCode mCode;

    /** Arguments to pass to i18n(). */
    QStringList mArguments;

  private:
    //@cond PRIVATE
    Q_DISABLE_COPY( Exception )
    class Private;
    Private *const d;
    //@endcond
};

} // namespace

#endif
