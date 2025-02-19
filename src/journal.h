/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Journal class.
*/
#ifndef KCALCORE_JOURNAL_H
#define KCALCORE_JOURNAL_H

#include "incidence.h"
#include "kcalendarcore_export.h"

namespace KCalendarCore
{

class JournalPrivate;

/*!
  \class KCalendarCore::Journal
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Journal
  \brief
  Provides a Journal in the sense of RFC2445.
*/
class KCALENDARCORE_EXPORT Journal : public Incidence
{
public:
    /*!
      A shared pointer to a Journal object.
    */
    typedef QSharedPointer<Journal> Ptr;

    /*!
      List of journals.
    */
    typedef QList<Ptr> List;

    ///@cond PRIVATE
    // needed for Akonadi polymorphic payload support
    typedef Incidence SuperClass;
    ///@endcond

    /*!
      Constructs an empty journal.
    */
    Journal();

    /*! Copy a journey object. */
    Journal(const Journal &);

    /*!
      Destroys a journal.
    */
    ~Journal() override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT IncidenceType type() const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT QByteArray typeStr() const override;

    /*!
      Returns an exact copy of this journal. The returned object is owned
      by the caller.
    */
    Journal *clone() const override;

    /*!
      \reimp
    */
    Q_REQUIRED_RESULT QDateTime dateTime(DateTimeRole role) const override;

    /*!
      \reimp
    */
    void setDateTime(const QDateTime &dateTime, DateTimeRole role) override;

    /*!
       \reimp
    */
    Q_REQUIRED_RESULT QLatin1String mimeType() const override;

    /*!
       \reimp
    */
    Q_REQUIRED_RESULT QLatin1String iconName(const QDateTime &recurrenceId = {}) const override;

    /*!
       \reimp
    */
    Q_REQUIRED_RESULT bool supportsGroupwareCommunication() const override;

    /*!
       Returns the Akonadi specific sub MIME type of a KCalendarCore::Journal.
    */
    Q_REQUIRED_RESULT static QLatin1String journalMimeType();

protected:
    /*!
      Compare this with \a journal for equality.

      \a journal is the journal to compare.

      Returns true if \a journal is equal to this object, or false if they are different.
    */
    bool equals(const IncidenceBase &journal) const override;

    /*!
      \reimp
    */
    IncidenceBase &assign(const IncidenceBase &other) override;

    /*!
      \reimp
    */
    void virtual_hook(VirtualHook id, void *data) override;

private:
    /*!
      \reimp
    */
    bool accept(Visitor &v, const IncidenceBase::Ptr &incidence) override;

    /*!
      Disabled, otherwise could be dangerous if you subclass Journal.
      Use IncidenceBase::operator= which is safe because it calls
      virtual function assign().

      \a other is another Journal object to assign to this one.

     */
    Journal &operator=(const Journal &other) = delete;

    // For polymorphic serialization
    void serialize(QDataStream &out) const override;
    void deserialize(QDataStream &in) override;

    //@cond PRIVATE
    Q_DECLARE_PRIVATE(Journal)
    //@endcond
};

} // namespace KCalendarCore

//@cond PRIVATE
Q_DECLARE_TYPEINFO(KCalendarCore::Journal::Ptr, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(KCalendarCore::Journal::Ptr)
Q_DECLARE_METATYPE(KCalendarCore::Journal *)
//@endcond

#endif
