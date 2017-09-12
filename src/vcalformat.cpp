/*
  This file is part of the kcalcore library.

  Copyright (c) 1998 Preston Brown <pbrown@kde.org>
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
  defines the VCalFormat base class.

  This class implements the vCalendar format. It provides methods for
  loading/saving/converting vCalendar format data into the internal
  representation as Calendar and Incidences.

  @brief
  vCalendar format implementation.

  @author Preston Brown \<pbrown@kde.org\>
  @author Cornelius Schumacher \<schumacher@kde.org\>
*/
#include "vcalformat.h"
#include "calendar.h"
#include "event.h"
#include "exceptions.h"
#include "icaltimezones.h"
#include "todo.h"
#include "utils.h"
#include "versit/vcc.h"
#include "versit/vobject.h"
#include "utils.h"

#include <KCodecs>
#include "kcalcore_debug.h"

#include <QBitArray>
#include <QFile>
#include <QTextDocument> // for .toHtmlEscaped() and Qt::mightBeRichText()
#include <QTimeZone>

using namespace KCalCore;

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
template <typename K>
void removeAllVCal(QVector< QSharedPointer<K> > &c, const QSharedPointer<K> &x)
{
    if (c.count() < 1) {
        return;
    }

    int cnt = c.count(x);
    if (cnt != 1) {
        qCritical() << "There number of relatedTos for this incidence is "
                    << cnt << " (there must be 1 relatedTo only)";
        Q_ASSERT_X(false, "removeAllVCal", "Count is not 1.");
        return;
    }

    c.remove(c.indexOf(x));
}

static QString dayFromNum(int day)
{
    static const QStringList days = { QStringLiteral("MO "), QStringLiteral("TU "), QStringLiteral("WE "),
                                      QStringLiteral("TH "), QStringLiteral("FR "), QStringLiteral("SA "), QStringLiteral("SU ")
                                    };

    return days[day];
}

class Q_DECL_HIDDEN KCalCore::VCalFormat::Private
{
public:
    Calendar::Ptr mCalendar;
    Event::List mEventsRelate;  // Events with relations
    Todo::List mTodosRelate;    // To-dos with relations
    QSet<QByteArray> mManuallyWrittenExtensionFields; // X- fields that are manually dumped
};
//@endcond

VCalFormat::VCalFormat() : d(new KCalCore::VCalFormat::Private)
{
}

VCalFormat::~VCalFormat()
{
    delete d;
}

bool VCalFormat::load(const Calendar::Ptr &calendar, const QString &fileName)
{
    d->mCalendar = calendar;

    clearException();

    VObject *vcal = nullptr;

    // this is not necessarily only 1 vcal.  Could be many vcals, or include
    // a vcard...
    vcal = Parse_MIME_FromFileName(const_cast<char *>(QFile::encodeName(fileName).data()));

    if (!vcal) {
        setException(new Exception(Exception::CalVersionUnknown));
        return false;
    }

    // any other top-level calendar stuff should be added/initialized here

    // put all vobjects into their proper places
    auto savedTimeZoneId = d->mCalendar->timeZoneId();
    populate(vcal, false, fileName);
    d->mCalendar->setTimeZoneId(savedTimeZoneId);

    // clean up from vcal API stuff
    cleanVObjects(vcal);
    cleanStrTbl();

    return true;
}

bool VCalFormat::save(const Calendar::Ptr &calendar, const QString &fileName)
{
    d->mCalendar = calendar;

    ICalTimeZones *tzlist = d->mCalendar->timeZones();

    VObject *vcal, *vo;

    vcal = newVObject(VCCalProp);

    //  addPropValue(vcal,VCLocationProp, "0.0");
    addPropValue(vcal, VCProdIdProp, productId().toLatin1().constData());
    addPropValue(vcal, VCVersionProp, _VCAL_VERSION);

    // TODO STUFF
    Todo::List todoList = d->mCalendar->rawTodos();
    Todo::List::ConstIterator it;
    for (it = todoList.constBegin(); it != todoList.constEnd(); ++it) {
        if ((*it)->dtStart().timeZone().name().mid(0, 4) == QLatin1String("VCAL")) {
            ICalTimeZone zone = tzlist->zone((*it)->dtStart().timeZone().name());
            if (zone.isValid()) {
                QByteArray timezone = zone.vtimezone();
                addPropValue(vcal, VCTimeZoneProp, parseTZ(timezone).toLocal8Bit().constData());
                QString dst = parseDst(timezone);
                while (!dst.isEmpty()) {
                    addPropValue(vcal, VCDayLightProp, dst.toLocal8Bit().constData());
                    dst = parseDst(timezone);
                }
            }
        }
        vo = eventToVTodo(*it);
        addVObjectProp(vcal, vo);
    }
    // EVENT STUFF
    Event::List events = d->mCalendar->rawEvents();
    Event::List::ConstIterator it2;
    for (it2 = events.constBegin(); it2 != events.constEnd(); ++it2) {
        if ((*it2)->dtStart().timeZone().name().mid(0, 4) == QLatin1String("VCAL")) {
            ICalTimeZone zone = tzlist->zone((*it2)->dtStart().timeZone().name());
            if (zone.isValid()) {
                QByteArray timezone = zone.vtimezone();
                addPropValue(vcal, VCTimeZoneProp, parseTZ(timezone).toLocal8Bit().constData());
                QString dst = parseDst(timezone);
                while (!dst.isEmpty()) {
                    addPropValue(vcal, VCDayLightProp, dst.toLocal8Bit().constData());
                    dst = parseDst(timezone);
                }
            }
        }
        vo = eventToVEvent(*it2);
        addVObjectProp(vcal, vo);
    }
    writeVObjectToFile(QFile::encodeName(fileName).data(), vcal);
    cleanVObjects(vcal);
    cleanStrTbl();

    if (QFile::exists(fileName)) {
        return true;
    } else {
        return false; // error
    }

    return false;
}

bool VCalFormat::fromString(const Calendar::Ptr &calendar, const QString &string,
                            bool deleted, const QString &notebook)
{
    return fromRawString(calendar, string.toUtf8(), deleted, notebook);
}

bool VCalFormat::fromRawString(const Calendar::Ptr &calendar, const QByteArray &string,
                               bool deleted, const QString &notebook)
{
    d->mCalendar = calendar;

    if (!string.size()) {
        return false;
    }

    VObject *vcal = Parse_MIME(string.data(), string.size());
    if (!vcal) {
        return false;
    }

    VObjectIterator i;
    initPropIterator(&i, vcal);

    // put all vobjects into their proper places
    auto savedTimeZoneId = d->mCalendar->timeZoneId();
    populate(vcal, deleted, notebook);
    d->mCalendar->setTimeZoneId(savedTimeZoneId);

    // clean up from vcal API stuff
    cleanVObjects(vcal);
    cleanStrTbl();

    return true;
}

QString VCalFormat::toString(const Calendar::Ptr &calendar,
                             const QString &notebook, bool deleted)
{
    // TODO: Factor out VCalFormat::asString()
    d->mCalendar = calendar;

    ICalTimeZones *tzlist = d->mCalendar->timeZones();

    VObject *vo;
    VObject *vcal = newVObject(VCCalProp);

    addPropValue(vcal, VCProdIdProp, CalFormat::productId().toLatin1().constData());
    addPropValue(vcal, VCVersionProp, _VCAL_VERSION);

    // TODO STUFF
    Todo::List todoList = deleted ? d->mCalendar->deletedTodos() : d->mCalendar->rawTodos();
    Todo::List::ConstIterator it;
    for (it = todoList.constBegin(); it != todoList.constEnd(); ++it) {
        if (!deleted || !d->mCalendar->todo((*it)->uid(), (*it)->recurrenceId())) {
            // use existing ones, or really deleted ones
            if (notebook.isEmpty() ||
                    (!calendar->notebook(*it).isEmpty() &&
                     notebook.endsWith(calendar->notebook(*it)))) {
                if ((*it)->dtStart().timeZone().name().mid(0, 4) == QLatin1String("VCAL")) {
                    ICalTimeZone zone = tzlist->zone((*it)->dtStart().timeZone().name());
                    if (zone.isValid()) {
                        QByteArray timezone = zone.vtimezone();
                        addPropValue(vcal, VCTimeZoneProp, parseTZ(timezone).toUtf8().constData());
                        QString dst = parseDst(timezone);
                        while (!dst.isEmpty()) {
                            addPropValue(vcal, VCDayLightProp, dst.toUtf8().constData());
                            dst = parseDst(timezone);
                        }
                    }
                }
                vo = eventToVTodo(*it);
                addVObjectProp(vcal, vo);
            }
        }
    }

    // EVENT STUFF
    Event::List events = deleted ? d->mCalendar->deletedEvents() : d->mCalendar->rawEvents();
    Event::List::ConstIterator it2;
    for (it2 = events.constBegin(); it2 != events.constEnd(); ++it2) {
        if (!deleted || !d->mCalendar->event((*it2)->uid(), (*it2)->recurrenceId())) {
            // use existing ones, or really deleted ones
            if (notebook.isEmpty() ||
                    (!calendar->notebook(*it2).isEmpty() &&
                     notebook.endsWith(calendar->notebook(*it2)))) {
                if ((*it2)->dtStart().timeZone().name().mid(0, 4) == QLatin1String("VCAL")) {
                    ICalTimeZone zone = tzlist->zone((*it2)->dtStart().timeZone().name());
                    if (zone.isValid()) {
                        QByteArray timezone = zone.vtimezone();
                        addPropValue(vcal, VCTimeZoneProp, parseTZ(timezone).toUtf8().constData());
                        QString dst = parseDst(timezone);
                        while (!dst.isEmpty()) {
                            addPropValue(vcal, VCDayLightProp, dst.toUtf8().constData());
                            dst = parseDst(timezone);
                        }
                    }
                }
                vo = eventToVEvent(*it2);
                addVObjectProp(vcal, vo);
            }
        }
    }

    char *buf = writeMemVObject(nullptr, nullptr, vcal);

    QString result(QString::fromUtf8(buf));

    deleteStr(buf);

    cleanVObject(vcal);

    return result;
}

