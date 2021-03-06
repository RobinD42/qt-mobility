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
#include "qmediacontent.h"
#include "qmediaplayercontrol.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qthread.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qbuffer.h>

#include "mfplayercontrol.h"
#ifndef Q_WS_SIMULATOR
#include "evr9videowindowcontrol.h"
#endif
#include "mfvideorenderercontrol.h"
#include "mfaudioendpointcontrol.h"

#include "mfplayersession.h"
#include "mfplayerservice.h"
#include "mfmetadatacontrol.h"
#include <Mferror.h>
#include <nserror.h>
#include <sourceresolver.h>

//#define DEBUG_MEDIAFOUNDATION
//#define TEST_STREAMING

namespace
{
    //MFStream is added for supporting QIODevice type of media source.
    //It is used to delegate invocations from media foundation(through IMFByteStream) to QIODevice.
    class MFStream : public QObject, public IMFByteStream
    {
        Q_OBJECT
    public:
        MFStream(QIODevice *stream, bool ownStream)
            : m_cRef(1)
            , m_stream(stream)
            , m_ownStream(ownStream)
            , m_currentReadResult(0)
        {
            //Move to the thread of the stream object
            //to make sure invocations on stream
            //are happened in the same thread of stream object
            this->moveToThread(stream->thread());
            connect(stream, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
        }

        ~MFStream()
        {
            if (m_currentReadResult)
                m_currentReadResult->Release();
            if (m_ownStream)
                m_stream->deleteLater();
        }

        //from IUnknown
        STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObject)
        {
            if (!ppvObject)
                return E_POINTER;
            if (riid == IID_IMFByteStream) {
                *ppvObject = static_cast<IMFByteStream*>(this);
            } else if (riid == IID_IUnknown) {
                *ppvObject = static_cast<IUnknown*>(this);
            } else {
                *ppvObject =  NULL;
                return E_NOINTERFACE;
            }
            AddRef();
            return S_OK;
        }

        STDMETHODIMP_(ULONG) AddRef(void)
        {
            return InterlockedIncrement(&m_cRef);
        }

        STDMETHODIMP_(ULONG) Release(void)
        {
            LONG cRef = InterlockedDecrement(&m_cRef);
            if (cRef == 0) {
                this->deleteLater();
            }
            return cRef;
        }


        //from IMFByteStream
        STDMETHODIMP GetCapabilities(DWORD *pdwCapabilities)
        {
            if (!pdwCapabilities)
                return E_INVALIDARG;
            *pdwCapabilities = MFBYTESTREAM_IS_READABLE;
            if (!m_stream->isSequential())
                *pdwCapabilities |= MFBYTESTREAM_IS_SEEKABLE;
            return S_OK;
        }

        STDMETHODIMP GetLength(QWORD *pqwLength)
        {
            if (!pqwLength)
                return E_INVALIDARG;
            QMutexLocker locker(&m_mutex);
            *pqwLength = QWORD(m_stream->size());
            return S_OK;
        }

        STDMETHODIMP SetLength(QWORD)
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP GetCurrentPosition(QWORD *pqwPosition)
        {
            if (!pqwPosition)
                return E_INVALIDARG;
            QMutexLocker locker(&m_mutex);
            *pqwPosition = m_stream->pos();
            return S_OK;
        }

        STDMETHODIMP SetCurrentPosition(QWORD qwPosition)
        {
            QMutexLocker locker(&m_mutex);
            //SetCurrentPosition may happend during the BeginRead/EndRead pair,
            //refusing to execute SetCurrentPosition during that time seems to be
            //the simplest workable solution
            if (m_currentReadResult)
                return S_FALSE;

            bool seekOK = m_stream->seek(qint64(qwPosition));
            if (seekOK)
                return S_OK;
            else
                return S_FALSE;
        }

        STDMETHODIMP IsEndOfStream(BOOL *pfEndOfStream)
        {
            if (!pfEndOfStream)
                return E_INVALIDARG;
            QMutexLocker locker(&m_mutex);
            *pfEndOfStream = m_stream->atEnd() ? TRUE : FALSE;
            return S_OK;
        }

        STDMETHODIMP Read(BYTE *pb, ULONG cb, ULONG *pcbRead)
        {
            QMutexLocker locker(&m_mutex);
            qint64 read = m_stream->read((char*)(pb), qint64(cb));
            if (pcbRead)
                *pcbRead = ULONG(read);
            return S_OK;
        }

        STDMETHODIMP BeginRead(BYTE *pb, ULONG cb, IMFAsyncCallback *pCallback,
                               IUnknown *punkState)
        {
            if (!pCallback || !pb)
                return E_INVALIDARG;

            Q_ASSERT(m_currentReadResult == NULL);

            AsyncReadState *state = new (std::nothrow) AsyncReadState(pb, cb);
            if (state == NULL)
                return E_OUTOFMEMORY;

            HRESULT hr = MFCreateAsyncResult(state, pCallback, punkState, &m_currentReadResult);
            state->Release();
            if (FAILED(hr))
                return hr;

            QCoreApplication::postEvent(this, new QEvent(QEvent::User));
            return hr;
        }

