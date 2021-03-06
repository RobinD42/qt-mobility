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

#ifndef QWMPPLAYLIST_H
#define QWMPPLAYLIST_H

#include <qmediaplaylistprovider.h>

#include <QtCore/qvariant.h>

#include <wmp.h>

class QWmpEvents;

QT_USE_NAMESPACE
class QWmpPlaylist : public QMediaPlaylistProvider
{
    Q_OBJECT
public:
    QWmpPlaylist(IWMPCore3 *player, QWmpEvents *events, QObject *parent = 0);
    ~QWmpPlaylist();

    bool load(const QString &location, const char *format = 0);
    bool load(QIODevice * device, const char *format = 0);
    bool save(const QString &location, const char *format = 0);
    bool save(QIODevice * device, const char *format);

    int mediaCount() const;
    QMediaContent media(int pos) const;

    bool isReadOnly() const;

    bool addMedia(const QMediaContent &content);
    bool insertMedia(int pos, const QMediaContent &content);
    bool removeMedia(int pos);
    bool removeMedia(int start, int end);
    bool clear();

    QStringList keys(int index) const;
    QVariant value(int index, const QString &key) const;

public Q_SLOTS:
    virtual void shuffle();

private Q_SLOTS:
    void currentPlaylistChangeEvent(WMPPlaylistChangeEventType change);
    void openPlaylistChangeEvent(IDispatch *dispatch);
    void mediaChangeEvent(IDispatch *dispatch);

private:
    IWMPCore3 *m_player;
    IWMPPlaylist *m_playlist;
    long m_count;
};

#endif
