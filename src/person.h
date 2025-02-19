/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001-2003 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Person class.
*/

#ifndef KCALCORE_PERSON_H
#define KCALCORE_PERSON_H

#include "kcalendarcore_export.h"

#include <QHash>
#include <QMetaType>
#include <QSharedDataPointer>
#include <QString>

namespace KCalendarCore
{
/*!
  \qmlvaluetype person
  \inqmlmodule org.kde.kcalendarcore
  \nativetype KCalendarCore::Person

  \brief
  Represents a person, by name and email address.

  This class represents a person, with a name and an email address.
  It supports the "FirstName LastName\ <mail@domain\>" format.
*/

/*!
  \class KCalendarCore::Person
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Person
  \brief
  Represents a person, by name and email address.

  This class represents a person, with a name and an email address.
  It supports the "FirstName LastName\ <mail@domain\>" format.
*/
class KCALENDARCORE_EXPORT Person
{
    Q_GADGET

    /*!
     * \qmlproperty bool person::isEmpty
     */

    /*!
     * \property KCalendarCore::Person::isEmpty
     */
    Q_PROPERTY(bool isEmpty READ isEmpty)

    /*!
     * \qmlproperty string person::fullName
     */

    /*!
     * \property KCalendarCore::Person::fullName
     */
    Q_PROPERTY(QString fullName READ fullName)

    /*!
     * \qmlproperty string person::name
     */

    /*!
     * \property KCalendarCore::Person::name
     */
    Q_PROPERTY(QString name READ name WRITE setName)

    /*!
     * \qmlproperty string person::email
     */

    /*!
     * \property KCalendarCore::Person::email
     */
    Q_PROPERTY(QString email READ email WRITE setEmail)

public:
    /*!
      List of persons.
    */
    typedef QList<Person> List;

    /*!
      Constructs a blank person.
    */
    Person();

    /*!
      Constructs a person with name and email address taken from \a fullName.

      \a fullName is the name and email of the person in the form
        "FirstName LastName \<mail@domain\>".

      Returns A Person object.
    */
    static Person fromFullName(const QString &fullName);

    /*!
      Constructs a person with the name \a name and email address \a email.

      \a name is the name of this person.

      \a email is the email address of this person.

    */
    Person(const QString &name, const QString &email);

    /*!
       Constructs a person as a copy of another person object.
       \a person is the person to copy.
     */
    Person(const Person &person);

    /*!
      Destroys a person.
    */
    virtual ~Person();

    /*!
      Returns true if the person name and email address are empty.
    */
    Q_REQUIRED_RESULT bool isEmpty() const;

    /*!
      Returns the full name of this person.
      Returns A QString containing the person's full name in the form
        "FirstName LastName \<mail@domain\>".
    */
    Q_REQUIRED_RESULT QString fullName() const;

    /*!
      Sets the name of the person to \a name.

      \a name is the name of this person.

      \sa name()
    */
    void setName(const QString &name);

    /*!
      Returns the person name string.

      \sa setName()
    */
    Q_REQUIRED_RESULT QString name() const;

    /*!
      Sets the email address for this person to \a email.

      \a email is the email address for this person.

      \sa email()
    */
    void setEmail(const QString &email);

    /*!
      Returns the email address for this person.
      Returns A QString containing the person's email address.
      \sa setEmail()
    */
    Q_REQUIRED_RESULT QString email() const;

    /*!
      Returns true if person's email address is valid.
      Simple email validity check, test that there:
      \list
        \li is at least one @
        \li is at least one character in the local part
        \li is at least one dot in the domain part
        \li is at least four characters in the domain (assuming that no-one has an address at the tld,
                                                   that the tld is at least 2 chars)
      \endlist

      \a email is the email address to validate

    */
    Q_REQUIRED_RESULT static bool isValidEmail(const QString &email);

    /*!
      Compares this with \a person for equality.

      \a person is the person to compare.

      Returns true of \a person is equal to this object, or false if they are different.
    */
    bool operator==(const Person &person) const;

    /*!
      Compares this with \a person for non-equality.

      \a person is the person to compare.

      Returns true of \a person is not equal to this object, or false if they are equal.
    */
    bool operator!=(const Person &person) const;

    /*!
      Sets this person equal to \a person.

      \a person is the person to copy.

    */
    Person &operator=(const Person &person);

private:
    //@cond PRIVATE
    class Private;
    QSharedDataPointer<Private> d;
    //@endcond

    friend KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &s, const KCalendarCore::Person &person);
    friend KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &s, KCalendarCore::Person &person);
};

/*!
  Serializes the \a person object into the \a stream.
*/
KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &stream, const KCalendarCore::Person &person);

/*!
  Initializes the \a person object from the \a stream.
*/
KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &stream, KCalendarCore::Person &person);

/*!
  Return a hash value for a Person argument.
  \a key is a Person.
*/
KCALENDARCORE_EXPORT size_t qHash(const KCalendarCore::Person &key, size_t seed = 0);

}

//@cond PRIVATE
Q_DECLARE_TYPEINFO(KCalendarCore::Person, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(KCalendarCore::Person)
//@endcond

#endif