        STDMETHODIMP EndRead(IMFAsyncResult* pResult, ULONG *pcbRead)
        {
            if (!pcbRead)
                return E_INVALIDARG;
            IUnknown *pUnk;
            pResult->GetObject(&pUnk);
            AsyncReadState *state = static_cast<AsyncReadState*>(pUnk);
            *pcbRead = state->bytesRead();
            pUnk->Release();

            m_currentReadResult->Release();
            m_currentReadResult = NULL;

            return S_OK;
        }

        STDMETHODIMP Write(const BYTE *, ULONG, ULONG *)
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP BeginWrite(const BYTE *, ULONG ,
                                IMFAsyncCallback *,
                                IUnknown *)
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP EndWrite(IMFAsyncResult *,
                              ULONG *)
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP Seek(
            MFBYTESTREAM_SEEK_ORIGIN SeekOrigin,
            LONGLONG llSeekOffset,
            DWORD,
            QWORD *pqwCurrentPosition)
        {
            QMutexLocker locker(&m_mutex);
            if (m_currentReadResult)
                return S_FALSE;

            qint64 pos = qint64(llSeekOffset);
            switch (SeekOrigin) {
            case msoCurrent:
                pos += m_stream->pos();
                break;
            }
            bool seekOK = m_stream->seek(pos);
            if (*pqwCurrentPosition)
                *pqwCurrentPosition = pos;
            if (seekOK)
                return S_OK;
            else
                return S_FALSE;
        }

        STDMETHODIMP Flush()
        {
            return E_NOTIMPL;
        }

        STDMETHODIMP Close()
        {
            QMutexLocker locker(&m_mutex);
            if (m_ownStream)
                m_stream->close();
            return S_OK;
        }

    private:
        //AsyncReadState is a helper class used in BeginRead for asynchronous operation
        //to record some BeginRead parameters, so these parameters could be
        //used later when actually executing the read operation in another thread.
        class AsyncReadState : public IUnknown
        {
        public:
            AsyncReadState(BYTE *pb, ULONG cb)
                : m_cRef(1)
                , m_pb(pb)
                , m_cb(cb)
                , m_cbRead(0)
            {
            }

            //from IUnknown
            STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObject)
            {
                if (!ppvObject)
                    return E_POINTER;

                if (riid == IID_IUnknown) {
                    *ppvObject = static_cast<IUnknown*>(this);
                } else {
                    *ppvObject =  NULL;
                    return E_NOINTERFACE;
                }
                AddRef();
                return S_OK;
            }

            STDMETHODIMP_(ULONG) AddRef(void)
            {
                return InterlockedIncrement(&m_cRef);
            }

            STDMETHODIMP_(ULONG) Release(void)
            {
                LONG cRef = InterlockedDecrement(&m_cRef);
                if (cRef == 0)
                    delete this;
                // For thread safety, return a temporary variable.
                return cRef;
            }

            BYTE* pb() const { return m_pb; }
            ULONG cb() const { return m_cb; }
            ULONG bytesRead() const { return m_cbRead; }

            void setBytesRead(ULONG cbRead) { m_cbRead = cbRead; }

        private:
            long m_cRef;
            BYTE *m_pb;
            ULONG m_cb;
            ULONG m_cbRead;
        };

        long m_cRef;
        QIODevice *m_stream;
        bool m_ownStream;
        DWORD m_workQueueId;
        QMutex m_mutex;

        void doRead()
        {
            bool readDone = true;
            IUnknown *pUnk = NULL;
            HRESULT    hr = m_currentReadResult->GetObject(&pUnk);
            if (SUCCEEDED(hr)) {
                //do actual read
                AsyncReadState *state =  static_cast<AsyncReadState*>(pUnk);
                ULONG cbRead;
                Read(state->pb(), state->cb() - state->bytesRead(), &cbRead);
                pUnk->Release();

                state->setBytesRead(cbRead + state->bytesRead());
                if (state->cb() > state->bytesRead() && !m_stream->atEnd()) {
                    readDone = false;
                }
            }

            if (readDone) {
                //now inform the original caller
                m_currentReadResult->SetStatus(hr);
                MFInvokeCallback(m_currentReadResult);
            }
        }


    private Q_SLOTS:
        void handleReadyRead()
        {
            doRead();
        }

    protected:
        void customEvent(QEvent *event)
        {
            if (event->type() != QEvent::User) {
                QObject::customEvent(event);
                return;
            }
            doRead();
        }
        IMFAsyncResult *m_currentReadResult;
    };
}


MFPlayerSession::MFPlayerSession(QObject *parent)
    : m_parent(parent)
    , m_cRef(1)
    , m_session(0)
    , m_presentationClock(0)
    , m_rateControl(0)
    , m_rateSupport(0)
    , m_volumeControl(0)
    , m_netsourceStatistics(0)
    , m_hCloseEvent(0)
    , m_pendingRate(1)
    , m_volume(1)
    , m_muted(false)
    , m_status(QMediaPlayer::NoMedia)
    , m_scrubbing(false)
    , m_restoreRate(1)
    , m_mediaTypes(0)
{
    m_hCloseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_sourceResolver = new SourceResolver(this);
    QObject::connect(m_sourceResolver, SIGNAL(mediaSourceReady()), this, SLOT(handleMediaSourceReady()));
    QObject::connect(m_sourceResolver, SIGNAL(error(long)), this, SLOT(handleSourceError(long)));
    QObject::connect(this, SIGNAL(sessionEvent(IMFMediaEvent *)), this, SLOT(handleSessionEvent(IMFMediaEvent *)));

    m_pendingState = NoPending;
    ZeroMemory(&m_state, sizeof(m_state));
    m_state.command = CmdStop;
    m_state.prevCmd = CmdNone;
    m_state.rate = 1.0f;
    ZeroMemory(&m_request, sizeof(m_request));
    m_request.command = CmdNone;
    m_request.prevCmd = CmdNone;
    m_request.rate = 1.0f;

    createSession();
    PropVariantInit(&m_varStart);
    m_varStart.vt = VT_I8;
    m_varStart.uhVal.QuadPart = 0;
}

