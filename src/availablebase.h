/*
  This file is part of the kcalcore library.


  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/**
  @file
  This file is part of the API for handling calendar data and
  defines the AvailableBase class.

*/

#ifndef KCALCORE_AVAILABLEBASE_H
#define KCALCORE_AVAILABLEBASE_H

#include "customproperties.h"
#include "duration.h"
#include "kcalendarcore_debug.h"

#include <QSharedPointer>

namespace KCalendarCore
{

/**
  @brief
  An abstract class that provides a common base for Availability/Available classes.

  AvailableBase
  + Availability
  + Available

  AvailableBase contains the properties and their getter/setter methods that are common
  to Available and Availability classes. The two derived classes then has some more
  members that are not common to both.
*/
class KCALENDARCORE_EXPORT AvailableBase : public CustomProperties
{
protected:
    class AvailableBasePrivate;
    std::unique_ptr<AvailableBasePrivate> d;

public:
    /**
      Different type of available, per RFC7953
      @see type()
     */
    enum AvailableType {
        TypeAvailable = 0, /**< Type is an Available */
        TypeVAvailability, /**< Type is an Availability */
    };

    AvailableBase() = delete;

    /**
      Constructs an empty AvailableBase.
      @param p (non-null) a Private data object provided by the instantiated
      class (Available, Availability).  It takes ownership of the object.
    */
    KCALENDARCORE_NO_EXPORT explicit AvailableBase(AvailableBasePrivate *p);

    /**
      Destroys the AvailableBase.
    */
    ~AvailableBase() override;

    /**
      Assignment operator.
      All data belonging to dreived classes are also copied. @see assign()
      The caller guarantees that both types match.

      @param other is the AvailableBase to assign.
     */
    AvailableBase &operator=(const AvailableBase &other);

    /**
      Compares this with AvailableBase @p ib for equality.
      All data belonging to derived classes are also compared. @see equals().
      @param ib is the AvailableBase to compare against.
      @return true if they are equal; false otherwise.
    */
    bool operator==(const AvailableBase &ib) const;

    /**
      Provides polymorfic comparison for equality.
      Only called by AvailableBase::operator==() which guarantees that
      @p availableBase is of the right type.
      @param availableBase is the AvailableBase to compare against.
      @return true if they are equal; false otherwise.
    */
    virtual bool equals(const AvailableBase &availableBase) const;

    /**
      Provides polymorfic assignment.
      @param other is the AvailableBase to assign.
    */
    virtual AvailableBase &assign(const AvailableBase &other);

    /**
      This is not allowed. Use AvailableBase(const AvailableBase &ib, AvailableBasePrivate *p).
     */
    AvailableBase(const AvailableBase &) = delete;

    /**
      Constructs an AvailableBase as a copy of another AvailableBase object.
      @param ib is the AvailableBase to copy.
      @param p (non-null) a Private data object provided by the instantiated
      class (Available, Availability).  It takes ownership of the object.
    */
    KCALENDARCORE_NO_EXPORT AvailableBase(const AvailableBase &ib, AvailableBasePrivate *p);

    /**
      A shared pointer to an AvailableBase.
    */
    typedef QSharedPointer<AvailableBase> Ptr;

    /**
      Returns the type.
     */
    virtual AvailableType type() const = 0;

    /**
      Sets the unique id for the available component to @p uid.
      @param uid is the string containing the @ref uid.
      @see uid()
    */
    void setUid(const QString &uid);

    /**
      Returns the unique id (@ref uid) for the available component.
      @see setUid()
    */
    Q_REQUIRED_RESULT QString uid() const;

    /**
      Sets the creation date/time. It is stored as a UTC date/time.

      @param dt is the creation date/time.
      @see created().
    */
    void setCreated(const QDateTime &dt);

    /**
      Returns the creation date/time.
      @see setCreated().
    */
    Q_REQUIRED_RESULT QDateTime created() const;

    /**
      Sets the timestamp of creation.

      @param dt is the dtstamp.
      @see dtStart().
    */
    void setDtStamp(const QDateTime &dt);

    /**
      Returns dtStamp as a QDateTime.
      @see setDtStart().
    */
    Q_REQUIRED_RESULT QDateTime dtStamp() const;

    /**
      Sets the starting date/time.

      @param dt is the starting date/time.
      @see dtStart().
    */
    void setDtStart(const QDateTime &dt);

    /**
      Returns starting date/time as a QDateTime.
      @see setDtStart().
    */
    Q_REQUIRED_RESULT QDateTime dtStart() const;

