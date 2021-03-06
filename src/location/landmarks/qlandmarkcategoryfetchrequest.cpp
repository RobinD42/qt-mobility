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

#include "qlandmarkcategoryfetchrequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmarkcategory.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkCategoryFetchRequest
    \brief The QLandmarkCategoryFetchRequest class allows a client to asynchronously
    request a list of categories from a landmark manager.

    For a QLandmarkCategoryFetchRequest, the resultsAvailable() signal will be emitted
    as resultant categories are found (these are retrievable by calling categories()),
     as well as if an overall operation error occurred(which may be retrieved by calling
     QLandmarkAbstractRequest::error()).

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.

    \inmodule QtLocation
    \since 1.1

    \ingroup landmarks-request
*/

/*!
    Creates a new category fetch request object with the given \a manager and \a parent.
*/
QLandmarkCategoryFetchRequest::QLandmarkCategoryFetchRequest(QLandmarkManager *manager, QObject *parent)
    :QLandmarkAbstractRequest(new QLandmarkCategoryFetchRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkCategoryFetchRequest::~QLandmarkCategoryFetchRequest()
{
}

/*!
    Returns the list of categories that have been found during the
    request.
    \since 1.1
*/
QList<QLandmarkCategory> QLandmarkCategoryFetchRequest::categories() const
{
    Q_D(const QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->categories;
}

/*!
    Returns the sorting of the categories.
    By default the sorting is case insensitive and in ascending order
    according to the category name.
    \since 1.1
*/
QLandmarkNameSort QLandmarkCategoryFetchRequest::sorting() const
{
    Q_D(const QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->sorting;
}

/*!
    Sets \a nameSort to specify the sorting of the returned categories.
    \since 1.1
*/
void QLandmarkCategoryFetchRequest::setSorting(const QLandmarkNameSort &nameSort)
{
    Q_D(QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->sorting = nameSort;
}

/*!
    Returns the maximum number of categories to be returned.  By default the limit
    is -1 indicating that all categories should be retrieved.
    \since 1.1
*/
int QLandmarkCategoryFetchRequest::limit() const
{
    Q_D(const QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->limit;
}

/*!
    Sets the maximum number of categories to be returned to \a limit.
    A limit of -1 will retrieve all categories.

    (A limit of 0 will retrieve no categories.)
    \since 1.1
 */
void QLandmarkCategoryFetchRequest::setLimit(int limit)
{
    Q_D(QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->limit = limit;
}

/*!
    Returns the index offset for the request.  By default the offset is 0.
    The offset determines the first index which is retrieved, it is generally
    used in conjunction with limit() to facilitate paging.

    For example, if there are 10 categories in the landmark store, setting the offset
    to 2 and limit to 5 will retrieve the 3rd to 7th categories inclusively.  (The order of categories
    is specified by the sorting field).
    \since 1.1
*/
int QLandmarkCategoryFetchRequest::offset() const
{
    Q_D(const QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    return d->offset;
}

/*!
    Sets the index \a offset for the request.
    \since 1.1
*/
void QLandmarkCategoryFetchRequest::setOffset(int offset) {
    Q_D(QLandmarkCategoryFetchRequest);
    QMutexLocker ml(&d->mutex);
    d->offset = offset;
}

#include "moc_qlandmarkcategoryfetchrequest.cpp"

QTM_END_NAMESPACE
