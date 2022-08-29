/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the Incidence class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
*/

#ifndef KCALCORE_INCIDENCE_H
#define KCALCORE_INCIDENCE_H

#include "alarm.h"
#include "attachment.h"
#include "conference.h"
#include "incidencebase.h"
#include "kcalendarcore_export.h"
#include "recurrence.h"

#include <QMetaType>

//@cond PRIVATE
// Value used to signal invalid/unset latitude or longitude.
#define INVALID_LATLON 255.0 // krazy:exclude=defines (part of the API)
//@endcond

namespace KCalendarCore
{

class IncidencePrivate;

/**
  @brief
  Provides the abstract base class common to non-FreeBusy (Events, To-dos,
  Journals) calendar components known as incidences.

  Several properties are not allowed for VFREEBUSY objects (see rfc:2445),
  so they are not in IncidenceBase. The hierarchy is:

  IncidenceBase
  + FreeBusy
  + Incidence
    + Event
    + Todo
    + Journal

  So IncidenceBase contains all properties that are common to all classes,
  and Incidence contains all additional properties that are common to
  Events, Todos and Journals, but are not allowed for FreeBusy entries.
*/
class KCALENDARCORE_EXPORT Incidence : public IncidenceBase, public Recurrence::RecurrenceObserver
{
    Q_GADGET
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QString summary READ summary WRITE setSummary)
    Q_PROPERTY(QString location READ location WRITE setLocation)
#if KCALENDARCORE_BUILD_DEPRECATED_SINCE(5, 89)
    Q_PROPERTY(bool hasGeo READ hasGeo WRITE setHasGeo)
#else
    Q_PROPERTY(bool hasGeo READ hasGeo)
#endif
    Q_PROPERTY(float geoLatitude READ geoLatitude WRITE setGeoLatitude)
    Q_PROPERTY(float geoLongitude READ geoLongitude WRITE setGeoLongitude)
    Q_PROPERTY(QStringList categories READ categories WRITE setCategories)
    Q_PROPERTY(int priority READ priority WRITE setPriority)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated)
    Q_PROPERTY(KCalendarCore::Incidence::Secrecy secrecy READ secrecy WRITE setSecrecy)
    Q_PROPERTY(KCalendarCore::Incidence::Status status READ status WRITE setStatus)
    Q_PROPERTY(QVariantList attachments READ attachmentsVariant)
    Q_PROPERTY(QVariantList conferences READ conferencesVariant)
public:
    /**
      The different types of overall incidence status or confirmation.
      The meaning is specific to the incidence type in context.
    */
    enum Status {
        StatusNone, /**< No status */
        StatusTentative, /**< event is tentative */
        StatusConfirmed, /**< event is definite */
        StatusCompleted, /**< to-do completed */
        StatusNeedsAction, /**< to-do needs action */
        StatusCanceled, /**< event or to-do canceled; journal removed */
        StatusInProcess, /**< to-do in process */
        StatusDraft, /**< journal is draft */
        StatusFinal, /**< journal is final */
        StatusX, /**< a non-standard status string */
    };
    Q_ENUM(Status)

    /**
      The different types of incidence access classifications.
    */
    enum Secrecy {
        SecrecyPublic, /**< Not secret (default) */
        SecrecyPrivate, /**< Secret to the owner */
        SecrecyConfidential, /**< Secret to the owner and some others */
    };
    Q_ENUM(Secrecy)

    /**
       The different types of RELTYPE values specified by the RFC.
       Only RelTypeParent is supported for now.
    */
    enum RelType {
        RelTypeParent, /**< The related incidence is a parent. */
        RelTypeChild, /**< The related incidence is a child. */
        RelTypeSibling, /**< The related incidence is a peer. */
    };

    /**
      A shared pointer to an Incidence.
    */
    typedef QSharedPointer<Incidence> Ptr;

    /**
      List of incidences.
    */
    typedef QVector<Ptr> List;

#if KCALENDARCORE_BUILD_DEPRECATED_SINCE(5, 91)
    /**
      Constructs an empty incidence.
      @deprecated Use Incidence(IncidencePrivate *p).
    */
    Incidence();
#else
    Incidence() = delete;
