/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarative.h>

#include <qaccelerometer.h>
#include <qaltimeter.h>
#include <qambientlightsensor.h>
#include <qambienttemperaturesensor.h>
#include <qcompass.h>
#include <qmagnetometer.h>
#include <qorientationsensor.h>
#include <qirproximitysensor.h>
#include <qproximitysensor.h>
#include <qrotationsensor.h>
#include <qtapsensor.h>
#include <qlightsensor.h>
#include <qgyroscope.h>
#include "qmlsensorgesture.h"

#include <qpressuresensor.h>
#include <qholstersensor.h>

QT_BEGIN_NAMESPACE

QTM_USE_NAMESPACE

class QSensorsDeclarativeModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri)
    {
        char const * const package = "QtMobility.sensors";
        if (QLatin1String(uri) != QLatin1String(package)) return;

        // Register the 1.1 interfaces
        int major = 1;
        int minor = 1;
        qmlRegisterUncreatableType<QSensorReading       >(package, major, minor, "SensorReading",        QLatin1String("Cannot create SensorReading"));
        qmlRegisterType           <QAccelerometer       >(package, major, minor, "Accelerometer");
        qmlRegisterType           <QAccelerometerReading>(package, major, minor, "AccelerometerReading");
        qmlRegisterType           <QAmbientLightSensor  >(package, major, minor, "AmbientLightSensor");
        qmlRegisterType           <QAmbientLightReading >(package, major, minor, "AmbientLightReading");
        qmlRegisterType           <QCompass             >(package, major, minor, "Compass");
        qmlRegisterType           <QCompassReading      >(package, major, minor, "CompassReading");
        qmlRegisterType           <QMagnetometer        >(package, major, minor, "Magnetometer");
        qmlRegisterType           <QMagnetometerReading >(package, major, minor, "MagnetometerReading");
        qmlRegisterType           <QOrientationSensor   >(package, major, minor, "OrientationSensor");
        qmlRegisterType           <QOrientationReading  >(package, major, minor, "OrientationReading");
        qmlRegisterType           <QProximitySensor     >(package, major, minor, "ProximitySensor");
        qmlRegisterType           <QProximityReading    >(package, major, minor, "ProximityReading");
        qmlRegisterType           <QRotationSensor      >(package, major, minor, "RotationSensor");
        qmlRegisterType           <QRotationReading     >(package, major, minor, "RotationReading");
        qmlRegisterType           <QTapSensor           >(package, major, minor, "TapSensor");
        qmlRegisterType           <QTapReading          >(package, major, minor, "TapReading");

        // Register the 1.2 interfaces
        major = 1;
        minor = 2;
        qmlRegisterUncreatableType<QSensor              >(package, major, minor, "Sensor",               QLatin1String("Cannot create Sensor"));
        qmlRegisterUncreatableType<QSensorReading       >(package, major, minor, "SensorReading",        QLatin1String("Cannot create SensorReading"));
        qmlRegisterType           <QAccelerometer       >(package, major, minor, "Accelerometer");
        qmlRegisterUncreatableType<QAccelerometerReading>(package, major, minor, "AccelerometerReading", QLatin1String("Cannot create AccelerometerReading"));
        qmlRegisterType           <QAmbientLightSensor  >(package, major, minor, "AmbientLightSensor");
        qmlRegisterUncreatableType<QAmbientLightReading >(package, major, minor, "AmbientLightReading",  QLatin1String("Cannot create AmbientLightReading"));
        qmlRegisterType           <QCompass             >(package, major, minor, "Compass");
        qmlRegisterUncreatableType<QCompassReading      >(package, major, minor, "CompassReading",       QLatin1String("Cannot create CompassReading"));
        qmlRegisterType           <QMagnetometer        >(package, major, minor, "Magnetometer");
        qmlRegisterUncreatableType<QMagnetometerReading >(package, major, minor, "MagnetometerReading",  QLatin1String("Cannot create MagnetometerReading"));
        qmlRegisterType           <QOrientationSensor   >(package, major, minor, "OrientationSensor");
        qmlRegisterUncreatableType<QOrientationReading  >(package, major, minor, "OrientationReading",   QLatin1String("Cannot create OrientationReading"));
        qmlRegisterType           <QProximitySensor     >(package, major, minor, "ProximitySensor");
        qmlRegisterUncreatableType<QProximityReading    >(package, major, minor, "ProximityReading",     QLatin1String("Cannot create ProximityReading"));
        qmlRegisterType           <QRotationSensor      >(package, major, minor, "RotationSensor");
        qmlRegisterUncreatableType<QRotationReading     >(package, major, minor, "RotationReading",      QLatin1String("Cannot create RotationReading"));
        qmlRegisterType           <QTapSensor           >(package, major, minor, "TapSensor");
        qmlRegisterUncreatableType<QTapReading          >(package, major, minor, "TapReading",           QLatin1String("Cannot create TapReading"));
        qmlRegisterType           <QLightSensor         >(package, major, minor, "LightSensor");
        qmlRegisterUncreatableType<QLightReading        >(package, major, minor, "LightReading",         QLatin1String("Cannot create LightReading"));
        qmlRegisterType           <QGyroscope           >(package, major, minor, "Gyroscope");
        qmlRegisterUncreatableType<QGyroscopeReading    >(package, major, minor, "GyroscopeReading",     QLatin1String("Cannot create GyroscopeReading"));

        major = 1;
        minor = 3;
        qmlRegisterUncreatableType<QSensor              >(package, major, minor, "Sensor",               QLatin1String("Cannot create Sensor"));
        qmlRegisterUncreatableType<QSensorReading       >(package, major, minor, "SensorReading",        QLatin1String("Cannot create SensorReading"));
        qmlRegisterType           <QAccelerometer       >(package, major, minor, "Accelerometer");
        qmlRegisterUncreatableType<QAccelerometerReading>(package, major, minor, "AccelerometerReading", QLatin1String("Cannot create AccelerometerReading"));
        qmlRegisterType           <QAltimeter           >(package, major, minor, "Altimeter");
        qmlRegisterUncreatableType<QAltimeterReading    >(package, major, minor, "AltimeterReading",     QLatin1String("Cannot create AltimeterReading"));
        qmlRegisterType           <QAmbientLightSensor  >(package, major, minor, "AmbientLightSensor");
        qmlRegisterUncreatableType<QAmbientLightReading >(package, major, minor, "AmbientLightReading",  QLatin1String("Cannot create AmbientLightReading"));
        qmlRegisterType           <QAmbientTemperatureSensor  >(package, major, minor, "AmbientTemperatureSensor");
        qmlRegisterUncreatableType<QAmbientTemperatureReading >(package, major, minor, "AmbientTemperatureReading", QLatin1String("Cannot create AmbientTemperatureReading"));
        qmlRegisterType           <QCompass             >(package, major, minor, "Compass");
        qmlRegisterUncreatableType<QCompassReading      >(package, major, minor, "CompassReading",       QLatin1String("Cannot create CompassReading"));
        qmlRegisterType           <QIRProximitySensor   >(package, major, minor, "IRProximitySensor");
        qmlRegisterUncreatableType<QIRProximityReading  >(package, major, minor, "IRProximityReading",     QLatin1String("Cannot create IRProximityReading"));
        qmlRegisterType           <QMagnetometer        >(package, major, minor, "Magnetometer");
        qmlRegisterUncreatableType<QMagnetometerReading >(package, major, minor, "MagnetometerReading",  QLatin1String("Cannot create MagnetometerReading"));
        qmlRegisterType           <QOrientationSensor   >(package, major, minor, "OrientationSensor");
        qmlRegisterUncreatableType<QOrientationReading  >(package, major, minor, "OrientationReading",   QLatin1String("Cannot create OrientationReading"));
        qmlRegisterType           <QProximitySensor     >(package, major, minor, "ProximitySensor");
        qmlRegisterUncreatableType<QProximityReading    >(package, major, minor, "ProximityReading",     QLatin1String("Cannot create ProximityReading"));
        qmlRegisterType           <QRotationSensor      >(package, major, minor, "RotationSensor");
        qmlRegisterUncreatableType<QRotationReading     >(package, major, minor, "RotationReading",      QLatin1String("Cannot create RotationReading"));
        qmlRegisterType           <QTapSensor           >(package, major, minor, "TapSensor");
        qmlRegisterUncreatableType<QTapReading          >(package, major, minor, "TapReading",           QLatin1String("Cannot create TapReading"));
        qmlRegisterType           <QLightSensor         >(package, major, minor, "LightSensor");
        qmlRegisterUncreatableType<QLightReading        >(package, major, minor, "LightReading",         QLatin1String("Cannot create LightReading"));
        qmlRegisterType           <QGyroscope           >(package, major, minor, "Gyroscope");
        qmlRegisterUncreatableType<QGyroscopeReading    >(package, major, minor, "GyroscopeReading",     QLatin1String("Cannot create GyroscopeReading"));

        qmlRegisterType           <QmlSensorGesture     >(package, major, minor, "SensorGesture");
        qmlRegisterType           <QHolsterSensor       >(package, major, minor, "HolsterSensor");
        qmlRegisterUncreatableType<QHolsterReading      >(package, major, minor, "HolsterReading",       QLatin1String("Cannot create HolsterReading"));
        qmlRegisterType           <QPressureSensor      >(package, major, minor, "PressureSensor");
        qmlRegisterUncreatableType<QPressureReading     >(package, major, minor, "PressureReading",      QLatin1String("Cannot create PressureReading"));
    }
};

