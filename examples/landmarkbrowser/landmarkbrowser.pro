# #####################################################################
# Automatically generated by qmake (2.01a) Mon Jun 21 13:06:14 2010
# #####################################################################
TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += ../../src/location
include(../examples.pri)

# Input
SOURCES += main.cpp \
    landmarkbrowser.cpp \
    landmarkadddialog.cpp
HEADERS += landmarkbrowser.h \
    landmarkadddialog.h
CONFIG += mobility
FORMS += landmarkbrowser.ui \
    landmarkadddialog.ui
MOBILITY += location
samplegpx.path = $$QT_MOBILITY_PREFIX/bin
samplegpx.files = places.gpx
INSTALLS += samplegpx