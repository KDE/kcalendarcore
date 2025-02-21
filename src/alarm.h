/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2003 David Jarvie <djarvie@kde.org>
  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Alarm class.
*/

#ifndef KCALCORE_ALARM_H
#define KCALCORE_ALARM_H

#include "customproperties.h"
#include "duration.h"
#include "kcalendarcore_export.h"
#include "person.h"

#include <QDataStream>
#include <QDateTime>
#include <QList>
#include <QMetaType>
#include <QSharedPointer>
#include <QString>
#include <QStringList>

class QTimeZone;

namespace KCalendarCore
{
class Incidence;

/*!
  \class KCalendarCore::Alarm
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Alarm

  \brief
  Represents an alarm notification.

  Alarms are user notifications that occur at specified times.
  Notifications can be on-screen pop-up dialogs, email messages,
  the playing of audio files, or the running of another program.

  Alarms always belong to a parent Incidence.
*/
class KCALENDARCORE_EXPORT Alarm : public CustomProperties
{
public:
    /*!
      \enum KCalendarCore::Alarm::Type
      \brief The different types of alarms.

      \value Invalid
      Invalid, or no alarm.
      \value Display
      Display a dialog box.
      \value Procedure
      Call a script.
      \value Email
      Send email.
      \value Audio
      Play an audio file.
    */
    enum Type {
        Invalid,
        Display,
        Procedure,
        Email,
        Audio,
    };

    /*!
      A shared pointer to an Alarm object.
    */
    typedef QSharedPointer<Alarm> Ptr;

    /*!
      List of alarms.
    */
    typedef QList<Ptr> List;

    // Can't find a way to use a shared pointer here.
    // Inside incidence.cpp, it does alarm->setParent( this )
    /*!
      Constructs an alarm belonging to the \a parent Incidence.

      \a parent is the Incidence this alarm will belong to.

    */
    explicit Alarm(Incidence *parent);

    /*!
      Copy constructor.

      \a other is the alarm to copy.

    */
    Alarm(const Alarm &other);

    /*!
      Destroys the alarm.
    */
    ~Alarm() override;

    /*!
      Copy operator.
    */
    Alarm &operator=(const Alarm &);

    /*!
      Compares two alarms for equality.

      \a a is the comparison alarm.

      Returns true if \a a is equal to this object, or false if they are different.
    */
    bool operator==(const Alarm &a) const;

    /*!
      Compares two alarms for inequality.

      \a a is the comparison alarm.

      Returns true if \a a is not equal to this object, or false if they are equal.
    */
    bool operator!=(const Alarm &a) const;

    // Is there a way to use QSharedPointer here?
    // although it's safe, Incidence's dtor calls setParent( 0 )
    // se we don't dereference a deleted pointer here.
    // Also, I renamed "Incidence *parent()" to "QString parentUid()"
    // So we don't return raw pointers
    /*!
      Sets the \a parent Incidence of the alarm.

      \a parent is alarm parent Incidence to set.

      \sa parentUid()
    */
    void setParent(Incidence *parent);

    // We don't have a share pointer to return, so return the UID.
    /*!
      Returns the parent's incidence UID of the alarm.

      \sa setParent()
    */
    Q_REQUIRED_RESULT QString parentUid() const;

    /*!
      Sets the type for this alarm to \a type.
      If the specified type is different from the current type of the alarm,
      then the alarm's type-specific properties are re-initialized.

      \a type is the alarm type to set.

      \sa type()
    */
    void setType(Type type);

    /*!
      Returns the type of the alarm.

      \sa setType()
    */
    Q_REQUIRED_RESULT Type type() const;

    /*!
      Sets the Display type for this alarm.
      If \a text is specified non-empty, then it is used as the description
      text to display when the alarm is triggered.

      \a text is the description to display when the alarm is triggered.

      \sa setText(), text()
    */
    void setDisplayAlarm(const QString &text = QString());

    /*!
      Sets the description \a text to be displayed when the alarm is triggered.
      Ignored if the alarm is not a display alarm.

      \a text is the description to display when the alarm is triggered.

      \sa setDisplayAlarm(), text()
    */
    void setText(const QString &text);

    /*!
      Returns the display text string for a Display alarm type.
      Returns an empty string if the alarm is not a Display type.

      \sa setDisplayAlarm(), setText()
    */
    Q_REQUIRED_RESULT QString text() const;

    /*!
      Sets the Audio type for this alarm and the name of the audio file
      to play when the alarm is triggered.

      \a audioFile is the name of the audio file to play when the alarm
      is triggered.

      \sa setAudioFile(), audioFile()
    */
    void setAudioAlarm(const QString &audioFile = QString());