VObject *VCalFormat::eventToVTodo(const Todo::Ptr &anEvent)
{
    VObject *vtodo;
    QString tmpStr;

    vtodo = newVObject(VCTodoProp);

    // due date
    if (anEvent->hasDueDate()) {
        tmpStr = kDateTimeToISO(anEvent->dtDue(), !anEvent->allDay());
        addPropValue(vtodo, VCDueProp, tmpStr.toUtf8().constData());
    }

    // start date
    if (anEvent->hasStartDate()) {
        tmpStr = kDateTimeToISO(anEvent->dtStart(), !anEvent->allDay());
        addPropValue(vtodo, VCDTstartProp, tmpStr.toUtf8().constData());
    }

    // creation date
    tmpStr = qDateTimeToISO(anEvent->created());
    addPropValue(vtodo, VCDCreatedProp, tmpStr.toUtf8().constData());

    // unique id
    addPropValue(vtodo, VCUniqueStringProp,
                 anEvent->uid().toUtf8().constData());

    // revision
    tmpStr.sprintf("%i", anEvent->revision());
    addPropValue(vtodo, VCSequenceProp, tmpStr.toUtf8().constData());

    // last modification date
    tmpStr = qDateTimeToISO(anEvent->lastModified());
    addPropValue(vtodo, VCLastModifiedProp, tmpStr.toUtf8().constData());

    // organizer stuff
    // @TODO: How about the common name?
    if (!anEvent->organizer()->email().isEmpty()) {
        tmpStr = QStringLiteral("MAILTO:") + anEvent->organizer()->email();
        addPropValue(vtodo, ICOrganizerProp, tmpStr.toUtf8().constData());
    }

    // attendees
    if (anEvent->attendeeCount() > 0) {
        Attendee::Ptr curAttendee;
        auto attendees = anEvent->attendees();
        for (auto it = attendees.constBegin(); it != attendees.constEnd();
                ++it) {
            curAttendee = *it;
            if (!curAttendee->email().isEmpty() && !curAttendee->name().isEmpty()) {
                tmpStr = QStringLiteral("MAILTO:") + curAttendee->name() + QStringLiteral(" <") + curAttendee->email() + QLatin1Char('>');
            } else if (curAttendee->name().isEmpty() && curAttendee->email().isEmpty()) {
                tmpStr = QStringLiteral("MAILTO: ");
                qCDebug(KCALCORE_LOG) << "warning! this Event has an attendee w/o name or email!";
            } else if (curAttendee->name().isEmpty()) {
                tmpStr = QStringLiteral("MAILTO: ") + curAttendee->email();
            } else {
                tmpStr = QStringLiteral("MAILTO: ") + curAttendee->name();
            }
            VObject *aProp = addPropValue(vtodo, VCAttendeeProp, tmpStr.toUtf8().constData());
            addPropValue(aProp, VCRSVPProp, curAttendee->RSVP() ? "TRUE" : "FALSE");
            addPropValue(aProp, VCStatusProp, writeStatus(curAttendee->status()).constData());
        }
    }

    // recurrence rule stuff
    const Recurrence *recur = anEvent->recurrence();
    if (recur->recurs()) {
        bool validRecur = true;
        QString tmpStr2;
        switch (recur->recurrenceType()) {
        case Recurrence::rDaily:
            tmpStr.sprintf("D%i ", recur->frequency());
            break;
        case Recurrence::rWeekly:
            tmpStr.sprintf("W%i ", recur->frequency());
            for (int i = 0; i < 7; ++i) {
                QBitArray days(recur->days());
                if (days.testBit(i)) {
                    tmpStr += dayFromNum(i);
                }
            }
            break;
        case Recurrence::rMonthlyPos: {
            tmpStr.sprintf("MP%i ", recur->frequency());
            // write out all rMonthPos's
            QList<RecurrenceRule::WDayPos> tmpPositions = recur->monthPositions();
            for (QList<RecurrenceRule::WDayPos>::ConstIterator posit = tmpPositions.constBegin();
                    posit != tmpPositions.constEnd(); ++posit) {
                int pos = (*posit).pos();
                tmpStr2.sprintf("%i", (pos > 0) ? pos : (-pos));
                if (pos < 0) {
                    tmpStr2 += QStringLiteral("- ");
                } else {
                    tmpStr2 += QStringLiteral("+ ");
                }
                tmpStr += tmpStr2;
                tmpStr += dayFromNum((*posit).day() - 1);
            }
            break;
        }
        case Recurrence::rMonthlyDay: {
            tmpStr.sprintf("MD%i ", recur->frequency());
            // write out all rMonthDays;
            const QList<int> tmpDays = recur->monthDays();
            for (QList<int>::ConstIterator tmpDay = tmpDays.constBegin();
                    tmpDay != tmpDays.constEnd(); ++tmpDay) {
                tmpStr2.sprintf("%i ", *tmpDay);
                tmpStr += tmpStr2;
            }
            break;
        }
        case Recurrence::rYearlyMonth: {
            tmpStr.sprintf("YM%i ", recur->frequency());
            // write out all the months;'
            // TODO: Any way to write out the day within the month???
            const QList<int> months = recur->yearMonths();
            for (QList<int>::ConstIterator mit = months.constBegin();
                    mit != months.constEnd(); ++mit) {
                tmpStr2.sprintf("%i ", *mit);
                tmpStr += tmpStr2;
            }
            break;
        }
        case Recurrence::rYearlyDay: {
            tmpStr.sprintf("YD%i ", recur->frequency());
            // write out all the rYearNums;
            const QList<int> tmpDays = recur->yearDays();
            for (QList<int>::ConstIterator tmpDay = tmpDays.begin();
                    tmpDay != tmpDays.end(); ++tmpDay) {
                tmpStr2.sprintf("%i ", *tmpDay);
                tmpStr += tmpStr2;
            }
            break;
        }
        default:
            // TODO: Write rYearlyPos and arbitrary rules!
            qCDebug(KCALCORE_LOG) << "ERROR, it should never get here in eventToVTodo!";
            validRecur = false;
            break;
        } // switch

        if (recur->duration() > 0) {
            tmpStr2.sprintf("#%i", recur->duration());
            tmpStr += tmpStr2;
        } else if (recur->duration() == -1) {
            tmpStr += QLatin1String("#0"); // defined as repeat forever
        } else {
            tmpStr += kDateTimeToISO(q2k(recur->endDateTime()), false);
        }
        // Only write out the rrule if we have a valid recurrence (i.e. a known
        // type in thee switch above)
        if (validRecur) {
            addPropValue(vtodo, VCRRuleProp, tmpStr.toUtf8().constData());
        }

    } // event repeats

    // exceptions dates to recurrence
    DateList dateList = recur->exDates();
    DateList::ConstIterator id;
    QString tmpStr2;

    for (id = dateList.constBegin(); id != dateList.constEnd(); ++id) {
        tmpStr = qDateToISO(*id) + QLatin1Char(';');
        tmpStr2 += tmpStr;
    }
    if (!tmpStr2.isEmpty()) {
        tmpStr2.truncate(tmpStr2.length() - 1);
        addPropValue(vtodo, VCExpDateProp, tmpStr2.toUtf8().constData());
    }
    // exceptions datetimes to recurrence
    auto dateTimeList = recur->exDateTimes();
    tmpStr2.clear();

    for (auto idt = dateTimeList.constBegin(); idt != dateTimeList.constEnd(); ++idt) {
        tmpStr = kDateTimeToISO(q2k(*idt)) + QLatin1Char(';');
        tmpStr2 += tmpStr;
    }
    if (!tmpStr2.isEmpty()) {
        tmpStr2.truncate(tmpStr2.length() - 1);
        addPropValue(vtodo, VCExpDateProp, tmpStr2.toUtf8().constData());
    }

    // description BL:
    if (!anEvent->description().isEmpty()) {
        QByteArray in = anEvent->description().toUtf8();
        QByteArray out;
        KCodecs::quotedPrintableEncode(in, out, true);
        if (out != in) {
            VObject *d = addPropValue(vtodo, VCDescriptionProp, out.constData());
            addPropValue(d, VCEncodingProp, VCQuotedPrintableProp);
            addPropValue(d, VCCharSetProp, VCUtf8Prop);
        } else {
            addPropValue(vtodo, VCDescriptionProp, in.constData());
        }
    }

    // summary
    if (!anEvent->summary().isEmpty()) {
        QByteArray in = anEvent->summary().toUtf8();
        QByteArray out;
        KCodecs::quotedPrintableEncode(in, out, true);
        if (out != in) {
            VObject *d = addPropValue(vtodo, VCSummaryProp, out.constData());
            addPropValue(d, VCEncodingProp, VCQuotedPrintableProp);
            addPropValue(d, VCCharSetProp, VCUtf8Prop);
        } else {
            addPropValue(vtodo, VCSummaryProp, in.constData());
        }
    }

    // location
    if (!anEvent->location().isEmpty()) {
        QByteArray in = anEvent->location().toUtf8();
        QByteArray out;
        KCodecs::quotedPrintableEncode(in, out, true);
        if (out != in) {
            VObject *d = addPropValue(vtodo, VCLocationProp, out.constData());
            addPropValue(d, VCEncodingProp, VCQuotedPrintableProp);
            addPropValue(d, VCCharSetProp, VCUtf8Prop);
        } else {
            addPropValue(vtodo, VCLocationProp, in.constData());
        }
    }

    // completed status
    // backward compatibility, KOrganizer used to interpret only these two values
    addPropValue(vtodo, VCStatusProp, anEvent->isCompleted() ? "COMPLETED" : "NEEDS ACTION");

    // completion date
    if (anEvent->hasCompletedDate()) {
        tmpStr = kDateTimeToISO(anEvent->completed());
        addPropValue(vtodo, VCCompletedProp, tmpStr.toUtf8().constData());
    }

    // priority
    tmpStr.sprintf("%i", anEvent->priority());
    addPropValue(vtodo, VCPriorityProp, tmpStr.toUtf8().constData());

    // related event
    if (!anEvent->relatedTo().isEmpty()) {
        addPropValue(vtodo, VCRelatedToProp,
                     anEvent->relatedTo().toUtf8().constData());
    }

    // secrecy
    const char *text = nullptr;
    switch (anEvent->secrecy()) {
    case Incidence::SecrecyPublic:
        text = "PUBLIC";
        break;
    case Incidence::SecrecyPrivate:
        text = "PRIVATE";
        break;
    case Incidence::SecrecyConfidential:
        text = "CONFIDENTIAL";
        break;
    }
    if (text) {
        addPropValue(vtodo, VCClassProp, text);
    }

    // categories
    const QStringList tmpStrList = anEvent->categories();
    tmpStr.clear();
    QString catStr;
    QStringList::const_iterator its;
    for (its = tmpStrList.constBegin(); its != tmpStrList.constEnd(); ++its) {
        catStr = *its;
        if (catStr[0] == QLatin1Char(' ')) {
            tmpStr += catStr.midRef(1);
        } else {
            tmpStr += catStr;
        }
        // this must be a ';' character as the vCalendar specification requires!
        // vcc.y has been hacked to translate the ';' to a ',' when the vcal is
        // read in.
        tmpStr += QLatin1Char(';');
    }
    if (!tmpStr.isEmpty()) {
        tmpStr.truncate(tmpStr.length() - 1);
        addPropValue(vtodo, VCCategoriesProp, tmpStr.toUtf8().constData());
    }

    // alarm stuff
    auto alarms = anEvent->alarms();
    for (auto it = alarms.constBegin(); it != alarms.constEnd(); ++it) {
        Alarm::Ptr alarm = *it;
        if (alarm->enabled()) {
            VObject *a;
            if (alarm->type() == Alarm::Display) {
                a = addProp(vtodo, VCDAlarmProp);
                tmpStr = qDateTimeToISO(alarm->time());
                addPropValue(a, VCRunTimeProp, tmpStr.toUtf8().constData());
                addPropValue(a, VCRepeatCountProp, "1");
                if (alarm->text().isNull()) {
                    addPropValue(a, VCDisplayStringProp, "beep!");
                } else {
                    addPropValue(a, VCDisplayStringProp, alarm->text().toLatin1().data());
                }
            } else if (alarm->type() == Alarm::Audio) {
                a = addProp(vtodo, VCAAlarmProp);
                tmpStr = qDateTimeToISO(alarm->time());
                addPropValue(a, VCRunTimeProp, tmpStr.toUtf8().constData());
                addPropValue(a, VCRepeatCountProp, "1");
                addPropValue(a, VCAudioContentProp, QFile::encodeName(alarm->audioFile()).constData());
            } else if (alarm->type() == Alarm::Procedure) {
                a = addProp(vtodo, VCPAlarmProp);
                tmpStr = qDateTimeToISO(alarm->time());
                addPropValue(a, VCRunTimeProp, tmpStr.toUtf8().constData());
                addPropValue(a, VCRepeatCountProp, "1");
                addPropValue(a, VCProcedureNameProp, QFile::encodeName(alarm->programFile()).constData());
            }
        }
    }

    return vtodo;
}

