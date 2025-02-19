/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2002 Michael Brade <brade@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the Attachment class.
*/

#ifndef KCALCORE_ATTACHMENT_H
#define KCALCORE_ATTACHMENT_H

#include "kcalendarcore_export.h"

#include <QHash>
#include <QMetaType>
#include <QSharedDataPointer>
#include <QString>

namespace KCalendarCore
{
/*!
  \qmlvaluetype attachment
  \inqmlmodule org.kde.kcalendarcore
  \nativetype KCalendarCore::Attachment
  \brief
  Represents information related to an attachment for a Calendar Incidence.

  This is not an email message attachment.

  Calendar Incidence attachments consist of:
  \list
     \li A \l {https://en.wikipedia.org/wiki/Uniform_Resource_Identifier}
         {Uniform Resource Identifier (URI)}
         or a
         \l {https://en.wikipedia.org/wiki/Base64#MIME} {base64 encoded}
         binary blob.
     \li A \l {https://en.wikipedia.org/wiki/MIME}
         {Multipurpose Internet Mail Extensions (MIME)} type.
  \endlist

  This class is used to associate files (local or remote) or other resources
  with a Calendar Incidence.
*/

/*!
  \class KCalendarCore::Attachment
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/Attachment
  \brief
  Represents information related to an attachment for a Calendar Incidence.

  This is not an email message attachment.

  Calendar Incidence attachments consist of:
  \list
     \li A \l {https://en.wikipedia.org/wiki/Uniform_Resource_Identifier}
         {Uniform Resource Identifier (URI)}
         or a
         \l {https://en.wikipedia.org/wiki/Base64#MIME} {base64 encoded}
         binary blob.
     \li A \l {https://en.wikipedia.org/wiki/MIME}
         {Multipurpose Internet Mail Extensions (MIME)} type.
  \endlist

  This class is used to associate files (local or remote) or other resources
  with a Calendar Incidence.
*/
class KCALENDARCORE_EXPORT Attachment
{
    Q_GADGET

    /*!
     * \qmlproperty bool attachment::isEmpty
     */

    /*!
     * \property KCalendarCore::Attachment::isEmpty
     */
    Q_PROPERTY(bool isEmpty READ isEmpty)

    /*!
     * \qmlproperty string attachment::uri
     */

    /*!
     * \property KCalendarCore::Attachment::uri
     */
    Q_PROPERTY(QString uri READ uri WRITE setUri)

    /*!
     * \qmlproperty bool attachment::isUri
     */

    /*!
     * \property KCalendarCore::Attachment::isUri
     */
    Q_PROPERTY(bool isUri READ isUri)

    /*!
     * \qmlproperty bool attachment::isBinary
     */

    /*!
     * \property KCalendarCore::Attachment::isBinary
     */
    Q_PROPERTY(bool isBinary READ isBinary)

    /*!
     * \qmlproperty int attachment::size
     */

    /*!
     * \property KCalendarCore::Attachment::size
     */
    Q_PROPERTY(int size READ size)

    /*!
     * \qmlproperty string attachment::mimeType
     */

    /*!
     * \property KCalendarCore::Attachment::mimeType
     */
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType)

    /*!
     * \qmlproperty bool attachment::showInline
     */

    /*!
     * \property KCalendarCore::Attachment::showInline
     */
    Q_PROPERTY(bool showInline READ showInline WRITE setShowInline)

    /*!
     * \qmlproperty string attachment::label
     */

    /*!
     * \property KCalendarCore::Attachment::label
     */
    Q_PROPERTY(QString label READ label WRITE setLabel)

    /*!
     * \qmlproperty bool attachment::isLocal
     */

    /*!
     * \property KCalendarCore::Attachment::isLocal
     */
    Q_PROPERTY(bool isLocal READ isLocal WRITE setLocal)

public:
    /*!
      List of attachments.
    */
    typedef QList<Attachment> List;

    /*!
      Constructs an empty attachment.
    */
    explicit Attachment();

    /*!
      Constructs an attachment consisting of a \a uri and a \a mime type.

      \a uri is the URI referred to by this attachment.

      \a mime is the (optional) MIME type of the \a uri

    */
    explicit Attachment(const QString &uri, const QString &mime = QString());

    /*!
      Constructs an attachment consisting of a binary blob of data
      and a \a mime type.

      \a base64 is the binary data in base64 format for the attachment.

      \a mime is the (optional) MIME type of the attachment

    */
    explicit Attachment(const QByteArray &base64, const QString &mime = QString());

    /*!
      Constructs an attachment by copying another attachment.

      \a attachment is the attachment to be copied.

    */
    Attachment(const Attachment &attachment);

    /*!
      Destroys the attachment.
    */
    ~Attachment();