    /*!
      Sets the name of the audio file to play when the audio alarm is triggered.
      Ignored if the alarm is not an Audio type.

      \a audioFile is the name of the audio file to play when the alarm
      is triggered.

      \sa setAudioAlarm(), audioFile()
    */
    void setAudioFile(const QString &audioFile);

    /*!
      Returns the audio file name for an Audio alarm type.
      Returns an empty string if the alarm is not an Audio type.

      \sa setAudioAlarm(), setAudioFile()
    */
    Q_REQUIRED_RESULT QString audioFile() const;

    /*!
      Sets the Procedure type for this alarm and the program (with arguments)
      to execute when the alarm is triggered.

      \a programFile is the name of the program file to execute when
      the alarm is triggered.

      \a arguments is a string of arguments to supply to \a programFile.

      \sa setProgramFile(), programFile(),
      setProgramArguments(), programArguments()
    */
    void setProcedureAlarm(const QString &programFile, const QString &arguments = QString());

    /*!
      Sets the program file to execute when the alarm is triggered.
      Ignored if the alarm is not a Procedure type.

      \a programFile is the name of the program file to execute when
      the alarm is triggered.

      \sa setProcedureAlarm(), programFile(),
      setProgramArguments(), programArguments()
    */
    void setProgramFile(const QString &programFile);

    /*!
      Returns the program file name for a Procedure alarm type.
      Returns an empty string if the alarm is not a Procedure type.

      \sa setProcedureAlarm(), setProgramFile(),
      setProgramArguments(), programArguments()
    */
    Q_REQUIRED_RESULT QString programFile() const;

    /*!
      Sets the program arguments string when the alarm is triggered.
      Ignored if the alarm is not a Procedure type.

      \a arguments is a string of arguments to supply to the program.

      \sa setProcedureAlarm(), setProgramFile(), programFile(),
      programArguments()
    */
    void setProgramArguments(const QString &arguments);

    /*!
      Returns the program arguments string for a Procedure alarm type.
      Returns an empty string if the alarm is not a Procedure type.

      \sa setProcedureAlarm(), setProgramFile(), programFile(),
      setProgramArguments()
    */
    Q_REQUIRED_RESULT QString programArguments() const;

    /*!
      Sets the Email type for this alarm and the email \a subject, \a text,
      \a addressees, and \a attachments that make up an email message to be
      sent when the alarm is triggered.

      \a subject is the email subject.

      \a text is a string containing the body of the email message.

      \a addressees is Person list of email addresses.

      \a attachments is a a QStringList of optional file names
      of email attachments.

      \sa setMailSubject(), setMailText(), setMailAddresses(),
      setMailAttachments()
    */
    void setEmailAlarm(const QString &subject, const QString &text, const Person::List &addressees, const QStringList &attachments = QStringList());

    /*!
      Sets the email address of an Email type alarm.
      Ignored if the alarm is not an Email type.

      \a mailAlarmAddress is a Person to receive a mail message when
      an Email type alarm is triggered.

      \sa setMailSubject(), setMailText(), setMailAddresses(),
      setMailAttachment(), setMailAttachments(), mailAddresses()
    */
    void setMailAddress(const Person &mailAlarmAddress);

    /*!
      Sets a list of email addresses of an Email type alarm.
      Ignored if the alarm is not an Email type.

      \a mailAlarmAddresses is a Person list to receive a mail message
      when an Email type alarm is triggered.

      \sa setMailSubject(), setMailText(), setMailAddress(),
      setMailAttachments(), setMailAttachment(), mailAddresses()
    */
    void setMailAddresses(const Person::List &mailAlarmAddresses);

    /*!
      Adds an address to the list of email addresses to send mail to when the
      alarm is triggered.
      Ignored if the alarm is not an Email type.

      \a mailAlarmAddress is a Person to add to the list of addresses to
      receive a mail message when an Email type alarm is triggered.

      \sa setMailAddress(), setMailAddresses(), mailAddresses()
    */
    void addMailAddress(const Person &mailAlarmAddress);

    /*!
      Returns the list of addresses for an Email alarm type.
      Returns an empty list if the alarm is not an Email type.

      \sa addMailAddress(), setMailAddress(), setMailAddresses()
    */
    Q_REQUIRED_RESULT Person::List mailAddresses() const;

    /*!
      Sets the subject line of a mail message for an Email alarm type.
      Ignored if the alarm is not an Email type.

      \a mailAlarmSubject is a string to be used as a subject line
      of an email message to send when the Email type alarm is triggered.

      \sa setMailText(), setMailAddress(), setMailAddresses(),
      setMailAttachment(), setMailAttachments(), mailSubject()
    */
    void setMailSubject(const QString &mailAlarmSubject);

    /*!
      Returns the subject line string for an Email alarm type.
      Returns an empty string if the alarm is not an Email type.

      \sa setMailSubject()
    */
    Q_REQUIRED_RESULT QString mailSubject() const;

