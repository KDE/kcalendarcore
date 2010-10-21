/*
  This file is part of the kcalcore library.

  Copyright (c) 2002 Michael Brade <brade@kde.org>

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
  defines the Attachment class.

  @author Michael Brade \<brade@kde.org\>
*/

#ifndef KCALCORE_ATTACHMENT_H
#define KCALCORE_ATTACHMENT_H

#include "kcalcore_export.h"

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>

namespace KCalCore {

/**
  @brief
  Represents information related to an attachment for a Calendar Incidence.

  This is not an email message attachment.

  Calendar Incidence attachments consist of:
  - A <a href="http://en.wikipedia.org/wiki/Uniform_Resource_Identifier">
    Uniform Resource Identifier (URI)</a>
    or a
    <a href="http://en.wikipedia.org/wiki/Base64#MIME">base64 encoded</a>
    binary blob.
  - A <a href="http://en.wikipedia.org/wiki/MIME">
    Multipurpose Internet Mail Extensions (MIME)</a> type.

  This class is used to associate files (local or remote) or other resources
  with a Calendar Incidence.
*/
class KCALCORE_EXPORT Attachment
{
  public:
    /**
      A shared pointer to an Attachment object.
    */
    typedef QSharedPointer<Attachment> Ptr;

    /**
      List of attachments.
    */
    typedef QVector<Ptr> List;

    /**
      Constructs an attachment consisting of a @p uri and a @p mime type.

      @param uri is the @acronym URI referred to by this attachment.
      @param mime is the (optional) @acronym MIME type of the @p uri
    */
    explicit Attachment( const QString &uri, const QString &mime = QString() );

    /**
      Constructs an attachment consisting of a binary blob of data
      and a @p mime type.

      @param base64 is the binary data in base64 format for the attachment.
      @param mime is the (optional) @acronym MIME type of the attachment
    */
    explicit Attachment( const QByteArray &base64,
                         const QString &mime = QString() );

    /**
      Constructs an attachment by copying another attachment.

      @param attachment is the attachment to be copied.
    */
    Attachment( const Attachment &attachment );

    /**
      Destroys the attachment.
    */
    ~Attachment();

    /**
      Sets the @acronym URI for this attachment to @p uri.

      @param uri is the @acronym URI to use for the attachment.

      @see uri(), isUri()
    */
    void setUri( const QString &uri );

    /**
      Returns the @acronym URI of the attachment.

      @see setUri(), isUri()
    */
    QString uri() const;

    /**
      Returns true if the attachment has a @acronym URI; false otherwise.

      @see uri(), setUri(I), isBinary()
    */
    bool isUri() const;

    /**
      Returns true if the attachment has a binary blob; false otherwise.

      @see isUri()
    */
    bool isBinary() const;

    /**
      Sets the base64 encoded binary blob data of the attachment.

      @param base64 contains the base64 encoded binary data.

      @see data(), decodedData()
    */
    void setData( const QByteArray &base64 );

    /**
      Returns a pointer to a QByteArray containing the base64 encoded
      binary data of the attachment.

      @see setData(), setDecodedData()
    */
    QByteArray data() const;

    /**
      Sets the decoded attachment data.

      @param data is the decoded base64 binary data.

      @see decodedData(), data()
    */
    void setDecodedData( const QByteArray &data );

    /**
      Returns a QByteArray containing the decoded base64 binary data of the
      attachment.

      @see setDecodedData(), setData()
    */
    QByteArray decodedData() const;

    /**
      Returns the size of the attachment, in bytes.
      If the attachment is binary (i.e, there is no @acronym URI associated
      with the attachment) then a value of 0 is returned.
    */
    uint size() const;

    /**
      Sets the @acronym MIME-type of the attachment to @p mime.

      @param mime is the string to use for the attachment @acronym MIME-type.

      @see mimeType()
    */
    void setMimeType( const QString &mime );

    /**
      Returns the @acronym MIME-type of the attachment.

      @see setMimeType()
    */
    QString mimeType() const;

    /**
      Sets the attachment "show in-line" option, which is derived from
      the Calendar Incidence @b X-CONTENT-DISPOSITION parameter.

      @param showinline is the flag to set (true) or unset (false)
      for the attachment "show in-line" option.

      @see showInline()
    */
    void setShowInline( bool showinline );

    /**
      Returns the attachment "show in-line" flag.

      @see setShowInline()
    */
    bool showInline() const;

    /**
      Sets the attachment label to @p label, which is derived from
      the Calendar Incidence @b X-LABEL parameter.

      @param label is the string to use for the attachment label.

      @see label()
    */
    void setLabel( const QString &label );

    /**
      Returns the attachment label string.
    */
    QString label() const;

    /**
      Sets the attachment "local" option, which is derived from the
      Calendar Incidence @b X-KONTACT-TYPE parameter.

      @param local is the flag to set (true) or unset (false) for the
      attachment "local" option.

      @see local()
    */
    void setLocal( bool local );

    /**
      Returns the attachment "local" flag.
    */
    bool isLocal() const;

    /**
       Assignment operator.
    */
    Attachment &operator=( const Attachment &attachment );

    /**
      Compare this with @p attachment for equality.
      @param attachment is the attachment to compare.
      @return true if the attachments are equal; false otherwise.
     */
    bool operator==( const Attachment &attachment ) const;

    /**
      Compare this with @p attachment for inequality.
      @param attachment is the attachment to compare.
      @return true if the attachments are /not/ equal; false otherwise.
     */
    bool operator!=( const Attachment &attachment ) const;

  private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond
};

}

Q_DECLARE_TYPEINFO( KCalCore::Attachment::Ptr, Q_MOVABLE_TYPE );

//@cond PRIVATE
inline uint qHash( const QSharedPointer<KCalCore::Attachment> &key )
{
  return qHash<KCalCore::Attachment>( key.data() );
}
//@endcond

#endif