QT_END_NAMESPACE

#include "sensors.moc"

Q_EXPORT_PLUGIN2(qsensorsdeclarativemodule, QT_PREPEND_NAMESPACE(QSensorsDeclarativeModule));

// =====================================================================

/*!
    \qmlclass Sensor QSensor
    \ingroup qml-sensors_type
    \since Mobility 1.2
    \brief The Sensor element serves as a base type for sensors.

    The Sensor element serves as a base type for sensors.

    This element wraps the QSensor class. Please see the documentation for
    QSensor for details.

    This element cannot be directly created. Please use one of the sub-classes instead.

    \sa {Sensors QML Limitations}
*/

/*!
    \qmlproperty int Sensor::dataRate
    \since Mobility 1.2
    Sets the rate at which data is returned by the sensor.
    Please see QSensor::dataRate for information about this property.
*/

/*!
    \qmlproperty bool Sensor::active
    \since Mobility 1.2
    Starts or stops the sensor.
    Please see QSensor::active for information about this property.
*/

/*!
    \qmlproperty bool Sensor::skipDuplicates
    Omitts readings with the same or very similar values.
    \sa QSensor::skipDuplicates
    \since Mobility 1.3
*/

/*!
    \qmlproperty SensorReading Sensor::reading
    \since Mobility 1.2
    Holds the most recent sensor reading.
    Please see QSensor::reading for information about this property.
*/

