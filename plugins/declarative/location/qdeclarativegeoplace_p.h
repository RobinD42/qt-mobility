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
***************************************************************************/

#ifndef QDECLARATIVEGEOPLACE_P_H
#define QDECLARATIVEGEOPLACE_P_H

#include <qgeoplace.h>
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativegeoaddress_p.h"

#include <QVariant>
#include <QtDeclarative/qdeclarative.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoPlace : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeGeoBoundingBox* viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QDeclarativeGeoAddress* address READ address WRITE setAddress NOTIFY addressChanged)

public:
    explicit QDeclarativeGeoPlace(QObject* parent = 0);
    QDeclarativeGeoPlace(const QGeoPlace& place, QObject* parent);
    virtual ~QDeclarativeGeoPlace();
    void setPlace(const QGeoPlace& place);

    QDeclarativeGeoBoundingBox* viewport();
    void setViewport(QDeclarativeGeoBoundingBox* box);

    QDeclarativeCoordinate* coordinate();
    void setCoordinate(QDeclarativeCoordinate* coordinate);

    QDeclarativeGeoAddress* address();
    void setAddress(QDeclarativeGeoAddress* address);

signals:
    void viewportChanged();
    void coordinateChanged();
    void addressChanged();

private:
    QDeclarativeGeoBoundingBox m_declarativeBox;
    QDeclarativeCoordinate m_declarativeCoordinate;
    QDeclarativeGeoAddress m_declarativeAddress;
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGeoPlace))

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoPlace));

#endif // QDECLARATIVEGEOPLACE_P_H