VObject *VCalFormat::eventToVEvent(const Event::Ptr &anEvent)
{
    VObject *vevent;
    QString tmpStr;

    vevent = newVObject(VCEventProp);

    // start and end time
    tmpStr = kDateTimeToISO(anEvent->dtStart(), !anEvent->allDay());
    addPropValue(vevent, VCDTstartProp, tmpStr.toUtf8().constData());

    // events that have time associated but take up no time should
    // not have both DTSTART and DTEND.
    if (anEvent->dtStart() != anEvent->dtEnd()) {
        tmpStr = kDateTimeToISO(anEvent->dtEnd(), !anEvent->allDay());
        addPropValue(vevent, VCDTendProp, tmpStr.toUtf8().constData());
    }

    // creation date
    tmpStr = qDateTimeToISO(anEvent->created());
    addPropValue(vevent, VCDCreatedProp, tmpStr.toUtf8().constData());

    // unique id
    addPropValue(vevent, VCUniqueStringProp,
                 anEvent->uid().toUtf8().constData());

    // revision
    tmpStr.sprintf("%i", anEvent->revision());
    addPropValue(vevent, VCSequenceProp, tmpStr.toUtf8().constData());

    // last modification date
    tmpStr = qDateTimeToISO(anEvent->lastModified());
    addPropValue(vevent, VCLastModifiedProp, tmpStr.toUtf8().constData());

    // attendee and organizer stuff
    // TODO: What to do with the common name?
    if (!anEvent->organizer()->email().isEmpty()) {
        tmpStr = QStringLiteral("MAILTO:") + anEvent->organizer()->email();
        addPropValue(vevent, ICOrganizerProp, tmpStr.toUtf8().constData());
    }

    // TODO: Put this functionality into Attendee class
    if (anEvent->attendeeCount() > 0) {
        auto attendees = anEvent->attendees();
        for (auto it = attendees.constBegin(); it != attendees.constEnd(); ++it) {
            Attendee::Ptr curAttendee = *it;
            if (!curAttendee->email().isEmpty() && !curAttendee->name().isEmpty()) {
                tmpStr = QStringLiteral("MAILTO:") + curAttendee->name() + QStringLiteral(" <") + curAttendee->email() + QLatin1Char('>');
            } else if (curAttendee->name().isEmpty() && curAttendee->email().isEmpty()) {
                tmpStr = QStringLiteral("MAILTO: ");
                qCDebug(KCALCORE_LOG) << "warning! this Event has an attendee w/o name or email!";
            } else if (curAttendee->name().isEmpty()) {
                tmpStr = QStringLiteral("MAILTO: ") + curAttendee->email();
            } else {
                tmpStr = QStringLiteral("MAILTO: ") + curAttendee->name();
            }
            VObject *aProp = addPropValue(vevent, VCAttendeeProp, tmpStr.toUtf8().constData());
            addPropValue(aProp, VCRSVPProp, curAttendee->RSVP() ? "TRUE" : "FALSE");
            addPropValue(aProp, VCStatusProp, writeStatus(curAttendee->status()).constData());
        }
    }

    // recurrence rule stuff
    const Recurrence *recur = anEvent->recurrence();
    if (recur->recurs()) {
        bool validRecur = true;
        QString tmpStr2;
        switch (recur->recurrenceType()) {
        case Recurrence::rDaily:
            tmpStr.sprintf("D%i ", recur->frequency());
            break;
        case Recurrence::rWeekly:
            tmpStr.sprintf("W%i ", recur->frequency());
            for (int i = 0; i < 7; ++i) {
                QBitArray days(recur->days());
                if (days.testBit(i)) {
                    tmpStr += dayFromNum(i);
                }
            }
            break;
        case Recurrence::rMonthlyPos: {
            tmpStr.sprintf("MP%i ", recur->frequency());
            // write out all rMonthPos's
            QList<RecurrenceRule::WDayPos> tmpPositions = recur->monthPositions();
            for (QList<RecurrenceRule::WDayPos>::ConstIterator posit = tmpPositions.constBegin();
                    posit != tmpPositions.constEnd(); ++posit) {
                int pos = (*posit).pos();
                tmpStr2.sprintf("%i", (pos > 0) ? pos : (-pos));
                if (pos < 0) {
                    tmpStr2 += QLatin1String("- ");
                } else {
                    tmpStr2 += QLatin1String("+ ");
                }
                tmpStr += tmpStr2;
                tmpStr += dayFromNum((*posit).day() - 1);
            }
            break;
        }
        case Recurrence::rMonthlyDay: {
            tmpStr.sprintf("MD%i ", recur->frequency());
            // write out all rMonthDays;
            const QList<int> tmpDays = recur->monthDays();
            for (QList<int>::ConstIterator tmpDay = tmpDays.constBegin();
                    tmpDay != tmpDays.constEnd(); ++tmpDay) {
                tmpStr2.sprintf("%i ", *tmpDay);
                tmpStr += tmpStr2;
            }
            break;
        }
        case Recurrence::rYearlyMonth: {
            tmpStr.sprintf("YM%i ", recur->frequency());
            // write out all the months;'
            // TODO: Any way to write out the day within the month???
            const QList<int> months = recur->yearMonths();
            for (QList<int>::ConstIterator mit = months.constBegin();
                    mit != months.constEnd(); ++mit) {
                tmpStr2.sprintf("%i ", *mit);
                tmpStr += tmpStr2;
            }
            break;
        }
        case Recurrence::rYearlyDay: {
            tmpStr.sprintf("YD%i ", recur->frequency());
            // write out all the rYearNums;
            const QList<int> tmpDays = recur->yearDays();
            for (QList<int>::ConstIterator tmpDay = tmpDays.begin();
                    tmpDay != tmpDays.end(); ++tmpDay) {
                tmpStr2.sprintf("%i ", *tmpDay);
                tmpStr += tmpStr2;
            }
            break;
        }
        default:
            // TODO: Write rYearlyPos and arbitrary rules!
            qCDebug(KCALCORE_LOG) << "ERROR, it should never get here in eventToVEvent!";
            validRecur = false;
            break;
        } // switch

        if (recur->duration() > 0) {
            tmpStr2.sprintf("#%i", recur->duration());
            tmpStr += tmpStr2;
        } else if (recur->duration() == -1) {
            tmpStr += QLatin1String("#0"); // defined as repeat forever
        } else {
            tmpStr += kDateTimeToISO(q2k(recur->endDateTime()), false);
        }
        // Only write out the rrule if we have a valid recurrence (i.e. a known
        // type in thee switch above)
        if (validRecur) {
            addPropValue(vevent, VCRRuleProp, tmpStr.toUtf8().constData());
        }

    } // event repeats

    // exceptions dates to recurrence
    DateList dateList = recur->exDates();
    DateList::ConstIterator it;
    QString tmpStr2;

    for (it = dateList.constBegin(); it != dateList.constEnd(); ++it) {
        tmpStr = qDateToISO(*it) + QLatin1Char(';');
        tmpStr2 += tmpStr;
    }
    if (!tmpStr2.isEmpty()) {
        tmpStr2.truncate(tmpStr2.length() - 1);
        addPropValue(vevent, VCExpDateProp, tmpStr2.toUtf8().constData());
    }
    // exceptions datetimes to recurrence
    auto dateTimeList = recur->exDateTimes();
    tmpStr2.clear();

    for (auto idt = dateTimeList.constBegin(); idt != dateTimeList.constEnd(); ++idt) {
        tmpStr = kDateTimeToISO(q2k(*idt)) + QLatin1Char(';');
        tmpStr2 += tmpStr;
    }
    if (!tmpStr2.isEmpty()) {
        tmpStr2.truncate(tmpStr2.length() - 1);
        addPropValue(vevent, VCExpDateProp, tmpStr2.toUtf8().constData());
    }

    // description
    if (!anEvent->description().isEmpty()) {
        QByteArray in = anEvent->description().toUtf8();
        QByteArray out;
        KCodecs::quotedPrintableEncode(in, out, true);
        if (out != in) {
            VObject *d = addPropValue(vevent, VCDescriptionProp, out.constData());
            addPropValue(d, VCEncodingProp, VCQuotedPrintableProp);
            addPropValue(d, VCCharSetProp, VCUtf8Prop);
        } else {
            addPropValue(vevent, VCDescriptionProp, in.constData());
        }
    }

    // summary
    if (!anEvent->summary().isEmpty()) {
        QByteArray in = anEvent->summary().toUtf8();
        QByteArray out;
        KCodecs::quotedPrintableEncode(in, out, true);
        if (out != in) {
            VObject *d = addPropValue(vevent, VCSummaryProp, out.constData());
            addPropValue(d, VCEncodingProp, VCQuotedPrintableProp);
            addPropValue(d, VCCharSetProp, VCUtf8Prop);
        } else {
            addPropValue(vevent, VCSummaryProp, in.constData());
        }
    }

    // location
    if (!anEvent->location().isEmpty()) {
        QByteArray in = anEvent->location().toUtf8();
        QByteArray out;
        KCodecs::quotedPrintableEncode(in, out, true);
        if (out != in) {
            VObject *d = addPropValue(vevent, VCLocationProp, out.constData());
            addPropValue(d, VCEncodingProp, VCQuotedPrintableProp);
            addPropValue(d, VCCharSetProp, VCUtf8Prop);
        } else {
            addPropValue(vevent, VCLocationProp, in.constData());
        }
    }

    // status
// TODO: define Event status
//  addPropValue( vevent, VCStatusProp, anEvent->statusStr().toUtf8() );

    // secrecy
    const char *text = nullptr;
    switch (anEvent->secrecy()) {
    case Incidence::SecrecyPublic:
        text = "PUBLIC";
        break;
    case Incidence::SecrecyPrivate:
        text = "PRIVATE";
        break;
    case Incidence::SecrecyConfidential:
        text = "CONFIDENTIAL";
        break;
    }
    if (text) {
        addPropValue(vevent, VCClassProp, text);
    }

    // categories
    QStringList tmpStrList = anEvent->categories();
    tmpStr.clear();
    QString catStr;
    for (QStringList::const_iterator it = tmpStrList.constBegin(); it != tmpStrList.constEnd();
            ++it) {
        catStr = *it;
        if (catStr[0] == QLatin1Char(' ')) {
            tmpStr += catStr.midRef(1);
        } else {
            tmpStr += catStr;
        }
        // this must be a ';' character as the vCalendar specification requires!
        // vcc.y has been hacked to translate the ';' to a ',' when the vcal is
        // read in.
        tmpStr += QLatin1Char(';');
    }
    if (!tmpStr.isEmpty()) {
        tmpStr.truncate(tmpStr.length() - 1);
        addPropValue(vevent, VCCategoriesProp, tmpStr.toUtf8().constData());
    }

    // attachments
    // TODO: handle binary attachments!
    Attachment::List attachments = anEvent->attachments();
    Attachment::List::ConstIterator atIt;
    for (atIt = attachments.constBegin(); atIt != attachments.constEnd(); ++atIt) {
        addPropValue(vevent, VCAttachProp, (*atIt)->uri().toUtf8().constData());
    }

    // resources
    tmpStrList = anEvent->resources();
    tmpStr = tmpStrList.join(QLatin1Char(';'));
    if (!tmpStr.isEmpty()) {
        addPropValue(vevent, VCResourcesProp, tmpStr.toUtf8().constData());
    }

    // alarm stuff
    auto alarms = anEvent->alarms();
    for (auto it2 = alarms.constBegin(); it2 != alarms.constEnd(); ++it2) {
        Alarm::Ptr alarm = *it2;
        if (alarm->enabled()) {
            VObject *a;
            if (alarm->type() == Alarm::Display) {
                a = addProp(vevent, VCDAlarmProp);
                tmpStr = qDateTimeToISO(alarm->time());
                addPropValue(a, VCRunTimeProp, tmpStr.toUtf8().constData());
                addPropValue(a, VCRepeatCountProp, "1");
                if (alarm->text().isNull()) {
                    addPropValue(a, VCDisplayStringProp, "beep!");
                } else {
                    addPropValue(a, VCDisplayStringProp, alarm->text().toLatin1().data());
                }
            } else if (alarm->type() == Alarm::Audio) {
                a = addProp(vevent, VCAAlarmProp);
                tmpStr = qDateTimeToISO(alarm->time());
                addPropValue(a, VCRunTimeProp, tmpStr.toUtf8().constData());
                addPropValue(a, VCRepeatCountProp, "1");
                addPropValue(a, VCAudioContentProp, QFile::encodeName(alarm->audioFile()).constData());
            }
            if (alarm->type() == Alarm::Procedure) {
                a = addProp(vevent, VCPAlarmProp);
                tmpStr = qDateTimeToISO(alarm->time());
                addPropValue(a, VCRunTimeProp, tmpStr.toUtf8().constData());
                addPropValue(a, VCRepeatCountProp, "1");
                addPropValue(a, VCProcedureNameProp, QFile::encodeName(alarm->programFile()).constData());
            }
        }
    }

    // priority
    tmpStr.sprintf("%i", anEvent->priority());
    addPropValue(vevent, VCPriorityProp, tmpStr.toUtf8().constData());

    // transparency
    tmpStr.sprintf("%i", anEvent->transparency());
    addPropValue(vevent, VCTranspProp, tmpStr.toUtf8().constData());

    // related event
    if (!anEvent->relatedTo().isEmpty()) {
        addPropValue(vevent, VCRelatedToProp, anEvent->relatedTo().toUtf8().constData());
    }

    return vevent;
}