/*!
    \qmlsignal Sensor::onReadingChanged()
    \since Mobility 1.2
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
*/

/*!
    \qmlclass SensorReading QSensorReading
    \ingroup qml-sensors_reading
    \since Mobility 1.2
    \brief The SensorReading element serves as a base type for sensor readings.

    The SensorReading element serves as a base type for sensor readings.

    This element wraps the QSensorReading class. Please see the documentation for
    QSensorReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty quint64 SensorReading::timestamp
    \since Mobility 1.2
    A timestamp for the reading.
    Please see QSensorReading::timestamp for information about this property.
*/

/*!
    \qmlclass Accelerometer QAccelerometer
    \ingroup qml-sensors_type
    \since Mobility 1.1
    \inherits Sensor
    \brief The Accelerometer element reports on linear acceleration
           along the X, Y and Z axes.

    The Accelerometer element reports on linear acceleration
    along the X, Y and Z axes.

    This element wraps the QAccelerometer class. Please see the documentation for
    QAccelerometer for details.

    \sa {Sensors QML Limitations}
*/

#if defined(Q_QDOC) && defined(Q_OS_BLACKBERRY)
/*!
    \qmlproperty AxesOrientationMode Accelerometer::axesOrientationMode
    Holds the mode that affects how the screen orientation changes reading values.
    \sa QOrientableSensorBase::axesOrientationMode
    \since Mobility 1.3
*/

/*!
    \qmlproperty int Accelerometer::currentOrientation
    Holds the current orientation that is used for rotating the reading values.
    \sa QOrientableSensorBase::currentOrientation
    \since Mobility 1.3
*/

/*!
    \qmlproperty int Accelerometer::userOrientation
    Holds the angle used for rotating the reading values in the UserOrientation mode.
    \sa QOrientableSensorBase::userOrientation
    \since Mobility 1.3
*/
#endif

/*!
    \qmlproperty AccelerometerReading Accelerometer::reading
    Holds the most recent accelerometer reading.
    Please see QSensor::reading for information about this property.
    \since Mobility 1.2
    */

/*!
    \qmlsignal Accelerometer::onReadingChanged()
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
    \since Mobility 1.2
    */

