
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVEFEEDBACK_H
#define QDECLARATIVEFEEDBACK_H

#include <QtDeclarative/qdeclarative.h>
#include <qfeedbackeffect.h>

QTM_USE_NAMESPACE

// This is a wrapper for QFeedbackEffect, to rewrite enum values
class QDeclarativeFeedbackEffect: public QObject
{
    Q_OBJECT

    Q_ENUMS(ThemeEffect)
    Q_ENUMS(Duration)
    Q_ENUMS(State)
    Q_ENUMS(ErrorType)

public:
    enum ThemeEffect {
        Basic = QFeedbackEffect::ThemeBasic,
        Sensitive = QFeedbackEffect::ThemeSensitive,
        BasicButton = QFeedbackEffect::ThemeBasicButton,
        SensitiveButton = QFeedbackEffect::ThemeSensitiveButton,
        BasicKeypad = QFeedbackEffect::ThemeBasicKeypad,
        SensitiveKeypad = QFeedbackEffect::ThemeSensitiveKeypad,
        BasicSlider = QFeedbackEffect::ThemeBasicSlider,
        SensitiveSlider = QFeedbackEffect::ThemeSensitiveSlider,
        BasicItem = QFeedbackEffect::ThemeBasicItem,
        SensitiveItem = QFeedbackEffect::ThemeSensitiveItem,
        ItemScroll = QFeedbackEffect::ThemeItemScroll,
        ItemPick = QFeedbackEffect::ThemeItemPick,
        ItemDrop = QFeedbackEffect::ThemeItemDrop,
        ItemMoveOver = QFeedbackEffect::ThemeItemMoveOver,
        BounceEffect = QFeedbackEffect::ThemeBounceEffect,
        CheckBox = QFeedbackEffect::ThemeCheckBox,
        MultipleCheckBox = QFeedbackEffect::ThemeMultipleCheckBox,
        Editor = QFeedbackEffect::ThemeEditor,
        TextSelection = QFeedbackEffect::ThemeTextSelection,
        BlankSelection = QFeedbackEffect::ThemeBlankSelection,
        LineSelection = QFeedbackEffect::ThemeLineSelection,
        EmptyLineSelection = QFeedbackEffect::ThemeEmptyLineSelection,
        PopUp = QFeedbackEffect::ThemePopUp,
        PopupOpen = QFeedbackEffect::ThemePopupOpen,
        PopupClose = QFeedbackEffect::ThemePopupClose,
        Flick = QFeedbackEffect::ThemeFlick,
        StopFlick = QFeedbackEffect::ThemeStopFlick,
        MultiPointTouchActivate = QFeedbackEffect::ThemeMultiPointTouchActivate,
        RotateStep = QFeedbackEffect::ThemeRotateStep,
        LongPress = QFeedbackEffect::ThemeLongPress,
        PositiveTacticon = QFeedbackEffect::ThemePositiveTacticon,
        NeutralTacticon = QFeedbackEffect::ThemeNeutralTacticon,
        NegativeTacticon = QFeedbackEffect::ThemeNegativeTacticon,

        NumberOfThemeEffects = QFeedbackEffect::NumberOfThemeEffects,
        ThemeUser = QFeedbackEffect::ThemeUser
    };

    // These don't need rewriting, they're already nice
    enum Duration {
        Infinite = QFeedbackEffect::Infinite
    };

    enum State {
        Stopped = QFeedbackEffect::Stopped,
        Paused = QFeedbackEffect::Paused,
        Running = QFeedbackEffect::Running,
        Loading = QFeedbackEffect::Loading
    };

    enum ErrorType {
        UnknownError = QFeedbackEffect::UnknownError,
        DeviceBusy = QFeedbackEffect::DeviceBusy
    };

    QDeclarativeFeedbackEffect(QObject* parent) : QObject(parent)
    {

    }
};

QML_DECLARE_TYPE(QDeclarativeFeedbackEffect);


#endif // QDECLARATIVEFEEDBACK_H