void MFPlayerSession::shutdown()
{
    clear();
    HRESULT hr = S_OK;
    if (m_session) {
        hr = m_session->Close();
        if (SUCCEEDED(hr)) {
            DWORD dwWaitResult = WaitForSingleObject(m_hCloseEvent, 5000);
            if (dwWaitResult == WAIT_TIMEOUT) {
                qWarning() << "session close time out!";
            }
        }
    }

    m_sourceResolver->shutdown();
    m_sourceResolver->Release();
    m_sourceResolver = 0;

    if (SUCCEEDED(hr)) {
        if (m_session)
            m_session->Shutdown();
    }

    if (m_session)
        m_session->Release();

    CloseHandle(m_hCloseEvent);
}


void MFPlayerSession::load(const QMediaContent &media, QIODevice *stream)
{
#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "load";
#endif
    clear();
    QMediaResourceList resources = media.resources();

    if (m_status == QMediaPlayer::LoadingMedia)
        m_sourceResolver->cancel();

    if (resources.isEmpty() && !stream) {
        m_sourceResolver->shutdown();
        changeStatus(QMediaPlayer::NoMedia);
    } else if (stream && (!stream->isReadable())) {
        changeStatus(QMediaPlayer::InvalidMedia);
        emit error(QMediaPlayer::ResourceError, tr("Invalid stream source!"), true);
    } else {
        changeStatus(QMediaPlayer::LoadingMedia);
        m_sourceResolver->load(resources, stream);
    }
    emit positionChanged(position());
}

void MFPlayerSession::handleSourceError(long hr)
{
    QString errorString;
    QMediaPlayer::Error errorCode = QMediaPlayer::ResourceError;
    switch (hr) {
    case QMediaPlayer::FormatError:
        errorCode = QMediaPlayer::FormatError;
        errorString = tr("Attempting to play invalid Qt resource");
        break;
    case NS_E_FILE_NOT_FOUND:
        errorString = tr("The system cannot find the file specified.");
        break;
    case NS_E_SERVER_NOT_FOUND:
        errorString = tr("The specified server could not be found.");
        break;
    default:
        errorString = tr("Failed to load source.");
        break;
    }
    changeStatus(QMediaPlayer::InvalidMedia);
    emit error(errorCode, errorString, true);
}

void MFPlayerSession::handleMediaSourceReady()
{
    if (!m_sourceResolver)
        return;

    if (QMediaPlayer::LoadingMedia != m_status)
        return;
#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "handleMediaSourceReady";
#endif
    HRESULT hr = S_OK;
    IMFPresentationDescriptor* sourcePD;
    IMFMediaSource* mediaSource = m_sourceResolver->mediaSource();
    hr = mediaSource->CreatePresentationDescriptor(&sourcePD);
    if (SUCCEEDED(hr)) {
        m_duration = 0;
        static_cast<MFPlayerService*>(m_parent)->metaDataControl()->updateSource(sourcePD, mediaSource);
        sourcePD->GetUINT64(MF_PD_DURATION, &m_duration);
        //convert from 100 nanosecond to milisecond
        emit durationUpdate(qint64(m_duration / 10000));
        setupPlaybackTopology(mediaSource, sourcePD);
    } else {
        changeStatus(QMediaPlayer::InvalidMedia);
        emit error(QMediaPlayer::ResourceError, tr("Can't create presentation descriptor!"), true);
    }
    sourcePD->Release();
}