/*!
    \qmlclass AccelerometerReading QAccelerometerReading
    \ingroup qml-sensors_reading
    \since Mobility 1.1
    \inherits SensorReading
    \brief The AccelerometerReading element holds the most recent Accelerometer reading.

    The AccelerometerReading element holds the most recent Accelerometer reading.

    This element wraps the QAccelerometerReading class. Please see the documentation for
    QAccelerometerReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty qreal AccelerometerReading::x
    Please see QAccelerometerReading::x for information about this property.
    \since Mobility 1.2
    */

/*!
    \qmlproperty qreal AccelerometerReading::y
    Please see QAccelerometerReading::y for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlproperty qreal AccelerometerReading::z
    Please see QAccelerometerReading::z for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlclass Altimeter QAltimeter
    \ingroup qml-sensors_type
    \since Mobility 1.3
    \inherits Sensor
    \brief The Altimeter element reports on altitude.

    The Altimeter element reports on altitude.

    This element wraps the QAltimeter class. Please see the documentation for
    QAltimeter for details.

    \sa AltimeterReading
*/

/*!
    \qmlclass AltimeterReading QAltimeterReading
    \ingroup qml-sensors_reading
    \since Mobility 1.3
    \inherits SensorReading
    \brief The AltimeterReading element holds the most recent Altimeter reading.

    The AltimeterReading element holds the most recent Altimeter reading.

    This element wraps the QAltimeterReading class. Please see the documentation for
    QAltimeterReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty qreal AltimeterReading::altitude
    This property holds the altitude of the device.

    Please see QAltimeterReading::altitude for information about this property.

    \since Mobility 1.3
*/

/*!
    \qmlclass AmbientLightSensor QAmbientLightSensor
    \ingroup qml-sensors_type
    \since Mobility 1.1
    \inherits Sensor
    \brief The AmbientLightSensor element repors on ambient lighting conditions.

    The AmbientLightSensor element repors on ambient lighting conditions.

    This element wraps the QAmbientLightSensor class. Please see the documentation for
    QAmbientLightSensor for details.

    \sa {Sensors QML Limitations}
*/

/*!
    \qmlproperty AmbientLightReading AmbientLightSensor::reading
    Holds the most recent ambient light reading.
    Please see QSensor::reading for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlsignal AmbientLightSensor::onReadingChanged()
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
    \since Mobility 1.2
*/

/*!
    \qmlclass AmbientLightReading QAmbientLightReading
    \ingroup qml-sensors_reading
    \since Mobility 1.1
    \inherits SensorReading
    \brief The AmbientLightReading element holds the most AmbientLightSensor reading.

    The AmbientLightReading element holds the most AmbientLightSensor reading.

    This element wraps the QAmbientLightReading class. Please see the documentation for
    QAmbientLightReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty QAmbientLightReading::LightLevel AmbientLightReading::lightLevel
    Please see QAmbientLightReading::lightLevel for information about this property.

    LightLevel constants are exposed through the AmbientLightReading class.
    \code
        AmbientLightSensor {
            onReadingChanged: {
                if (reading.lightLevel == AmbientLightReading.Dark)
                    // do something
            }
        }
    \endcode

\since Mobility 1.2
*/

/*!
    \qmlclass AmbientTemperatureSensor QAmbientTemperatureSensor
    \ingroup qml-sensors_type
    \since Mobility 1.3
    \inherits Sensor
    \brief The AmbientTemperatureSensor element reports on the ambient temperature.

    The AmbientTemperatureSensor element reports on the ambient temperature.

    This element wraps the QAmbientTemperatureSensor class. Please see the documentation for
    QAmbientTemperatureSensor for details.

    \sa AmbientTemperatureReading
*/

/*!
    \qmltype AmbientTemperatureReading QAmbientTemperatureReading
    \ingroup qml-sensors_reading
    \since Mobility 1.3
    \inherits SensorReading
    \brief The AmbientTemperatureReading element holds the most recent temperature reading.

    The AmbientTemperatureReading element holds the most recent temperature reading.

    This element wraps the QAmbientTemperatureReading class. Please see the documentation for
    QAmbientTemperatureReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty qreal AmbientTemperatureReading::temperature
    This property holds the ambient temperature in degree Celsius.

    Please see QAmbientTemperatureReading::temperature for information about this property.
    \since Mobility 1.3
*/

