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

#include "qgeoroutingmanagerengine.h"
#include "qgeoroutingmanagerengine_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRoutingManagerEngine

    \brief The QGeoRoutingManagerEngine class provides an interface and
    convenience methods to implementers of QGeoServiceProvider plugins who want
    to provide access to geographic routing information.


    \inmodule QtLocation
    \since 1.1

    \ingroup maps-impl

    Subclasses of QGeoRoutingManagerEngine need to provide an implementation of
    calculateRoute().

    In the default implementation, supportsRouteUpdates() returns false and
    updateRoute() returns a QGeoRouteReply object containing a
    QGeoRouteReply::UnsupportedOptionError.

    If the routing service supports updating routes as they are being
    travelled, the subclass should provide an implementation of updateRoute()
    and call setSupportsRouteUpdates(true) at some point in time before
    updateRoute() is called.

    The function setSupportsRouteUpdates() is one of several functions which
    configure the reported capabilities of the engine.  If the capabilities
    of an engine differ from the default values these functions should be
    used so that the reported capabilies are accurate.

    It is important that this is done before calculateRoute(), updateRoute()
    or any of the capability reporting functions are used to prevent
    incorrect or inconsistent behaviour.

    A subclass of QGeoRouteManagerEngine will often make use of a subclass
    fo QGeoRouteReply internally, in order to add any engine-specific
    data (such as a QNetworkReply object for network-based services) to the
    QGeoRouteReply instances used by the engine.

    \sa QGeoRoutingManager
*/

/*!
    Constructs a new engine with the specified \a parent, using \a parameters
    to pass any implementation specific data to the engine.
    \since 1.1
*/
QGeoRoutingManagerEngine::QGeoRoutingManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeoRoutingManagerEnginePrivate())
{
    Q_UNUSED(parameters)
}

/*!
    Destroys this engine.
*/
QGeoRoutingManagerEngine::~QGeoRoutingManagerEngine()
{
    delete d_ptr;
}

/*!
    Sets the name which this engine implementation uses to distinguish itself
    from the implementations provided by other plugins to \a managerName.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setManagerName(const QString &managerName)
{
    d_ptr->managerName = managerName;
}

/*!
    Returns the name which this engine implementation uses to distinguish
    itself from the implementations provided by other plugins.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
    \since 1.1
*/
QString QGeoRoutingManagerEngine::managerName() const
{
    return d_ptr->managerName;
}

/*!
    Sets the version of this engine implementation to \a managerVersion.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setManagerVersion(int managerVersion)
{
    d_ptr->managerVersion = managerVersion;
}

/*!
    Returns the version of this engine implementation.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
    \since 1.1
*/
int QGeoRoutingManagerEngine::managerVersion() const
{
    return d_ptr->managerVersion;
}

/*!
\fn QGeoRouteReply* QGeoRoutingManagerEngine::calculateRoute(const QGeoRouteRequest& request)

    Begins the calculation of the route specified by \a request.

    A QGeoRouteReply object will be returned, which can be used to manage the
    routing operation and to return the results of the operation.

    This engine and the returned QGeoRouteReply object will emit signals
    indicating if the operation completes or if errors occur.

    Once the operation has completed, QGeoRouteReply::routes can be used to
    retrieve the calculated route or routes.

    If \a request includes features which are not supported by this engine, as
    reported by the methods in this engine, then a
    QGeoRouteReply::UnsupportedOptionError will occur.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoRoutingManagerEngine::finished(),
    QGeoRoutingManagerEngine::error(), QGeoRouteReply::finished() or
    QGeoRouteReply::error() with deleteLater().
    \since 1.1
*/

/*!
    Begins the process of updating \a route based on the current position \a
    position.

    A QGeoRouteReply object will be returned, which can be used to manage the
    routing operation and to return the results of the operation.

    This engine and the returned QGeoRouteReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsRouteUpdates() returns false an
    QGeoRouteReply::UnsupportedOptionError will occur.

    Once the operation has completed, QGeoRouteReply::routes can be used to
    retrieve the updated route.

    The returned route could be entirely different to the original route,
    especially if \a position is far enough away from the initial route.
    Otherwise the route will be similar, although the remaining time and
    distance will be updated and any segments of the original route which
    have been traversed will be removed.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoRoutingManagerEngine::finished(),
    QGeoRoutingManagerEngine::error(), QGeoRouteReply::finished() or
    QGeoRouteReply::error() with deleteLater().
    \since 1.1
*/
QGeoRouteReply* QGeoRoutingManagerEngine::updateRoute(const QGeoRoute &route, const QGeoCoordinate &position)
{
    Q_UNUSED(route)
    Q_UNUSED(position)
    return new QGeoRouteReply(QGeoRouteReply::UnsupportedOptionError,
                              "The updating of routes is not supported by this service provider.", this);
}

/*!
    Sets whether this engine supports updating routes to \a supported.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it does not support updating routes.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setSupportsRouteUpdates(bool supported)
{
    d_ptr->supportsRouteUpdates = supported;
}

/*!
    Returns whether this engine supports updating routes.
    \since 1.1
*/
bool QGeoRoutingManagerEngine::supportsRouteUpdates() const
{
    return d_ptr->supportsRouteUpdates;
}

/*!
    Sets whether this engine supports request for alternative routes to \a supported.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it does not support alternative routes.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setSupportsAlternativeRoutes(bool supported)
{
    d_ptr->supportsAlternativeRoutes = supported;
}

/*!
    Returns whether this engine supports request for alternative routes.
    \since 1.1
*/
bool QGeoRoutingManagerEngine::supportsAlternativeRoutes() const
{
    return d_ptr->supportsAlternativeRoutes;
}