Todo::Ptr VCalFormat::VTodoToEvent(VObject *vtodo)
{
    VObject *vo = nullptr;
    VObjectIterator voi;
    char *s = nullptr;

    Todo::Ptr anEvent(new Todo);

    // creation date
    if ((vo = isAPropertyOf(vtodo, VCDCreatedProp)) != nullptr) {
        anEvent->setCreated(ISOToQDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo)))));
        deleteStr(s);
    }

    // unique id
    vo = isAPropertyOf(vtodo, VCUniqueStringProp);
    // while the UID property is preferred, it is not required.  We'll use the
    // default Event UID if none is given.
    if (vo) {
        anEvent->setUid(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo))));
        deleteStr(s);
    }

    // last modification date
    if ((vo = isAPropertyOf(vtodo, VCLastModifiedProp)) != nullptr) {
        anEvent->setLastModified(ISOToQDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo)))));
        deleteStr(s);
    } else {
        anEvent->setLastModified(QDateTime::currentDateTimeUtc());
    }

    // organizer
    // if our extension property for the event's ORGANIZER exists, add it.
    if ((vo = isAPropertyOf(vtodo, ICOrganizerProp)) != nullptr) {
        anEvent->setOrganizer(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo))));
        deleteStr(s);
    } else {
        if (d->mCalendar->owner()->name() != QLatin1String("Unknown Name")) {
            anEvent->setOrganizer(d->mCalendar->owner());
        }
    }

    // attendees.
    initPropIterator(&voi, vtodo);
    while (moreIteration(&voi)) {
        vo = nextVObject(&voi);
        if (strcmp(vObjectName(vo), VCAttendeeProp) == 0) {
            Attendee::Ptr a;
            VObject *vp;
            s = fakeCString(vObjectUStringZValue(vo));
            QString tmpStr = QString::fromUtf8(s);
            deleteStr(s);
            tmpStr = tmpStr.simplified();
            int emailPos1, emailPos2;
            if ((emailPos1 = tmpStr.indexOf(QLatin1Char('<'))) > 0) {
                // both email address and name
                emailPos2 = tmpStr.lastIndexOf(QLatin1Char('>'));
                a = Attendee::Ptr(new Attendee(tmpStr.left(emailPos1 - 1),
                                               tmpStr.mid(emailPos1 + 1,
                                                       emailPos2 - (emailPos1 + 1))));
            } else if (tmpStr.indexOf(QLatin1Char('@')) > 0) {
                // just an email address
                a = Attendee::Ptr(new Attendee(QString(), tmpStr));
            } else {
                // just a name
                // WTF??? Replacing the spaces of a name and using this as email?
                QString email = tmpStr.replace(QLatin1Char(' '), QLatin1Char('.'));
                a = Attendee::Ptr(new Attendee(tmpStr, email));
            }

            // is there an RSVP property?
            if ((vp = isAPropertyOf(vo, VCRSVPProp)) != nullptr) {
                a->setRSVP(vObjectStringZValue(vp));
            }
            // is there a status property?
            if ((vp = isAPropertyOf(vo, VCStatusProp)) != nullptr) {
                a->setStatus(readStatus(vObjectStringZValue(vp)));
            }
            // add the attendee
            anEvent->addAttendee(a);
        }
    }

    // description for todo
    if ((vo = isAPropertyOf(vtodo, VCDescriptionProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        anEvent->setDescription(QString::fromUtf8(s), Qt::mightBeRichText(QString::fromUtf8(s)));
        deleteStr(s);
    }

    // summary
    if ((vo = isAPropertyOf(vtodo, VCSummaryProp))) {
        s = fakeCString(vObjectUStringZValue(vo));
        anEvent->setSummary(QString::fromUtf8(s), Qt::mightBeRichText(QString::fromUtf8(s)));
        deleteStr(s);
    }

    // location
    if ((vo = isAPropertyOf(vtodo, VCLocationProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        anEvent->setLocation(QString::fromUtf8(s), Qt::mightBeRichText(QString::fromUtf8(s)));
        deleteStr(s);
    }

    // completed
    // was: status
    if ((vo = isAPropertyOf(vtodo, VCStatusProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        if (s && strcmp(s, "COMPLETED") == 0) {
            anEvent->setCompleted(true);
        } else {
            anEvent->setCompleted(false);
        }
        deleteStr(s);
    } else {
        anEvent->setCompleted(false);
    }

    // completion date
    if ((vo = isAPropertyOf(vtodo, VCCompletedProp)) != nullptr) {
        anEvent->setCompleted(ISOToKDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo)))));
        deleteStr(s);
    }

    // priority
    if ((vo = isAPropertyOf(vtodo, VCPriorityProp))) {
        s = fakeCString(vObjectUStringZValue(vo));
        if (s) {
            anEvent->setPriority(atoi(s));
            deleteStr(s);
        }
    }

    anEvent->setAllDay(false);

    // due date
    if ((vo = isAPropertyOf(vtodo, VCDueProp)) != nullptr) {
        anEvent->setDtDue(ISOToKDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo)))));
        deleteStr(s);
        if (anEvent->dtDue().time().hour() == 0 &&
                anEvent->dtDue().time().minute() == 0 &&
                anEvent->dtDue().time().second() == 0) {
            anEvent->setAllDay(true);
        }
    } else {
        anEvent->setDtDue(KDateTime());
    }

    // start time
    if ((vo = isAPropertyOf(vtodo, VCDTstartProp)) != nullptr) {
        anEvent->setDtStart(ISOToKDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo)))));
        deleteStr(s);
        if (anEvent->dtStart().time().hour() == 0 &&
                anEvent->dtStart().time().minute() == 0 &&
                anEvent->dtStart().time().second() == 0) {
            anEvent->setAllDay(true);
        }
    } else {
        anEvent->setDtStart(KDateTime());
    }

    // repeat stuff
    if ((vo = isAPropertyOf(vtodo, VCRRuleProp)) != nullptr) {
        QString tmpStr = (QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo))));
        deleteStr(s);
        tmpStr = tmpStr.simplified();
        tmpStr = tmpStr.toUpper();
        // first, read the type of the recurrence
        int typelen = 1;
        uint type = Recurrence::rNone;
        if (tmpStr.left(1) == QStringLiteral("D")) {
            type = Recurrence::rDaily;
        } else if (tmpStr.left(1) == QStringLiteral("W")) {
            type = Recurrence::rWeekly;
        } else {
            typelen = 2;
            if (tmpStr.left(2) == QStringLiteral("MP")) {
                type = Recurrence::rMonthlyPos;
            } else if (tmpStr.left(2) == QStringLiteral("MD")) {
                type = Recurrence::rMonthlyDay;
            } else if (tmpStr.left(2) == QStringLiteral("YM")) {
                type = Recurrence::rYearlyMonth;
            } else if (tmpStr.left(2) == QStringLiteral("YD")) {
                type = Recurrence::rYearlyDay;
            }
        }

        if (type != Recurrence::rNone) {

            // Immediately after the type is the frequency
            int index = tmpStr.indexOf(QLatin1Char(' '));
            int last = tmpStr.lastIndexOf(QLatin1Char(' ')) + 1;   // find last entry
            int rFreq = tmpStr.midRef(typelen, (index - 1)).toInt();
            ++index; // advance to beginning of stuff after freq

            // Read the type-specific settings
            switch (type) {
            case Recurrence::rDaily:
                anEvent->recurrence()->setDaily(rFreq);
                break;

            case Recurrence::rWeekly: {
                QBitArray qba(7);
                QString dayStr;
                if (index == last) {
                    // e.g. W1 #0
                    qba.setBit(anEvent->dtStart().date().dayOfWeek() - 1);
                } else {
                    // e.g. W1 SU #0
                    while (index < last) {
                        dayStr = tmpStr.mid(index, 3);
                        int dayNum = numFromDay(dayStr);
                        if (dayNum >= 0) {
                            qba.setBit(dayNum);
                        }
                        index += 3; // advance to next day, or possibly "#"
                    }
                }
                anEvent->recurrence()->setWeekly(rFreq, qba);
                break;
            }

            case Recurrence::rMonthlyPos: {
                anEvent->recurrence()->setMonthly(rFreq);

                QBitArray qba(7);
                short tmpPos;
                if (index == last) {
                    // e.g. MP1 #0
                    tmpPos = anEvent->dtStart().date().day() / 7 + 1;
                    if (tmpPos == 5) {
                        tmpPos = -1;
                    }
                    qba.setBit(anEvent->dtStart().date().dayOfWeek() - 1);
                    anEvent->recurrence()->addMonthlyPos(tmpPos, qba);
                } else {
                    // e.g. MP1 1+ SU #0
                    while (index < last) {
                        tmpPos = tmpStr.mid(index, 1).toShort();
                        index += 1;
                        if (tmpStr.mid(index, 1) == QLatin1String("-")) {
                            // convert tmpPos to negative
                            tmpPos = 0 - tmpPos;
                        }
                        index += 2; // advance to day(s)
                        while (numFromDay(tmpStr.mid(index, 3)) >= 0) {
                            int dayNum = numFromDay(tmpStr.mid(index, 3));
                            qba.setBit(dayNum);
                            index += 3; // advance to next day, or possibly pos or "#"
                        }
                        anEvent->recurrence()->addMonthlyPos(tmpPos, qba);
                        qba.detach();
                        qba.fill(false);   // clear out
                    } // while != "#"
                }
                break;
            }

            case Recurrence::rMonthlyDay:
                anEvent->recurrence()->setMonthly(rFreq);
                if (index == last) {
                    // e.g. MD1 #0
                    short tmpDay = anEvent->dtStart().date().day();
                    anEvent->recurrence()->addMonthlyDate(tmpDay);
                } else {
                    // e.g. MD1 3 #0
                    while (index < last) {
                        int index2 = tmpStr.indexOf(QLatin1Char(' '), index);
                        if ((tmpStr.mid((index2 - 1), 1) == QLatin1String("-")) ||
                                (tmpStr.mid((index2 - 1), 1) == QLatin1String("+"))) {
                            index2 = index2 - 1;
                        }
                        short tmpDay = tmpStr.mid(index, (index2 - index)).toShort();
                        index = index2;
                        if (tmpStr.mid(index, 1) == QLatin1String("-")) {
                            tmpDay = 0 - tmpDay;
                        }
                        index += 2; // advance the index;
                        anEvent->recurrence()->addMonthlyDate(tmpDay);
                    } // while != #
                }
                break;

            case Recurrence::rYearlyMonth:
                anEvent->recurrence()->setYearly(rFreq);

                if (index == last) {
                    // e.g. YM1 #0
                    short tmpMonth = anEvent->dtStart().date().month();
                    anEvent->recurrence()->addYearlyMonth(tmpMonth);
                } else {
                    // e.g. YM1 3 #0
                    while (index < last) {
                        int index2 = tmpStr.indexOf(QLatin1Char(' '), index);
                        short tmpMonth = tmpStr.mid(index, (index2 - index)).toShort();
                        index = index2 + 1;
                        anEvent->recurrence()->addYearlyMonth(tmpMonth);
                    } // while != #
                }
                break;

            case Recurrence::rYearlyDay:
                anEvent->recurrence()->setYearly(rFreq);

                if (index == last) {
                    // e.g. YD1 #0
                    short tmpDay = anEvent->dtStart().date().dayOfYear();
                    anEvent->recurrence()->addYearlyDay(tmpDay);
                } else {
                    // e.g. YD1 123 #0
                    while (index < last) {
                        int index2 = tmpStr.indexOf(QLatin1Char(' '), index);
                        short tmpDay = tmpStr.mid(index, (index2 - index)).toShort();
                        index = index2 + 1;
                        anEvent->recurrence()->addYearlyDay(tmpDay);
                    } // while != #
                }
                break;

            default:
                break;
            }

            // find the last field, which is either the duration or the end date
            index = last;
            if (tmpStr.mid(index, 1) == QLatin1String("#")) {
                // Nr of occurrences
                index++;
                int rDuration = tmpStr.midRef(index, tmpStr.length() - index).toInt();
                if (rDuration > 0) {
                    anEvent->recurrence()->setDuration(rDuration);
                }
            } else if (tmpStr.indexOf(QLatin1Char('T'), index) != -1) {
                KDateTime rEndDate = ISOToKDateTime(tmpStr.mid(index, tmpStr.length() - index));
                anEvent->recurrence()->setEndDateTime(k2q(rEndDate));
            }
        } else {
            qCDebug(KCALCORE_LOG) << "we don't understand this type of recurrence!";
        } // if known recurrence type
    } // repeats

    // recurrence exceptions
    if ((vo = isAPropertyOf(vtodo, VCExpDateProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        QStringList exDates = QString::fromUtf8(s).split(QLatin1Char(','));
        QStringList::ConstIterator it;
        for (it = exDates.constBegin(); it != exDates.constEnd(); ++it) {
            KDateTime exDate = ISOToKDateTime(*it);
            if (exDate.time().hour() == 0 &&
                    exDate.time().minute() == 0 &&
                    exDate.time().second() == 0) {
                anEvent->recurrence()->addExDate(ISOToQDate(*it));
            } else {
                anEvent->recurrence()->addExDateTime(k2q(exDate));
            }
        }
        deleteStr(s);
    }

    // alarm stuff
    if ((vo = isAPropertyOf(vtodo, VCDAlarmProp))) {
        Alarm::Ptr alarm;
        VObject *a = isAPropertyOf(vo, VCRunTimeProp);
        VObject *b = isAPropertyOf(vo, VCDisplayStringProp);

        if (a || b) {
            alarm = anEvent->newAlarm();
            if (a) {
                alarm->setTime(ISOToQDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(a)))));
                deleteStr(s);
            }
            alarm->setEnabled(true);
            if (b) {
                s = fakeCString(vObjectUStringZValue(b));
                alarm->setDisplayAlarm(QString::fromUtf8(s));
                deleteStr(s);
            } else {
                alarm->setDisplayAlarm(QString());
            }
        }
    }

    if ((vo = isAPropertyOf(vtodo, VCAAlarmProp))) {
        Alarm::Ptr alarm;
        VObject *a;
        VObject *b;
        a = isAPropertyOf(vo, VCRunTimeProp);
        b = isAPropertyOf(vo, VCAudioContentProp);

        if (a || b) {
            alarm = anEvent->newAlarm();
            if (a) {
                alarm->setTime(ISOToQDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(a)))));
                deleteStr(s);
            }
            alarm->setEnabled(true);
            if (b) {
                s = fakeCString(vObjectUStringZValue(b));
                alarm->setAudioAlarm(QFile::decodeName(s));
                deleteStr(s);
            } else {
                alarm->setAudioAlarm(QString());
            }
        }
    }

    if ((vo = isAPropertyOf(vtodo, VCPAlarmProp))) {
        Alarm::Ptr alarm;
        VObject *a = isAPropertyOf(vo, VCRunTimeProp);
        VObject *b = isAPropertyOf(vo, VCProcedureNameProp);

        if (a || b) {
            alarm = anEvent->newAlarm();
            if (a) {
                alarm->setTime(ISOToQDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(a)))));
                deleteStr(s);
            }
            alarm->setEnabled(true);

            if (b) {
                s = fakeCString(vObjectUStringZValue(b));
                alarm->setProcedureAlarm(QFile::decodeName(s));
                deleteStr(s);
            } else {
                alarm->setProcedureAlarm(QString());
            }
        }
    }

    // related todo
    if ((vo = isAPropertyOf(vtodo, VCRelatedToProp)) != nullptr) {
        anEvent->setRelatedTo(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo))));
        deleteStr(s);
        d->mTodosRelate.append(anEvent);
    }

    // secrecy
    Incidence::Secrecy secrecy = Incidence::SecrecyPublic;
    if ((vo = isAPropertyOf(vtodo, VCClassProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        if (s && strcmp(s, "PRIVATE") == 0) {
            secrecy = Incidence::SecrecyPrivate;
        } else if (s && strcmp(s, "CONFIDENTIAL") == 0) {
            secrecy = Incidence::SecrecyConfidential;
        }
        deleteStr(s);
    }
    anEvent->setSecrecy(secrecy);

    // categories
    if ((vo = isAPropertyOf(vtodo, VCCategoriesProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        QString categories = QString::fromUtf8(s);
        deleteStr(s);
        QStringList tmpStrList = categories.split(QLatin1Char(';'));
        anEvent->setCategories(tmpStrList);
    }

    return anEvent;
}

Event::Ptr VCalFormat::VEventToEvent(VObject *vevent)
{
    VObject *vo = nullptr;
    VObjectIterator voi;
    char *s = nullptr;

    Event::Ptr anEvent(new Event);

    // creation date
    if ((vo = isAPropertyOf(vevent, VCDCreatedProp)) != nullptr) {
        anEvent->setCreated(ISOToQDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo)))));
        deleteStr(s);
    }

    // unique id
    vo = isAPropertyOf(vevent, VCUniqueStringProp);
    // while the UID property is preferred, it is not required.  We'll use the
    // default Event UID if none is given.
    if (vo) {
        anEvent->setUid(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo))));
        deleteStr(s);
    }

    // revision
    // again NSCAL doesn't give us much to work with, so we improvise...
    anEvent->setRevision(0);
    if ((vo = isAPropertyOf(vevent, VCSequenceProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        if (s) {
            anEvent->setRevision(atoi(s));
            deleteStr(s);
        }
    }

    // last modification date
    if ((vo = isAPropertyOf(vevent, VCLastModifiedProp)) != nullptr) {
        anEvent->setLastModified(ISOToQDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo)))));
        deleteStr(s);
    } else {
        anEvent->setLastModified(QDateTime::currentDateTimeUtc());
    }

    // organizer
    // if our extension property for the event's ORGANIZER exists, add it.
    if ((vo = isAPropertyOf(vevent, ICOrganizerProp)) != nullptr) {
        // FIXME:  Also use the full name, not just the email address
        anEvent->setOrganizer(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo))));
        deleteStr(s);
    } else {
        if (d->mCalendar->owner()->name() != QStringLiteral("Unknown Name")) {
            anEvent->setOrganizer(d->mCalendar->owner());
        }
    }

    // deal with attendees.
    initPropIterator(&voi, vevent);
    while (moreIteration(&voi)) {
        vo = nextVObject(&voi);
        if (strcmp(vObjectName(vo), VCAttendeeProp) == 0) {
            Attendee::Ptr a;
            VObject *vp = nullptr;
            s = fakeCString(vObjectUStringZValue(vo));
            QString tmpStr = QString::fromUtf8(s);
            deleteStr(s);
            tmpStr = tmpStr.simplified();
            int emailPos1, emailPos2;
            if ((emailPos1 = tmpStr.indexOf(QLatin1Char('<'))) > 0) {
                // both email address and name
                emailPos2 = tmpStr.lastIndexOf(QLatin1Char('>'));
                a = Attendee::Ptr(new Attendee(tmpStr.left(emailPos1 - 1),
                                               tmpStr.mid(emailPos1 + 1,
                                                       emailPos2 - (emailPos1 + 1))));
            } else if (tmpStr.indexOf(QLatin1Char('@')) > 0) {
                // just an email address
                a = Attendee::Ptr(new Attendee(QString(), tmpStr));
            } else {
                // just a name
                QString email = tmpStr.replace(QLatin1Char(' '), QLatin1Char('.'));
                a = Attendee::Ptr(new Attendee(tmpStr, email));
            }

            // is there an RSVP property?
            if ((vp = isAPropertyOf(vo, VCRSVPProp)) != nullptr) {
                a->setRSVP(vObjectStringZValue(vp));
            }
            // is there a status property?
            if ((vp = isAPropertyOf(vo, VCStatusProp)) != nullptr) {
                a->setStatus(readStatus(vObjectStringZValue(vp)));
            }
            // add the attendee
            anEvent->addAttendee(a);
        }
    }

    // This isn't strictly true.  An event that doesn't have a start time
    // or an end time isn't all-day, it has an anchor in time but it doesn't
    // "take up" any time.
    /*if ((isAPropertyOf(vevent, VCDTstartProp) == 0) ||
        (isAPropertyOf(vevent, VCDTendProp) == 0)) {
      anEvent->setAllDay(true);
      } else {
      }*/

    anEvent->setAllDay(false);

    // start time
    if ((vo = isAPropertyOf(vevent, VCDTstartProp)) != nullptr) {
        anEvent->setDtStart(ISOToKDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo)))));
        deleteStr(s);

        if (anEvent->dtStart().time().hour() == 0 &&
                anEvent->dtStart().time().minute() == 0 &&
                anEvent->dtStart().time().second() == 0) {
            anEvent->setAllDay(true);
        }
    }

    // stop time
    if ((vo = isAPropertyOf(vevent, VCDTendProp)) != nullptr) {
        anEvent->setDtEnd(ISOToKDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo)))));
        deleteStr(s);

        if (anEvent->dtEnd().time().hour() == 0 &&
                anEvent->dtEnd().time().minute() == 0 &&
                anEvent->dtEnd().time().second() == 0) {
            anEvent->setAllDay(true);
        }
    }

    // at this point, there should be at least a start or end time.
    // fix up for events that take up no time but have a time associated
    if (!isAPropertyOf(vevent, VCDTstartProp)) {
        anEvent->setDtStart(anEvent->dtEnd());
    }
    if (! isAPropertyOf(vevent, VCDTendProp)) {
        anEvent->setDtEnd(anEvent->dtStart());
    }

    ///////////////////////////////////////////////////////////////////////////

    // repeat stuff
    if ((vo = isAPropertyOf(vevent, VCRRuleProp)) != nullptr) {
        QString tmpStr = (QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo))));
        deleteStr(s);
        tmpStr = tmpStr.simplified();
        tmpStr = tmpStr.toUpper();
        // first, read the type of the recurrence
        int typelen = 1;
        uint type = Recurrence::rNone;
        if (tmpStr.left(1) == QLatin1String("D")) {
            type = Recurrence::rDaily;
        } else if (tmpStr.left(1) == QLatin1String("W")) {
            type = Recurrence::rWeekly;
        } else {
            typelen = 2;
            if (tmpStr.left(2) == QLatin1String("MP")) {
                type = Recurrence::rMonthlyPos;
            } else if (tmpStr.left(2) == QLatin1String("MD")) {
                type = Recurrence::rMonthlyDay;
            } else if (tmpStr.left(2) == QLatin1String("YM")) {
                type = Recurrence::rYearlyMonth;
            } else if (tmpStr.left(2) == QLatin1String("YD")) {
                type = Recurrence::rYearlyDay;
            }
        }

        if (type != Recurrence::rNone) {

            // Immediately after the type is the frequency
            int index = tmpStr.indexOf(QLatin1Char(' '));
            int last = tmpStr.lastIndexOf(QLatin1Char(' ')) + 1;   // find last entry
            int rFreq = tmpStr.midRef(typelen, (index - 1)).toInt();
            ++index; // advance to beginning of stuff after freq

            // Read the type-specific settings
            switch (type) {
            case Recurrence::rDaily:
                anEvent->recurrence()->setDaily(rFreq);
                break;

            case Recurrence::rWeekly: {
                QBitArray qba(7);
                QString dayStr;
                if (index == last) {
                    // e.g. W1 #0
                    qba.setBit(anEvent->dtStart().date().dayOfWeek() - 1);
                } else {
                    // e.g. W1 SU #0
                    while (index < last) {
                        dayStr = tmpStr.mid(index, 3);
                        int dayNum = numFromDay(dayStr);
                        if (dayNum >= 0) {
                            qba.setBit(dayNum);
                        }
                        index += 3; // advance to next day, or possibly "#"
                    }
                }
                anEvent->recurrence()->setWeekly(rFreq, qba);
                break;
            }

            case Recurrence::rMonthlyPos: {
                anEvent->recurrence()->setMonthly(rFreq);

                QBitArray qba(7);
                short tmpPos;
                if (index == last) {
                    // e.g. MP1 #0
                    tmpPos = anEvent->dtStart().date().day() / 7 + 1;
                    if (tmpPos == 5) {
                        tmpPos = -1;
                    }
                    qba.setBit(anEvent->dtStart().date().dayOfWeek() - 1);
                    anEvent->recurrence()->addMonthlyPos(tmpPos, qba);
                } else {
                    // e.g. MP1 1+ SU #0
                    while (index < last) {
                        tmpPos = tmpStr.mid(index, 1).toShort();
                        index += 1;
                        if (tmpStr.mid(index, 1) == QStringLiteral("-")) {
                            // convert tmpPos to negative
                            tmpPos = 0 - tmpPos;
                        }
                        index += 2; // advance to day(s)
                        while (numFromDay(tmpStr.mid(index, 3)) >= 0) {
                            int dayNum = numFromDay(tmpStr.mid(index, 3));
                            qba.setBit(dayNum);
                            index += 3; // advance to next day, or possibly pos or "#"
                        }
                        anEvent->recurrence()->addMonthlyPos(tmpPos, qba);
                        qba.detach();
                        qba.fill(false);   // clear out
                    } // while != "#"
                }
                break;
            }

            case Recurrence::rMonthlyDay:
                anEvent->recurrence()->setMonthly(rFreq);
                if (index == last) {
                    // e.g. MD1 #0
                    short tmpDay = anEvent->dtStart().date().day();
                    anEvent->recurrence()->addMonthlyDate(tmpDay);
                } else {
                    // e.g. MD1 3 #0
                    while (index < last) {
                        int index2 = tmpStr.indexOf(QLatin1Char(' '), index);
                        if ((tmpStr.mid((index2 - 1), 1) == QStringLiteral("-")) ||
                                (tmpStr.mid((index2 - 1), 1) == QStringLiteral("+"))) {
                            index2 = index2 - 1;
                        }
                        short tmpDay = tmpStr.mid(index, (index2 - index)).toShort();
                        index = index2;
                        if (tmpStr.mid(index, 1) == QStringLiteral("-")) {
                            tmpDay = 0 - tmpDay;
                        }
                        index += 2; // advance the index;
                        anEvent->recurrence()->addMonthlyDate(tmpDay);
                    } // while != #
                }
                break;

            case Recurrence::rYearlyMonth:
                anEvent->recurrence()->setYearly(rFreq);

                if (index == last) {
                    // e.g. YM1 #0
                    short tmpMonth = anEvent->dtStart().date().month();
                    anEvent->recurrence()->addYearlyMonth(tmpMonth);
                } else {
                    // e.g. YM1 3 #0
                    while (index < last) {
                        int index2 = tmpStr.indexOf(QLatin1Char(' '), index);
                        short tmpMonth = tmpStr.mid(index, (index2 - index)).toShort();
                        index = index2 + 1;
                        anEvent->recurrence()->addYearlyMonth(tmpMonth);
                    } // while != #
                }
                break;

            case Recurrence::rYearlyDay:
                anEvent->recurrence()->setYearly(rFreq);

                if (index == last) {
                    // e.g. YD1 #0
                    short tmpDay = anEvent->dtStart().date().dayOfYear();
                    anEvent->recurrence()->addYearlyDay(tmpDay);
                } else {
                    // e.g. YD1 123 #0
                    while (index < last) {
                        int index2 = tmpStr.indexOf(QLatin1Char(' '), index);
                        short tmpDay = tmpStr.mid(index, (index2 - index)).toShort();
                        index = index2 + 1;
                        anEvent->recurrence()->addYearlyDay(tmpDay);
                    } // while != #
                }
                break;

            default:
                break;
            }

            // find the last field, which is either the duration or the end date
            index = last;
            if (tmpStr.mid(index, 1) == QLatin1String("#")) {
                // Nr of occurrences
                index++;
                int rDuration = tmpStr.midRef(index, tmpStr.length() - index).toInt();
                if (rDuration > 0) {
                    anEvent->recurrence()->setDuration(rDuration);
                }
            } else if (tmpStr.indexOf(QLatin1Char('T'), index) != -1) {
                KDateTime rEndDate = ISOToKDateTime(tmpStr.mid(index, tmpStr.length() - index));
                anEvent->recurrence()->setEndDateTime(k2q(rEndDate));
            }
// anEvent->recurrence()->dump();

        } else {
            qCDebug(KCALCORE_LOG) << "we don't understand this type of recurrence!";
        } // if known recurrence type
    } // repeats

    // recurrence exceptions
    if ((vo = isAPropertyOf(vevent, VCExpDateProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        QStringList exDates = QString::fromUtf8(s).split(QLatin1Char(','));
        QStringList::ConstIterator it;
        for (it = exDates.constBegin(); it != exDates.constEnd(); ++it) {
            KDateTime exDate = ISOToKDateTime(*it);
            if (exDate.time().hour() == 0 &&
                    exDate.time().minute() == 0 &&
                    exDate.time().second() == 0) {
                anEvent->recurrence()->addExDate(ISOToQDate(*it));
            } else {
                anEvent->recurrence()->addExDateTime(k2q(exDate));
            }
        }
        deleteStr(s);
    }

    // summary
    if ((vo = isAPropertyOf(vevent, VCSummaryProp))) {
        s = fakeCString(vObjectUStringZValue(vo));
        anEvent->setSummary(QString::fromUtf8(s), Qt::mightBeRichText(QString::fromUtf8(s)));
        deleteStr(s);
    }

    // description
    if ((vo = isAPropertyOf(vevent, VCDescriptionProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        bool isRich = Qt::mightBeRichText(QString::fromUtf8(s));
        if (!anEvent->description().isEmpty()) {
            anEvent->setDescription(
                anEvent->description() + QLatin1Char('\n') + QString::fromUtf8(s), isRich);
        } else {
            anEvent->setDescription(QString::fromUtf8(s), isRich);
        }
        deleteStr(s);
    }

    // location
    if ((vo = isAPropertyOf(vevent, VCLocationProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        anEvent->setLocation(QString::fromUtf8(s), Qt::mightBeRichText(QString::fromUtf8(s)));
        deleteStr(s);
    }

    // some stupid vCal exporters ignore the standard and use Description
    // instead of Summary for the default field.  Correct for this.
    if (anEvent->summary().isEmpty() && !(anEvent->description().isEmpty())) {
        QString tmpStr = anEvent->description().simplified();
        anEvent->setDescription(QString());
        anEvent->setSummary(tmpStr);
    }

#if 0
    // status
    if ((vo = isAPropertyOf(vevent, VCStatusProp)) != 0) {
        QString tmpStr(s = fakeCString(vObjectUStringZValue(vo)));
        deleteStr(s);
// TODO: Define Event status
//    anEvent->setStatus( tmpStr );
    } else {
//    anEvent->setStatus( "NEEDS ACTION" );
    }
#endif

    // secrecy
    Incidence::Secrecy secrecy = Incidence::SecrecyPublic;
    if ((vo = isAPropertyOf(vevent, VCClassProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        if (s && strcmp(s, "PRIVATE") == 0) {
            secrecy = Incidence::SecrecyPrivate;
        } else if (s && strcmp(s, "CONFIDENTIAL") == 0) {
            secrecy = Incidence::SecrecyConfidential;
        }
        deleteStr(s);
    }
    anEvent->setSecrecy(secrecy);

    // categories
    if ((vo = isAPropertyOf(vevent, VCCategoriesProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        QString categories = QString::fromUtf8(s);
        deleteStr(s);
        QStringList tmpStrList = categories.split(QLatin1Char(','));
        anEvent->setCategories(tmpStrList);
    }

    // attachments
    initPropIterator(&voi, vevent);
    while (moreIteration(&voi)) {
        vo = nextVObject(&voi);
        if (strcmp(vObjectName(vo), VCAttachProp) == 0) {
            s = fakeCString(vObjectUStringZValue(vo));
            anEvent->addAttachment(Attachment::Ptr(new Attachment(QString::fromUtf8(s))));
            deleteStr(s);
        }
    }

    // resources
    if ((vo = isAPropertyOf(vevent, VCResourcesProp)) != nullptr) {
        QString resources = (QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo))));
        deleteStr(s);
        QStringList tmpStrList = resources.split(QLatin1Char(';'));
        anEvent->setResources(tmpStrList);
    }

    // alarm stuff
    if ((vo = isAPropertyOf(vevent, VCDAlarmProp))) {
        Alarm::Ptr alarm;
        VObject *a = isAPropertyOf(vo, VCRunTimeProp);
        VObject *b = isAPropertyOf(vo, VCDisplayStringProp);

        if (a || b) {
            alarm = anEvent->newAlarm();
            if (a) {
                alarm->setTime(ISOToQDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(a)))));
                deleteStr(s);
            }
            alarm->setEnabled(true);

            if (b) {
                s = fakeCString(vObjectUStringZValue(b));
                alarm->setDisplayAlarm(QString::fromUtf8(s));
                deleteStr(s);
            } else {
                alarm->setDisplayAlarm(QString());
            }
        }
    }

    if ((vo = isAPropertyOf(vevent, VCAAlarmProp))) {
        Alarm::Ptr alarm;
        VObject *a;
        VObject *b;
        a = isAPropertyOf(vo, VCRunTimeProp);
        b = isAPropertyOf(vo, VCAudioContentProp);

        if (a || b) {
            alarm = anEvent->newAlarm();
            if (a) {
                alarm->setTime(ISOToQDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(a)))));
                deleteStr(s);
            }
            alarm->setEnabled(true);

            if (b) {
                s = fakeCString(vObjectUStringZValue(b));
                alarm->setAudioAlarm(QFile::decodeName(s));
                deleteStr(s);
            } else {
                alarm->setAudioAlarm(QString());
            }
        }
    }

    if ((vo = isAPropertyOf(vevent, VCPAlarmProp))) {
        Alarm::Ptr alarm;
        VObject *a;
        VObject *b;
        a = isAPropertyOf(vo, VCRunTimeProp);
        b = isAPropertyOf(vo, VCProcedureNameProp);

        if (a || b) {
            alarm = anEvent->newAlarm();
            if (a) {
                alarm->setTime(ISOToQDateTime(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(a)))));
                deleteStr(s);
            }
            alarm->setEnabled(true);

            if (b) {
                s = fakeCString(vObjectUStringZValue(b));
                alarm->setProcedureAlarm(QFile::decodeName(s));
                deleteStr(s);
            } else {
                alarm->setProcedureAlarm(QString());
            }
        }
    }

    // priority
    if ((vo = isAPropertyOf(vevent, VCPriorityProp))) {
        s = fakeCString(vObjectUStringZValue(vo));
        if (s) {
            anEvent->setPriority(atoi(s));
            deleteStr(s);
        }
    }

    // transparency
    if ((vo = isAPropertyOf(vevent, VCTranspProp)) != nullptr) {
        s = fakeCString(vObjectUStringZValue(vo));
        if (s) {
            int i = atoi(s);
            anEvent->setTransparency(i == 1 ? Event::Transparent : Event::Opaque);
            deleteStr(s);
        }
    }

    // related event
    if ((vo = isAPropertyOf(vevent, VCRelatedToProp)) != nullptr) {
        anEvent->setRelatedTo(QString::fromUtf8(s = fakeCString(vObjectUStringZValue(vo))));
        deleteStr(s);
        d->mEventsRelate.append(anEvent);
    }

    /* Rest of the custom properties */
    readCustomProperties(vevent, anEvent);

    return anEvent;
}