/*!
    \qmlclass Compass QCompass
    \ingroup qml-sensors_type
    \since Mobility 1.1
    \inherits Sensor
    \brief The Compass element reports on heading using magnetic north as a reference.

    The Compass element reports on heading using magnetic north as a reference.

    This element wraps the QCompass class. Please see the documentation for
    QCompass for details.

    \sa {Sensors QML Limitations}
*/

#if defined(Q_QDOC) && defined(Q_OS_BLACKBERRY)
/*!
    \qmlproperty AxesOrientationMode Compass::axesOrientationMode
    Holds the mode that affects how the screen orientation changes reading values.
    \sa QOrientableSensorBase::axesOrientationMode
    \since Mobility 1.3
*/

/*!
    \qmlproperty int Compass::currentOrientation
    Holds the current orientation that is used for rotating the reading values.
    \sa QOrientableSensorBase::currentOrientation
    \since Mobility 1.3
*/

/*!
    \qmlproperty int Compass::userOrientation
    Holds the angle used for rotating the reading values in the UserOrientation mode.
    \sa QOrientableSensorBase::userOrientation
    \since Mobility 1.3
*/
#endif

/*!
    \qmlproperty CompassReading Compass::reading
    Holds the most recent compass reading.
    Please see QSensor::reading for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlsignal Compass::onReadingChanged()
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
    \since Mobility 1.2
*/

/*!
    \qmlclass CompassReading QCompassReading
    \ingroup qml-sensors_reading
    \since Mobility 1.1
    \inherits SensorReading
    \brief The CompassReading element holds the most recent Compass reading.

    The CompassReading element holds the most recent Compass reading.

    This element wraps the QCompassReading class. Please see the documentation for
    QCompassReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty qreal CompassReading::azimuth
    Please see QCompassReading::azimuth for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlproperty qreal CompassReading::calibrationLevel
    Please see QCompassReading::calibrationLevel for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlclass Magnetometer QMagnetometer
    \ingroup qml-sensors_type
    \since Mobility 1.1
    \inherits Sensor
    \brief The Magnetometer element reports on magnetic field strength
           along the Z, Y and Z axes.

    The Magnetometer element reports on magnetic field strength
    along the Z, Y and Z axes.

    This element wraps the QMagnetometer class. Please see the documentation for
    QMagnetometer for details.

    \sa {Sensors QML Limitations}
*/

#if defined(Q_QDOC) && defined(Q_OS_BLACKBERRY)
/*!
    \qmlproperty AxesOrientationMode Magnetometer::axesOrientationMode
    Holds the mode that affects how the screen orientation changes reading values.
    \sa QOrientableSensorBase::axesOrientationMode
    \since Mobility 1.3
*/

/*!
    \qmlproperty int Magnetometer::currentOrientation
    Holds the current orientation that is used for rotating the reading values.
    \sa QOrientableSensorBase::currentOrientation
    \since Mobility 1.3
*/

/*!
    \qmlproperty int Magnetometer::userOrientation
    Holds the angle used for rotating the reading values in the UserOrientation mode.
    \sa QOrientableSensorBase::userOrientation
    \since Mobility 1.3
*/
#endif

/*!
    \qmlproperty MagnetometerReading Magnetometer::reading
    Holds the most recent magnetometer reading.
    Please see QSensor::reading for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlsignal Magnetometer::onReadingChanged()
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
    \since Mobility 1.2
*/

/*!
    \qmlclass MagnetometerReading QMagnetometerReading
    \ingroup qml-sensors_reading
    \since Mobility 1.1
    \inherits SensorReading
    \brief The MagnetometerReading element holds the most recent Magnetometer reading.

    The MagnetometerReading element holds the most recent Magnetometer reading.

    This element wraps the QMagnetometerReading class. Please see the documentation for
    QMagnetometerReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty qreal MagnetometerReading::x
    Please see QMagnetometerReading::x for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlproperty qreal MagnetometerReading::y
    Please see QMagnetometerReading::y for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlproperty qreal MagnetometerReading::z
    Please see QMagnetometerReading::z for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlproperty qreal MagnetometerReading::calibrationLevel
    Please see QMagnetometerReading::calibrationLevel for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlclass OrientationSensor QOrientationSensor
    \ingroup qml-sensors_type
    \since Mobility 1.1
    \inherits Sensor
    \brief The OrientationSensor element reports device orientation.

    The OrientationSensor element reports device orientation.

    This element wraps the QOrientationSensor class. Please see the documentation for
    QOrientationSensor for details.

    \sa {Sensors QML Limitations}
*/