#endif

    /**
      Constructs an empty incidence.
      @param p (non-null) a Private data object provided by the instantiated
      class (Event, Todo, Journal, FreeBusy). It passes ownership of the object
      to IncidenceBase.
    */
    Incidence(IncidencePrivate *p);

    /**
      Destroys an incidence.
    */
    ~Incidence() override;

    /**
      Returns an exact copy of this incidence. The returned object is owned
      by the caller.

      Dirty fields are cleared.
    */
    virtual Incidence *clone() const = 0;

    /**
      Returns a unique identifier for a specific instance of an incidence.

      Due to the recurrence-id, the uid is not unique for a KCalendarCore::Incidence.
      @since 4.11
    */
    Q_REQUIRED_RESULT QString instanceIdentifier() const;

    /**
      Set readonly state of incidence.

      @param readonly If true, the incidence is set to readonly, if false the
                      incidence is set to readwrite.
    */
    void setReadOnly(bool readonly) override;

    /**
      @copydoc IncidenceBase::setLastModified().
    */
    void setLastModified(const QDateTime &lm) override;

    /**
      Set localOnly state of incidence.
      A local only incidence can be updated but it will not increase the revision
      number neither the modified date.

      @param localonly If true, the incidence is set to localonly, if false the
                      incidence is set to normal stat.
    */
    void setLocalOnly(bool localonly);

    /**
      Get the localOnly status.
      @return true if Local only, false otherwise.

      @see setLocalOnly()
    */
    Q_REQUIRED_RESULT bool localOnly() const;

    /**
      @copydoc IncidenceBase::setAllDay().
    */
    void setAllDay(bool allDay) override;

    /**
      Recreate incidence. The incidence is made a new unique incidence, but already stored
      information is preserved. Sets unique id, creation date, last
      modification date and revision number.
    */
    void recreate();

    /**
      Sets the incidence creation date/time. It is stored as a UTC date/time.

      @param dt is the creation date/time.
      @see created().
    */
    void setCreated(const QDateTime &dt);

    /**
      Returns the incidence creation date/time.
      @see setCreated().
    */
    Q_REQUIRED_RESULT QDateTime created() const;

    /**
      Sets the number of revisions this incidence has seen.

      @param rev is the incidence revision number.
      @see revision().
    */
    void setRevision(int rev);

    /**
      Returns the number of revisions this incidence has seen.
      @see setRevision().
    */
    Q_REQUIRED_RESULT int revision() const;

    /**
      Sets the incidence starting date/time.

      @param dt is the starting date/time.
      @see IncidenceBase::dtStart().
    */
    void setDtStart(const QDateTime &dt) override;

    /**
      @copydoc IncidenceBase::shiftTimes()
    */
    void shiftTimes(const QTimeZone &oldZone, const QTimeZone &newZone) override;

    /**
      Sets the incidence description.

      @param description is the incidence description string.
      @param isRich if true indicates the description string contains richtext.
      @see description().
    */
    void setDescription(const QString &description, bool isRich);

    /**
      Sets the incidence description and tries to guess if the description
      is rich text.

      @param description is the incidence description string.
      @see description().
    */
    void setDescription(const QString &description);

    /**
      Returns the incidence description.
      @see setDescription().
      @see richDescription().
    */
    Q_REQUIRED_RESULT QString description() const;

    /**
      Returns the incidence description in rich text format.
      @see setDescription().
      @see description().
    */
    Q_REQUIRED_RESULT QString richDescription() const;

    /**
      Returns true if incidence description contains RichText; false otherwise.
      @see setDescription(), description().
    */
    Q_REQUIRED_RESULT bool descriptionIsRich() const;

    /**
      Sets the incidence summary.

      @param summary is the incidence summary string.
      @param isRich if true indicates the summary string contains richtext.
      @see summary().
    */
    void setSummary(const QString &summary, bool isRich);

    /**
      Sets the incidence summary and tries to guess if the summary is richtext.

      @param summary is the incidence summary string.
      @see summary().
    */
    void setSummary(const QString &summary);

    /**
      Returns the incidence summary.
      @see setSummary().
      @see richSummary().
    */
    Q_REQUIRED_RESULT QString summary() const;

    /**
      Returns the incidence summary in rich text format.
      @see setSummary().
      @see summary().
    */
    Q_REQUIRED_RESULT QString richSummary() const;

    /**
      Returns true if incidence summary contains RichText; false otherwise.
      @see setSummary(), summary().
    */
    Q_REQUIRED_RESULT bool summaryIsRich() const;

    /**
      Sets the incidence location. Do _not_ use with journals.

      @param location is the incidence location string.
      @param isRich if true indicates the location string contains richtext.
      @see location().
    */
    void setLocation(const QString &location, bool isRich);

    /**
      Sets the incidence location and tries to guess if the location is
      richtext. Do _not_ use with journals.

      @param location is the incidence location string.
      @see location().
    */
    void setLocation(const QString &location);

    /**
      Returns the incidence location. Do _not_ use with journals.
      @see setLocation().
      @see richLocation().
    */
    Q_REQUIRED_RESULT QString location() const;

    /**
      Returns the incidence location in rich text format.
      @see setLocation().
      @see location().
    */
    Q_REQUIRED_RESULT QString richLocation() const;

    /**
      Returns true if incidence location contains RichText; false otherwise.
      @see setLocation(), location().
    */
    Q_REQUIRED_RESULT bool locationIsRich() const;

    /**
      Sets the incidence category list.

      @param categories is a list of category strings.
      @see setCategories( const QString &), categories().
    */
    void setCategories(const QStringList &categories);

    /**
      Sets the incidence category list based on a comma delimited string.

      @param catStr is a QString containing a list of categories which
      are delimited by a comma character.
      @see setCategories( const QStringList &), categories().
    */
    void setCategories(const QString &catStr);

    /**
      Returns the incidence categories as a list of strings.
      @see setCategories( const QStringList &), setCategories( const QString &).
    */
    Q_REQUIRED_RESULT QStringList categories() const;

    /**
      Returns the incidence categories as a comma separated string.
      @see categories().
    */
    Q_REQUIRED_RESULT QString categoriesStr() const;

    /**
      Relates another incidence to this one, by UID. This function should only
      be used when constructing a calendar before the related incidence exists.

      @param uid is a QString containing a UID for another incidence.
      @param relType specifies the relation type.

      @warning KCalendarCore only supports one related-to field per reltype for now.

      @see relatedTo().
    */
    void setRelatedTo(const QString &uid, RelType relType = RelTypeParent);

    /**
      Returns a UID string for the incidence that is related to this one.
      This function should only be used when constructing a calendar before
      the related incidence exists.

      @warning KCalendarCore only supports one related-to field per reltype for now.

      @param relType specifies the relation type.

      @see setRelatedTo().
    */
    Q_REQUIRED_RESULT QString relatedTo(RelType relType = RelTypeParent) const;

    /**
      Set the incidence color, as added in RFC7986.

      @param colorName a named color as defined in CSS3 color name, see
       https://www.w3.org/TR/css-color-3/#svg-color.
      @since: 5.76
     */
    void setColor(const QString &colorName);

    /**
      Returns the color, if any is defined, for this incidence.

      @since: 5.76
     */
    Q_REQUIRED_RESULT QString color() const;

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%  Convenience wrappers for property handling
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /**
       Returns true if the alternative (=text/html) description is
       available.
       @see setAltDescription(), altDescription()
    */
    Q_REQUIRED_RESULT bool hasAltDescription() const;
    /**
      Sets the incidence's alternative (=text/html) description. If
      the text is empty, the property is removed.

      @param altdescription is the incidence altdescription string.
      @see altAltdescription().
    */
    void setAltDescription(const QString &altdescription);

    /**
      Returns the incidence alternative (=text/html) description.
      @see setAltDescription().
    */
    Q_REQUIRED_RESULT QString altDescription() const;

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%  Recurrence-related methods
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Returns the recurrence rule associated with this incidence. If there is
      none, returns an appropriate (non-0) object.
    */
    Recurrence *recurrence() const;

    /**
      Removes all recurrence and exception rules and dates.
    */
    void clearRecurrence();

    /**
      @copydoc Recurrence::recurs()
    */
    Q_REQUIRED_RESULT bool recurs() const;

    /**
      @copydoc Recurrence::recurrenceType()
    */
    Q_REQUIRED_RESULT ushort recurrenceType() const;

    /**
      @copydoc Recurrence::recursOn()
    */
    virtual bool recursOn(const QDate &date, const QTimeZone &timeZone) const;

    /**
      @copydoc Recurrence::recursAt()
    */
    Q_REQUIRED_RESULT bool recursAt(const QDateTime &dt) const;

    /**
      Calculates the start date/time for all recurrences that happen at some
      time on the given date (might start before that date, but end on or
      after the given date).

      @param date the date when the incidence should occur
      @param timeSpec time specification for @p date.
      @return the start date/time of all occurrences that overlap with the
      given date; an empty list if the incidence does not overlap with the
      date at all.
    */
    virtual QList<QDateTime> startDateTimesForDate(const QDate &date, const QTimeZone &timeZone) const;

    /**
      Calculates the start date/time for all recurrences that happen at the
      given time.

      @param datetime the date/time when the incidence should occur.
      @return the start date/time of all occurrences that overlap with the
      given date/time; an empty list if the incidence does not happen at the
      given time at all.
    */
    Q_REQUIRED_RESULT virtual QList<QDateTime> startDateTimesForDateTime(const QDateTime &datetime) const;

    /**
      Returns the end date/time of the incidence occurrence if it starts at
      specified date/time.

      @param startDt is the specified starting date/time.
      @return the corresponding end date/time for the occurrence; or the start
      date/time if the end date/time is invalid; or the end date/time if
      the start date/time is invalid.
    */
    Q_REQUIRED_RESULT virtual QDateTime endDateForStart(const QDateTime &startDt) const;

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%  Attachment-related methods
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Adds an attachment to the incidence.

      @param attachment a valid Attachment object.
    */
    void addAttachment(const Attachment &attachment);

    /**
      Removes all attachments of the specified MIME type from the incidence.
      The memory used by all the removed attachments is freed.

      @param mime is a QString containing the MIME type.
      @see deleteAttachment().
    */
    void deleteAttachments(const QString &mime);

    /**
      Returns a list of all incidence attachments.
      @see attachments( const QString &).
    */
    Q_REQUIRED_RESULT Attachment::List attachments() const;

    /**
      Returns a list of all incidence attachments with the specified MIME type.

      @param mime is a QString containing the MIME type.
      @see attachments().
    */
    Q_REQUIRED_RESULT Attachment::List attachments(const QString &mime) const;

    /**
      Removes all attachments and frees the memory used by them.
      @see deleteAttachments( const QString &).
    */
    void clearAttachments();

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%  Secrecy and Status methods
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Sets the incidence #Secrecy.

      @param secrecy is the incidence #Secrecy to set.
      @see secrecy(), secrecyStr().
    */
    void setSecrecy(Secrecy secrecy);

    /**
      Returns the incidence #Secrecy.
      @see setSecrecy(), secrecyStr().
    */
    Q_REQUIRED_RESULT Secrecy secrecy() const;

    /**
      Sets the incidence status to a standard #Status value.
      Events, Todos, and Journals each have a different set of
      valid statuses.  Note that StatusX cannot be specified.
      Invalid statuses are logged and ignored.

      @param status is the incidence #Status to set.
      @see status(), setCustomStatus().
    */
    void setStatus(Status status);

    /**
      Sets the incidence #Status to a non-standard status value.

      @param status is a non-standard status string. If empty,
      the incidence #Status will be set to StatusNone.
      @see setStatus(), status() customStatus().
    */
    void setCustomStatus(const QString &status);

    /**
       Returns the non-standard status value.
       @see setCustomStatus().
    */
    Q_REQUIRED_RESULT QString customStatus() const;

    /**
      Returns the incidence #Status.
      @see setStatus(), setCustomStatus(), statusStr().
    */
    Q_REQUIRED_RESULT Status status() const;

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%  Other methods
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Sets a list of incidence resources. (Note: resources in this context
      means items used by the incidence such as money, fuel, hours, etc).

      @param resources is a list of resource strings.
      @see resources().
    */
    void setResources(const QStringList &resources);

    /**
      Returns the incidence resources as a list of strings.
      @see setResources().
    */
    Q_REQUIRED_RESULT QStringList resources() const;

    /**
      Sets the incidences priority. The priority must be an integer value
      between 0 and 9, where 0 is undefined, 1 is the highest, and 9 is the
      lowest priority (decreasing order).

      @param priority is the incidence priority to set.
      @see priority().
    */
    void setPriority(int priority);

    /**
      Returns the incidence priority.
      @see setPriority().
    */
    Q_REQUIRED_RESULT int priority() const;

    /**
      Returns true if the incidence has geo data, otherwise return false.
      @see setHasGeo(), setGeoLatitude(float), setGeoLongitude(float).
    */
    Q_REQUIRED_RESULT bool hasGeo() const;

