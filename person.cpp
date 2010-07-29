/*
  This file is part of the kcalcore library.

  Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>
  Copyright (C) 2009-2010 Klaralvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>

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
  defines the Person class.

  @brief
  Represents a person, by name and email address.

  @author Cornelius Schumacher \<schumacher@kde.org\>
  @author Reinhold Kainhofer \<reinhold@kainhofer.com\>
*/

#include "person.h"
#include <QtCore/QRegExp>

using namespace KCalCore;

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class KCalCore::Person::Private
{
  public:
    Private() : mCount( 0 ) {}
    QString mName;   // person name
    QString mEmail;  // person email address
    int mCount;      // person reference count
};
//@endcond

Person::Person() : d( new KCalCore::Person::Private )
{
}

Person::Person( const QString &name, const QString &email )
  : d( new KCalCore::Person::Private )
{
  d->mName = name;
  d->mEmail = email;
}

Person::Person( const Person &person )
  : d( new KCalCore::Person::Private( *person.d ) )
{
}

Person::~Person()
{
  delete d;
}

bool KCalCore::Person::operator==( const Person &person ) const
{
  return
    d->mName == person.d->mName &&
    d->mEmail == person.d->mEmail;
}

bool KCalCore::Person::operator!=( const Person &person ) const
{
  return !( *this == person );
}

Person &KCalCore::Person::operator=( const Person &person )
{
  // check for self assignment
  if ( &person == this ) {
    return *this;
  }

  *d = *person.d;
  return *this;
}

QString Person::fullName() const
{
  if ( d->mName.isEmpty() ) {
    return d->mEmail;
  } else {
    if ( d->mEmail.isEmpty() ) {
      return d->mName;
    } else {
      // Taken from KABC::Addressee::fullEmail
      QString name = d->mName;
      QRegExp needQuotes( "[^ 0-9A-Za-z\\x0080-\\xFFFF]" );
      bool weNeedToQuote = name.indexOf( needQuotes ) != -1;
      if ( weNeedToQuote ) {
        if ( name[0] != '"' ) {
          name.prepend( '"' );
        }
        if ( name[ name.length()-1 ] != '"' ) {
          name.append( '"' );
        }
      }
      return name + " <" + d->mEmail + '>';
    }
  }
}

QString Person::name() const
{
  return d->mName;
}

QString Person::email() const
{
  return d->mEmail;
}

bool Person::isEmpty() const
{
  return d->mEmail.isEmpty() && d->mName.isEmpty();
}

void Person::setName( const QString &name )
{
  d->mName = name;
}

void Person::setEmail( const QString &email )
{
  if ( email.startsWith( QLatin1String( "mailto:" ), Qt::CaseInsensitive ) ) {
    d->mEmail = email.mid( 7 );
  } else {
    d->mEmail = email;
  }
}

/** static */
bool Person::isValidEmail( const QString &email )
{
  int pos = email.lastIndexOf( "@" );
  return ( pos > 0 ) && ( email.lastIndexOf( "." ) > pos ) && ( ( email.length() - pos ) > 4 );
}

void Person::setCount( int count )
{
  d->mCount = count;
}

int Person::count() const
{
  return d->mCount;
}

uint qHash( const KCalCore::Person &key )
{
  return qHash( key.fullName() );
}

QDataStream &KCalCore::operator<<( QDataStream &stream, const KCalCore::Person::Ptr &person )
{
  return stream << person->d->mName
                << person->d->mEmail
                << person->d->mCount;
}

QDataStream &KCalCore::operator>>( QDataStream &stream, Person::Ptr &person )
{
  QString name, email;
  int count;

  stream >> name >> email >> count;

  Person::Ptr person_tmp( new Person( name, email ) );
  person_tmp->setCount( count );
  person.swap( person_tmp );
  return stream;
}

