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

#include "explorer.h"
#include <QTimer>
#include <QDebug>
#include <qsensor.h>
#include <QMetaObject>
#include <QMetaProperty>

QTM_USE_NAMESPACE

Explorer::Explorer(QWidget *parent)
    : QMainWindow(parent)
    , m_sensor(0)
    , ignoreItemChanged(false)
{
    ui.setupUi(this);
#ifdef MAEMO5
    ui.label->hide();
#endif
    // Clear out example data from the .ui file
    ui.sensors->clear();
    clearSensorProperties();
    clearReading();
}

Explorer::~Explorer()
{
}

void Explorer::loadSensors()
{
    qDebug() << "Explorer::loadSensors";

    // Clear out anything that's in there now
    ui.sensors->clear();

    foreach (const QByteArray &type, QSensor::sensorTypes()) {
        qDebug() << "Found type" << type;
        foreach (const QByteArray &identifier, QSensor::sensorsForType(type)) {
            qDebug() << "Found identifier" << identifier;
            // Don't put in sensors we can't connect to
            QSensor sensor;
            sensor.setType(type);
            sensor.setIdentifier(identifier);
            if (!sensor.connect()) {
                qDebug() << "Couldn't connect to" << identifier;
                continue;
            }

            qDebug() << "Adding identifier" << identifier;
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << QString::fromLatin1(identifier));
            item->setData(0, Qt::UserRole, QString::fromLatin1(type));
            ui.sensors->addTopLevelItem(item);
        }
    }

    resizeSensors();
}

void Explorer::resizeSensors()
{
    ui.sensors->resizeColumnToContents(0);
    int length = ui.sensors->header()->length() + 4;
    ui.sensors->setFixedWidth(length);
}

void Explorer::on_sensors_currentItemChanged()
{
    qDebug() << "Explorer::sensorSelected";

    // Clear out anything that's in there now
    if (m_sensor) {
        delete m_sensor;
        m_sensor = 0;
    }
    clearSensorProperties();
    clearReading();

    // Check that we've selected an item
    QTreeWidgetItem *item = ui.sensors->currentItem();
    if (!item) {
        qWarning() << "Didn't select an item!";
        return;
    }

    QByteArray type = item->data(0, Qt::UserRole).toString().toLatin1();
    QByteArray identifier = item->data(0, Qt::DisplayRole).toString().toLatin1();

    // Connect to the sensor so we can probe it
    m_sensor = new QSensor(this);
    connect(m_sensor, SIGNAL(readingChanged()), this, SLOT(sensor_changed()));
    m_sensor->setType(type);
    m_sensor->setIdentifier(identifier);
    if (!m_sensor->connect()) {
        delete m_sensor;
        m_sensor = 0;
        qWarning() << "Can't connect to the sensor!";
        return;
    }
    m_sensor->setUpdateInterval(200);

    loadSensorProperties();
    loadReading();

    adjustTableColumns(ui.sensorprops);
    adjustTableColumns(ui.reading);
    QTimer::singleShot(100, this, SLOT(adjustSizes()));
}

void Explorer::clearReading()
{
    ui.reading->setRowCount(0);
}

void Explorer::loadReading()
{
    // Probe the reading using Qt's meta-object facilities
    QSensorReading *reading = m_sensor->reading();
    const QMetaObject *mo = reading->metaObject();
    int firstProperty = QSensorReading::staticMetaObject.propertyOffset();

    ui.reading->setRowCount(mo->propertyCount() - firstProperty);

    for(int i = firstProperty; i < mo->propertyCount(); ++i) {
        int row = i - firstProperty;
        QTableWidgetItem *index;
        if (row == 0)
            // timestamp is not available via index
            index = new QTableWidgetItem(QLatin1String("N/A"));
        else
            index = new QTableWidgetItem(QVariant(row - 1).toString());
        QTableWidgetItem *prop = new QTableWidgetItem(mo->property(i).name());
        QString typeName = QLatin1String(mo->property(i).typeName());
        int crap = typeName.lastIndexOf("::");
        if (crap != -1)
            typeName = typeName.mid(crap + 2);
        QTableWidgetItem *type = new QTableWidgetItem(typeName);
        QTableWidgetItem *value = new QTableWidgetItem();

        index->setFlags(value->flags() ^ Qt::ItemIsEditable);
        prop->setFlags(value->flags() ^ Qt::ItemIsEditable);
        type->setFlags(value->flags() ^ Qt::ItemIsEditable);
        value->setFlags(value->flags() ^ Qt::ItemIsEditable);

        ui.reading->setItem(row, 0, index);
        ui.reading->setItem(row, 1, prop);
        ui.reading->setItem(row, 2, type);
        ui.reading->setItem(row, 3, value);
    }
}

void Explorer::clearSensorProperties()
{
    ui.sensorprops->setRowCount(0);
}