QString VCalFormat::parseTZ(const QByteArray &timezone) const
{
    // qCDebug(KCALCORE_LOG) << timezone;
    QString pZone = QString::fromUtf8(timezone.mid(timezone.indexOf("TZID:VCAL") + 9));
    return pZone.mid(0, pZone.indexOf(QLatin1Char('\n')));
}

QString VCalFormat::parseDst(QByteArray &timezone) const
{
    if (!timezone.contains("BEGIN:DAYLIGHT")) {
        return QString();
    }

    timezone = timezone.mid(timezone.indexOf("BEGIN:DAYLIGHT"));
    timezone = timezone.mid(timezone.indexOf("TZNAME:") + 7);
    QString sStart = QString::fromUtf8(timezone.mid(0, (timezone.indexOf("COMMENT:"))));
    sStart.chop(2);
    timezone = timezone.mid(timezone.indexOf("TZOFFSETTO:") + 11);
    QString sOffset = QString::fromUtf8(timezone.mid(0, (timezone.indexOf("DTSTART:"))));
    sOffset.chop(2);
    sOffset.insert(3, QLatin1Char(':'));
    timezone = timezone.mid(timezone.indexOf("TZNAME:") + 7);
    QString sEnd = QString::fromUtf8(timezone.mid(0, (timezone.indexOf("COMMENT:"))));
    sEnd.chop(2);

    return QStringLiteral("TRUE;") + sOffset + QLatin1Char(';') + sStart + QLatin1Char(';') + sEnd + QLatin1String(";;");
}