// The following function was lifted directly from KPIMUtils
// in order to eliminate the dependency on that library.
// Any changes made here should be ported there, and vice versa.
static bool extractEmailAddressAndName( const QString &aStr, QString &mail, QString &name )
{
  name.clear();
  mail.clear();

  const int len = aStr.length();
  const char cQuotes = '"';

  bool bInComment = false;
  bool bInQuotesOutsideOfEmail = false;
  int i=0, iAd=0, iMailStart=0, iMailEnd=0;
  QChar c;
  unsigned int commentstack = 0;

  // Find the '@' of the email address
  // skipping all '@' inside "(...)" comments:
  while ( i < len ) {
    c = aStr[i];
    if ( '(' == c ) {
      commentstack++;
    }
    if ( ')' == c ) {
      commentstack--;
    }
    bInComment = commentstack != 0;
    if ( '"' == c && !bInComment ) {
      bInQuotesOutsideOfEmail = !bInQuotesOutsideOfEmail;
    }

    if( !bInComment && !bInQuotesOutsideOfEmail ) {
      if ( '@' == c ) {
        iAd = i;
        break; // found it
      }
    }
    ++i;
  }

  if ( !iAd ) {
    // We suppose the user is typing the string manually and just
    // has not finished typing the mail address part.
    // So we take everything that's left of the '<' as name and the rest as mail
    for ( i = 0; len > i; ++i ) {
      c = aStr[i];
      if ( '<' != c ) {
        name.append( c );
      } else {
        break;
      }
    }
    mail = aStr.mid( i + 1 );
    if ( mail.endsWith( '>' ) ) {
      mail.truncate( mail.length() - 1 );
    }

  } else {
    // Loop backwards until we find the start of the string
    // or a ',' that is outside of a comment
    //          and outside of quoted text before the leading '<'.
    bInComment = false;
    bInQuotesOutsideOfEmail = false;
    for ( i = iAd-1; 0 <= i; --i ) {
      c = aStr[i];
      if ( bInComment ) {
        if ( '(' == c ) {
          if ( !name.isEmpty() ) {
            name.prepend( ' ' );
          }
          bInComment = false;
        } else {
          name.prepend( c ); // all comment stuff is part of the name
        }
      } else if ( bInQuotesOutsideOfEmail ) {
        if ( cQuotes == c ) {
          bInQuotesOutsideOfEmail = false;
        } else if ( c != '\\' ) {
          name.prepend( c );
        }
      } else {
        // found the start of this addressee ?
        if ( ',' == c ) {
          break;
        }
        // stuff is before the leading '<' ?
        if ( iMailStart ) {
          if ( cQuotes == c ) {
            bInQuotesOutsideOfEmail = true; // end of quoted text found
          } else {
            name.prepend( c );
          }
        } else {
          switch ( c.toLatin1() ) {
          case '<':
            iMailStart = i;
            break;
          case ')':
            if ( !name.isEmpty() ) {
              name.prepend( ' ' );
            }
            bInComment = true;
            break;
          default:
            if ( ' ' != c ) {
              mail.prepend( c );
            }
          }
        }
      }
    }

    name = name.simplified();
    mail = mail.simplified();

    if ( mail.isEmpty() ) {
      return false;
    }

    mail.append( '@' );

    // Loop forward until we find the end of the string
    // or a ',' that is outside of a comment
    //          and outside of quoted text behind the trailing '>'.
    bInComment = false;
    bInQuotesOutsideOfEmail = false;
    int parenthesesNesting = 0;
    for ( i = iAd+1; len > i; ++i ) {
      c = aStr[i];
      if ( bInComment ) {
        if ( ')' == c ) {
          if ( --parenthesesNesting == 0 ) {
            bInComment = false;
            if ( !name.isEmpty() ) {
              name.append( ' ' );
            }
          } else {
            // nested ")", add it
            name.append( ')' ); // name can't be empty here
          }
        } else {
          if ( '(' == c ) {
            // nested "("
            ++parenthesesNesting;
          }
          name.append( c ); // all comment stuff is part of the name
        }
      } else if ( bInQuotesOutsideOfEmail ) {
        if ( cQuotes == c ) {
          bInQuotesOutsideOfEmail = false;
        } else if ( c != '\\' ) {
          name.append( c );
        }
      } else {
        // found the end of this addressee ?
        if ( ',' == c ) {
          break;
        }
        // stuff is behind the trailing '>' ?
        if ( iMailEnd ){
          if ( cQuotes == c ) {
            bInQuotesOutsideOfEmail = true; // start of quoted text found
          } else {
            name.append( c );
          }
        } else {
          switch ( c.toLatin1() ) {
          case '>':
            iMailEnd = i;
            break;
          case '(':
            if ( !name.isEmpty() ) {
              name.append( ' ' );
            }
            if ( ++parenthesesNesting > 0 ) {
              bInComment = true;
            }
            break;
          default:
            if ( ' ' != c ) {
              mail.append( c );
            }
          }
        }
      }
    }
  }

  name = name.simplified();
  mail = mail.simplified();

  return ! ( name.isEmpty() || mail.isEmpty() );
}

/** static */
Person::Ptr Person::fromFullName( const QString &fullName )
{
  QString email, name;
  extractEmailAddressAndName( fullName, email, name );
  return Person::Ptr( new Person( name, email ) );
}
