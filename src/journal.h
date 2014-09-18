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
  defines the Journal class.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
*/
#ifndef KCALCORE_JOURNAL_H
#define KCALCORE_JOURNAL_H

#include "kcalcore_export.h"
#include "incidence.h"
#include "supertrait.h"

namespace KCalCore {

/**
  @brief
  Provides a Journal in the sense of RFC2445.
*/
class KCALCORE_EXPORT Journal : public Incidence
{
public:
    /**
      A shared pointer to a Journal object.
    */
    typedef QSharedPointer<Journal> Ptr;

    /**
      List of journals.
    */
    typedef QVector<Ptr> List;

    /**
      Constructs an empty journal.
    */
    Journal();

    /**
      Destroys a journal.
    */
    ~Journal();

    /**
      @copydoc
      IncidenceBase::type()
    */
    IncidenceType type() const;

    /**
      @copydoc
      IncidenceBase::typeStr()
    */
    QByteArray typeStr() const;

    /**
      Returns an exact copy of this journal. The returned object is owned
      by the caller.
    */
    Journal *clone() const;

    /**
      @copydoc
      IncidenceBase::dateTime(DateTimeRole)const
    */
    KDateTime dateTime(DateTimeRole role) const;

    /**
      @copydoc
      IncidenceBase::setDateTime(const KDateTime &, DateTimeRole )
    */
    void setDateTime(const KDateTime &dateTime, DateTimeRole role);

    /**
       @copydoc
       IncidenceBase::mimeType()
    */
    QLatin1String mimeType() const;

    /**
       @copydoc
       Incidence::iconName()
    */
    QLatin1String iconName(const KDateTime &recurrenceId = KDateTime()) const;

    /**
       Returns the Akonadi specific sub MIME type of a KCalCore::Journal.
    */
    static QLatin1String journalMimeType();

protected:
    /**
      Compare this with @p journal for equality.

      @param journal is the journal to compare.
    */
    bool equals(const IncidenceBase &journal) const;

    /**
      @copydoc
      IncidenceBase::assign()
    */
    virtual IncidenceBase &assign(const IncidenceBase &other);

    /**
      @copydoc
      IncidenceBase::virtual_hook()
    */
    virtual void virtual_hook(int id, void *data);

private:
    /**
      @copydoc
      IncidenceBase::accept(Visitor &, IncidenceBase::Ptr)
    */
    bool accept(Visitor &v, IncidenceBase::Ptr incidence);

    /**
      Disabled, otherwise could be dangerous if you subclass Journal.
      Use IncidenceBase::operator= which is safe because it calls
      virtual function assign().
      @param other is another Journal object to assign to this one.
     */
    Journal &operator=(const Journal &other);

    // For polymorfic serialization
    void serialize(QDataStream &out);
    void deserialize(QDataStream &in);

    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond
};


} // namespace KCalCore

//@cond PRIVATE
Q_DECLARE_TYPEINFO(KCalCore::Journal::Ptr, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(KCalCore::Journal::Ptr)
Q_DECLARE_METATYPE(KCalCore::Journal*)
//@endcond

//@cond PRIVATE
namespace Akonadi {
// super class trait specialization
template <> struct SuperClass<KCalCore::Journal> : public SuperClassTrait<KCalCore::Incidence> {};
}
//@endcond

#endif
