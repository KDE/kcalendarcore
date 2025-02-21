/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Exception class.

  We don't use actual C++ exceptions right now. These classes are currently
  returned by an error function; but we can build upon them, if/when we start
  to use C++ exceptions.

  \brief
  Exception base class.
*/

#ifndef KCALCORE_EXCEPTIONS_H
#define KCALCORE_EXCEPTIONS_H

#include "kcalendarcore_export.h"

#include <QString>
#include <QStringList>

#include <memory>

namespace KCalendarCore
{
class ExceptionPrivate;

/*!
  \class KCalendarCore::Exception
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Exception
  \brief Exception base class, currently used as a fancy kind of error code
  and not as an C++ exception.
*/
class KCALENDARCORE_EXPORT Exception
{
public:
    /*!
      \enum KCalendarCore::Exception::ErrorCode
      \brief The different types of error codes.
      \value LoadError
      Load error.
      \value SaveError
      Save error.
      \value ParseErrorIcal
      Parse error in libical.
      \value ParseErrorKcal
      Parse error in libkcal.
      \value NoCalendar
      No calendar component found.
      \value CalVersion1
      vCalendar v1.0 detected.
      \value CalVersion2
      iCalendar v2.0 detected.
      \value CalVersionUnknown
      Unknown calendar format detected.
      \value Restriction
      Restriction violation.
      \value UserCancel
      User canceled the operation.
      \value NoWritableFound
      No writable resource is available.
      \value SaveErrorOpenFile
      \value SaveErrorSaveFile
      \value LibICalError
      \value VersionPropertyMissing
      \value ExpectedCalVersion2
      \value ExpectedCalVersion2Unknown
      \value ParseErrorNotIncidence
      \value ParseErrorEmptyMessage
      \value ParseErrorUnableToParse
      \value ParseErrorMethodProperty
    */
    enum ErrorCode {
        LoadError,
        SaveError,
        ParseErrorIcal,
        ParseErrorKcal,
        NoCalendar,
        CalVersion1,
        CalVersion2,
        CalVersionUnknown,
        Restriction,
        UserCancel,
        NoWritableFound,
        SaveErrorOpenFile,
        SaveErrorSaveFile,
        LibICalError,
        VersionPropertyMissing,
        ExpectedCalVersion2,
        ExpectedCalVersion2Unknown,
        ParseErrorNotIncidence,
        ParseErrorEmptyMessage,
        ParseErrorUnableToParse,
        ParseErrorMethodProperty,
    };

    /*!
      Construct an exception.

      \a code is the error code.

      \a arguments is a list of arguments that can be passed
             to an i18n engine to help build a descriptive message for the user, a common
             argument is for example the filename where the error occurred.

    */
    explicit Exception(const ErrorCode code, const QStringList &arguments = QStringList());

    /*!
      Destructor.
    */
    virtual ~Exception();

    /*!
      Returns the error code.
      Returns The ErrorCode for this exception.
    */
    Q_REQUIRED_RESULT virtual ErrorCode code() const;

    /*!
      Returns the arguments.
      Returns A QStringList with the argument list for this exception.
    */
    Q_REQUIRED_RESULT virtual QStringList arguments() const;

private:
    std::unique_ptr<ExceptionPrivate> d;
};

} // namespace

#endif