void MFPlayerSession::setupPlaybackTopology(IMFMediaSource *source, IMFPresentationDescriptor *sourcePD)
{
    HRESULT hr = S_OK;
    // Get the number of streams in the media source.
    DWORD cSourceStreams = 0;
    hr = sourcePD->GetStreamDescriptorCount(&cSourceStreams);
    if (FAILED(hr)) {
        changeStatus(QMediaPlayer::UnknownMediaStatus);
        emit error(QMediaPlayer::ResourceError, tr("Failed to get stream count"), true);
        return;
    }

    IMFTopology *topology;
    hr = MFCreateTopology(&topology);
    if (FAILED(hr)) {
        changeStatus(QMediaPlayer::UnknownMediaStatus);
        emit error(QMediaPlayer::ResourceError, tr("Failed to create topology!"), true);
        return;
    }

    // For each stream, create the topology nodes and add them to the topology.
    DWORD succeededCount = 0;
    for (DWORD i = 0; i < cSourceStreams; i++)
    {
        BOOL fSelected = FALSE;
        IMFStreamDescriptor *streamDesc = NULL;

        HRESULT hr = sourcePD->GetStreamDescriptorByIndex(i, &fSelected, &streamDesc);
        if (SUCCEEDED(hr)) {
            MediaType mediaType = Unknown;
            IMFTopologyNode *sourceNode = addSourceNode(topology, source, sourcePD, streamDesc);
            if (sourceNode) {
                IMFTopologyNode *outputNode = addOutputNode(streamDesc, mediaType, topology, 0);
                if (outputNode) {
                    hr = sourceNode->ConnectOutput(0, outputNode, 0);
                    if (FAILED(hr)) {
                        emit error(QMediaPlayer::FormatError, tr("Unable to play some stream"), false);
                    }
                    else {
                        succeededCount++;
                        m_mediaTypes |= mediaType;
                        switch (mediaType) {
                        case Audio:
                            emit audioAvailable();
                            break;
                        case Video:
                            emit videoAvailable();
                            break;
                        }
                    }
                    outputNode->Release();
                }
                sourceNode->Release();
            }
            streamDesc->Release();
        }
    }

    if (succeededCount == 0) {
        changeStatus(QMediaPlayer::InvalidMedia);
        emit error(QMediaPlayer::ResourceError, tr("Unable to play"), true);
    } else {
        hr = m_session->SetTopology(MFSESSION_SETTOPOLOGY_IMMEDIATE, topology);
        if (FAILED(hr)) {
            changeStatus(QMediaPlayer::UnknownMediaStatus);
            emit error(QMediaPlayer::ResourceError, tr("Failed to set topology!"), true);
        }
    }
    topology->Release();
}

IMFTopologyNode* MFPlayerSession::addSourceNode(IMFTopology* topology, IMFMediaSource* source,
                                                IMFPresentationDescriptor* presentationDesc, IMFStreamDescriptor *streamDesc)
{
    IMFTopologyNode *node = NULL;
    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &node);
    if (SUCCEEDED(hr)) {
        hr = node->SetUnknown(MF_TOPONODE_SOURCE, source);
        if (SUCCEEDED(hr)) {
            hr = node->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, presentationDesc);
            if (SUCCEEDED(hr)) {
                hr = node->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, streamDesc);
                if (SUCCEEDED(hr)) {
                    hr = topology->AddNode(node);
                    if (SUCCEEDED(hr))
                        return node;
                }
            }
        }
        node->Release();
    }
    return NULL;
}

IMFTopologyNode* MFPlayerSession::addOutputNode(IMFStreamDescriptor *streamDesc, MediaType& mediaType, IMFTopology* topology, DWORD sinkID)
{
    IMFTopologyNode *node = NULL;
    HRESULT hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &node);
    if (FAILED(hr))
        return NULL;
    node->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);

    mediaType = Unknown;
    IMFMediaTypeHandler *handler = NULL;
    hr = streamDesc->GetMediaTypeHandler(&handler);
    if (SUCCEEDED(hr)) {
        GUID guidMajorType;
        hr = handler->GetMajorType(&guidMajorType);
        if (SUCCEEDED(hr)) {
            IMFActivate *activate = NULL;
            MFPlayerService *service = static_cast<MFPlayerService*>(m_parent);
            if (MFMediaType_Audio == guidMajorType) {
                mediaType = Audio;
                activate = service->audioEndpointControl()->currentActivate();
            } else if (MFMediaType_Video == guidMajorType) {
                mediaType = Video;
                if (service->videoRendererControl()) {
                    activate = service->videoRendererControl()->currentActivate();
#ifndef Q_WS_SIMULATOR
                } else if (service->videoWindowControl()) {
                    activate = service->videoWindowControl()->currentActivate();
#endif
                } else {
                    qWarning() << "no videoWindowControl or videoRendererControl, unable to add output node for video data";
                }
            } else {
                // Unknown stream type.
                emit error(QMediaPlayer::FormatError, tr("Unknown stream type"), false);
            }

            if (activate) {
                hr = node->SetObject(activate);
                if (SUCCEEDED(hr)) {
                    hr = node->SetUINT32(MF_TOPONODE_STREAMID, sinkID);
                    if (SUCCEEDED(hr)) {
                        if (SUCCEEDED(topology->AddNode(node)))
                            return node;
                    }
                }
            }
        }
    }
    node->Release();
    return NULL;
}

void MFPlayerSession::stop(bool immediate)
{
#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "stop";
#endif
    if (!immediate && m_pendingState != NoPending) {
        m_request.setCommand(CmdStop);
    } else {
        if (m_state.command == CmdStop)
            return;

        if (m_scrubbing)
            scrub(false);

        if (SUCCEEDED(m_session->Stop())) {
            m_state.setCommand(CmdStop);
            m_pendingState = CmdPending;
            if (m_status != QMediaPlayer::EndOfMedia) {
                m_varStart.vt = VT_I8;
                m_varStart.hVal.QuadPart = 0;
            }
        } else {
            emit error(QMediaPlayer::ResourceError, tr("failed to stop"), true);
        }
    }
}