/*!
    \qmlproperty OrientationReading OrientationSensor::reading
    Holds the most recent orientation reading.
    Please see QSensor::reading for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlsignal OrientationSensor::onReadingChanged()
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
    \since Mobility 1.2
*/

/*!
    \qmlclass OrientationReading QOrientationReading
    \ingroup qml-sensors_reading
    \since Mobility 1.1
    \inherits SensorReading
    \brief The OrientationReading element holds the most recent OrientationSensor reading.

    The OrientationReading element holds the most recent OrientationSensor reading.

    This element wraps the QOrientationReading class. Please see the documentation for
    QOrientationReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty QOrientationReading::Orientation OrientationReading::orientation
    Please see QOrientationReading::orientation for information about this property.

    Orientation constants are exposed through the OrientationReading class.
    \code
        OrientationSensor {
            onReadingChanged: {
                if (reading.orientation == OrientationReading.TopUp)
                    // do something
            }
        }
    \endcode
    \since Mobility 1.2
*/

/*!
    \qmlclass ProximitySensor QProximitySensor
    \ingroup qml-sensors_type
    \since Mobility 1.1
    \inherits Sensor
    \brief The ProximitySensor element reports on object proximity.

    The ProximitySensor element reports on object proximity.

    This element wraps the QProximitySensor class. Please see the documentation for
    QProximitySensor for details.

    \sa {Sensors QML Limitations}
*/

/*!
    \qmlproperty ProximityReading ProximitySensor::reading
    Holds the most recent proximity reading.
    Please see QSensor::reading for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlsignal ProximitySensor::onReadingChanged()
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
    \since Mobility 1.2
*/

/*!
    \qmlclass ProximityReading QProximityReading
    \ingroup qml-sensors_reading
    \since Mobility 1.1
    \inherits SensorReading
    \brief The ProximityReading element holds the most recent ProximitySensor reading.

    The ProximityReading element holds the most recent ProximitySensor reading.

    This element wraps the QProximityReading class. Please see the documentation for
    QProximityReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty bool ProximityReading::close
    Please see QProximityReading::close for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlclass RotationSensor QRotationSensor
    \ingroup qml-sensors_type
    \since Mobility 1.1
    \inherits Sensor
    \brief The RotationSensor element reports on device rotation
           around the X, Y and Z axes.

    The RotationSensor element reports on device rotation
    around the X, Y and Z axes.

    This element wraps the QRotationSensor class. Please see the documentation for
    QRotationSensor for details.

    \sa {Sensors QML Limitations}
*/

#if defined(Q_QDOC) && defined(Q_OS_BLACKBERRY)
/*!
    \qmlproperty AxesOrientationMode RotationSensor::axesOrientationMode
    Holds the mode that affects how the screen orientation changes reading values.
    \sa QOrientableSensorBase::axesOrientationMode
    \since Mobility 1.3
*/

/*!
    \qmlproperty int RotationSensor::currentOrientation
    Holds the current orientation that is used for rotating the reading values.
    \sa QOrientableSensorBase::currentOrientation
    \since Mobility 1.3
*/

/*!
    \qmlproperty int RotationSensor::userOrientation
    Holds the angle used for rotating the reading values in the UserOrientation mode.
    \sa QOrientableSensorBase::userOrientation
    \since Mobility 1.3
*/
#endif

/*!
    \qmlproperty RotationReading RotationSensor::reading
    Holds the most recent rotation reading.
    Please see QSensor::reading for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlsignal RotationSensor::onReadingChanged()
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
    \since Mobility 1.2
*/

/*!
    \qmlclass RotationReading QRotationReading
    \ingroup qml-sensors_reading
    \since Mobility 1.1
    \inherits SensorReading
    \brief The RotationReading element holds the most recent RotationSensor reading.

    The RotationReading element holds the most recent RotationSensor reading.

    This element wraps the QRotationReading class. Please see the documentation for
    QRotationReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty qreal RotationReading::x
    Please see QRotationReading::x for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlproperty qreal RotationReading::y
    Please see QRotationReading::y for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlproperty qreal RotationReading::z
    Please see QRotationReading::z for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlclass TapSensor QTapSensor
    \ingroup qml-sensors_type
    \since Mobility 1.1
    \inherits Sensor
    \brief The TapSensor element reports tap and double tap events
           along the X, Y and Z axes.

    The TapSensor element reports tap and double tap events
    along the X, Y and Z axes.

    This element wraps the QTapSensor class. Please see the documentation for
    QTapSensor for details.

    \sa {Sensors QML Limitations}
*/

