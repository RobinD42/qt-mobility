/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qndefnfctextrecord.h>

#include <QtCore/QTextCodec>
#include <QtCore/QLocale>

QTM_BEGIN_NAMESPACE

/*!
    \class QNdefNfcTextRecord
    \brief The QNdefNfcTextRecord class provides an NFC RTD-Text

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
    \since 1.2

    RTD-Text encapsulates a user displayable text record.
*/

/*!
    \enum QNdefNfcTextRecord::Encoding

    This enum describes the text encoding standard used.

    \value Utf8     The text is encoded with UTF-8.
    \value Utf16    The text is encoding with UTF-16.
*/

/*!
    Returns the locale of the text record.
*/
QString QNdefNfcTextRecord::locale() const
{
    const QByteArray p = payload();

    if (p.isEmpty())
        return QString();

    quint8 status = p.at(0);

    quint8 codeLength = status & 0x3f;

    return QString::fromAscii(p.constData() + 1, codeLength);
}

/*!
    Sets the locale of the text record to \a locale.
*/
void QNdefNfcTextRecord::setLocale(const QString &locale)
{
    QByteArray p = payload();

    quint8 status = p.isEmpty() ? 0 : p.at(0);

    quint8 codeLength = status & 0x3f;

    quint8 newStatus = (status & 0xd0) | locale.length();

    p[0] = newStatus;
    p.replace(1, codeLength, locale.toAscii());

    setPayload(p);
}

/*!
    Returns the contents of the text record as a string.
*/
QString QNdefNfcTextRecord::text() const
{
    const QByteArray p = payload();

    if (p.isEmpty())
        return QString();

    quint8 status = p.at(0);

    bool utf16 = status & 0x80;
    quint8 codeLength = status & 0x3f;

    QTextCodec *codec = QTextCodec::codecForName(utf16 ? "UTF-16BE" : "UTF-8");

    return codec->toUnicode(p.constData() + 1 + codeLength, p.length() - 1 - codeLength);
}

/*!
    Sets the contents of the text record to \a text.
*/
void QNdefNfcTextRecord::setText(const QString text)
{
    if (payload().isEmpty())
        setLocale(QLocale().name());

    QByteArray p = payload();

    quint8 status = p.at(0);

    bool utf16 = status & 0x80;
    quint8 codeLength = status & 0x3f;

    p.truncate(1 + codeLength);

    QTextCodec *codec = QTextCodec::codecForName(utf16 ? "UTF-16BE" : "UTF-8");

    p += codec->fromUnicode(text);

    setPayload(p);
}

/*!
    Returns the encoding of the contents.
*/
QNdefNfcTextRecord::Encoding QNdefNfcTextRecord::encoding() const
{
    if (payload().isEmpty())
        return Utf8;

    QByteArray p = payload();

    quint8 status = p.at(0);

    bool utf16 = status & 0x80;

    if (utf16)
        return Utf16;
    else
        return Utf8;
}

/*!
    Sets the enconding of the contents to \a encoding.
*/
void QNdefNfcTextRecord::setEncoding(Encoding encoding)
{
    QByteArray p = payload();

    quint8 status = p.isEmpty() ? 0 : p.at(0);

    QString string = text();

    if (encoding == Utf8)
        status &= ~0x80;
    else
        status |= 0x80;

    p[0] = status;

    setPayload(p);

    setText(string);
}

QTM_END_NAMESPACE