void Explorer::loadSensorProperties()
{
    ignoreItemChanged = true;

    // Probe the sensor using Qt's meta-object facilities
    const QMetaObject *mo = m_sensor->metaObject();
    int firstProperty = QSensor::staticMetaObject.propertyOffset();

    int rows = mo->propertyCount() - firstProperty;
    ui.sensorprops->setRowCount(rows);

    int offset = 0;
    for(int i = firstProperty; i < mo->propertyCount(); ++i) {
        int row = i - firstProperty - offset;
        QLatin1String name(mo->property(i).name());
        if (name == "sensorid" ||
            //name == "type" ||
            name == "reading" ||
            name == "connected" ||
            name == "running" ||
            name == "supportsPolling") {
            ++offset;
            continue;
        }
        QTableWidgetItem *prop = new QTableWidgetItem(name);
        QString typeName = QLatin1String(mo->property(i).typeName());
        int crap = typeName.lastIndexOf("::");
        if (crap != -1)
            typeName = typeName.mid(crap + 2);
        QTableWidgetItem *type = new QTableWidgetItem(typeName);
        QString val = mo->property(i).read(m_sensor).toString();
        QTableWidgetItem *value = new QTableWidgetItem(val);

        prop->setFlags(value->flags() ^ Qt::ItemIsEditable);
        type->setFlags(value->flags() ^ Qt::ItemIsEditable);
        if (!mo->property(i).isWritable()) {
            // clear the editable flag
            value->setFlags(value->flags() ^ Qt::ItemIsEditable);
        }

        ui.sensorprops->setItem(row, 0, prop);
        ui.sensorprops->setItem(row, 1, type);
        ui.sensorprops->setItem(row, 2, value);
    }

    // We don't add all properties
    ui.sensorprops->setRowCount(rows - offset);

    ui.poll->setEnabled(m_sensor->supportsPolling());

    ignoreItemChanged = false;
}

void Explorer::showEvent(QShowEvent *event)
{
    // Once we're visible, load the sensors
    // (don't delay showing the UI while we load plugins, connect to backends, etc.)
    QTimer::singleShot(0, this, SLOT(loadSensors()));

    QMainWindow::showEvent(event);
}

// Resize columns to fit the space.
// This shouldn't be so hard!
void Explorer::adjustTableColumns(QTableWidget *table)
{
    if (table->rowCount() == 0) {
        table->setFixedHeight(0);
        return;
    }

    // At least this is easy to do
    table->resizeColumnsToContents();
    int length = table->verticalHeader()->length();
    length += (length / static_cast<qreal>(table->verticalHeader()->count())); // Add 1 more (the header itself)
#ifdef MAEMO5
    length += 10; // required for N900 UI
#endif
    table->setFixedHeight(length);

    int columns = table->columnCount();
    QList<int> width;
    int suggestedWidth = 0;
    for (int i = 0; i < columns; ++i) {
        int cwidth = table->columnWidth(i);
        width << cwidth;
        suggestedWidth += cwidth;
    }

    int actualWidth = table->size().width();
    //qDebug() << "suggestedWidth" << suggestedWidth << "actualWidth" << actualWidth;

    // We only scale the columns up, we don't scale down
    if (actualWidth <= suggestedWidth)
        return;

    qreal multiplier = actualWidth / static_cast<qreal>(suggestedWidth);
    int currentSpace = 4;
    for (int i = 0; i < columns; ++i) {
        width[i] = multiplier * width[i];
        currentSpace += width[i];
    }

    // It ends up too big due to cell decorations or something.
    // Make things smaller one pixel at a time in round robin fashion until we're good.
    int i = 0;
    while (currentSpace > actualWidth) {
        --width[i];
        --currentSpace;
        i = (i + 1) % columns;
    }

    for (int i = 0; i < columns; ++i) {
        table->setColumnWidth(i, width[i]);
    }

    table->setMinimumWidth(suggestedWidth);
}

void Explorer::adjustSizes()
{
    adjustTableColumns(ui.reading);
    adjustTableColumns(ui.sensorprops);
}

void Explorer::resizeEvent(QResizeEvent *event)
{
    resizeSensors();
    adjustSizes();

    QMainWindow::resizeEvent(event);
}

void Explorer::on_start_clicked()
{
    m_sensor->start();
    QTimer::singleShot(0, this, SLOT(loadSensorProperties()));
}

void Explorer::on_stop_clicked()
{
    m_sensor->stop();
    QTimer::singleShot(0, this, SLOT(loadSensorProperties()));
}

void Explorer::on_poll_clicked()
{
    m_sensor->poll();
    sensor_changed();
}

void Explorer::sensor_changed()
{
    QSensorReading *reading = m_sensor->reading();
    filter(reading);
}

bool Explorer::filter(QSensorReading *reading)
{
    const QMetaObject *mo = reading->metaObject();
    int firstProperty = QSensorReading::staticMetaObject.propertyOffset();

    for(int i = firstProperty; i < mo->propertyCount(); ++i) {
        int row = i - firstProperty;
        QLatin1String name(mo->property(i).name());
        QTableWidgetItem *value = ui.reading->item(row, 3);
        QVariant val = mo->property(i).read(reading);
        if (val.userType() == QMetaType::type("QtMobility::qtimestamp")) {
            value->setText(QString("%1").arg(val.value<QtMobility::qtimestamp>()));
        } else {
            value->setText(val.toString());
        }
    }

    adjustTableColumns(ui.reading);
    //QTimer::singleShot(0, this, SLOT(adjustSizes()));

    return false;
}

void Explorer::on_sensorprops_itemChanged(QTableWidgetItem *item)
{
    if (ignoreItemChanged)
        return;
    if (!(item->flags() & Qt::ItemIsEditable))
        return;

    int row = item->row();
    QString name = ui.sensorprops->item(row, 0)->text();
    QVariant value = item->text();

    qDebug() << "setProperty" << name << value;
    m_sensor->setProperty(name.toLatin1().constData(), QVariant(value));

    QTimer::singleShot(0, this, SLOT(loadSensorProperties()));
}
