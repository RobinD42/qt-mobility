/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tst_qvcard21writer.h"
#include "qvcard21writer_p.h"
#include "qversitproperty.h"
#include "qversitdocument.h"
#include <QtTest/QtTest>
#include <QByteArray>
#include <QVariant>

// This says "NOKIA" in Katakana
const QString KATAKANA_NOKIA(QString::fromUtf8("\xe3\x83\x8e\xe3\x82\xad\xe3\x82\xa2"));

QTM_USE_NAMESPACE

void tst_QVCard21Writer::init()
{
    mWriter = new QVCard21Writer;
    mWriter->setCodec(QTextCodec::codecForName("ISO_8859-1"));
}

void tst_QVCard21Writer::cleanup()
{
    delete mWriter;
}

void tst_QVCard21Writer::testEncodeVersitProperty()
{
    QByteArray encodedProperty;
    QBuffer buffer(&encodedProperty);
    mWriter->setDevice(&buffer);
    buffer.open(QIODevice::WriteOnly);

    // No parameters
    QByteArray expectedResult = "FN:John Citizen\r\n";
    QVersitProperty property;
    property.setName(QString::fromAscii("FN"));
    property.setValue(QString::fromAscii("John Citizen"));
    mWriter->encodeVersitProperty(property);
    QCOMPARE(encodedProperty, expectedResult);

    // With parameter(s). No special characters in the value.
    // -> No need to Quoted-Printable encode the value.
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedProperty.clear();
    buffer.open(QIODevice::WriteOnly);
    expectedResult = "TEL;HOME:123\r\n";
    property.setName(QString::fromAscii("TEL"));
    property.setValue(QString::fromAscii("123"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    mWriter->encodeVersitProperty(property);
    QCOMPARE(encodedProperty, expectedResult);

    // With parameter(s). Special characters in the value.
    // -> The value needs to be Quoted-Printable encoded.
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedProperty.clear();
    buffer.open(QIODevice::WriteOnly);
    expectedResult = "EMAIL;HOME;ENCODING=QUOTED-PRINTABLE:john.citizen=40example.com\r\n";
    property.setName(QString::fromAscii("EMAIL"));
    property.setValue(QString::fromAscii("john.citizen@example.com"));
    mWriter->encodeVersitProperty(property);
    QCOMPARE(encodedProperty, expectedResult);

    // AGENT property with parameter
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedProperty.clear();
    buffer.open(QIODevice::WriteOnly);
    expectedResult =
"AGENT;X-PARAMETER=VALUE:\r\n\
BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
FN:Secret Agent\r\n\
END:VCARD\r\n\
\r\n";
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("AGENT"));
    property.setValue(QString());
    property.insertParameter(QString::fromAscii("X-PARAMETER"),QString::fromAscii("VALUE"));
    QVersitDocument document;
    QVersitProperty embeddedProperty;
    embeddedProperty.setName(QString(QString::fromAscii("FN")));
    embeddedProperty.setValue(QString::fromAscii("Secret Agent"));
    document.addProperty(embeddedProperty);
    property.setValue(QVariant::fromValue(document));
    mWriter->encodeVersitProperty(property);
    QCOMPARE(encodedProperty, expectedResult);

    // Value is base64 encoded.
    // Check that the extra folding and the line break are added
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedProperty.clear();
    buffer.open(QIODevice::WriteOnly);
    QByteArray value("value");
    expectedResult = "Springfield.HOUSE.PHOTO;ENCODING=BASE64:\r\n " + value.toBase64() + "\r\n\r\n";
    QStringList groups(QString::fromAscii("Springfield"));
    groups.append(QString::fromAscii("HOUSE"));
    property.setGroups(groups);
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("PHOTO"));
    property.setValue(value);
    mWriter->encodeVersitProperty(property);
    QCOMPARE(encodedProperty, expectedResult);

    // Characters other than ASCII:
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedProperty.clear();
    buffer.open(QIODevice::WriteOnly);
    expectedResult = "ORG;CHARSET=UTF-8:" + KATAKANA_NOKIA.toUtf8() + "\r\n";
    property = QVersitProperty();
    property.setName(QLatin1String("ORG"));
    property.setValue(KATAKANA_NOKIA);
    mWriter->encodeVersitProperty(property);
    QCOMPARE(encodedProperty, expectedResult);

    // In Shift-JIS codec.
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedProperty.clear();
    buffer.open(QIODevice::WriteOnly);
    QTextCodec* jisCodec = QTextCodec::codecForName("Shift-JIS");
    expectedResult = jisCodec->fromUnicode(
            QLatin1String("ORG:") + KATAKANA_NOKIA + QLatin1String("\r\n"));
    property = QVersitProperty();
    property.setName(QLatin1String("ORG"));
    property.setValue(KATAKANA_NOKIA);
    mWriter->setCodec(jisCodec);
    mWriter->encodeVersitProperty(property);
    QCOMPARE(encodedProperty, expectedResult);

    // CHARSET and QUOTED-PRINTABLE
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedProperty.clear();
    buffer.open(QIODevice::WriteOnly);
    expectedResult = "EMAIL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:john=40"
                     + KATAKANA_NOKIA.toUtf8() + ".com\r\n";
    property = QVersitProperty();
    property.setName(QLatin1String("EMAIL"));
    property.setValue(QString::fromAscii("john@%1.com").arg(KATAKANA_NOKIA));
    mWriter->setCodec(QTextCodec::codecForName("ISO_8859-1"));
    mWriter->encodeVersitProperty(property);
    QCOMPARE(encodedProperty, expectedResult);
}

