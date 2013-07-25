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


#include "qhoversensorgesturerecognizer.h"
#include <math.h>

QTM_BEGIN_NAMESPACE

QHoverSensorGestureRecognizer::QHoverSensorGestureRecognizer(QObject *parent) :
    QSensorGestureRecognizer(parent),
    orientationReading(0),
    hoverOk(0), detecting(0), active(0), initialReflectance(0), useHack(0)
{
}

QHoverSensorGestureRecognizer::~QHoverSensorGestureRecognizer()
{
}

void QHoverSensorGestureRecognizer::create()
{
    timer2 = new QTimer(this);
    connect(timer2,SIGNAL(timeout()),this,SLOT(timeout2()));
    timer2->setSingleShot(true);
    timer2->setInterval(3000);
}

QString QHoverSensorGestureRecognizer::id() const
{
    return QString("QtSensors.hover");
}

bool QHoverSensorGestureRecognizer::start()
{
    if (QtSensorGestureSensorHandler::instance()->startSensor(QtSensorGestureSensorHandler::IrProximity)) {
        if (QtSensorGestureSensorHandler::instance()->startSensor(QtSensorGestureSensorHandler::Orientation)) {
            active = true;
            connect(QtSensorGestureSensorHandler::instance(),SIGNAL(irProximityReadingChanged(QIRProximityReading *)),
                    this,SLOT(irProximityReadingChanged(QIRProximityReading *)));
            connect(QtSensorGestureSensorHandler::instance(),SIGNAL(orientationReadingChanged(QOrientationReading *)),
                    this,SLOT(orientationReadingChanged(QOrientationReading *)));
        } else {
            QtSensorGestureSensorHandler::instance()->stopSensor(QtSensorGestureSensorHandler::IrProximity);
            active = false;
        }
    } else {
        active = false;
    }

    detecting = false;
    detectedHigh = 0;
    initialReflectance = 0;
    useHack = false;
    return active;
}

bool QHoverSensorGestureRecognizer::stop()
{
    QtSensorGestureSensorHandler::instance()->stopSensor(QtSensorGestureSensorHandler::IrProximity);
    QtSensorGestureSensorHandler::instance()->stopSensor(QtSensorGestureSensorHandler::Orientation);
    disconnect(QtSensorGestureSensorHandler::instance(),SIGNAL(irProximityReadingChanged(QProximityReading *)),
            this,SLOT(irProximityReadingChanged(QProximityReading *)));
    disconnect(QtSensorGestureSensorHandler::instance(),SIGNAL(orientationReadingChanged(QOrientationReading *)),
            this,SLOT(orientationReadingChanged(QOrientationReading *)));
    active = false;
    return active;
}

bool QHoverSensorGestureRecognizer::isActive()
{
    return active;
}


void QHoverSensorGestureRecognizer::orientationReadingChanged(QOrientationReading *reading)
{
    orientationReading = reading;
}

void QHoverSensorGestureRecognizer::irProximityReadingChanged(QProximityReading *reading)
{
//#warning FIXME  MSVC 2008 does not recognize this directive
//    reflectance = reading->reflectance();
//    if (reflectance == 0)
//        return;

//    if (initialReflectance == 0) {
//        initialReflectance = reflectance;
//    }

//    if (initialReflectance > .2) {
//        useHack = true;
//        initialReflectance -= .1;
//    }
//    if (useHack)
//        reflectance -= .1;

//    if (detecting && !hoverOk) {
//        detectedHigh = qMax(detectedHigh, reflectance);
//    }

//    if (reflectance > 0.4) {
//        // if close stop detecting
//        hoverOk = false;
//        detecting = false;
//        detectedHigh = 0;
//    }


//    qreal detectedPercent =  100 - (detectedHigh / reflectance * 100);

//    qint16 percentCheck;
//    if (useHack)
//        percentCheck = -60;
//    else
//        percentCheck = -101;

//    if (!detecting
//            && checkForHovering()) {
//        detecting = true;
//        detectedHigh = reflectance;
//    } else if (detecting
//                && detectedPercent < percentCheck
//               && !checkForHovering()) {
//        // went light again after 1 seconds
//        Q_EMIT hover();
//        Q_EMIT detected("hover");
//        hoverOk = false;
//        detecting = false;
//        detectedHigh = 0;
//        timer2->stop();
//    }
//    if (detecting && reflectance <  0.2) {
//        timeout();
//    }
}

bool QHoverSensorGestureRecognizer::checkForHovering()
{
    if (orientationReading == 0) {
        return false;
    }
    if (orientationReading->orientation() != QOrientationReading::FaceUp)
        return false;
    if ( (reflectance > 0.2 && reflectance < 0.4)
         && (initialReflectance - reflectance) < -0.1)
        return true;

    return false;
}


void QHoverSensorGestureRecognizer::timeout()
{
    if (checkForHovering()) {
        hoverOk = true;
        timer2->start();
    } else {
        detecting = false;
        detectedHigh = 0;
    }
}

void QHoverSensorGestureRecognizer::timeout2()
{
    detecting = false;
    hoverOk = false;
    detectedHigh = 0;
}

#include "moc_qhoversensorgesturerecognizer.cpp"

QTM_END_NAMESPACE