#if KCALENDARCORE_BUILD_DEPRECATED_SINCE(5, 89)
    /**
      Sets if the incidence has geo data.
      @param hasGeo if true, latitude and longitude must be set to valid values.
      If false, geoLatitude() and geoLongitude() will return INVALID_LATLON.
      @deprecated since 5.89.  Use setGeoLatitude() and setGeoLongitude().
      @see hasGeo(), geoLatitude(), geoLongitude().
    */
    KCALENDARCORE_DEPRECATED_VERSION(5, 89, "Use setGeoLatitude() and setGeoLongitude()")
    void setHasGeo(bool hasGeo);
#endif

    /**
      Set the incidence's geoLatitude.
      @param geolatitude is the incidence geolatitude to set; a value between -90.0 and 90.0,
      or INVALID_LATLON (or NaN, which is treated as INVALID_LATLON).
      @see geoLatitude().
    */
    void setGeoLatitude(float geolatitude);

    /**
      Returns the incidence's geoLatitude as a value between -90.0 and 90.0 or INVALID_LATLON.
      If either of geoLatitude() and geoLongitude() are INVALID_LATLON, then both are, and hasGeo() is false.
      @return incidences geolatitude value
      @see setGeoLatitude().
    */
    Q_REQUIRED_RESULT float geoLatitude() const;

    /**
      Set the incidence's geoLongitude.
      @param geolongitude is the incidence geolongitude to set; a value between -180.0 and 180.0,
      or INVALID_LATLON (or NaN, which is treated as INVALID_LATLON).
      @see geoLongitude().
    */
    void setGeoLongitude(float geolongitude);

    /**
      Returns the incidence's geoLongitude as a value between -180.0 and 180.0 or INVALID_LATLON.
      If either of geoLatitude() and geoLongitude() are INVALID_LATLON, then both are, and hasGeo() is false.
      @return incidences geolongitude value
      @see setGeoLongitude().
    */
    Q_REQUIRED_RESULT float geoLongitude() const;

    /**
      Returns true if the incidence has recurrenceId, otherwise return false.
      @see setRecurrenceId(QDateTime)
    */
    Q_REQUIRED_RESULT bool hasRecurrenceId() const;

    /**
      Set the incidences recurrenceId.
      This field indicates that this is an exception to a recurring incidence.
      The uid of this incidence MUST be the same as the one of the recurring main incidence.
      @param recurrenceId is the incidence recurrenceId to set
      @see recurrenceId().
    */
    void setRecurrenceId(const QDateTime &recurrenceId);

    /**
      Returns the incidence recurrenceId.
      @return incidences recurrenceId value
      @see setRecurrenceId().
    */
    Q_REQUIRED_RESULT QDateTime recurrenceId() const override;

    /**
      Set to true if the exception also applies to all future occurrences.
      This option is only relevant if the incidence has a recurrenceId set.
      @param thisAndFuture value
      @see thisAndFuture(), setRecurrenceId()
      @since 4.11
    */
    void setThisAndFuture(bool thisAndFuture);

    /**
      Returns true if the exception also applies to all future occurrences.
      @return incidences thisAndFuture value
      @see setThisAndFuture()
      @since 4.11
    */
    Q_REQUIRED_RESULT bool thisAndFuture() const;

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%  Alarm-related methods
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Returns a list of all incidence alarms.
    */
    Q_REQUIRED_RESULT Alarm::List alarms() const;

    /**
      Create a new incidence alarm.
    */
    Alarm::Ptr newAlarm();

    /**
      Adds an alarm to the incidence.

      @param alarm is a pointer to a valid Alarm object.
      @see removeAlarm().
    */
    void addAlarm(const Alarm::Ptr &alarm);

    /**
      Removes the specified alarm from the incidence.

      @param alarm is a pointer to a valid Alarm object.
      @see addAlarm().
    */
    void removeAlarm(const Alarm::Ptr &alarm);

    /**
      Removes all alarms.
      @see removeAlarm().
    */
    void clearAlarms();

    /**
      Returns true if any of the incidence alarms are enabled; false otherwise.
    */
    Q_REQUIRED_RESULT bool hasEnabledAlarms() const;

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%% Conferences-related method
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
     * Returns list of all incidence conferencing methods.
     * @since 5.77
     */
    Q_REQUIRED_RESULT Conference::List conferences() const;

    /**
     * Replaces all conferences in the incidence with given @p conferences
     *
     * @param conferences New conferences to store in the incidence.
     * @since 5.77
     */
    void setConferences(const Conference::List &conferences);

    /**
     * Adds a conference to the incidence.
     *
     * @param conferene A conference to add.
     * @since 5.77
     */
    void addConference(const Conference &conference);

    /**
     * Removes all conferences from the incidence.
     * @since 5.77
     */
    void clearConferences();

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %%%%%  Other methods
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Set the incidence scheduling ID. Do _not_ use with journals.
      This is used for accepted invitations as the place to store the UID
      of the invitation. It is later used again if updates to the
      invitation comes in.
      If we did not set a new UID on incidences from invitations, we can
      end up with more than one resource having events with the same UID,
      if you have access to other peoples resources.

      While constructing an incidence, when setting the scheduling ID,
      you will always want to set the incidence UID too. Instead of calling
      setUID() separately, you can pass the UID through @p uid so both
      members are changed in one atomic operation ( don't forget that
      setUID() emits incidenceUpdated() and whoever catches that signal
      will have an half-initialized incidence, therefore, always set
      the schedulingID and UID at the same time, and never with two separate
      calls).

      @param sid is a QString containing the scheduling ID.
      @param uid is a QString containing the incidence UID to set, if not
             specified, the current UID isn't changed, and this parameter
             is ignored.
      @see schedulingID().
    */
    void setSchedulingID(const QString &sid, const QString &uid = QString());

    /**
      Returns the incidence scheduling ID. Do _not_ use with journals.
      If a scheduling ID is not set, then return the incidence UID.
      @see setSchedulingID().
    */
    Q_REQUIRED_RESULT QString schedulingID() const;

    /**
      Observer interface for the recurrence class. If the recurrence is
      changed, this method will be called for the incidence the recurrence
      object belongs to.

      @param recurrence is a pointer to a valid Recurrence object.
    */
    void recurrenceUpdated(Recurrence *recurrence) override;

    /**
      Returns the name of the icon that best represents this incidence.

      @param recurrenceId Some recurring incidences might use a different icon,
      for example, completed to-do occurrences. Use this parameter to identify
      the specific occurrence in a recurring serie.
    */
    virtual QLatin1String iconName(const QDateTime &recurrenceId = {}) const = 0;

    /**
     * Returns true if the incidence type supports groupware communication.
     * @since 4.10
     */
    virtual bool supportsGroupwareCommunication() const = 0;

    /**
      Returns the list of possible mime types in an Incidence object:
          "text/calendar"
          "application/x-vnd.akonadi.calendar.event"
          "application/x-vnd.akonadi.calendar.todo"
          "application/x-vnd.akonadi.calendar.journal"

      @since 4.12
     */
    Q_REQUIRED_RESULT static QStringList mimeTypes();

