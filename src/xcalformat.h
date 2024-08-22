/*
  SPDX-FileCopyrightText: 2024 Volker Krause <vkrause@kde.org>
  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCALCORE_XCALFORMAT_H
#define KCALCORE_XCALFORMAT_H

#include "calformat.h"

namespace KCalendarCore
{

class XCalFormatPrivate;

/** Read support for xCal events.
 *
 *  This is currently only exposed via the KCalendarCore::FileStorage API.
 *
 *  Supported variants:
 *  * RFC 6321 (https://datatracker.ietf.org/doc/html/rfc6321)
 *  * The pre-standardized draft version (https://datatracker.ietf.org/doc/html/draft-royer-calsch-xcal-03) used by Pretalx
 *
 *  @since 6.6
 */
class XCalFormat : public CalFormat
{
public:
    explicit XCalFormat();
    ~XCalFormat() override;

    /**
      @copydoc
      CalFormat::load()
    */
    bool load(const Calendar::Ptr &calendar, const QString &fileName) override;

    /** Does nothing. */
    bool save(const Calendar::Ptr &calendar, const QString &fileName) override;

    /**
      @copydoc
      CalFormat::fromRawString()
    */
    bool fromRawString(const Calendar::Ptr &calendar, const QByteArray &string) override;

    /** Does nothing. */
    [[nodiscard]] QString toString(const Calendar::Ptr &calendar) override;

private:
    //@cond PRIVATE
    Q_DECLARE_PRIVATE(XCalFormat)
    //@endcond
};

}

#endif