QString VCalFormat::qDateToISO(const QDate &qd)
{
    QString tmpStr;

    if (!qd.isValid()) {
        return QString();
    }

    tmpStr.sprintf("%.2d%.2d%.2d", qd.year(), qd.month(), qd.day());
    return tmpStr;

}

QString VCalFormat::kDateTimeToISO(const KDateTime &dt, bool zulu)
{
    QString tmpStr;

    if (!dt.isValid()) {
        return QString();
    }

    QDateTime tmpDT;
    if (zulu) {
        tmpDT = dt.toUtc().dateTime();
    } else {
        tmpDT = dt.toTimeSpec(zoneToSpec(d->mCalendar->timeZone())).dateTime();
    }
    tmpStr.sprintf("%.2d%.2d%.2dT%.2d%.2d%.2d",
                   tmpDT.date().year(), tmpDT.date().month(),
                   tmpDT.date().day(), tmpDT.time().hour(),
                   tmpDT.time().minute(), tmpDT.time().second());
    if (zulu || dt.isUtc()) {
        tmpStr += QLatin1Char('Z');
    }
    return tmpStr;
}

QString VCalFormat::qDateTimeToISO(const QDateTime &dt, bool zulu)
{
    QString tmpStr;

    if (!dt.isValid()) {
        return QString();
    }

    QDateTime tmpDT;
    if (zulu) {
        tmpDT = dt.toUTC();
    } else {
        tmpDT = dt.toTimeZone(d->mCalendar->timeZone());
    }
    tmpStr.sprintf("%.2d%.2d%.2dT%.2d%.2d%.2d",
                   tmpDT.date().year(), tmpDT.date().month(),
                   tmpDT.date().day(), tmpDT.time().hour(),
                   tmpDT.time().minute(), tmpDT.time().second());
    if (zulu || dt.timeZone() == QTimeZone::utc()) {
        tmpStr += QLatin1Char('Z');
    }
    return tmpStr;
}

