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

#include "ql2capserver.h"
#include "ql2capserver_p.h"
#include "qbluetoothsocket.h"
#include "qbluetoothsocket_p.h"
#include "symbian/utils_symbian_p.h"

#include <QTimer>
#include <QCoreApplication>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QL2capServerPrivate::QL2capServerPrivate()
: socket(0),pending(false),maxPendingConnections(1),securityFlags(QBluetooth::NoSecurity)
{
}

QL2capServerPrivate::~QL2capServerPrivate()
{
    delete socket;
}

void QL2capServer::close()
{
    Q_D(QL2capServer);
    if(!d->socket)
        {
        // there is no way to propagate the error to user
        // so just ignore the problem.
        return;
        }
    d->socket->close();
    // force active object (socket) to run and shutdown socket.
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

bool QL2capServer::listen(const QBluetoothAddress &address, quint16 port)
{
    Q_D(QL2capServer);
    // listen has already been called before
    if(d->socket)
        return true;
    
    d->socket = new QBluetoothSocket(QBluetoothSocket::L2capSocket,this);
    
    if(!d->socket)
        {
        return false;
        }
    
    d->ds = d->socket->d_ptr;
    
    if(!d->ds)
        {
        delete d->socket;
        d->socket = 0;
        return false;
        }
    
    TL2CAPSockAddr addr;
    
    if(!address.isNull())
        {
        // TBTDevAddr constructor may panic
        TRAPD(err,addr.SetBTAddr(TBTDevAddr(address.toUInt64())));
        if(err != KErrNone)
            {
            delete d->socket;
            d->socket = 0;
            return false;
            }
        }
    
    if (port == 0)
        addr.SetPort(KL2CAPPassiveAutoBind);
    else
        addr.SetPort(port);

    TBTServiceSecurity security;
    switch (d->securityFlags) {
        case QBluetooth::Authentication:
            security.SetAuthentication(EMitmDesired);
            break;
        case QBluetooth::Authorization:
            security.SetAuthorisation(ETrue);
            break;
        case QBluetooth::Encryption:
        // "Secure" is BlueZ specific we just make sure the code remain compatible
        case QBluetooth::Secure:
            // authentication required
            security.SetAuthentication(EMitmDesired);
            security.SetEncryption(ETrue);
            break;
        case QBluetooth::NoSecurity:
        default:
            break;
    }
    addr.SetSecurity(security);
    if(d->ds->iSocket->Bind(addr) == KErrNone)
        {
        d->socket->setSocketState(QBluetoothSocket::BoundState);
        }
    else
        {
        delete d->socket;
        d->socket = 0;
        return false;
        }

    if(d->ds->iSocket->Listen(d->maxPendingConnections) != KErrNone)
        {
        delete d->socket;
        d->socket = 0;
        return false;
        }
    // unknown socket type is used for blank socket
    QBluetoothSocket *pendingSocket = new QBluetoothSocket(QBluetoothSocket::UnknownSocketType, this);
    if(!pendingSocket)
        {
        delete d->socket;
        d->socket = 0;
        return false;
        }
    QBluetoothSocketPrivate *pd = pendingSocket->d_ptr;
    pd->ensureBlankNativeSocket(QBluetoothSocket::L2capSocket);
    connect(d->socket, SIGNAL(disconnected()), this, SLOT(_q_disconnected()));
    connect(d->socket, SIGNAL(connected()), this, SLOT(_q_connected()));
    connect(d->socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(_q_socketError(QBluetoothSocket::SocketError)));
    if (d->ds->iSocket->Accept(*pd->iSocket) == KErrNone)
        {
        d->socket->setSocketState(QBluetoothSocket::ListeningState);
        d->activeSockets.append(pendingSocket);
        return true;
        }
    else
        {
        delete d->socket, pendingSocket;
        d->socket = 0;
        return false;
        }
}

void QL2capServer::setMaxPendingConnections(int numConnections)
{
    Q_D(QL2capServer);
    d->maxPendingConnections = numConnections;
}

bool QL2capServer::hasPendingConnections() const
{
    Q_D(const QL2capServer);
    return !d->activeSockets.isEmpty();
}

QBluetoothSocket *QL2capServer::nextPendingConnection()
{
    Q_D(QL2capServer);

    if (d->activeSockets.isEmpty())
        return 0;

    QBluetoothSocket *next = d->activeSockets.takeFirst();
    return next;
}

QBluetoothAddress QL2capServer::serverAddress() const
{
    Q_D(const QL2capServer);
    if(d->socket)
        return d->socket->localAddress();
    else
        return QBluetoothAddress();
}

quint16 QL2capServer::serverPort() const
{
    Q_D(const QL2capServer);
    if(d->socket)
        return d->socket->localPort();
    else
        return 0;
}

void QL2capServerPrivate::_q_connected()
{
    Q_Q(QL2capServer);
    if(!activeSockets.isEmpty())
        {
        // update state of the pending socket and start receiving
        (activeSockets.last())->setSocketState(QBluetoothSocket::ConnectedState);
        (activeSockets.last())->d_ptr->startReceive();
        }
    else
        return;
    emit q->newConnection();
    QBluetoothSocket *pendingSocket = new QBluetoothSocket(QBluetoothSocket::UnknownSocketType);
    if(!pendingSocket)
        {
        delete socket;
        socket = 0;
        return;
        }
    QBluetoothSocketPrivate *pd = pendingSocket->d_ptr;
    pd->ensureBlankNativeSocket(QBluetoothSocket::L2capSocket);
    if (ds->iSocket->Accept(*pd->iSocket) == KErrNone)
        {
        socket->setSocketState(QBluetoothSocket::ListeningState);
        activeSockets.append(pendingSocket);
        return;
        }
    else
        {
        // we might reach this statement if we have reach
        // maxPendingConnections
        qDebug() << "QL2capServer::connected accept failed";
        delete socket, pendingSocket;
        socket = 0;
        return;
        }
}

void QL2capServerPrivate::_q_disconnected()
{
    delete socket;
    socket = 0;
}

void QL2capServerPrivate::_q_socketError(QBluetoothSocket::SocketError err)
{
    delete socket;
    socket = 0;
}

void QL2capServer::setSecurityFlags(QBluetooth::SecurityFlags security)
{
    Q_D(QL2capServer);
    d->securityFlags = security;
}

QBluetooth::SecurityFlags QL2capServer::securityFlags() const
{
    Q_D(const QL2capServer);
    return d->securityFlags;
}

QTM_END_NAMESPACE
