/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the CalFormat abstract base class.
*/

#ifndef KCALCORE_CALFORMAT_H
#define KCALCORE_CALFORMAT_H

#include "calendar.h"
#include "kcalendarcore_export.h"

#include <QString>

#include <memory>

namespace KCalendarCore
{
class CalFormatPrivate;
class Exception;

/*!
  \class KCalendarCore::CalFormat
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/CalFormat
  \brief
  An abstract base class that provides an interface to various calendar formats.

  This is the base class for calendar formats. It provides an interface for the
  generation/interpretation of a textual representation of a calendar.
*/
class KCALENDARCORE_EXPORT CalFormat
{
public:
    /*!
      Destructor.
    */
    virtual ~CalFormat();

    /*!
      Loads a calendar on disk into the calendar associated with this format.

      \a calendar is the Calendar to be loaded.

      \a fileName is the name of the disk file containing the Calendar data.

      Returns true if successful; false otherwise.
    */
    virtual bool load(const Calendar::Ptr &calendar, const QString &fileName) = 0;

    /*!
      Writes the calendar to disk.

      \a calendar is the Calendar containing the data to be saved.

      \a fileName is the name of the file to write the calendar data.

      Returns true if successful; false otherwise.
    */
    virtual bool save(const Calendar::Ptr &calendar, const QString &fileName) = 0;

    /*!
      Loads a calendar from a string.

      \a calendar is the Calendar to be loaded.

      \a string is the QString containing the Calendar data.

      Returns true if successful; false otherwise.
      \sa fromRawString(), toString().

      \since 5.97
    */
    bool fromString(const Calendar::Ptr &calendar, const QString &string);

    /*!
      Parses a utf8 encoded string, returning the first iCal component
      encountered in that string. This is an overload used for efficient
      reading to avoid utf8 conversions, which are expensive when reading
      from disk.

      \a calendar is the Calendar to be loaded.

      \a string is the QByteArray containing the Calendar data.

      Returns true if successful; false otherwise.
      \sa fromString(), toString().
    */
    virtual bool fromRawString(const Calendar::Ptr &calendar, const QByteArray &string) = 0;

    /*!
      Returns the calendar as a string.

      \a calendar is the Calendar containing the data to be saved.

      Returns a QString containing the Calendar data if successful;
      an empty string otherwise.
      \sa fromString(), fromRawString().
    */
    virtual QString toString(const Calendar::Ptr &calendar) = 0;

    /*!
      Clears the exception status.
    */
    void clearException();

    /*!
      Returns an exception, if there is any, containing information about the
      last error that occurred.
    */
    Exception *exception() const;

    /*!
      Sets the application name for use in unique IDs and error messages,
      and product ID for incidence PRODID property

      \a application is a string containing the application name.

      \a productID is a string containing the product identifier.

    */
    static void setApplication(const QString &application, const QString &productID);

    /*!
      Returns the application name used in unique IDs and error messages.
    */
    static const QString &application();

    /*!
      Returns the our library's PRODID string to write into calendar files.
    */
    static const QString &productId();

    /*!
      Returns the PRODID string loaded from calendar file.
      \sa setLoadedProductId()
    */
    QString loadedProductId();

    /*!
      Creates a unique id string.
    */
    static QString createUniqueId();

    /*!
      Sets an exception that is to be used by the functions of this class
      to report errors.

      \a error is a pointer to an Exception which contains the exception.

    */
    void setException(Exception *error);

protected:
    /*!
      Sets the PRODID string loaded from calendar file.

      \a id is a product Id string to set for the calendar file.

      \sa loadedProductId()
    */
    void setLoadedProductId(const QString &id);

    //@cond PRIVATE
    KCALENDARCORE_NO_EXPORT explicit CalFormat(CalFormatPrivate *dd);
    std::unique_ptr<CalFormatPrivate> d_ptr;
    //@endcond

private:
    //@cond PRIVATE
    Q_DISABLE_COPY(CalFormat)
    Q_DECLARE_PRIVATE(CalFormat)
    //@endcond
};

}

#endif
