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

#include "qwmpplaylistcontrol.h"

#include "qwmpevents.h"
#include "qwmpglobal.h"
#include "qwmpmetadata.h"
#include "qwmpplaylist.h"

QWmpPlaylistControl::QWmpPlaylistControl(IWMPCore3 *player, QWmpEvents *events, QObject *parent)
    : QMediaPlaylistControl(parent)
    , m_player(player)
    , m_controls(0)
    , m_playlist(new QWmpPlaylist(player, events))
    , m_playbackMode(QMediaPlaylist::Sequential)
{
    m_player->get_controls(&m_controls);

    connect(events, SIGNAL(CurrentItemChange(IDispatch*)),
            this, SLOT(currentItemChangeEvent(IDispatch*)));
}

QWmpPlaylistControl::~QWmpPlaylistControl()
{
    if (m_controls)
        m_controls->Release();

    delete m_playlist;
}

QMediaPlaylistProvider *QWmpPlaylistControl::playlistProvider() const
{
    return m_playlist;
}

bool QWmpPlaylistControl::setPlaylistProvider(QMediaPlaylistProvider *playlist)
{
    Q_UNUSED(playlist);

    return false;
}

int QWmpPlaylistControl::currentIndex() const
{
    int position = 0;

    IWMPMedia *media = 0;
    if (m_controls && m_player->get_currentMedia(&media) == S_OK) {
        position = QWmpMetaData::value(media, QAutoBStr(L"PlaylistIndex")).toInt();

        media->Release();
    }

    return position;
}

void QWmpPlaylistControl::setCurrentIndex(int position)
{

    IWMPPlaylist *playlist = 0;
    if (m_player->get_currentPlaylist(&playlist) == S_OK) {
        IWMPMedia *media = 0;
        if (playlist->get_item(position, &media) == S_OK) {
            m_player->put_currentMedia(media);

            media->Release();
        }
        playlist->Release();
    }
}

int QWmpPlaylistControl::nextIndex(int steps) const
{
    return currentIndex() + steps;
}

int QWmpPlaylistControl::previousIndex(int steps) const
{
    return currentIndex() - steps;
}

void QWmpPlaylistControl::next()
{
    if (m_controls)
        m_controls->next();
}

void QWmpPlaylistControl::previous()
{
    if (m_controls)
        m_controls->previous();
}

QMediaPlaylist::PlaybackMode QWmpPlaylistControl::playbackMode() const
{
    return m_playbackMode;
}

void QWmpPlaylistControl::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)
{
    m_playbackMode = mode;
}

void QWmpPlaylistControl::currentItemChangeEvent(IDispatch *dispatch)
{
    IWMPMedia *media = 0;
    if (dispatch && dispatch->QueryInterface(
            __uuidof(IWMPMedia), reinterpret_cast<void **>(&media)) == S_OK) {
        int index = QWmpMetaData::value(media, QAutoBStr(L"PlaylistIndex")).toInt();

        emit currentIndexChanged(index);
        emit currentMediaChanged(m_playlist->media(index));
    }
}