    /*!
      Sets the filename to attach to a mail message for an Email alarm type.
      Ignored if the alarm is not an Email type.

      \a mailAttachFile is a string containing a filename to be attached
      to an email message to send when the Email type alarm is triggered.

      \sa setMailSubject(), setMailText(), setMailAddress(),
      setMailAddresses(), setMailAttachments(), mailAttachments()
    */
    void setMailAttachment(const QString &mailAttachFile);

    /*!
      Sets a list of filenames to attach to a mail message for an Email
      alarm type. Ignored if the alarm is not an Email type.

      \a mailAttachFiles is a QString list of filenames to attach to
      a mail message when an Email type alarm is triggered.

      \sa setMailSubject(), setMailText(), setMailAttachment(),
      setMailAddress(), setMailAddresses()
    */
    void setMailAttachments(const QStringList &mailAttachFiles);

    /*!
      Adds a filename to the list of files to attach to a mail message for
      an Email alarm type. Ignored if the alarm is not an Email type.

      \a mailAttachFile is a string containing a filename to be attached
      to an email message to send when the Email type alarm is triggered.

      \sa setMailAttachment(), setMailAttachments(), mailAttachments()
    */
    void addMailAttachment(const QString &mailAttachFile);

    /*!
      Returns the list of attachment filenames for an Email alarm type.
      Returns an empty list if the alarm is not an Email type.

      \sa addMailAttachment(), setMailAttachment(), setMailAttachments()
    */
    Q_REQUIRED_RESULT QStringList mailAttachments() const;

    /*!
      Sets the body text for an Email alarm type.
      Ignored if the alarm is not an Email type.

      \a text is a string containing the body text of a mail message
      when an Email type alarm is triggered.

      \sa setMailSubject(), setMailAddress(), setMailAddresses(),
      setMailAttachment(), setMailAttachments()
    */
    void setMailText(const QString &text);

    /*!
      Returns the body text for an Email alarm type.
      Returns an empty string if the alarm is not an Email type.

      \sa setMailText()
    */
    Q_REQUIRED_RESULT QString mailText() const;

    /*!
      Sets the trigger time of the alarm.

      \a alarmTime is the QDateTime alarm trigger.

      \sa time()
    */
    void setTime(const QDateTime &alarmTime);

    /*!
      Returns the alarm trigger date/time.

      \sa setTime()
    */
    Q_REQUIRED_RESULT QDateTime time() const;

    /*!
      Returns the next alarm trigger date/time after given date/time.
      Takes recurrent incidences into account.

      \a preTime date/time from where to start

      \a ignoreRepetitions don't take repetitions into account

      \sa nextRepetition()
    */
    Q_REQUIRED_RESULT QDateTime nextTime(const QDateTime &preTime, bool ignoreRepetitions = false) const;

    /*!
      Returns the date/time when the last repetition of the alarm goes off.
      If the alarm does not repeat this is equivalent to calling time().

      \sa setTime()
    */
    Q_REQUIRED_RESULT QDateTime endTime() const;

    /*!
      Returns true if the alarm has a trigger date/time.
    */
    Q_REQUIRED_RESULT bool hasTime() const;

    /*!
      Sets the alarm offset relative to the start of the parent Incidence.

      \a offset is a Duration to be used as a time relative to the
      start of the parent Incidence to be used as the alarm trigger.

      \sa setEndOffset(), startOffset(), endOffset()
    */
    void setStartOffset(const Duration &offset);

    /*!
      Returns offset of alarm in time relative to the start of the parent
      Incidence.  If the alarm's time is not defined in terms of an offset
      relative  to the start of the event, returns zero.

      \sa setStartOffset(), hasStartOffset()
    */
    Q_REQUIRED_RESULT Duration startOffset() const;

    /*!
      Returns whether the alarm is defined in terms of an offset relative
      to the start of the parent Incidence.

      \sa startOffset(), setStartOffset()
    */
    bool hasStartOffset() const;

    /*!
      Sets the alarm offset relative to the end of the parent Incidence.

      \a offset is a Duration to be used as a time relative to the
      end of the parent Incidence to be used as the alarm trigger.

      \sa setStartOffset(), startOffset(), endOffset()
    */
    void setEndOffset(const Duration &offset);

    /*!
      Returns offset of alarm in time relative to the end of the event.
      If the alarm's time is not defined in terms of an offset relative
      to the end of the event, returns zero.

      \sa setEndOffset(), hasEndOffset()
    */
    Q_REQUIRED_RESULT Duration endOffset() const;

    /*!
      Returns whether the alarm is defined in terms of an offset relative
      to the end of the event.

      \sa endOffset(), setEndOffset()
    */
    bool hasEndOffset() const;