KDateTime VCalFormat::ISOToKDateTime(const QString &dtStr)
{
    QDate tmpDate;
    QTime tmpTime;
    QString tmpStr;
    int year, month, day, hour, minute, second;

    tmpStr = dtStr;
    year = tmpStr.leftRef(4).toInt();
    month = tmpStr.midRef(4, 2).toInt();
    day = tmpStr.midRef(6, 2).toInt();
    hour = tmpStr.midRef(9, 2).toInt();
    minute = tmpStr.midRef(11, 2).toInt();
    second = tmpStr.midRef(13, 2).toInt();
    tmpDate.setDate(year, month, day);
    tmpTime.setHMS(hour, minute, second);

    if (tmpDate.isValid() && tmpTime.isValid()) {
        // correct for GMT if string is in Zulu format
        if (dtStr.at(dtStr.length() - 1) == QLatin1Char('Z')) {
            return KDateTime(tmpDate, tmpTime, KDateTime::UTC);
        } else {
            return KDateTime(tmpDate, tmpTime, zoneToSpec(d->mCalendar->timeZone()));
        }
    } else {
        return KDateTime();
    }
}

QDateTime VCalFormat::ISOToQDateTime(const QString &dtStr)
{
    QDate tmpDate;
    QTime tmpTime;
    QString tmpStr;
    int year, month, day, hour, minute, second;

    tmpStr = dtStr;
    year = tmpStr.leftRef(4).toInt();
    month = tmpStr.midRef(4, 2).toInt();
    day = tmpStr.midRef(6, 2).toInt();
    hour = tmpStr.midRef(9, 2).toInt();
    minute = tmpStr.midRef(11, 2).toInt();
    second = tmpStr.midRef(13, 2).toInt();
    tmpDate.setDate(year, month, day);
    tmpTime.setHMS(hour, minute, second);

    if (tmpDate.isValid() && tmpTime.isValid()) {
        // correct for GMT if string is in Zulu format
        if (dtStr.at(dtStr.length() - 1) == QLatin1Char('Z')) {
            return QDateTime(tmpDate, tmpTime, Qt::UTC);
        } else {
            return QDateTime(tmpDate, tmpTime, d->mCalendar->timeZone());
        }
    } else {
        return QDateTime();
    }
}

QDate VCalFormat::ISOToQDate(const QString &dateStr)
{
    int year, month, day;

    year = dateStr.leftRef(4).toInt();
    month = dateStr.midRef(4, 2).toInt();
    day = dateStr.midRef(6, 2).toInt();

    return QDate(year, month, day);
}

bool VCalFormat::parseTZOffsetISO8601(const QString &s, int &result)
{
    // ISO8601 format(s):
    // +- hh : mm
    // +- hh mm
    // +- hh

    // We also accept broken one without +
    int mod = 1;
    int v = 0;
    QString str = s.trimmed();
    int ofs = 0;
    result = 0;

    // Check for end
    if (str.size() <= ofs) {
        return false;
    }
    if (str[ofs] == QLatin1Char('-')) {
        mod = -1;
        ofs++;
    } else if (str[ofs] == QLatin1Char('+')) {
        ofs++;
    }
    if (str.size() <= ofs) {
        return false;
    }

    // Make sure next two values are numbers
    bool ok;

    if (str.size() < (ofs + 2)) {
        return false;
    }

    v = str.midRef(ofs, 2).toInt(&ok) * 60;
    if (!ok) {
        return false;
    }
    ofs += 2;

    if (str.size() > ofs) {
        if (str[ofs] == QLatin1Char(':')) {
            ofs++;
        }
        if (str.size() > ofs) {
            if (str.size() < (ofs + 2)) {
                return false;
            }
            v += str.midRef(ofs, 2).toInt(&ok);
            if (!ok) {
                return false;
            }
        }
    }
    result = v * mod * 60;
    return true;
}