void MFPlayerSession::start()
{
    switch (m_status) {
    case QMediaPlayer::EndOfMedia:
        m_varStart.hVal.QuadPart = 0;
        //since it must be loaded already, just fallthrough
    case QMediaPlayer::LoadedMedia:
        changeStatus(QMediaPlayer::BufferedMedia);
        return;
    }

#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "start";
#endif

    if (m_pendingState != NoPending) {
        m_request.setCommand(CmdStart);
    } else {
        if (m_state.command == CmdStart)
            return;

        if (m_scrubbing)
            scrub(false);

        if (SUCCEEDED(m_session->Start(&GUID_NULL, &m_varStart))) {
            m_state.setCommand(CmdStart);
            m_pendingState = CmdPending;
            PropVariantInit(&m_varStart);
        } else {
            emit error(QMediaPlayer::ResourceError, tr("failed to start playback"), true);
        }
    }
}

void MFPlayerSession::pause()
{
#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "pause";
#endif
    if (m_pendingState != NoPending) {
        m_request.setCommand(CmdPause);
    } else {
        if (m_state.command == CmdPause)
            return;
        if (SUCCEEDED(m_session->Pause())) {
            m_state.setCommand(CmdPause);
            m_pendingState = CmdPending;
        } else {
            emit error(QMediaPlayer::ResourceError, tr("failed to pause"), false);
        }
    }
}

void MFPlayerSession::changeStatus(QMediaPlayer::MediaStatus newStatus)
{
    if (m_status == newStatus)
        return;
#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "MFPlayerSession::changeStatus" << newStatus;
#endif
    m_status = newStatus;
    emit statusChanged();
}

QMediaPlayer::MediaStatus MFPlayerSession::status() const
{
    return m_status;
}

void MFPlayerSession::createSession()
{
    Q_ASSERT(m_session == NULL);
    HRESULT hr = MFCreateMediaSession(NULL, &m_session);
    if (FAILED(hr)) {
        changeStatus(QMediaPlayer::UnknownMediaStatus);
        emit error(QMediaPlayer::ResourceError, tr("Unable to create mediasession"), true);
    }

    hr = m_session->BeginGetEvent(this, m_session);

    if (FAILED(hr)) {
        changeStatus(QMediaPlayer::UnknownMediaStatus);
        emit error(QMediaPlayer::ResourceError, tr("Unable to pulling session events"), false);
    }
}

qint64 MFPlayerSession::position()
{
    if (m_request.command == CmdSeek || m_request.command == CmdSeekResume)
        return m_request.start;

    if (m_pendingState == SeekPending)
        return m_state.start;

    if (m_state.command == CmdStop)
        return qint64(m_varStart.hVal.QuadPart / 10000);

    if (m_presentationClock) {
        MFTIME time, sysTime;
        if (FAILED(m_presentationClock->GetCorrelatedTime(0, &time, &sysTime)))
            return 0;
        return qint64(time / 10000);
    }
    return 0;
}

void MFPlayerSession::setPosition(qint64 position)
{
#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "setPosition";
#endif
    if (m_pendingState != NoPending) {
        m_request.setCommand(CmdSeek);
        m_request.start = position;
    } else {
        setPositionInternal(position, CmdNone);
    }
}

void MFPlayerSession::setPositionInternal(qint64 position, Command requestCmd)
{
    if (m_status == QMediaPlayer::EndOfMedia)
        changeStatus(QMediaPlayer::LoadedMedia);
    if (m_state.command == CmdStop && requestCmd != CmdSeekResume) {
        m_varStart.vt = VT_I8;
        m_varStart.hVal.QuadPart = LONGLONG(position * 10000);
        return;
    }

    if (m_state.command == CmdPause)
        scrub(true);

#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "setPositionInternal";
#endif

    PROPVARIANT varStart;
    varStart.vt = VT_I8;
    varStart.hVal.QuadPart = LONGLONG(position * 10000);
    if (SUCCEEDED(m_session->Start(NULL, &varStart)))
    {
        PropVariantInit(&m_varStart);
        // Store the pending state.
        m_state.setCommand(CmdStart);
        m_state.start = position;
        m_pendingState = SeekPending;
    } else {
        emit error(QMediaPlayer::ResourceError, tr("failed to seek"), true);
    }
}

qreal MFPlayerSession::playbackRate() const
{
    if (m_pendingState != NoPending)
        return m_request.rate;
    return m_state.rate;
}

void MFPlayerSession::setPlaybackRate(qreal rate)
{
    if (m_scrubbing) {
        m_restoreRate = rate;
        return;
    }
    setPlaybackRateInternal(rate);
}

void MFPlayerSession::setPlaybackRateInternal(qreal rate)
{
    if (rate == m_request.rate)
        return;

    m_pendingRate = rate;
    if (!m_rateSupport)
        return;

#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "setPlaybackRate";
#endif
    BOOL isThin = FALSE;

    //from MSDN http://msdn.microsoft.com/en-us/library/aa965220%28v=vs.85%29.aspx
    //Thinning applies primarily to video streams.
    //In thinned mode, the source drops delta frames and deliver only key frames.
    //At very high playback rates, the source might skip some key frames (for example, deliver every other key frame).

    if (FAILED(m_rateSupport->IsRateSupported(FALSE, rate, NULL))) {
        isThin = TRUE;
        if (FAILED(m_rateSupport->IsRateSupported(isThin, rate, NULL))) {
            qWarning() << "unable to set playbackrate = " << rate;
        }
    }
    if (m_pendingState != NoPending) {
        m_request.rate = rate;
        m_request.isThin = isThin;
        // Remember the current transport state (play, paused, etc), so that we
        // can restore it after the rate change, if necessary. However, if
        // anothercommand is already pending, that one takes precedent.
        if (m_request.command == CmdNone)
            m_request.setCommand(m_state.command);
    } else {
        //No pending operation. Commit the new rate.
        commitRateChange(rate, isThin);
    }
}

