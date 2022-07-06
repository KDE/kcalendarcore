/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the CalFormat base class.

  @brief
  Base class providing an interface to various calendar formats.

  @author Cornelius Schumacher \<schumacher@kde.org\>
*/

#include "calformat.h"
#include "calformat_p.h"
#include "exceptions.h"

#include <QUuid>

using namespace KCalendarCore;

CalFormatPrivate::~CalFormatPrivate() = default;

QString CalFormatPrivate::mApplication = QStringLiteral("libkcal");
QString CalFormatPrivate::mProductId = QStringLiteral("-//K Desktop Environment//NONSGML libkcal 4.3//EN");

#if KCALENDARCORE_BUILD_DEPRECATED_SINCE(5, 96)
CalFormat::CalFormat()
    : d_ptr(new KCalendarCore::CalFormatPrivate)
{
}
#endif

CalFormat::CalFormat(CalFormatPrivate *dd)
    : d_ptr(dd)
{
}

CalFormat::~CalFormat()
{
    clearException();
}

bool CalFormat::fromString(const Calendar::Ptr &calendar, const QString &string, const QString &notebook)
{
    return fromRawString(calendar, string.toUtf8(), false, notebook);
}

void CalFormat::clearException()
{
    d_ptr->mException.reset();
}

void CalFormat::setException(Exception *exception)
{
    d_ptr->mException.reset(exception);
}

Exception *CalFormat::exception() const
{
    return d_ptr->mException.get();
}

void CalFormat::setApplication(const QString &application, const QString &productID)
{
    CalFormatPrivate::mApplication = application;
    CalFormatPrivate::mProductId = productID;
}

const QString &CalFormat::application()
{
    return CalFormatPrivate::mApplication;
}

const QString &CalFormat::productId()
{
    return CalFormatPrivate::mProductId;
}

QString CalFormat::loadedProductId()
{
    return d_ptr->mLoadedProductId;
}

void CalFormat::setLoadedProductId(const QString &id)
{
    d_ptr->mLoadedProductId = id;
}

QString CalFormat::createUniqueId()
{
    return QUuid::createUuid().toString().mid(1, 36);
}

#if KCALENDARCORE_BUILD_DEPRECATED_SINCE(5, 96)
void CalFormat::virtual_hook(int id, void *data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);
    Q_ASSERT(false);
}
#endif