    /*!
       Returns whether this is an empty or default constructed object.
    */
    bool isEmpty() const;

    /*!
      Sets the URI for this attachment to \a uri.

      \a uri is the URI to use for the attachment.

      \sa uri(), isUri()
    */
    void setUri(const QString &uri);

    /*!
      Returns the URI of the attachment.

      \sa setUri(), isUri()
    */
    Q_REQUIRED_RESULT QString uri() const;

    /*!
      Returns true if the attachment has a URI; false otherwise.

      \sa uri(), setUri(), isBinary()
    */
    Q_REQUIRED_RESULT bool isUri() const;

    /*!
      Returns true if the attachment has a binary blob; false otherwise.

      \sa isUri()
    */
    Q_REQUIRED_RESULT bool isBinary() const;

    /*!
      Sets the base64 encoded binary blob data of the attachment.

      \a base64 contains the base64 encoded binary data.

      \sa data(), decodedData()
    */
    void setData(const QByteArray &base64);

    /*!
      Returns a pointer to a QByteArray containing the base64 encoded
      binary data of the attachment.

      \sa setData(), setDecodedData()
    */
    Q_REQUIRED_RESULT QByteArray data() const;

    /*!
      Sets the decoded attachment data.

      \a data is the decoded base64 binary data.

      \sa decodedData(), data()
    */
    void setDecodedData(const QByteArray &data);

    /*!
      Returns a QByteArray containing the decoded base64 binary data of the
      attachment.

      \sa setDecodedData(), setData()
    */
    Q_REQUIRED_RESULT QByteArray decodedData() const;

    /*!
      Returns the size of the attachment, in bytes.
      If the attachment is binary (i.e, there is no URI associated
      with the attachment) then a value of 0 is returned.
    */
    uint size() const;

    /*!
      Sets the MIME-type of the attachment to \a mime.

      \a mime is the string to use for the attachment MIME-type.

      \sa mimeType()
    */
    void setMimeType(const QString &mime);

    /*!
      Returns the MIME-type of the attachment.

      \sa setMimeType()
    */
    Q_REQUIRED_RESULT QString mimeType() const;

    /*!
      Sets the attachment "show in-line" option, which is derived from
      the Calendar Incidence \c X-CONTENT-DISPOSITION parameter.

      \a showinline is the flag to set (true) or unset (false)
      for the attachment "show in-line" option.

      \sa showInline()
    */
    void setShowInline(bool showinline);

    /*!
      Returns the attachment "show in-line" flag.

      \sa setShowInline()
    */
    Q_REQUIRED_RESULT bool showInline() const;

    /*!
      Sets the attachment label to \a label, which is derived from
      the Calendar Incidence \c X-LABEL parameter.

      \a label is the string to use for the attachment label.

      \sa label()
    */
    void setLabel(const QString &label);

    /*!
      Returns the attachment label string.
    */
    Q_REQUIRED_RESULT QString label() const;

    /*!
      Sets the attachment "local" option, which is derived from the
      Calendar Incidence \c X-KONTACT-TYPE parameter.

      \a local is the flag to set (true) or unset (false) for the
      attachment "local" option.

      \sa isLocal()
    */
    void setLocal(bool local);

    /*!
      Returns the attachment "local" flag.
    */
    Q_REQUIRED_RESULT bool isLocal() const;

    /*!
      Assignment operator.

      \a attachment is the attachment to assign.

    */
    Attachment &operator=(const Attachment &attachment);

    /*!
      Compare this with \a attachment for equality.

      \a attachment is the attachment to compare.

      Returns true if the attachments are equal; false otherwise.
     */
    bool operator==(const Attachment &attachment) const;

    /*!
      Compare this with \a attachment for inequality.

      \a attachment is the attachment to compare.

      Returns true if the attachments are /not/ equal; false otherwise.
     */
    bool operator!=(const Attachment &attachment) const;

private:
    //@cond PRIVATE
    class Private;
    QSharedDataPointer<Private> d;
    //@endcond

    friend KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &s, const KCalendarCore::Attachment &);
    friend KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &s, KCalendarCore::Attachment &);
};

/*!
 * Attachment serializer.
 *
 * \since 4.12
 *
 * \a out is the output data stream
 *
 */
KCALENDARCORE_EXPORT QDataStream &operator<<(QDataStream &out, const KCalendarCore::Attachment &);

/*!
 * Attachment deserializer.
 *
 * \since 4.12
 *
 * \a in is the input data stream
 *
 */
KCALENDARCORE_EXPORT QDataStream &operator>>(QDataStream &in, KCalendarCore::Attachment &);

}

//@cond PRIVATE
Q_DECLARE_TYPEINFO(KCalendarCore::Attachment, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(KCalendarCore::Attachment)
//@endcond

#endif