void MFPlayerSession::commitRateChange(qreal rate, BOOL isThin)
{
#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "commitRateChange";
#endif
    Q_ASSERT(m_pendingState == NoPending);
    MFTIME  hnsSystemTime = 0;
    MFTIME  hnsClockTime = 0;
    Command cmdNow = m_state.command;
    // Allowed rate transitions:
    // Positive <-> negative:   Stopped
    // Negative <-> zero:       Stopped
    // Postive <-> zero:        Paused or stopped
    if ((rate > 0 && m_state.rate <= 0) || (rate < 0 && m_state.rate >= 0)) {
        // Transition to stopped.
        if (cmdNow == CmdStart) {
            // Get the current clock position. This will be the restart time.
            m_presentationClock->GetCorrelatedTime(0, &hnsClockTime, &hnsSystemTime);
            Q_ASSERT(hnsSystemTime != 0);

            // Stop and set the rate
            stop();

            //Cache Request: Restart from stop.
            m_request.setCommand(CmdSeekResume);
            m_request.start = hnsClockTime / 10000;
        } else if (cmdNow == CmdPause) {
            // The current state is paused.
            // For this rate change, the session must be stopped. However, the
            // session cannot transition back from stopped to paused.
            // Therefore, this rate transition is not supported while paused.
            if (rate < 0 || m_state.rate < 0) {
                qWarning() << "Unable to change rate from positive to negative or vice versa in paused state";
                return;
            }
        }
    } else if (rate == 0 && m_state.rate != 0) {
        if (cmdNow != CmdPause) {
            // Transition to paused.
            // This transisition requires the paused state.
            // Pause and set the rate.
            pause();

            // Request: Switch back to current state.
            m_request.setCommand(cmdNow);
        }
    }

    // Set the rate.
    if (FAILED(m_rateControl->SetRate(isThin, rate))) {
        qWarning() << "failed to set playbackrate = " << rate;
        return;
    }

    // Adjust our current rate and requested rate.
    m_pendingRate = m_request.rate = m_state.rate = rate;

}

void MFPlayerSession::scrub(bool enableScrub)
{
    if (m_scrubbing == enableScrub)
        return;

    m_scrubbing = enableScrub;

    if (!canScrub()) {
        if (!enableScrub)
            m_pendingRate = m_restoreRate;
        return;
    }

    if (enableScrub)  {
        // Enter scrubbing mode. Cache the rate.
        m_restoreRate = m_request.rate;
        setPlaybackRateInternal(0.0f);
    } else {
        // Leaving scrubbing mode. Restore the old rate.
        setPlaybackRateInternal(m_restoreRate);
    }
}

int MFPlayerSession::volume() const
{
    return m_volume;
}

void MFPlayerSession::setVolume(int volume)
{
    if (m_volume == volume)
        return;
    m_volume = volume;
    if (m_volumeControl)
        m_volumeControl->SetMasterVolume(m_volume * 0.01f);
    emit volumeChanged(m_volume);
}

bool MFPlayerSession::isMuted() const
{
    return m_muted;
}

void MFPlayerSession::setMuted(bool muted)
{
    if (m_muted == muted)
        return;
    m_muted = muted;
    if (m_volumeControl)
        m_volumeControl->SetMute(BOOL(m_muted));
    emit mutedChanged(m_muted);
}

int MFPlayerSession::bufferStatus()
{
    if (!m_netsourceStatistics)
        return 0;
    PROPVARIANT var;
    PROPERTYKEY key;
    key.fmtid = MFNETSOURCE_STATISTICS;
    key.pid = MFNETSOURCE_BUFFERPROGRESS_ID;
    int progress = -1;
    if (SUCCEEDED(m_netsourceStatistics->GetValue(key, &var))) {
        progress = var.lVal;
    }
    PropVariantClear(&var);

#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "bufferStatus: progress = " << progress;
#endif

    return progress;
}

QMediaTimeRange MFPlayerSession::availablePlaybackRanges()
{
    if (!m_netsourceStatistics)
        return QMediaTimeRange();

    qint64 start = 0, end = 0;
    PROPVARIANT var;
    PROPERTYKEY key;
    key.fmtid = MFNETSOURCE_STATISTICS;
    key.pid = MFNETSOURCE_SEEKRANGESTART_ID;
    if (SUCCEEDED(m_netsourceStatistics->GetValue(key, &var))) {
        start = qint64(var.uhVal.QuadPart / 10000);
        key.pid = MFNETSOURCE_SEEKRANGEEND_ID;
        if (SUCCEEDED(m_netsourceStatistics->GetValue(key, &var))) {
            end = qint64(var.uhVal.QuadPart / 10000);
        }
    }
    PropVariantClear(&var);
    return QMediaTimeRange(start, end);
}

