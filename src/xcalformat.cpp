/*
  SPDX-FileCopyrightText: 2024 Volker Krause <vkrause@kde.org>
  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "xcalformat.h"

#include "calformat_p.h"
#include "event.h"
#include "icalformat.h"
#include "icalformat_p.h"
#include "kcalendarcore_debug.h"

#include <QFile>
#include <QXmlStreamReader>

extern "C" {
#include <libical/ical.h>
}

using namespace Qt::Literals;
using namespace KCalendarCore;

namespace KCalendarCore
{

// TODO why is this not in libical??
struct {
    icalproperty_class cls;
    const char *name;
} constexpr static inline const ical_class_map[] = {
    {ICAL_CLASS_PUBLIC, "PUBLIC"},
    {ICAL_CLASS_PRIVATE, "PRIVATE"},
    {ICAL_CLASS_CONFIDENTIAL, "CONFIDENTIAL"},
};

icalproperty_class icalenum_string_to_class(const char *name)
{
    for (const auto &m : ical_class_map) {
        if (std::strcmp(m.name, name) == 0) {
            return m.cls;
        }
    }
    return ICAL_CLASS_NONE;
}

icalproperty_transp icalenum_string_to_transp(const char *name)
{
    return std::strcmp("TRANSPARENT", name) == 0 ? ICAL_TRANSP_TRANSPARENT : ICAL_TRANSP_OPAQUE;
}

class XCalProperty
{
public:
    QVariant value;
    QHash<QString, QString> params;

    [[nodiscard]] QString toString() const;

    [[nodiscard]] bool isDate() const;
    [[nodiscard]] QDateTime toDateTime() const;
    [[nodiscard]] QDate toDate() const;
};

class XCalFormatPrivate : public CalFormatPrivate
{
public:
    enum {
        Rfc6321, // https://datatracker.ietf.org/doc/html/rfc6321
        Legacy // https://datatracker.ietf.org/doc/html/draft-royer-calsch-xcal-03
    } m_format = Rfc6321;

    void parseXCal(QXmlStreamReader &reader, const Calendar::Ptr &calendar);
    void parseVcalendar(QXmlStreamReader &reader, const Calendar::Ptr &calendar, QStringView elemName);
    void parseVevent(QXmlStreamReader &reader, const Event::Ptr &event, QStringView elemName);
    void parseRRule(QXmlStreamReader &reader, RecurrenceRule *rrule, QStringView elemName);
    XCalProperty parseProperty(QXmlStreamReader &reader);
};
}

QString XCalProperty::toString() const
{
    return value.toString();
}

bool XCalProperty::isDate() const
{
    return value.typeId() == QMetaType::QDate || (value.typeId() == QMetaType::QString && value.toString().size() == 8);
}

QDateTime XCalProperty::toDateTime() const
{
    switch (value.typeId()) {
    case QMetaType::QDateTime: {
        auto dt = value.toDateTime();
        if (const auto tzId = params.value("tzid"_L1); !tzId.isEmpty()) {
            dt.setTimeZone(QTimeZone(tzId.toUtf8()));
        }
        return dt;
    }
    case QMetaType::QString:
        if (value.toString().size() == 16) {
            auto dt = QDateTime::fromString(value.toString(), u"yyyyMMddThhmmssZ");
            dt.setTimeZone(QTimeZone::utc());
            return dt;
        }
        return QDateTime::fromString(value.toString(), u"yyyyMMddThhmmss");
    }
    return {};
}

QDate XCalProperty::toDate() const
{
    switch (value.typeId()) {
    case QMetaType::QDate:
        return value.toDate();
    case QMetaType::QString:
        return QDate::fromString(value.toString(), u"yyyyMMdd");
    }
    return {};
}

void XCalFormatPrivate::parseXCal(QXmlStreamReader &reader, const Calendar::Ptr &calendar)
{
    while (!reader.atEnd() && !reader.hasError()) {
        if (reader.isEndDocument()) {
            return;
        }
        if (!reader.isStartElement()) {
            reader.readNext();
            continue;
        }

        if (reader.name().compare("iCalendar"_L1, Qt::CaseInsensitive) == 0) {
            for (const auto &a : reader.namespaceDeclarations()) {
                if (a.namespaceUri() == "urn:ietf:params:xml:ns:icalendar-2.0"_L1) {
                    m_format = Rfc6321;
                } else if (a.namespaceUri() == "urn:ietf:params:xml:ns:xcal"_L1) {
                    m_format = Legacy;
                }
            }
            reader.readNextStartElement();
        } else if (reader.name() == "vcalendar"_L1) {
            parseVcalendar(reader, calendar, reader.name());
        } else {
            qCDebug(KCALCORE_LOG) << "unhandled xcal element" << reader.name();
            reader.skipCurrentElement();
        }
    }
}

void XCalFormatPrivate::parseVcalendar(QXmlStreamReader &reader, const Calendar::Ptr &calendar, QStringView elemName)
{
    reader.readNext();
    while (!reader.atEnd() && !reader.hasError()) {
        if (reader.isEndElement() && reader.name() == elemName) {
            return;
        }
        if (!reader.isStartElement()) {
            reader.readNext();
            continue;
        }

        if (reader.name() == "components"_L1 && m_format == Rfc6321) {
            parseVcalendar(reader, calendar, reader.name());
        } else if (reader.name() == "prodid"_L1) {
            mProductId = parseProperty(reader).toString();
        } else if (reader.name() == "properties"_L1 && m_format == Rfc6321) {
            parseVcalendar(reader, calendar, reader.name());
        } else if (reader.name() == "vevent"_L1) {
            Event::Ptr event(new Event());
            parseVevent(reader, event, reader.name());
            calendar->addEvent(event);
        } else {
            qCDebug(KCALCORE_LOG) << "unhandled xcal element" << reader.name();
            reader.skipCurrentElement();
        }
    }
}

void XCalFormatPrivate::parseVevent(QXmlStreamReader &reader, const Event::Ptr &event, QStringView elemName)
{
    reader.readNext();
    while (!reader.atEnd() && !reader.hasError()) {
        if (reader.isEndElement() && reader.name() == elemName) {
            return;
        }
        if (!reader.isStartElement()) {
            reader.readNext();
            continue;
        }

        if (reader.name() == "attendee"_L1) {
            Attendee a;
            if (reader.attributes().value("rsvp"_L1).compare("true"_L1, Qt::CaseInsensitive) == 0) {
                a.setRSVP(true);
            }
            if (const auto role = reader.attributes().value("role"_L1); !role.isEmpty()) {
                a.setRole(ICalFormatImpl::fromIcalEnum((icalparameter_role)icalparameter_string_to_enum(role.toUtf8().constData())));
            }
            // TODO handle more attributes, handle RFC 6321 property parameters

            const auto p = Person::fromFullName(parseProperty(reader).toString());
            a.setName(p.name());
            a.setEmail(p.email());

            event->addAttendee(a);
        } else if (reader.name() == "category"_L1) {
            auto l = event->categories();
            l.push_back(parseProperty(reader).toString());
            event->setCategories(l);
        } else if (reader.name() == "categories"_L1) {
            event->setCategories(parseProperty(reader).toString().split(','_L1));
        } else if (reader.name() == "class"_L1) {
            event->setSecrecy(ICalFormatImpl::fromIcalEnum(icalenum_string_to_class(parseProperty(reader).toString().toUtf8().constData())));
        } else if (reader.name() == "description"_L1) {
            event->setDescription(parseProperty(reader).toString());
        } else if (reader.name() == "dtend"_L1) {
            const auto prop = parseProperty(reader);
            if (prop.isDate()) {
                event->setDtEnd(prop.toDate().endOfDay());
                event->setAllDay(true);
            } else {
                event->setDtEnd(prop.toDateTime());
            }
        } else if (reader.name() == "dtstamp"_L1) {
            event->setLastModified(parseProperty(reader).toDateTime());
        } else if (reader.name() == "dtstart"_L1) {
            const auto prop = parseProperty(reader);
            if (prop.isDate()) {
                event->setDtStart(prop.toDate().startOfDay());
                event->setAllDay(true);
            } else {
                event->setDtStart(prop.toDateTime());
            }
        } else if (reader.name() == "duration"_L1 && m_format == Rfc6321) {
            event->setDuration(parseProperty(reader).value.value<Duration>());
        } else if (reader.name() == "location"_L1) {
            event->setLocation(parseProperty(reader).toString());
        } else if (reader.name() == "organizer"_L1) {
            event->setOrganizer(Person::fromFullName(parseProperty(reader).toString()));
        } else if (reader.name() == "properties"_L1 && m_format == Rfc6321) {
            parseVevent(reader, event, reader.name());
        } else if (reader.name() == "rdate"_L1) {
            event->recurrence()->addRDateTimePeriod(parseProperty(reader).value.value<Period>());
        } else if (reader.name() == "recurrence-id"_L1) {
            event->setRecurrenceId(parseProperty(reader).toDateTime());
        } else if (reader.name() == "rrule"_L1) {
            if (m_format == Legacy) {
                auto rrule = std::make_unique<RecurrenceRule>();
                ICalFormat f;
                if (f.fromString(rrule.get(), parseProperty(reader).toString())) {
                    event->recurrence()->addRRule(rrule.release());
                }
            } else if (m_format == Rfc6321) {
                auto rrule = std::make_unique<RecurrenceRule>();
                parseRRule(reader, rrule.get(), reader.name());
                event->recurrence()->addRRule(rrule.release());
            } else {
                reader.skipCurrentElement();
            }
        } else if (reader.name() == "status"_L1) {
            event->setStatus(ICalFormatImpl::fromIcalEnum(icalproperty_string_to_status(parseProperty(reader).toString().toUtf8().constData())));
        } else if (reader.name() == "summary"_L1) {
            event->setSummary(parseProperty(reader).toString());
        } else if (reader.name() == "transp"_L1) {
            event->setTransparency(ICalFormatImpl::fromIcalEnum(icalenum_string_to_transp(parseProperty(reader).toString().toUtf8().constData())));
        } else if (reader.name() == "uid"_L1) {
            event->setUid(parseProperty(reader).toString());
        } else if (reader.name() == "url"_L1) {
            event->setUrl(QUrl(parseProperty(reader).toString()));
        } else if (reader.name().startsWith("x-"_L1) && m_format == Legacy) {
            event->setCustomProperties({{reader.name().toUtf8().toUpper(), parseProperty(reader).toString()}});
        } else {
            qCDebug(KCALCORE_LOG) << "unhandled xcal element" << reader.name();
            reader.skipCurrentElement();
        }
    }
}

void XCalFormatPrivate::parseRRule(QXmlStreamReader &reader, RecurrenceRule *rrule, QStringView elemName)
{
    reader.readNext();
    while (!reader.atEnd() && !reader.hasError()) {
        if (reader.isEndElement() && reader.name() == elemName) {
            return;
        }
        if (!reader.isStartElement()) {
            reader.readNext();
            continue;
        }

        // TODO misses more parameters
        if (reader.name() == "bymonth"_L1) {
            auto b = rrule->byMonths();
            b.push_back(reader.readElementText().toInt());
            rrule->setByMonths(b);
        } else if (reader.name() == "count"_L1) {
            rrule->setDuration(reader.readElementText().toInt());
        } else if (reader.name() == "freq"_L1) {
            rrule->setRecurrenceType(
                ICalFormatImpl::fromIcalEnum((icalrecurrencetype_frequency)icalrecur_string_to_freq(reader.readElementText().toUtf8().constData())));
        } else if (reader.name() == "interval"_L1) {
            rrule->setFrequency(reader.readElementText().toInt());
        } else if (reader.name() == "recur"_L1) {
            parseRRule(reader, rrule, reader.name());
        } else {
            qCDebug(KCALCORE_LOG) << "unhandled xcal element" << reader.name();
            reader.skipCurrentElement();
        }
    }
}

XCalProperty XCalFormatPrivate::parseProperty(QXmlStreamReader &reader)
{
    if (m_format == Legacy) {
        return {reader.readElementText(), {}};
    }

    Duration periodDuration;
    XCalProperty prop;
    auto elemName = reader.name();
    reader.readNext();
    while (!reader.atEnd() && !reader.hasError()) {
        if (reader.isEndElement() && reader.name() == elemName) {
            break;
        }
        if (!reader.isStartElement()) {
            reader.readNext();
            continue;
        }

        if (reader.name() == "date"_L1) {
            prop.value = QDate::fromString(reader.readElementText(), Qt::ISODate);
        } else if (reader.name() == "date-time"_L1) {
            prop.value = QDateTime::fromString(reader.readElementText(), Qt::ISODate);
        } else if (reader.name() == "duration"_L1) {
            ICalFormat f;
            prop.value = QVariant::fromValue(f.durationFromString(reader.readElementText()));
        } else if (reader.name() == "parameters"_L1) {
            reader.readNext();
            while (!reader.atEnd() && !reader.hasError()) {
                if (reader.isEndElement() && reader.name() == "parameters"_L1) {
                    break;
                }
                if (!reader.isStartElement()) {
                    reader.readNext();
                    continue;
                }

                prop.params.insert(reader.name().toString(), parseProperty(reader).toString());
            }
        } else if (reader.name() == "period"_L1) {
            reader.readNext();
            while (!reader.atEnd() && !reader.hasError()) {
                if (reader.isEndElement() && reader.name() == "period"_L1) {
                    break;
                }
                if (!reader.isStartElement()) {
                    reader.readNext();
                    continue;
                }

                if (reader.name() == "start"_L1) {
                    prop.value = QDateTime::fromString(reader.readElementText(), Qt::ISODate);
                } else if (reader.name() == "duration"_L1) {
                    ICalFormat f;
                    periodDuration = f.durationFromString(reader.readElementText());
                } else {
                    qCDebug(KCALCORE_LOG) << "unhandled xcal element" << reader.name();
                    reader.skipCurrentElement();
                }
            }

        } else if (reader.name() == "text"_L1) {
            prop.value = reader.readElementText();
        } else {
            qCDebug(KCALCORE_LOG) << "unhandled xcal element" << reader.name();
            reader.skipCurrentElement();
        }
    }

    // we need parameters fully parsed to get the timezone right
    if (!periodDuration.isNull()) {
        prop.value = QVariant::fromValue(Period(prop.toDateTime(), periodDuration));
    }

    return prop;
}

XCalFormat::XCalFormat()
    : CalFormat(new XCalFormatPrivate)
{
}

XCalFormat::~XCalFormat() = default;

bool XCalFormat::load(const Calendar::Ptr &calendar, const QString &fileName)
{
    Q_D(XCalFormat);
    clearException();

    QFile f(fileName);
    if (!f.open(QFile::ReadOnly)) {
        setException(new Exception(Exception::LoadError, {f.errorString()}));
        return false;
    }

    QXmlStreamReader reader(&f);
    d->parseXCal(reader, calendar);

    if (reader.hasError()) {
        setException(new Exception(Exception::ParseErrorUnableToParse, {reader.errorString()}));
        return false;
    }

    return true;
}

bool XCalFormat::save([[maybe_unused]] const Calendar::Ptr &calendar, [[maybe_unused]] const QString &fileName)
{
    qCWarning(KCALCORE_LOG) << "Exporting into xCalendar is not supported";
    return false;
}

bool XCalFormat::fromRawString(const Calendar::Ptr &calendar, const QByteArray &string)
{
    Q_D(XCalFormat);
    clearException();

    QXmlStreamReader reader(string);
    d->parseXCal(reader, calendar);

    if (reader.hasError()) {
        setException(new Exception(Exception::ParseErrorUnableToParse, {reader.errorString()}));
        return false;
    }

    return true;
}

QString XCalFormat::toString([[maybe_unused]] const Calendar::Ptr &calendar)
{
    qCWarning(KCALCORE_LOG) << "Exporting into xCalendar is not supported";
    return {};
}
