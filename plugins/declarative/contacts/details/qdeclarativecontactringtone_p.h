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

#ifndef QDECLARATIVECONTACTRINGTONE_H
#define QDECLARATIVECONTACTRINGTONE_H

#include <QUrl>

#include "qdeclarativecontactdetail_p.h"
#include "qcontactringtone.h"

class QDeclarativeContactRingtone : public QDeclarativeContactDetail
{
    Q_OBJECT

    Q_PROPERTY(QUrl audioRingtoneUrl READ audioRingtoneUrl WRITE setAudioRingtoneUrl NOTIFY fieldsChanged)
    Q_PROPERTY(QUrl videoRingtoneUrl READ videoRingtoneUrl WRITE setVideoRingtoneUrl NOTIFY fieldsChanged)
    Q_PROPERTY(QUrl vibrationRingtoneUrl READ vibrationRingtoneUrl WRITE setVibrationRingtoneUrl NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
    Q_CLASSINFO("DefaultProperty", "audioRingtoneUrl")
public:
    enum FieldType {
        AudioRingtoneUrl = 0,
        VideoRingtoneUrl,
        VibrationRingtoneUrl
    };

    QDeclarativeContactRingtone(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactRingtone());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Ringtone;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case AudioRingtoneUrl:
            return QContactRingtone::FieldAudioRingtoneUrl;
        case VideoRingtoneUrl:
            return QContactRingtone::FieldVideoRingtoneUrl;
        case VibrationRingtoneUrl:
            return QContactRingtone::FieldVibrationRingtoneUrl;
        default:
            break;
        }
        qmlInfo(0) << tr("Unknown field type.");
        return QString();
    }
    void setAudioRingtoneUrl(const QUrl& v)
    {
        if (!readOnly() && v != audioRingtoneUrl()) {
            detail().setValue(QContactRingtone::FieldAudioRingtoneUrl, v);
            emit fieldsChanged();
        }
    }
    QUrl audioRingtoneUrl() const {return detail().value(QContactRingtone::FieldAudioRingtoneUrl);}

    void setVideoRingtoneUrl(const QUrl& v)
    {
        if (!readOnly() && v != videoRingtoneUrl()) {
            detail().setValue(QContactRingtone::FieldVideoRingtoneUrl, v);
            emit fieldsChanged();
        }
    }
    QUrl videoRingtoneUrl() const {return detail().value(QContactRingtone::FieldVideoRingtoneUrl);}

    void setVibrationRingtoneUrl(const QUrl& v)
    {
        if (!readOnly() && v != vibrationRingtoneUrl()) {
            detail().setValue(QContactRingtone::FieldVibrationRingtoneUrl, v);
            emit fieldsChanged();
        }
    }
    QUrl vibrationRingtoneUrl() const {return detail().value(QContactRingtone::FieldVibrationRingtoneUrl);}
signals:
    void fieldsChanged();

};
QML_DECLARE_TYPE(QDeclarativeContactRingtone)
#endif