/*!
    Sets whether this engine supports request for excluding areas from routes to \a supported.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it does not support excluding areas.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setSupportsExcludeAreas(bool supported)
{
    d_ptr->supportsExcludeAreas = supported;
}

/*!
    Returns whether this engine supports the exclusion of areas from routes.
    \since 1.1
*/
bool QGeoRoutingManagerEngine::supportsExcludeAreas() const
{
    return d_ptr->supportsExcludeAreas;
}

/*!
    Sets the travel modes supported by this engine to \a travelModes.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it supports no travel modes at all.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setSupportedTravelModes(QGeoRouteRequest::TravelModes travelModes)
{
    d_ptr->supportedTravelModes = travelModes;
}

/*!
    Returns the travel modes supported by this engine.
    \since 1.1
*/
QGeoRouteRequest::TravelModes QGeoRoutingManagerEngine::supportedTravelModes() const
{
    return d_ptr->supportedTravelModes;
}

/*!
    Sets the types of features that this engine can take into account
    during route planning to \a featureTypes.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it supports no feature types at all.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setSupportedFeatureTypes(QGeoRouteRequest::FeatureTypes featureTypes)
{
    d_ptr->supportedFeatureTypes = featureTypes;
}

/*!
    Returns the types of features that this engine can take into account
    during route planning.
    \since 1.1
*/
QGeoRouteRequest::FeatureTypes QGeoRoutingManagerEngine::supportedFeatureTypes() const
{
    return d_ptr->supportedFeatureTypes;
}

/*!
    Sets the weightings which this engine can apply to different features
    during route planning to \a featureWeights.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it supports no feaure weights at all.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setSupportedFeatureWeights(QGeoRouteRequest::FeatureWeights featureWeights)
{
    d_ptr->supportedFeatureWeights = featureWeights;
    d_ptr->supportedFeatureWeights |= QGeoRouteRequest::NeutralFeatureWeight;
}

/*!
    Returns the weightings which this engine can apply to different features
    during route planning.
    \since 1.1
*/
QGeoRouteRequest::FeatureWeights QGeoRoutingManagerEngine::supportedFeatureWeights() const
{
    return d_ptr->supportedFeatureWeights;
}

/*!
    Sets the route optimizations supported by this engine to \a optimizations.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it supports no route optimizations at all.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setSupportedRouteOptimizations(QGeoRouteRequest::RouteOptimizations optimizations)
{
    d_ptr->supportedRouteOptimizations = optimizations;
}

/*!
    Returns the route optimizations supported by this engine.
    \since 1.1
*/
QGeoRouteRequest::RouteOptimizations QGeoRoutingManagerEngine::supportedRouteOptimizations() const
{
    return d_ptr->supportedRouteOptimizations;
}

/*!
    Sets the levels of detail for routing segments which can be
    requested by this engine to \a segmentDetails.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it supports no segment detail at all.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setSupportedSegmentDetails(QGeoRouteRequest::SegmentDetails segmentDetails)
{
    d_ptr->supportedSegmentDetails = segmentDetails;
}

/*!
    Returns the levels of detail for routing segments which can be
    requested by this engine.
    \since 1.1
*/
QGeoRouteRequest::SegmentDetails QGeoRoutingManagerEngine::supportedSegmentDetails() const
{
    return d_ptr->supportedSegmentDetails;
}

/*!
    Sets the levels of detail for navigation manuevers which can be
    requested by this engine to \a maneuverDetails.

    It is important that subclasses use this method to ensure that the engine
    reports its capabilities correctly.  If this function is not used the
    engine will report that it supports no maneuver details at all.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setSupportedManeuverDetails(QGeoRouteRequest::ManeuverDetails maneuverDetails)
{
    d_ptr->supportedManeuverDetails = maneuverDetails;
}

/*!
    Returns the levels of detail for navigation maneuvers which can be
    requested by this engine.
    \since 1.1
*/
QGeoRouteRequest::ManeuverDetails QGeoRoutingManagerEngine::supportedManeuverDetails() const
{
    return d_ptr->supportedManeuverDetails;
}

/*!
    Sets the locale to be used by the this manager to \a locale.

    If this routing manager supports returning addresses and instructions
    in different languages, they will be returned in the language of \a locale.

    The locale used defaults to the system locale if this is not set.
    \since 1.1
*/
void QGeoRoutingManagerEngine::setLocale(const QLocale &locale)
{
    d_ptr->locale = locale;
}

/*!
    Returns the locale used to hint to this routing manager about what
    language to use for addresses and instructions.
    \since 1.1
*/
QLocale QGeoRoutingManagerEngine::locale() const
{
    return d_ptr->locale;
}

/*!
\fn void QGeoRoutingManagerEngine::finished(QGeoRouteReply* reply)

This signal is emitted when \a reply has finished processing.

If reply::error() equals QGeoRouteReply::NoError then the processing
finished successfully.

This signal and QGeoRouteReply::finished() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
\since 1.1
*/

/*!
\fn void QGeoRoutingManagerEngine::error(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString)

This signal is emitted when an error has been detected in the processing of
\a reply.  The QGeoRoutingManagerEngine::finished() signal will probably follow.

The error will be described by the error code \a error.  If \a errorString is
not empty it will contain a textual description of the error.

This signal and QGeoRouteReply::error() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
\since 1.1
*/

/*******************************************************************************
*******************************************************************************/

QGeoRoutingManagerEnginePrivate::QGeoRoutingManagerEnginePrivate()
    : managerVersion(-1),
      supportsRouteUpdates(false),
      supportsAlternativeRoutes(false),
      supportsExcludeAreas(false)
{
}

QGeoRoutingManagerEnginePrivate::~QGeoRoutingManagerEnginePrivate() {}

#include "moc_qgeoroutingmanagerengine.cpp"

QTM_END_NAMESPACE