/*!
    \qmlproperty TapReading TapSensor::reading
    Holds the most recent tap sensor reading.
    Please see QSensor::reading for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlsignal TapSensor::onReadingChanged()
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
    \since Mobility 1.2
*/

/*!
    \qmlclass TapReading QTapReading
    \ingroup qml-sensors_reading
    \since Mobility 1.1
    \inherits SensorReading
    \brief The TapReading element holds the most recent TapSensor reading.

    The TapReading element holds the most recent TapSensor reading.

    This element wraps the QTapReading class. Please see the documentation for
    QTapReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty bool TapReading::doubleTap
    Please see QTapReading::doubleTap for information about this property.
    \since Mobility 1.2
*/

/*!
    \qmlproperty QTapReading::TapDirection TapReading::tapDirection
    Please see QTapReading::tapDirection for information about this property.

    TapDirection constants are exposed through the TapReading class.
    \code
        TapSensor {
            onReadingChanged: {
                if ((reading.tapDirection & TapReading.X_Both))
                    // do something
            }
        }
    \endcode

\since Mobility 1.2
*/

/*!
    \qmlclass LightSensor QLightSensor
    \ingroup qml-sensors_type
    \since Mobility 1.2
    \inherits Sensor
    \brief The LightSensor element reports on light levels using LUX.

    The LightSensor element reports on light levels using LUX.

    This element wraps the QLightSensor class. Please see the documentation for
    QLightSensor for details.

    \sa {Sensors QML Limitations}
*/

/*!
    \qmlproperty LightReading LightSensor::reading
    \since Mobility 1.2
    Holds the most recent light sensor reading.
    Please see QSensor::reading for information about this property.
*/

/*!
    \qmlsignal LightSensor::onReadingChanged()
    \since Mobility 1.2
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
*/

/*!
    \qmlclass LightReading QLightReading
    \ingroup qml-sensors_reading
    \since Mobility 1.2
    \inherits SensorReading
    \brief The LightReading element holds the most recent LightSensor reading.

    The LightReading element holds the most recent LightSensor reading.

    This element wraps the QLightReading class. Please see the documentation for
    QLightReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty qreal LightReading::lux
    \since Mobility 1.2
    Please see QLightReading::lux for information about this property.
*/

/*!
    \qmlclass Gyroscope QGyroscope
    \ingroup qml-sensors_type
    \since Mobility 1.2
    \inherits Sensor
    \brief The Gyroscope element reports on rotational acceleration
           around the X, Y and Z axes.

    This element wraps the QGyroscope class. Please see the documentation for
    QGyroscope for details.

    \sa {Sensors QML Limitations}
*/

#if defined(Q_QDOC) && defined(Q_OS_BLACKBERRY)
/*!
    \qmlproperty AxesOrientationMode Gyroscope::axesOrientationMode
    Holds the mode that affects how the screen orientation changes reading values.
    \sa QOrientableSensorBase::axesOrientationMode
    \since Mobility 1.3
*/

/*!
    \qmlproperty int Gyroscope::currentOrientation
    Holds the current orientation that is used for rotating the reading values.
    \sa QOrientableSensorBase::currentOrientation
    \since Mobility 1.3
*/

/*!
    \qmlproperty int Gyroscope::userOrientation
    Holds the angle used for rotating the reading values in the UserOrientation mode.
    \sa QOrientableSensorBase::userOrientation
    \since Mobility 1.3
*/
#endif

/*!
    \qmlproperty GyroscopeReading Gyroscope::reading
    \since Mobility 1.2
    Holds the most recent gyroscope reading.
    Please see QSensor::reading for information about this property.
*/

/*!
    \qmlsignal Gyroscope::onReadingChanged()
    \since Mobility 1.2
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
*/

/*!
    \qmlclass GyroscopeReading QGyroscopeReading
    \ingroup qml-sensors_reading
    \since Mobility 1.2
    \inherits SensorReading
    \brief The GyroscopeReading element holds the most recent Gyroscope reading.

    The GyroscopeReading element holds the most recent Gyroscope reading.

    This element wraps the QGyroscopeReading class. Please see the documentation for
    QGyroscopeReading for details.

    This element cannot be directly created.
*/

