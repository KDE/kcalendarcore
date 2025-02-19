/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2002, 2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the FileStorage class.
*/

#ifndef KCALCORE_FILESTORAGE_H
#define KCALCORE_FILESTORAGE_H

#include "calstorage.h"
#include "kcalendarcore_export.h"

namespace KCalendarCore
{
class CalFormat;
class Calendar;

/*!
  \class KCalendarCore::FileStorage
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/FileStorage
  \brief
  This class provides a calendar storage as a local file.
*/
class KCALENDARCORE_EXPORT FileStorage : public CalStorage
{
    Q_OBJECT
public:
    /*!
      A shared pointer to a FileStorage.
    */
    typedef QSharedPointer<FileStorage> Ptr;

    /*!
      Constructs a new FileStorage object for Calendar \a calendar with format
      \a format, and storage to file \a fileName.

      \a calendar is a pointer to a valid Calendar object.

      \a fileName is the name of the disk file containing the calendar data.

      \a format is a pointer to a valid CalFormat object that specifies
      the calendar format to be used. FileStorage takes ownership; i.e., the
      memory for \a format is deleted by this destructor. If no format is
      specified, then iCalendar format is assumed.
    */
    explicit FileStorage(const Calendar::Ptr &calendar, const QString &fileName = QString(), KCalendarCore::CalFormat *format = nullptr);

    /*!
      Destructor.
    */
    ~FileStorage() override;

    /*!
      Sets the name of the file that contains the calendar data.

      \a fileName is the name of the disk file containing the calendar data.

      \sa fileName().
    */
    void setFileName(const QString &fileName);

    /*!
      Returns the calendar file name.

      Returns a QString with the name of the calendar file for this storge.
      \sa setFileName().
    */
    Q_REQUIRED_RESULT QString fileName() const;

    /*!
      Sets the CalFormat object to use for this storage.

      \a format is a pointer to a valid CalFormat object that specifies
      the calendar format to be used. FileStorage takes ownership.

      \sa saveFormat().
    */
    void setSaveFormat(KCalendarCore::CalFormat *format);

    /*!
      Returns the CalFormat object used by this storage.
      Returns A pointer to the CalFormat object used by this storage.
      \sa setSaveFormat().
    */
    CalFormat *saveFormat() const;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT bool open() override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT bool load() override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT bool save() override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT bool close() override;

private:
    //@cond PRIVATE
    Q_DISABLE_COPY(FileStorage)
    class Private;
    Private *const d;
    //@endcond
};

}

#endif
