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

#include "qsystemdisplayinfo.h"
#include "qsysteminfocommon_p.h"
#include <QDesktopWidget>
#include <QMetaType>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSystemDisplayInfoPrivate, displayInfoPrivate)

#ifdef QT_SIMULATOR
QSystemDisplayInfoPrivate *getSystemDisplayInfoPrivate() { return displayInfoPrivate(); }
#endif // QT_SIMULATOR

/*!
    \enum QSystemDisplayInfo::DisplayOrientation
    This enum describes the orientation of the default window.

    \value Unknown                  Unknown orientation or error.
    \value Landscape                Landscape is wider than high.
    \value Portrait                 Portrait is higher than wide.
    \value InvertedLandscape        Landscape that is inverted.
    \value InvertedPortrait         Portrait that is inverted.
*/

/*!
    \enum QSystemDisplayInfo::BacklightState
    This enum describes the state of the Backlight.

    \value BacklightStateUnknown     Error, no, or unknown Backlight state.
    \value BacklightStateOff         Backlight is turned off.
    \value BacklightStateDimmed      Backlight has been dimmed.
    \value BacklightStateOn          Backlight is on.
*/

/*!
    \class QSystemDisplayInfo
    \ingroup systeminfo
    \inmodule QtSystemInfo
    \since 1.0
    \brief The QSystemDisplayInfo class provides access to display information from the system.

    \warning This class can only be used from the main thread, since it has been implemented using
     GUI functionality.
*/

/*!
    \fn void QSystemDisplayInfo::orientationChanged(QSystemDisplayInfo::DisplayOrientation orientation)

    This signal is emitted when QDesktopWidget's orientation has changed to \a orientation.
*/

/*!
    Constructs a QSystemDisplayInfo object with the given \a parent.
*/
QSystemDisplayInfo::QSystemDisplayInfo(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QSystemDisplayInfo::DisplayOrientation>("QSystemDisplayInfo::DisplayOrientation");
    qRegisterMetaType<QSystemDisplayInfo::BacklightState>("QSystemDisplayInfo::BacklightState");
    connect(displayInfoPrivate(), SIGNAL(orientationChanged(QSystemDisplayInfo::DisplayOrientation)),
            this, SIGNAL(orientationChanged(QSystemDisplayInfo::DisplayOrientation)), Qt::UniqueConnection);
}

/*!
    Destroys the QSystemDisplayInfo object.
*/
QSystemDisplayInfo::~QSystemDisplayInfo()
{
}

/*!
    Returns the display brightness of the screen with index \a screenNumber, in 1 - 100 scale.
    -1 is returned if not available or on error.

    \sa QDesktopWidget::screenCount()
    \since 1.0
*/
int QSystemDisplayInfo::displayBrightness(int screenNumber)
{
    QDesktopWidget wid;
    if (wid.screenCount() < 1 || wid.screenCount() - 1 < screenNumber)
        return -1;

    return displayInfoPrivate()->displayBrightness(screenNumber);
}

/*!
    Returns the color depth of the screen with the index \a screenNumber, in bits per pixel.
    -1 is returned if not available or on error.

    \sa QDesktopWidget::screenCount()
    \since 1.0
*/
int QSystemDisplayInfo::colorDepth(int screenNumber)
{
    QDesktopWidget wid;
    if (wid.screenCount() < 1 || wid.screenCount() - 1 < screenNumber)
        return -1;

    return displayInfoPrivate()->colorDepth(screenNumber);
}

/*!
    Returns the orientation of the UI QDesktopWidget for \a screen.

    \sa QDesktopWidget::screenCount()
    \since 1.2
*/
QSystemDisplayInfo::DisplayOrientation QSystemDisplayInfo::orientation(int screen)
{
    return displayInfoPrivate()->orientation(screen);
}

/*!
    Returns the current contrast of the screen \a screen, from 0 to 1. -1 is returned if
    not available or on error.

    \sa QDesktopWidget::screenCount()
    \since 1.2
*/
float QSystemDisplayInfo::contrast(int screen)
{
    return displayInfoPrivate()->contrast(screen);
}

/*!
    Returns the current dots per inch (DPI) for the width of \a screen. -1 is returned if
    not available or on error.

    \sa QDesktopWidget::screenCount()
    \since 1.2
*/
int QSystemDisplayInfo::getDPIWidth(int screen)
{
    return displayInfoPrivate()->getDPIWidth(screen);
}

/*!
    Returns the dpi (Dot Per Inch) of the height os \a screen. -1 is returned if not available
    or on error.

    \sa QDesktopWidget::screenCount()
    \since 1.2
*/
int QSystemDisplayInfo::getDPIHeight(int screen)
{
    return displayInfoPrivate()->getDPIHeight(screen);
}

/*!
    Returns the physical height of the \a screen in millimeters. -1 is returned if not available
    or on error.

    \sa QDesktopWidget::screenCount()
    \since 1.2
*/
int QSystemDisplayInfo::physicalHeight(int screen)
{
    return displayInfoPrivate()->physicalHeight(screen);
}

/*!
    Returns the physical width of \a screen in millimeters. -1 is returned if not available or
    on error.

    \sa QDesktopWidget::screenCount()
    \since 1.2
*/
int QSystemDisplayInfo::physicalWidth(int screen)
{
    return displayInfoPrivate()->physicalWidth(screen);
}

/*!
    Returns whether the QSystemDisplayInfo::BacklightState for the screen \a screen.
    \since 1.2
*/
QSystemDisplayInfo::BacklightState QSystemDisplayInfo::backlightStatus(int screen)
{
    return displayInfoPrivate()->backlightStatus(screen);
}

#include "moc_qsystemdisplayinfo.cpp"

QTM_END_NAMESPACE