void tst_QVCard21Writer::testEncodeParameters()
{
    QByteArray encodedParameters;
    QBuffer buffer(&encodedParameters);
    mWriter->setDevice(&buffer);
    buffer.open(QIODevice::WriteOnly);

    QString typeParameterName(QString::fromAscii("TYPE"));
    QString encodingParameterName(QString::fromAscii("ENCODING"));

    // No parameters
    QMultiHash<QString,QString> parameters;
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(""));

    // One TYPE parameter
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedParameters.clear();
    buffer.open(QIODevice::WriteOnly);
    parameters.insert(typeParameterName,QString::fromAscii("HOME"));
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(";HOME"));

    // Two TYPE parameters
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedParameters.clear();
    buffer.open(QIODevice::WriteOnly);
    parameters.insert(typeParameterName,QString::fromAscii("VOICE"));
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(";VOICE;HOME"));

    // One ENCODING parameter
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedParameters.clear();
    buffer.open(QIODevice::WriteOnly);
    parameters.clear();
    parameters.insert(encodingParameterName,QString::fromAscii("8BIT"));
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(";ENCODING=8BIT"));

    // Two parameters
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedParameters.clear();
    buffer.open(QIODevice::WriteOnly);
    parameters.insert(QString::fromAscii("X-PARAM"),QString::fromAscii("VALUE"));
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(";X-PARAM=VALUE;ENCODING=8BIT"));
}

void tst_QVCard21Writer::testEncodeGroupsAndName()
{
    QVersitProperty property;
    QByteArray result;
    QBuffer buffer(&result);
    mWriter->setDevice(&buffer);
    buffer.open(QIODevice::WriteOnly);

    // No groups

    property.setName(QString::fromAscii("name"));
    QByteArray expected("NAME");
    mWriter->encodeGroupsAndName(property);
    QCOMPARE(result, expected);

    // One group
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    result.clear();
    buffer.open(QIODevice::WriteOnly);
    property.setGroups(QStringList(QString::fromAscii("group")));
    expected = "group.NAME";
    mWriter->encodeGroupsAndName(property);
    QCOMPARE(result, expected);

    // Two groups
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    result.clear();
    buffer.open(QIODevice::WriteOnly);
    QStringList groups(QString::fromAscii("group1"));
    groups.append(QString::fromAscii("group2"));
    property.setGroups(groups);
    expected = "group1.group2.NAME";
    mWriter->encodeGroupsAndName(property);
    QCOMPARE(result, expected);
}


void tst_QVCard21Writer::testQuotedPrintableEncode()
{
    QByteArray encodedBytes;

    // Nothing to encode
    QString nothingToEncode(QLatin1String("nothing to encode"));
    QVERIFY(!mWriter->quotedPrintableEncode(nothingToEncode));

    // Special characters
    QString inputOutput(QLatin1String("\n"));
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=0A"));
    inputOutput = QLatin1String("\r");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=0D"));
    inputOutput = QLatin1String("!");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=21"));
    inputOutput = QLatin1String("\"");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=22"));
    inputOutput = QLatin1String("#");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=23"));
    inputOutput = QLatin1String("$");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=24"));
    inputOutput = QLatin1String("=");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=3D"));
    inputOutput = QLatin1String("@");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=40"));
    inputOutput = QLatin1String("[");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5B"));
    inputOutput = QLatin1String("\\");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5C"));
    inputOutput = QLatin1String("]");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5D"));
    inputOutput = QLatin1String("^");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5E"));
    inputOutput = QLatin1String("`");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=60"));
    inputOutput = QLatin1String("{");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7B"));
    inputOutput = QLatin1String("|");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7C"));
    inputOutput = QLatin1String("}");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7D"));
    inputOutput = QLatin1String("~");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7E"));
}

QTEST_MAIN(tst_QVCard21Writer)