    /*!
      Shift the times of the alarm so that they appear at the same clock
      time as before but in a new time zone. The shift is done from a viewing
      time zone rather than from the actual alarm time zone.

      For example, shifting an alarm whose start time is 09:00 America/New York,
      using an old viewing time zone (\a oldZone) of Europe/London, to a new
      time zone (\a newZone) of Europe/Paris, will result in the time being
      shifted from 14:00 (which is the London time of the alarm start) to
      14:00 Paris time.

      \a oldZone the time zone which provides the clock times

      \a newZone the new time zone

    */
    void shiftTimes(const QTimeZone &oldZone, const QTimeZone &newZone);

    /*!
      Sets the snooze time interval for the alarm.

      \a alarmSnoozeTime the time between snoozes.

      \sa snoozeTime()
    */
    void setSnoozeTime(const Duration &alarmSnoozeTime);

    /*!
      Returns the snooze time interval.

      \sa setSnoozeTime()
    */
    Q_REQUIRED_RESULT Duration snoozeTime() const;

    /*!
      Sets how many times an alarm is to repeat itself after its initial
      occurrence (w/snoozes).

      \a alarmRepeatCount is the number of times an alarm may repeat,
      excluding the initial occurrence.

      \sa repeatCount()
    */
    void setRepeatCount(int alarmRepeatCount);

    /*!
      Returns how many times an alarm may repeats after its initial occurrence.

      \sa setRepeatCount()
    */
    Q_REQUIRED_RESULT int repeatCount() const;

    /*!
      Returns the date/time of the alarm's initial occurrence or its next
      repetition after a given time.

      \a preTime the date/time after which to find the next repetition.

      Returns the date/time of the next repetition, or an invalid date/time
      if the specified time is at or after the alarm's last repetition.

      \sa previousRepetition()
    */
    Q_REQUIRED_RESULT QDateTime nextRepetition(const QDateTime &preTime) const;

    /*!
      Returns the date/time of the alarm's latest repetition or, if none,
      its initial occurrence before a given time.

      \a afterTime is the date/time before which to find the latest
      repetition.

      Returns the date and time of the latest repetition, or an invalid
      date/time if the specified time is at or before the alarm's initial
      occurrence.

      \sa nextRepetition()
    */
    Q_REQUIRED_RESULT QDateTime previousRepetition(const QDateTime &afterTime) const;

    /*!
      Returns the interval between the alarm's initial occurrence and
      its final repetition.
    */
    Q_REQUIRED_RESULT Duration duration() const;

    /*!
      Toggles the alarm status, i.e, an enable alarm becomes disabled
      and a disabled alarm becomes enabled.

      \sa enabled(), setEnabled()
    */
    void toggleAlarm();

    /*!
      Sets the enabled status of the alarm.

      \a enable if true, then enable the alarm; else disable the alarm.

      \sa enabled(), toggleAlarm()
    */
    void setEnabled(bool enable);

    /*!
      Returns the alarm enabled status: true (enabled) or false (disabled).

      \sa setEnabled(), toggleAlarm()
    */
    Q_REQUIRED_RESULT bool enabled() const;

    /*!
      Set if the location radius for the alarm has been defined.

      \a hasLocationRadius if true, then this alarm has a location radius.

      \sa setLocationRadius()
    */
    void setHasLocationRadius(bool hasLocationRadius);

    /*!
      Returns true if alarm has location radius defined.

      \sa setLocationRadius()
    */
    Q_REQUIRED_RESULT bool hasLocationRadius() const;

    /*!
      Set location radius for the alarm. This means that alarm will be
      triggered when user approaches the location. Given value will be
      stored into custom properties as X-LOCATION-RADIUS.

      \a locationRadius radius in meters

      \sa locationRadius()
    */
    void setLocationRadius(int locationRadius);

    /*!
      Returns the location radius in meters.

      \sa setLocationRadius()
    */
    Q_REQUIRED_RESULT int locationRadius() const;

protected:
    /*!
      \reimp
    */
    void customPropertyUpdated() override;

    /*
      TODO: appears to be unused
    */
    virtual void virtual_hook(int id, void *data);

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond
    friend KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &s, const KCalendarCore::Alarm::Ptr &);
    friend KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &s, const KCalendarCore::Alarm::Ptr &);
};
/*!
 * Alarm serializer.
 *
 * \since 4.12
 */
KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &out, const KCalendarCore::Alarm::Ptr &);

/*!
 * Alarm deserializer.
 *
 * \since 4.12
 */
KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &in, const KCalendarCore::Alarm::Ptr &);

}

//@cond PRIVATE
Q_DECLARE_TYPEINFO(KCalendarCore::Alarm::Ptr, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(KCalendarCore::Alarm::Ptr)
//@endcond

#endif