// take a raw vcalendar (i.e. from a file on disk, clipboard, etc. etc.
// and break it down from it's tree-like format into the dictionary format
// that is used internally in the VCalFormat.
void VCalFormat::populate(VObject *vcal, bool deleted, const QString &notebook)
{
    Q_UNUSED(notebook);
    // this function will populate the caldict dictionary and other event
    // lists. It turns vevents into Events and then inserts them.

    VObjectIterator i;
    VObject *curVO;
    Event::Ptr anEvent;
    bool hasTimeZone = false; //The calendar came with a TZ and not UTC
    QTimeZone previousZone; //If we add a new TZ we should leave the spec as it was before

    if ((curVO = isAPropertyOf(vcal, ICMethodProp)) != nullptr) {
        char *methodType = nullptr;
        methodType = fakeCString(vObjectUStringZValue(curVO));
        // qCDebug(KCALCORE_LOG) << "This calendar is an iTIP transaction of type '" << methodType << "'";
        deleteStr(methodType);
    }

    // warn the user that we might have trouble reading non-known calendar.
    if ((curVO = isAPropertyOf(vcal, VCProdIdProp)) != nullptr) {
        char *s = fakeCString(vObjectUStringZValue(curVO));
        if (!s || strcmp(productId().toUtf8().constData(), s) != 0) {
            qCDebug(KCALCORE_LOG) << "This vCalendar file was not created by KOrganizer or"
                                  << "any other product we support. Loading anyway...";
        }
        setLoadedProductId(QString::fromUtf8(s));
        deleteStr(s);
    }

    // warn the user we might have trouble reading this unknown version.
    if ((curVO = isAPropertyOf(vcal, VCVersionProp)) != nullptr) {
        char *s = fakeCString(vObjectUStringZValue(curVO));
        if (!s || strcmp(_VCAL_VERSION, s) != 0) {
            qCDebug(KCALCORE_LOG) << "This vCalendar file has version" << s
                                  << "We only support" << _VCAL_VERSION;
        }
        deleteStr(s);
    }

    // set the time zone (this is a property of the view, so just discard!)
    if ((curVO = isAPropertyOf(vcal, VCTimeZoneProp)) != nullptr) {
        char *s = fakeCString(vObjectUStringZValue(curVO));
        QString ts = QString::fromUtf8(s);
        QString name = QStringLiteral("VCAL") + ts;
        deleteStr(s);

        // TODO: While using the timezone-offset + vcal as timezone is is
        // most likely unique, we should REALLY actually create something
        // like vcal-tzoffset-daylightoffsets, or better yet,
        // vcal-hash<the former>

        QStringList tzList;
        QString tz;
        int utcOffset;
        int utcOffsetDst;
        if (parseTZOffsetISO8601(ts, utcOffset)) {
            // qCDebug(KCALCORE_LOG) << "got standard offset" << ts << utcOffset;
            // standard from tz
            // starting date for now 01011900
            KDateTime dt = KDateTime(QDateTime(QDate(1900, 1, 1), QTime(0, 0, 0)));
            tz = QStringLiteral("STD;%1;false;%2").arg(QString::number(utcOffset), dt.toString());
            tzList.append(tz);

            // go through all the daylight tags
            initPropIterator(&i, vcal);
            while (moreIteration(&i)) {
                curVO = nextVObject(&i);
                if (strcmp(vObjectName(curVO), VCDayLightProp) == 0) {
                    char *s = fakeCString(vObjectUStringZValue(curVO));
                    QString dst = QLatin1String(s);
                    QStringList argl = dst.split(QLatin1Char(','));
                    deleteStr(s);

                    // Too short -> not interesting
                    if (argl.size() < 4) {
                        continue;
                    }

                    // We don't care about the non-DST periods
                    if (argl[0] != QLatin1String("TRUE")) {
                        continue;
                    }

                    if (parseTZOffsetISO8601(argl[1], utcOffsetDst)) {

                        // qCDebug(KCALCORE_LOG) << "got DST offset" << argl[1] << utcOffsetDst;
                        // standard
                        QString strEndDate = argl[3];
                        KDateTime endDate = ISOToKDateTime(strEndDate);
                        // daylight
                        QString strStartDate = argl[2];
                        KDateTime startDate = ISOToKDateTime(strStartDate);

                        QString strRealEndDate = strEndDate;
                        QString strRealStartDate = strStartDate;
                        KDateTime realEndDate = endDate;
                        KDateTime realStartDate = startDate;
                        // if we get dates for some reason in wrong order, earlier is used for dst
                        if (endDate < startDate) {
                            strRealEndDate = strStartDate;
                            strRealStartDate = strEndDate;
                            realEndDate = startDate;
                            realStartDate = endDate;
                        }
                        tz = QStringLiteral("%1;%2;false;%3").
                             arg(strRealEndDate, QString::number(utcOffset), realEndDate.toString());
                        tzList.append(tz);

                        tz = QStringLiteral("%1;%2;true;%3").
                             arg(strRealStartDate, QString::number(utcOffsetDst), realStartDate.toString());
                        tzList.append(tz);
                    } else {
                        qCDebug(KCALCORE_LOG) << "unable to parse dst" << argl[1];
                    }
                }
            }
            ICalTimeZones *tzlist = d->mCalendar->timeZones();
            ICalTimeZoneSource tzs;
            ICalTimeZone zone = tzs.parse(name, tzList, *tzlist);
            if (!zone.isValid()) {
                qCDebug(KCALCORE_LOG) << "zone is not valid, parsing error" << tzList;
            } else {
                previousZone = d->mCalendar->timeZone();
                d->mCalendar->setTimeZoneId(name.toUtf8());
                hasTimeZone = true;
            }
        } else {
            qCDebug(KCALCORE_LOG) << "unable to parse tzoffset" << ts;
        }
    }

    // Store all events with a relatedTo property in a list for post-processing
    d->mEventsRelate.clear();
    d->mTodosRelate.clear();

    initPropIterator(&i, vcal);

    // go through all the vobjects in the vcal
    while (moreIteration(&i)) {
        curVO = nextVObject(&i);

        /************************************************************************/

        // now, check to see that the object is an event or todo.
        if (strcmp(vObjectName(curVO), VCEventProp) == 0) {

            if (!isAPropertyOf(curVO, VCDTstartProp) &&
                    !isAPropertyOf(curVO, VCDTendProp)) {
                qCDebug(KCALCORE_LOG) << "found a VEvent with no DTSTART and no DTEND! Skipping...";
                goto SKIP;
            }

            anEvent = VEventToEvent(curVO);
            if (anEvent) {
                if (hasTimeZone && !anEvent->allDay() && anEvent->dtStart().isUtc()) {
                    //This sounds stupid but is how others are doing it, so here
                    //we go. If there is a TZ in the VCALENDAR even if the dtStart
                    //and dtend are in UTC, clients interpret it using also the TZ defined
                    //in the Calendar. I know it sounds braindead but oh well
                    int utcOffSet = anEvent->dtStart().utcOffset();
                    KDateTime dtStart(anEvent->dtStart().dateTime().addSecs(utcOffSet),
                                      zoneToSpec(d->mCalendar->timeZone()));
                    KDateTime dtEnd(anEvent->dtEnd().dateTime().addSecs(utcOffSet),
                                    zoneToSpec(d->mCalendar->timeZone()));
                    anEvent->setDtStart(dtStart);
                    anEvent->setDtEnd(dtEnd);
                }
                Event::Ptr old = !anEvent->hasRecurrenceId() ?
                                 d->mCalendar->event(anEvent->uid()) :
                                 d->mCalendar->event(anEvent->uid(), anEvent->recurrenceId());

                if (old) {
                    if (deleted) {
                        d->mCalendar->deleteEvent(old);   // move old to deleted
                        removeAllVCal(d->mEventsRelate, old);
                    } else if (anEvent->revision() > old->revision()) {
                        d->mCalendar->deleteEvent(old);   // move old to deleted
                        removeAllVCal(d->mEventsRelate, old);
                        d->mCalendar->addEvent(anEvent);   // and replace it with this one
                    }
                } else if (deleted) {
                    old = !anEvent->hasRecurrenceId() ?
                          d->mCalendar->deletedEvent(anEvent->uid()) :
                          d->mCalendar->deletedEvent(anEvent->uid(), anEvent->recurrenceId());
                    if (!old) {
                        d->mCalendar->addEvent(anEvent);   // add this one
                        d->mCalendar->deleteEvent(anEvent);   // and move it to deleted
                    }
                } else {
                    d->mCalendar->addEvent(anEvent);   // just add this one
                }
            }
        } else if (strcmp(vObjectName(curVO), VCTodoProp) == 0) {
            Todo::Ptr aTodo = VTodoToEvent(curVO);
            if (aTodo) {
                if (hasTimeZone && !aTodo->allDay()  && aTodo->dtStart().isUtc()) {
                    //This sounds stupid but is how others are doing it, so here
                    //we go. If there is a TZ in the VCALENDAR even if the dtStart
                    //and dtend are in UTC, clients interpret it usint alse the TZ defined
                    //in the Calendar. I know it sounds braindead but oh well
                    int utcOffSet = aTodo->dtStart().utcOffset();
                    KDateTime dtStart(aTodo->dtStart().dateTime().addSecs(utcOffSet),
                                      zoneToSpec(d->mCalendar->timeZone()));
                    aTodo->setDtStart(dtStart);
                    if (aTodo->hasDueDate()) {
                        KDateTime dtDue(aTodo->dtDue().dateTime().addSecs(utcOffSet),
                                        zoneToSpec(d->mCalendar->timeZone()));
                        aTodo->setDtDue(dtDue);
                    }
                }
                Todo::Ptr old = !aTodo->hasRecurrenceId() ?
                                d->mCalendar->todo(aTodo->uid()) :
                                d->mCalendar->todo(aTodo->uid(), aTodo->recurrenceId());
                if (old) {
                    if (deleted) {
                        d->mCalendar->deleteTodo(old);   // move old to deleted
                        removeAllVCal(d->mTodosRelate, old);
                    } else if (aTodo->revision() > old->revision()) {
                        d->mCalendar->deleteTodo(old);   // move old to deleted
                        removeAllVCal(d->mTodosRelate, old);
                        d->mCalendar->addTodo(aTodo);   // and replace it with this one
                    }
                } else if (deleted) {
                    old = d->mCalendar->deletedTodo(aTodo->uid(), aTodo->recurrenceId());
                    if (!old) {
                        d->mCalendar->addTodo(aTodo);   // add this one
                        d->mCalendar->deleteTodo(aTodo);   // and move it to deleted
                    }
                } else {
                    d->mCalendar->addTodo(aTodo);   // just add this one
                }
            }
        } else if ((strcmp(vObjectName(curVO), VCVersionProp) == 0) ||
                   (strcmp(vObjectName(curVO), VCProdIdProp) == 0) ||
                   (strcmp(vObjectName(curVO), VCTimeZoneProp) == 0)) {
            // do nothing, we know these properties and we want to skip them.
            // we have either already processed them or are ignoring them.
            ;
        } else if (strcmp(vObjectName(curVO), VCDayLightProp) == 0) {
            // do nothing daylights are already processed
            ;
        } else {
            qCDebug(KCALCORE_LOG) << "Ignoring unknown vObject \"" << vObjectName(curVO) << "\"";
        }
    SKIP:
        ;
    } // while

    // Post-Process list of events with relations, put Event objects in relation
    Event::List::ConstIterator eIt;
    for (eIt = d->mEventsRelate.constBegin(); eIt != d->mEventsRelate.constEnd(); ++eIt) {
        (*eIt)->setRelatedTo((*eIt)->relatedTo());
    }
    Todo::List::ConstIterator tIt;
    for (tIt = d->mTodosRelate.constBegin(); tIt != d->mTodosRelate.constEnd(); ++tIt) {
        (*tIt)->setRelatedTo((*tIt)->relatedTo());
    }

    //Now lets put the TZ back as it was if we have changed it.
    if (hasTimeZone) {
        d->mCalendar->setTimeZone(previousZone);
    }

}

int VCalFormat::numFromDay(const QString &day)
{
    if (day == QLatin1String("MO ")) {
        return 0;
    }
    if (day == QLatin1String("TU ")) {
        return 1;
    }
    if (day == QLatin1String("WE ")) {
        return 2;
    }
    if (day == QLatin1String("TH ")) {
        return 3;
    }
    if (day == QLatin1String("FR ")) {
        return 4;
    }
    if (day == QLatin1String("SA ")) {
        return 5;
    }
    if (day == QLatin1String("SU ")) {
        return 6;
    }

    return -1; // something bad happened. :)
}

Attendee::PartStat VCalFormat::readStatus(const char *s) const
{
    QString statStr = QString::fromUtf8(s);
    statStr = statStr.toUpper();
    Attendee::PartStat status;

    if (statStr == QLatin1String("X-ACTION")) {
        status = Attendee::NeedsAction;
    } else if (statStr == QLatin1String("NEEDS ACTION")) {
        status = Attendee::NeedsAction;
    } else if (statStr == QLatin1String("ACCEPTED")) {
        status = Attendee::Accepted;
    } else if (statStr == QLatin1String("SENT")) {
        status = Attendee::NeedsAction;
    } else if (statStr == QLatin1String("TENTATIVE")) {
        status = Attendee::Tentative;
    } else if (statStr == QLatin1String("CONFIRMED")) {
        status = Attendee::Accepted;
    } else if (statStr == QLatin1String("DECLINED")) {
        status = Attendee::Declined;
    } else if (statStr == QLatin1String("COMPLETED")) {
        status = Attendee::Completed;
    } else if (statStr == QLatin1String("DELEGATED")) {
        status = Attendee::Delegated;
    } else {
        qCDebug(KCALCORE_LOG) << "error setting attendee mStatus, unknown mStatus!";
        status = Attendee::NeedsAction;
    }

    return status;
}

QByteArray VCalFormat::writeStatus(Attendee::PartStat status) const
{
    switch (status) {
    default:
    case Attendee::NeedsAction:
        return "NEEDS ACTION";
        break;
    case Attendee::Accepted:
        return "ACCEPTED";
        break;
    case Attendee::Declined:
        return "DECLINED";
        break;
    case Attendee::Tentative:
        return "TENTATIVE";
        break;
    case Attendee::Delegated:
        return "DELEGATED";
        break;
    case Attendee::Completed:
        return "COMPLETED";
        break;
    case Attendee::InProcess:
        return "NEEDS ACTION";
        break;
    }
}

void VCalFormat::readCustomProperties(VObject *o, const Incidence::Ptr &i)
{
    VObjectIterator iter;
    VObject *cur;
    const char *curname;
    char *s;

    initPropIterator(&iter, o);
    while (moreIteration(&iter)) {
        cur = nextVObject(&iter);
        curname = vObjectName(cur);
        Q_ASSERT(curname);
        if ((curname[0] == 'X' && curname[1] == '-') &&
                strcmp(curname, ICOrganizerProp) != 0) {
            // TODO - for the time being, we ignore the parameters part
            // and just do the value handling here
            i->setNonKDECustomProperty(
                curname, QString::fromUtf8(s = fakeCString(vObjectUStringZValue(cur))));
            deleteStr(s);
        }
    }
}

void VCalFormat::writeCustomProperties(VObject *o, const Incidence::Ptr &i)
{
    const QMap<QByteArray, QString> custom = i->customProperties();
    for (QMap<QByteArray, QString>::ConstIterator c = custom.begin();
            c != custom.end();  ++c) {
        if (d->mManuallyWrittenExtensionFields.contains(c.key()) ||
                c.key().startsWith("X-KDE-VOLATILE")) { //krazy:exclude=strings
            continue;
        }

        addPropValue(o, c.key().constData(), c.value().toUtf8().constData());
    }
}

void VCalFormat::virtual_hook(int id, void *data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);
    Q_ASSERT(false);
}