protected:
#if KCALENDARCORE_BUILD_DEPRECATED_SINCE(5, 91)
    /**
      Copy constructor.
      @param other is the incidence to copy.
      @deprecated Use Incidence(const Incidence &other, IncidencePrivate *p).
    */
    Incidence(const Incidence &other);
#else
    Incidence(const Incidence &) = delete;
#endif

    /**
      @param other is the incidence to copy.
      @param p (non-null) a Private data object provided by the instantiated
      class (Event, Todo, Journal, FreeBusy). It passes ownership of the object
      to IncidenceBase.
    */
    Incidence(const Incidence &other, IncidencePrivate *p);

    /**
      Compares this with Incidence @p incidence for equality.
      @param incidence is the Incidence to compare against.
      @return true if the incidences are equal; false otherwise.
    */
    bool equals(const IncidenceBase &incidence) const override;

    /**
      @copydoc IncidenceBase::assign()
    */
    IncidenceBase &assign(const IncidenceBase &other) override;

    void serialize(QDataStream &out) const override;
    void deserialize(QDataStream &in) override;

private:
    /**
      Disabled, not polymorphic.
      Use IncidenceBase::operator= which is safe because it calls
      virtual function assign.
      @param other is another Incidence object to assign to this one.
     */
    Incidence &operator=(const Incidence &other);

    Q_DECL_HIDDEN QVariantList attachmentsVariant() const;
    Q_DECL_HIDDEN QVariantList conferencesVariant() const;

protected:
    Q_DECLARE_PRIVATE(Incidence)
#if KCALENDARCORE_BUILD_DEPRECATED_SINCE(5, 91)
    KCALENDARCORE_DEPRECATED_VERSION(5, 91, "Do not use")
    IncidencePrivate *const _ = nullptr;    // TODO KF6 remove. ABI compatibility hack.
#endif
};

}

//@cond PRIVATE
inline uint qHash(const QSharedPointer<KCalendarCore::Incidence> &key)
{
    return qHash(key.data());
}
//@endcond

//@cond PRIVATE
Q_DECLARE_TYPEINFO(KCalendarCore::Incidence::Ptr, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(KCalendarCore::Incidence *)
//@endcond

#endif
