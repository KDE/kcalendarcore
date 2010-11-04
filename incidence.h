/*
  This file is part of the kcalcore library.

  Copyright (c) 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

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
  defines the Incidence class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
*/

#ifndef KCALCORE_INCIDENCE_H
#define KCALCORE_INCIDENCE_H

#include "kcalcore_export.h"
#include "alarm.h"
#include "attachment.h"
#include "incidencebase.h"
#include "recurrence.h"

#include <QtCore/QMetaType>

//@cond PRIVATE
// Value used to signal invalid/unset latitude or longitude.
#define INVALID_LATLON 255.0
//@endcond

namespace KCalCore {

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
class KCALCORE_EXPORT Incidence
  : public IncidenceBase, public Recurrence::RecurrenceObserver
{
  public:

    /**
      The different types of overall incidence status or confirmation.
      The meaning is specific to the incidence type in context.
    */
    enum Status {
      StatusNone,           /**< No status */
      StatusTentative,      /**< event is tentative */
      StatusConfirmed,      /**< event is definite */
      StatusCompleted,      /**< to-do completed */
      StatusNeedsAction,    /**< to-do needs action */
      StatusCanceled,       /**< event or to-do canceled; journal removed */
      StatusInProcess,      /**< to-do in process */
      StatusDraft,          /**< journal is draft */
      StatusFinal,          /**< journal is final */
      StatusX               /**< a non-standard status string */
    };

    /**
      The different types of incidence access classifications.
    */
    enum Secrecy {
      SecrecyPublic,      /**< Not secret (default) */
      SecrecyPrivate,     /**< Secret to the owner */
      SecrecyConfidential /**< Secret to the owner and some others */
    };

    /**
       The different types of RELTYPE values specified by the RFC.
       Only RelTypeParent is supported for now.
    */
    enum RelType {
      RelTypeParent,  /**< The related incidence is a parent. */
      RelTypeChild,   /**< The related incidence is a child. */
      RelTypeSibling  /**< The related incidence is a peer. */
    };

    /**
      A shared pointer to an Incidence.
    */
    typedef QSharedPointer<Incidence> Ptr;

    /**
      List of incidences.
    */
    typedef QVector<Ptr> List;

    /**
      Constructs an empty incidence.*
    */
    Incidence();

    /**
      Destroys an incidence.
    */
    virtual ~Incidence();

    /**
      Returns an exact copy of this incidence. The returned object is owned
      by the caller.

      Dirty fields are cleared.
    */
    virtual Incidence *clone() const = 0;

    /**
      Set readonly state of incidence.

      @param readonly If true, the incidence is set to readonly, if false the
                      incidence is set to readwrite.
    */
    void setReadOnly( bool readonly );

    /**
      @copydoc
      IncidenceBase::setLastModified().
    */
    void setLastModified( const KDateTime &lm );

    /**
      Set localOnly state of incidence.
      A local only incidence can be updated but it will not increase the revision
      number neither the modified date.

      @param localonly If true, the incidence is set to localonly, if false the
                      incidence is set to normal stat.
    */
    void setLocalOnly( bool localonly );

    /**
      Get the localOnly status.

      @see setLocalOnly()
      @return True if Local only, false otherwise
    */
    bool localOnly() const;

    /**
      @copydoc
      IncidenceBase::setAllDay().
    */
    void setAllDay( bool allDay );

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
    void setCreated( const KDateTime &dt );

    /**
      Returns the incidence creation date/time.
      @see setCreated().
    */
    KDateTime created() const;

    /**
      Sets the number of revisions this incidence has seen.

      @param rev is the incidence revision number.
      @see revision().
    */
    void setRevision( int rev );

    /**
      Returns the number of revisions this incidence has seen.
      @see setRevision().
    */
    int revision() const;

    /**
      Sets the incidence starting date/time.

      @param dt is the starting date/time.
      @see IncidenceBase::dtStart().
    */
    virtual void setDtStart( const KDateTime &dt );

    /**
      @copydoc
      IncidenceBase::shiftTimes()
    */
    virtual void shiftTimes( const KDateTime::Spec &oldSpec,
                             const KDateTime::Spec &newSpec );

    /**
      Sets the incidence description.

      @param description is the incidence description string.
      @param isRich if true indicates the description string contains richtext.
      @see description().
    */
    void setDescription( const QString &description, bool isRich );

    /**
      Sets the incidence description and tries to guess if the description
      is rich text.

      @param description is the incidence description string.
      @see description().
    */
    void setDescription( const QString &description );

    /**
      Returns the incidence description.
      @see setDescription().
      @see richDescription().
    */
    QString description() const;

    /**
      Returns the incidence description in rich text format.
      @see setDescription().
      @see description().
    */
    QString richDescription() const;

    /**
      Returns true if incidence description contains RichText; false otherwise.
      @see setDescription(), description().
    */
    bool descriptionIsRich() const;

    /**
      Sets the incidence summary.

      @param summary is the incidence summary string.
      @param isRich if true indicates the summary string contains richtext.
      @see summary().
    */
    void setSummary( const QString &summary, bool isRich );

    /**
      Sets the incidence summary and tries to guess if the summary is richtext.

      @param summary is the incidence summary string.
      @see summary().
    */
    void setSummary( const QString &summary );

    /**
      Returns the incidence summary.
      @see setSummary().
      @see richSummary().
    */
    QString summary() const;

    /**
      Returns the incidence summary in rich text format.
      @see setSummary().
      @see summary().
    */
    QString richSummary() const;

    /**
      Returns true if incidence summary contains RichText; false otherwise.
      @see setSummary(), summary().
    */
    bool summaryIsRich() const;

    /**
      Sets the incidence location. Do _not_ use with journals.

      @param location is the incidence location string.
      @param isRich if true indicates the location string contains richtext.
      @see location().
    */
    void setLocation( const QString &location, bool isRich );

    /**
      Sets the incidence location and tries to guess if the location is
      richtext. Do _not_ use with journals.

      @param location is the incidence location string.
      @see location().
    */
    void setLocation( const QString &location );

    /**
      Returns the incidence location. Do _not_ use with journals.
      @see setLocation().
      @see richLocation().
    */
    QString location() const;

    /**
      Returns the incidence location in rich text format.
      @see setLocation().
      @see location().
    */
    QString richLocation() const;

    /**
      Returns true if incidence location contains RichText; false otherwise.
      @see setLocation(), location().
    */
    bool locationIsRich() const;

    /**
      Sets the incidence category list.

      @param categories is a list of category strings.
      @see setCategories( const QString &), categories().
    */
    void setCategories( const QStringList &categories );

    /**
      Sets the incidence category list based on a comma delimited string.

      @param catStr is a QString containing a list of categories which
      are delimited by a comma character.
      @see setCategories( const QStringList &), categories().
    */
    void setCategories( const QString &catStr );

    /**
      Returns the incidence categories as a list of strings.
      @see setCategories( const QStringList &), setCategories( Const QString &).
    */
    QStringList categories() const;

    /**
      Returns the incidence categories as a comma separated string.
      @see categories().
    */
    QString categoriesStr() const;

    /**
      Relates another incidence to this one, by UID. This function should only
      be used when constructing a calendar before the related incidence exists.

      @param uid is a QString containing a UID for another incidence.
      @param relType specifies the relation type.

      @warning KCalCore only supports one related-to field per reltype for now.

      @see relatedTo().
    */
    void setRelatedTo( const QString &uid, RelType relType = RelTypeParent );

    /**
      Returns a UID string for the incidence that is related to this one.
      This function should only be used when constructing a calendar before
      the related incidence exists.

      @warning KCalCore only supports one related-to field per reltype for now.

      @param relType specifies the relation type.

      @see setRelatedTo().
    */
    QString relatedTo( RelType relType = RelTypeParent ) const;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%  Convenience wrappers for property handling
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    /**
       Returns true if the alternative (=text/html) description is
       available.
       @see setAltDescription(), altDescription()
    */
    bool hasAltDescription() const;
    /**
      Sets the incidence's alternative (=text/html) description. If
      the text is empty, the property is removed.

      @param altdescription is the incidence altdescription string.
      @see altAltdescription().
    */
    void setAltDescription( const QString &altdescription );

    /**
      Returns the incidence alternative (=text/html) description.
      @see setAltDescription().
    */
    QString altDescription() const;

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
      @copydoc
      Recurrence::recurs()
    */
    bool recurs() const;

    /**
      @copydoc
      Recurrence::recurrenceType()
    */
    ushort recurrenceType() const;

    /**
      @copydoc
      Recurrence::recursOn()
    */
    virtual bool recursOn( const QDate &date, const KDateTime::Spec &timeSpec ) const;

    /**
      @copydoc
      Recurrence::recursAt()
    */
    bool recursAt( const KDateTime &dt ) const;

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
    virtual QList<KDateTime> startDateTimesForDate(
      const QDate &date,
      const KDateTime::Spec &timeSpec = KDateTime::LocalZone ) const;

    /**
      Calculates the start date/time for all recurrences that happen at the
      given time.

      @param datetime the date/time when the incidence should occur.
      @return the start date/time of all occurrences that overlap with the
      given date/time; an empty list if the incidence does not happen at the
      given time at all.
    */
    virtual QList<KDateTime> startDateTimesForDateTime(
      const KDateTime &datetime ) const;

    /**
      Returns the end date/time of the incidence occurrence if it starts at
      specified date/time.

      @param startDt is the specified starting date/time.
      @return the corresponding end date/time for the occurrence; or the start
      date/time if the end date/time is invalid; or the end date/time if
      the start date/time is invalid.
    */
    virtual KDateTime endDateForStart( const KDateTime &startDt ) const;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%  Attachment-related methods
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Adds an attachment to the incidence.

      @param attachment is a pointer to a valid Attachment object.
      @see deleteAttachment().
    */
    void addAttachment( const Attachment::Ptr &attachment );

    /**
      Removes the specified attachment from the incidence.  Additionally,
      the memory used by the attachment is freed.

      @param attachment is a pointer to a valid Attachment object.
      @see addAttachment(), deleteAttachments().
    */
    void deleteAttachment( const Attachment::Ptr &attachment );

    /**
      Removes all attachments of the specified MIME type from the incidence.
      The memory used by all the removed attachments is freed.

      @param mime is a QString containing the MIME type.
      @see deleteAttachment().
    */
    void deleteAttachments( const QString &mime );

    /**
      Returns a list of all incidence attachments.
      @see attachments( const QString &).
    */
    Attachment::List attachments() const;

    /**
      Returns a list of all incidence attachments with the specified MIME type.

      @param mime is a QString containing the MIME type.
      @see attachments().
    */
    Attachment::List attachments( const QString &mime ) const;

    /**
      Removes all attachments and frees the memory used by them.
      @see deleteAttachment( Attachment::Ptr), deleteAttachments( const QString &).
    */
    void clearAttachments();

    /**
      Writes the data in the attachment @p attachment to a temporary file
      and returns the local name of the temporary file.

      @param attachment is a pointer to a valid Attachment instance.
      @return a string containing the name of the temporary file containing the attachment.
      @see clearTempFiles().
    */
    QString writeAttachmentToTempFile( const Attachment::Ptr &attachment ) const;

    /**
      Deletes all temporary files used by attachments and frees any memory in use by them.
      @see writeAttachmentToTempFile().
    */
    void clearTempFiles();

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%  Secrecy and Status methods
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Sets the incidence #Secrecy.

      @param secrecy is the incidence #Secrecy to set.
      @see secrecy(), secrecyStr().
    */
    void setSecrecy( Secrecy secrecy );

    /**
      Returns the incidence #Secrecy.
      @see setSecrecy(), secrecyStr().
    */
    Secrecy secrecy() const;

    /**
      Sets the incidence status to a standard #Status value.
      Note that StatusX cannot be specified.

      @param status is the incidence #Status to set.
      @see status(), setCustomStatus().
    */
    void setStatus( Status status );

    /**
      Sets the incidence #Status to a non-standard status value.

      @param status is a non-standard status string. If empty,
      the incidence #Status will be set to StatusNone.
      @see setStatus(), status() customStatus().
    */
    void setCustomStatus( const QString &status );

    /**
       Returns the non-standard status value.
       @see setCustomStatus().
    */
    QString customStatus() const;

    /**
      Returns the incidence #Status.
      @see setStatus(), setCustomStatus(), statusStr().
    */
    Status status() const;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%  Other methods
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Sets a list of incidence resources. (Note: resources in this context
      means items used by the incidence such as money, fuel, hours, etc).

      @param resources is a list of resource strings.
      @see resources().
    */
    void setResources( const QStringList &resources );

    /**
      Returns the incidence resources as a list of strings.
      @see setResources().
    */
    QStringList resources() const;

    /**
      Sets the incidences priority. The priority must be an integer value
      between 0 and 9, where 0 is undefined, 1 is the highest, and 9 is the
      lowest priority (decreasing order).

      @param priority is the incidence priority to set.
      @see priority().
    */
    void setPriority( int priority );

    /**
      Returns the incidence priority.
      @see setPriority().
    */
    int priority() const;

    /**
      Returns true if the incidence has geo data, otherwise return false.
      @see setHasGeo(), setGeoLatitude(float), setGeoLongitude(float).
    */
    bool hasGeo() const;

    /**
      Sets if the incidence has geo data.
      @param hasGeo true if incidence has geo data, otherwise false
      @see hasGeo(), geoLatitude(), geoLongitude().
    */
    void setHasGeo( bool hasGeo );

    /**
      Set the incidences geoLatitude.
      @param geolatitude is the incidence geolatitude to set
      @see geoLatitude().
    */
    void setGeoLatitude( float geolatitude );

    /**
      Returns the incidence geoLatidude.
      @return incidences geolatitude value
      @see setGeoLatitude().
    */
    float geoLatitude() const;

    /**
      Set the incidencesgeoLongitude.
      @param geolongitude is the incidence geolongitude to set
      @see geoLongitude().
    */
    void setGeoLongitude( float geolongitude );

    /**
      Returns the incidence geoLongitude.
      @return incidences geolongitude value
      @see setGeoLongitude().
    */
    float geoLongitude() const;

    /**
      Returns true if the incidence has recurrenceId, otherwise return false.
      @see setHasRecurrenceID(), setRecurrenceId(KDateTime)
    */
    bool hasRecurrenceId() const;

    /**
      Set the incidences recurrenceId.
      @param recurrenceId is the incidence recurrenceId to set
      @see recurrenceId().
    */
    void setRecurrenceId( const KDateTime &recurrenceId );

    /**
      Returns the incidence recurrenceId.
      @return incidences recurrenceId value
      @see setRecurrenceId().
    */
    KDateTime recurrenceId() const;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%  Alarm-related methods
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    /**
      Returns a list of all incidence alarms.
    */
    Alarm::List alarms() const;

    /**
      Create a new incidence alarm.
    */
    Alarm::Ptr newAlarm();

    /**
      Adds an alarm to the incidence.

      @param alarm is a pointer to a valid Alarm object.
      @see removeAlarm().
    */
    void addAlarm( const Alarm::Ptr &alarm );

    /**
      Removes the specified alarm from the incidence.

      @param alarm is a pointer to a valid Alarm object.
      @see addAlarm().
    */
    void removeAlarm( const Alarm::Ptr &alarm );

    /**
      Removes all alarms.
      @see removeAlarm().
    */
    void clearAlarms();

    /**
      Returns true if any of the incidence alarms are enabled; false otherwise.
    */
    bool hasEnabledAlarms() const;

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
    void setSchedulingID( const QString &sid,
                          const QString &uid = QString() );

    /**
      Returns the incidence scheduling ID. Do _not_ use with journals.
      If a scheduling ID is not set, then return the incidence UID.
      @see setSchedulingID().
    */
    QString schedulingID() const;

    /**
      Observer interface for the recurrence class. If the recurrence is
      changed, this method will be called for the incidence the recurrence
      object belongs to.

      @param recurrence is a pointer to a valid Recurrence object.
    */
    virtual void recurrenceUpdated( Recurrence *recurrence );

    /**
      Returns the name of the icon that best represents this incidence.

      @param recurrenceId Some recurring incidences might use a different icon,
      for example, completed to-do occurrences. Use this parameter to identify
      the specific occurrence in a recurring serie.
    */
    virtual QLatin1String iconName( const KDateTime &recurrenceId = KDateTime() ) const = 0;

  protected:

    /**
      Copy constructor.
      @param other is the incidence to copy.
    */
    Incidence( const Incidence &other );

    /**
      Compares this with Incidence @p incidence for equality.
      @param incidence is the Incidence to compare against.
      @return true if the incidences are equal; false otherwise.
    */
    virtual bool equals( const IncidenceBase &incidence ) const;

    /**
      @copydoc
      IncidenceBase::assign()
    */
    virtual IncidenceBase &assign( const IncidenceBase &other );

  private:
    /**
      Disabled, not polymorphic.
      Use IncidenceBase::operator= which is safe because it calls
      virtual function assign.
      @param other is another Incidence object to assign to this one.
     */
    Incidence &operator=( const Incidence &other );

    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond
};

}

//@cond PRIVATE
inline uint qHash( const QSharedPointer<KCalCore::Incidence> &key )
{
  return qHash<KCalCore::Incidence>( key.data() );
}
//@endcond

Q_DECLARE_TYPEINFO( KCalCore::Incidence::Ptr, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( KCalCore::Incidence * )

#endif