HRESULT MFPlayerSession::QueryInterface(REFIID riid, void** ppvObject)
{
    if (!ppvObject)
        return E_POINTER;
    if (riid == IID_IMFAsyncCallback) {
        *ppvObject = static_cast<IMFAsyncCallback*>(this);
    } else if (riid == IID_IUnknown) {
        *ppvObject = static_cast<IUnknown*>(this);
    } else {
        *ppvObject =  NULL;
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}

ULONG MFPlayerSession::AddRef(void)
{
    return InterlockedIncrement(&m_cRef);
}

ULONG MFPlayerSession::Release(void)
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0) {
        this->deleteLater();
    }
    return cRef;
}

HRESULT MFPlayerSession::Invoke(IMFAsyncResult *pResult)
{
    if (pResult->GetStateNoAddRef() != m_session)
        return S_OK;

    IMFMediaEvent *pEvent = NULL;
    // Get the event from the event queue.
    HRESULT hr = m_session->EndGetEvent(pResult, &pEvent);
    if (FAILED(hr)) {
        return S_OK;
    }

    MediaEventType meType = MEUnknown;
    hr = pEvent->GetType(&meType);
    if (FAILED(hr)) {
        pEvent->Release();
        return S_OK;
    }

    if (meType == MESessionClosed) {
        SetEvent(m_hCloseEvent);
        return S_OK;
    } else {
        hr = m_session->BeginGetEvent(this, m_session);
        if (FAILED(hr)) {
            pEvent->Release();
            return S_OK;
        }
    }

    emit sessionEvent(pEvent);
    return S_OK;
}

void MFPlayerSession::handleSessionEvent(IMFMediaEvent *sessionEvent)
{
    HRESULT hrStatus = S_OK;
    HRESULT hr = sessionEvent->GetStatus(&hrStatus);
    if (FAILED(hr)) {
        sessionEvent->Release();
        return;
    }

    MediaEventType meType = MEUnknown;
    hr = sessionEvent->GetType(&meType);

#ifdef DEBUG_MEDIAFOUNDATION
    if (FAILED(hrStatus))
        qDebug() << "handleSessionEvent: MediaEventType = " << meType << "Failed";
    else
        qDebug() << "handleSessionEvent: MediaEventType = " << meType;
#endif

    switch (meType) {
    case MENonFatalError: {
            PROPVARIANT var;
            PropVariantInit(&var);
            sessionEvent->GetValue(&var);
            qWarning() << "handleSessionEvent: non fatal error = " << var.ulVal;
            PropVariantClear(&var);
            emit error(QMediaPlayer::ResourceError, tr("media session non-fatal error!"), false);
        }
        break;
    case MESourceUnknown:
        changeStatus(QMediaPlayer::InvalidMedia);
    case MEError:
        qWarning() << "handleSessionEvent: serious error = " << hrStatus;
        emit error(QMediaPlayer::ResourceError, tr("media session serious error!"), true);
        break;
    case MESessionRateChanged:
        // If the rate change succeeded, we've already got the rate
        // cached. If it failed, try to get the actual rate.
        if (FAILED(hrStatus)) {
            PROPVARIANT var;
            PropVariantInit(&var);
            if (SUCCEEDED(sessionEvent->GetValue(&var)) && (var.vt == VT_R4))    {
                m_state.rate = var.fltVal;
            }
            emit playbackRateChanged(playbackRate());
        }
        break;
    case MESessionScrubSampleComplete :
        if (m_scrubbing)
            updatePendingCommands(CmdStart);
        break;
    case MESessionStarted:
        if (!m_scrubbing)
            updatePendingCommands(CmdStart);
        break;
    case MESessionStopped:
        if (m_status != QMediaPlayer::EndOfMedia) {
            m_varStart.vt = VT_I8;
            m_varStart.hVal.QuadPart = 0;

            //only change to loadedMedia when not loading a new media source
            if (m_status != QMediaPlayer::LoadingMedia) {
                changeStatus(QMediaPlayer::LoadedMedia);
            }
        }
        updatePendingCommands(CmdStop);
        break;
    case MESessionPaused:
        updatePendingCommands(CmdPause);
        break;
    case MEReconnectStart:
#ifdef DEBUG_MEDIAFOUNDATION
            qDebug() << "MEReconnectStart" << ((hrStatus == S_OK) ? "OK" : "Failed");
#endif
        break;
    case MEReconnectEnd:
#ifdef DEBUG_MEDIAFOUNDATION
            qDebug() << "MEReconnectEnd" << ((hrStatus == S_OK) ? "OK" : "Failed");
#endif
        break;
    }

    if (FAILED(hrStatus)) {
        sessionEvent->Release();
        return;
    }

    switch (meType) {
    case MEBufferingStarted:
        changeStatus(QMediaPlayer::StalledMedia);
        emit bufferStatusChanged(bufferStatus());
        break;
    case MEBufferingStopped:
        changeStatus(QMediaPlayer::BufferedMedia);
        emit bufferStatusChanged(bufferStatus());
        break;
    case MEEndOfPresentation:
        stop();
        changeStatus(QMediaPlayer::EndOfMedia);
        m_varStart.vt = VT_I8;
        //keep reporting the final position after end of media
        m_varStart.hVal.QuadPart = m_duration;
        break;
    case MESessionEnded:
        m_pendingState = NoPending;
        m_state.command = CmdStop;
        m_state.prevCmd = CmdNone;
        m_request.command = CmdNone;
        m_request.prevCmd = CmdNone;
        break;
    case MEEndOfPresentationSegment:
        break;
    case MEAudioSessionVolumeChanged:
        if (m_volumeControl) {
            float currentVolume = 1;
            if (SUCCEEDED(m_volumeControl->GetMasterVolume(&currentVolume))) {
                if (currentVolume != m_volume) {
                    m_volume = currentVolume;
                    emit volumeChanged(int(m_volume * 100));
                }
            }
            BOOL currentMuted = FALSE;
            if (SUCCEEDED(m_volumeControl->GetMute(&currentMuted))) {
                if (currentMuted != BOOL(m_muted)) {
                    m_muted = bool(currentMuted);
                    emit mutedChanged(m_muted);
                }
            }
        }
        break;
    case MESessionTopologySet: {
            if (SUCCEEDED(MFGetService(m_session, MR_POLICY_VOLUME_SERVICE, IID_PPV_ARGS(&m_volumeControl)))) {
                m_volumeControl->SetMasterVolume(m_volume);
                m_volumeControl->SetMute(m_muted);
            }
            DWORD dwCharacteristics = 0;
            m_sourceResolver->mediaSource()->GetCharacteristics(&dwCharacteristics);
            emit seekableUpdate(MFMEDIASOURCE_CAN_SEEK & dwCharacteristics);
            changeStatus(QMediaPlayer::LoadedMedia);
        }
        break;
    case MESessionTopologyStatus: {
            UINT32 status;
            if (SUCCEEDED(sessionEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &status))) {
                if (status == MF_TOPOSTATUS_READY) {
                    IMFClock* clock;
                    if (SUCCEEDED(m_session->GetClock(&clock))) {
                        clock->QueryInterface(IID_IMFPresentationClock, (void**)(&m_presentationClock));
                        clock->Release();
                    }

                    if (SUCCEEDED(MFGetService(m_session, MF_RATE_CONTROL_SERVICE, IID_PPV_ARGS(&m_rateControl)))) {
                        if (SUCCEEDED(MFGetService(m_session, MF_RATE_CONTROL_SERVICE, IID_PPV_ARGS(&m_rateSupport)))) {
                            if ((m_mediaTypes & Video) == Video
                                && SUCCEEDED(m_rateSupport->IsRateSupported(TRUE, 0, NULL)))
                                m_canScrub = true;
                        }
                        BOOL isThin = FALSE;
                        float rate = 1;
                        if (SUCCEEDED(m_rateControl->GetRate(&isThin, &rate))) {
                            if (m_pendingRate != rate) {
                                m_state.rate = m_request.rate = rate;
                                setPlaybackRate(m_pendingRate);
                            }
                        }
                    }
                    MFGetService(m_session, MFNETSOURCE_STATISTICS_SERVICE, IID_PPV_ARGS(&m_netsourceStatistics));
                }
            }
        }
        break;
    }

    sessionEvent->Release();
}