    /**
      Sets the ending date/time.

      @param dt is the ending date/time.
      @see dtStart().
    */
    void setDtEnd(const QDateTime &dt);

    /**
      Returns ending date/time as a QDateTime.
      @see setDtStart().
    */
    Q_REQUIRED_RESULT QDateTime dtEnd() const;

    /**
      Sets the duration.

      @param duration the duration

      @see duration()
    */
    void setDuration(const Duration &duration);

    /**
      Returns the length of the duration.
      @see setDuration()
    */
    Q_REQUIRED_RESULT Duration duration() const;

    /**
      Sets the time the available component was last modified to @p lm.
      It is stored as a UTC date/time.

      @param lm is the QDateTime when the available component was last modified.

      @see lastModified()
    */
    virtual void setLastModified(const QDateTime &lm);

    /**
      Returns the time the available component was last modified.
      @see setLastModified()
    */
    Q_REQUIRED_RESULT QDateTime lastModified() const;

    /**
      Sets the summary.

      @param summary is the summary string.
      @see summary().
    */
    void setSummary(const QString &summary);

    /**
      Returns the summary.
    */
    Q_REQUIRED_RESULT QString summary() const;

    /**
      Returns true if summary contains RichText; false otherwise.
      @see setSummary(), summary().
    */
    Q_REQUIRED_RESULT bool summaryIsRich() const;

    /**
      Sets the location. Do _not_ use with journals.

      @param location is the location string.
      @param isRich if true indicates the location string contains richtext.
      @see location().
    */
    void setLocation(const QString &location, bool isRich);

    /**
      Returns the location. Do _not_ use with journals.
      @see setLocation().
      @see richLocation().
    */
    Q_REQUIRED_RESULT QString location() const;

    /**
      Sets the description.

      @param description is the description string.
      @param isRich if true indicates the description string contains richtext.
      @see description().
    */
    void setDescription(const QString &description, bool isRich);

    /**
      Sets the description and tries to guess if the description
      is rich text.

      @param description is the description string.
      @see description().
    */
    void setDescription(const QString &description);

    /**
      Returns the description.
      @see setDescription().
      @see richDescription().
    */
    Q_REQUIRED_RESULT QString description() const;

    /**
      Returns the description in rich text format.
      @see setDescription().
      @see description().
    */
    Q_REQUIRED_RESULT QString richDescription() const;

    /**
      Returns true if description contains RichText; false otherwise.
      @see setDescription(), description().
    */
    Q_REQUIRED_RESULT bool descriptionIsRich() const;

    /**
      Sets the category list.

      @param categories is a list of category strings.
      @see setCategories( const QString &), categories().
    */
    void setCategories(const QStringList &categories);

    /**
      Sets the category list based on a comma delimited string.

      @param catStr is a QString containing a list of categories which
      are delimited by a comma character.
      @see setCategories( const QStringList &), categories().
    */
    void setCategories(const QString &catStr);

    /**
      Returns the categories as a list of strings.
      @see setCategories( const QStringList &), setCategories( const QString &).
    */
    Q_REQUIRED_RESULT QStringList categories() const;

    /**
      Returns the categories as a comma separated string.
      @see categories().
    */
    Q_REQUIRED_RESULT QString categoriesStr() const;

    /**
      Adds a comment. Does not add a linefeed character; simply
      appends the text as specified.

      @param comment is the QString containing the comment to add.
      @see removeComment().
    */
    void addComment(const QString &comment);

    /**
      Removes a comment. Removes the first comment whose
      string is an exact match for the specified string in @p comment.

      @param comment is the QString containing the comment to remove.
      @return true if match found, false otherwise.
      @see addComment().
     */
    Q_REQUIRED_RESULT bool removeComment(const QString &comment);

    /**
      Deletes all comments.
    */
    void clearComments();

    /**
      Returns all comments as a list of strings.
    */
    Q_REQUIRED_RESULT QStringList comments() const;

    /**
      Adds a contact. Does not add a linefeed character; simply
      appends the text as specified.

      @param contact is the QString containing the contact to add.
      @see removeContact().
    */
    void addContact(const QString &contact);

    /**
      Removes a contact. Removes the first contact whose
      string is an exact match for the specified string in @p contact.

      @param contact is the QString containing the contact to remove.
      @return true if match found, false otherwise.
      @see addContact().
     */
    Q_REQUIRED_RESULT bool removeContact(const QString &contact);

    /**
      Deletes all contacts.
    */
    void clearContacts();

    /**
      Returns all contacts as a list of strings.
    */
    Q_REQUIRED_RESULT QStringList contacts() const;
};
}

#endif
