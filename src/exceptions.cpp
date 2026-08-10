/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
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

#include <QCoreApplication>

using namespace KCalendarCore;

namespace KCalendarCore
{
class ExceptionPrivate
{
public:
    /**
      The current exception code.
    */
    Exception::ErrorCode mCode;

    /** Arguments to pass to i18n(). */
    QStringList mArguments;

    static QString tr(const char *s, const char *c = nullptr, int n = -1)
    {
        return QCoreApplication::translate("KCalendarCore::ExceptionPrivate", s, c, n);
    }
};

}

Exception::Exception(const ErrorCode code, const QStringList &arguments)
    : d(new ExceptionPrivate)
{
    d->mCode = code;
    d->mArguments = arguments;
}

Exception::~Exception() = default;

Exception::ErrorCode Exception::code() const
{
    return d->mCode;
}

QStringList Exception::arguments() const
{
    return d->mArguments;
}

QString Exception::errorMessage() const
{
    switch (code()) {
    case Exception::LoadError:
        return ExceptionPrivate::tr("Load Error", "@item");
    case Exception::SaveError:
        return ExceptionPrivate::tr("Save Error", "@item");
    case Exception::ParseErrorIcal:
        return ExceptionPrivate::tr("Parse Error in libical", "@item");
    case Exception::ParseErrorKcal:
        return ExceptionPrivate::tr("Parse Error in the kcalcore library", "@item");
    case Exception::NoCalendar:
        return ExceptionPrivate::tr("No calendar component found.", "@item");
    case Exception::CalVersion1:
        return ExceptionPrivate::tr("Expected iCalendar, got vCalendar format", "@item");
    case Exception::CalVersion2:
        return ExceptionPrivate::tr("iCalendar Version 2.0 detected.", "@item");
    case Exception::CalVersionUnknown:
        return ExceptionPrivate::tr("Expected iCalendar, got unknown format", "@item");
    case Exception::Restriction:
        return ExceptionPrivate::tr("Restriction violation", "@item");
    case Exception::NoWritableFound:
        return ExceptionPrivate::tr("No writable resource found", "@item");
    case Exception::SaveErrorOpenFile:
        Q_ASSERT(arguments().count() == 1);
        return ExceptionPrivate::tr("Error saving to '%1'.", "@item").arg(arguments().value(0));
    case Exception::SaveErrorSaveFile:
        Q_ASSERT(arguments().count() == 1);
        return ExceptionPrivate::tr("Could not save '%1'", "@item").arg(arguments().value(0));
    case Exception::LibICalError:
        return ExceptionPrivate::tr("libical error", "@item");
    case Exception::VersionPropertyMissing:
        return ExceptionPrivate::tr("No VERSION property found", "@item");
    case Exception::ExpectedCalVersion2:
        return ExceptionPrivate::tr("Expected iCalendar, got vCalendar format", "@item");
    case Exception::ExpectedCalVersion2Unknown:
        return ExceptionPrivate::tr("Expected iCalendar, got unknown format", "@item");
    case Exception::ParseErrorNotIncidence:
        return ExceptionPrivate::tr("Object is not a freebusy, event, todo or journal", "@item");
    case Exception::ParseErrorEmptyMessage:
        return ExceptionPrivate::tr("Message text is empty, unable to parse into a ScheduleMessage", "@item");
    case Exception::ParseErrorUnableToParse:
        return ExceptionPrivate::tr("icalparser is unable to parse message text into a ScheduleMessage", "@item");
    case Exception::ParseErrorMethodProperty:
        return ExceptionPrivate::tr("Message does not contain ICAL_METHOD_PROPERTY", "@item");
    case Exception::UserCancel:
        // no real error; the user canceled the operation
        break;
    }

    return {};
}