void MFPlayerSession::updatePendingCommands(Command command)
{
    emit positionChanged(position());
    if (m_state.command != command || m_pendingState == NoPending)
        return;

    // The current pending command has completed.
    if (m_pendingState == SeekPending && m_state.prevCmd == CmdPause) {
        m_pendingState = NoPending;
        //if we have pending seek request,
        //then we just keep current state to paused and continue the seek request,
        //otherwise we will restore to pause state
        if (m_request.command == CmdSeek) {
            m_state.setCommand(CmdPause);
        } else {
            pause();
            return;
        }
    }

    m_pendingState = NoPending;

    //First look for rate changes.
    if (m_request.rate != m_state.rate) {
        commitRateChange(m_request.rate, m_request.isThin);
    }

    // Now look for new requests.
    if (m_pendingState == NoPending) {
        switch (m_request.command) {
        case CmdStart:
            start();
            break;
        case CmdPause:
            pause();
            break;
        case CmdStop:
            stop();
            break;
        case CmdSeek:
        case CmdSeekResume:
            setPositionInternal(m_request.start, m_request.command);
        }
        m_request.setCommand(CmdNone);
    }

}

bool MFPlayerSession::canScrub() const
{
    return m_canScrub && m_rateSupport && m_rateControl;
}

void MFPlayerSession::clear()
{
#ifdef DEBUG_MEDIAFOUNDATION
    qDebug() << "MFPlayerSession::clear";
#endif
    m_mediaTypes = 0;
    m_canScrub = false;

    m_pendingState = NoPending;
    m_state.command = CmdStop;
    m_state.prevCmd = CmdNone;
    m_request.command = CmdNone;
    m_request.prevCmd = CmdNone;

    if (m_presentationClock) {
        m_presentationClock->Release();
        m_presentationClock = NULL;
    }
    if (m_rateControl) {
        m_rateControl->Release();
        m_rateControl = NULL;
    }
    if (m_rateSupport) {
        m_rateSupport->Release();
        m_rateSupport = NULL;
    }
    if (m_volumeControl) {
        m_volumeControl->Release();
        m_volumeControl = NULL;
    }
    if (m_netsourceStatistics) {
        m_netsourceStatistics->Release();
        m_netsourceStatistics = NULL;
    }
}