/*!
    \qmlproperty qreal GyroscopeReading::x
    \since Mobility 1.2
    Please see QGyroscopeReading::x for information about this property.
*/

/*!
    \qmlproperty qreal GyroscopeReading::y
    \since Mobility 1.2
    Please see QGyroscopeReading::y for information about this property.
*/

/*!
    \qmlproperty qreal GyroscopeReading::z
    \since Mobility 1.2
    Please see QGyroscopeReading::z for information about this property.
*/

/*!
    \qmlclass IRProximitySensor QIRProximitySensor
    \ingroup qml-sensors_type
    \since Mobility 1.3
    \inherits Sensor
    \brief The IRProximitySensor type reports on infra-red reflectance values.

    This type wraps the QIRProximitySensor class. Please see the documentation for
    QIRProximitySensor for details.

    \sa {Sensors QML Limitations}
*/

/*!
    \qmlproperty IRProximityReading IRProximitySensor::reading
    Holds the most recent IR proximity reading.
    Please see QSensor::reading for information about this property.
*/

/*!
    \qmlsignal IRProximitySensor::onReadingChanged()
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
*/

/*!
    \qmlclass IRProximityReading QIRProximityReading
    \ingroup qml-sensors_reading
    \since Mobility 1.3
    \inherits SensorReading
    \brief The IRProximityReading type holds the most recent IR proximity reading.

    The IRProximityReading type holds the most recent IR proximity reading.

    This type wraps the QIRProximityReading class. Please see the documentation for
    QIRProximityReading for details.

    This type cannot be directly created.
*/

/*!
    \qmlproperty qreal IRProximityReading::reflectance
    Please see QIRProximityReading::reflectance for information about this property.
*/

/*!
    \qmlclass PressureSensor QPressureSensor
    \ingroup qml-sensors_type
    \since Mobility 1.3
    \inherits Sensor
    \brief The PressureSensor type reports on atmospheric pressure values.

    This type wraps the QPressureSensor class. Please see the documentation of
    QPressureSensor for details.

    \sa {QML Limitations}
*/

/*!
    \qmlproperty PressureReading PressureSensor::reading
    Holds the most recent pressure reading.
    Please see QSensor::reading for information about this property.
*/

/*!
    \qmlsignal PressureSensor::onReadingChanged()
    Called when the reading object changes.
    Please see QSensor::readingChanged() for information about this signal.
*/

/*!
    \qmlclass PressureReading QPressureReading
    \ingroup qml-sensors_reading
    \since Mobility 1.3
    \inherits SensorReading
    \brief The PressureReading type holds the most recent pressure reading.

    The PressureReading type holds the most recent pressure reading.

    This type wraps the QPressureReading class. Please see the documentation of
    QPressureReading for details.

    This type cannot be directly created.
*/

/*!
    \qmlproperty qreal PressureReading::pressure
    Please see QPressureReading::pressure for information about this property.
*/

/*!
    \qmlproperty qreal PressureReading::temperature
    Please see QPressureReading::temperature for information about this property.
*/

/*!
    \qmlclass HolsterSensor QHolsterSensor
    \ingroup qml-sensors_type
    \since Mobility 1.3
    \inherits Sensor
    \brief The HolsterSensor type detects if a device is holstered or not.

    The HolsterSensor type detects if a device is holstered or not.

    This type wraps the QHolsterSensor class.

    \sa {QML Limitations}
    \sa QHolsterSensor
*/

/*!
    \qmlproperty HolsterReading HolsterSensor::reading
    Holds the most recent holster reading.
    \sa QSensor::reading
*/

/*!
    \qmlsignal HolsterSensor::onReadingChanged()
    Called when the reading object changes.
    \sa QSensor::readingChanged()
*/

/*!
    \qmlclass HolsterReading QHolsterReading
    \ingroup qml-sensors_reading
    \since Mobility 1.3
    \inherits SensorReading
    \brief The HolsterReading type holds the most recent HolsterSensor reading.

    The HolsterReading type holds the most recent HolsterSensor reading.

    This type wraps the QHolsterReading class.

    This type cannot be directly created.

    \sa QHolsterReading
*/

/*!
    \qmlproperty bool HolsterReading::holstered
    \sa QHolsterReading::holstered
*/
