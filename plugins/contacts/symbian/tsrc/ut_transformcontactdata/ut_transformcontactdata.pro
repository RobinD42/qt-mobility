######################################################################
# Automatically generated by qmake (2.01a) pe 27. helmi 08:49:52 2009
######################################################################

TEMPLATE = app
TARGET = 
QT += testlib
CONFIG += qtestlib
include(../tsrc.pri)
          
DEFINES += PBK_UNIT_TEST
DEPENDPATH += .
INCLUDEPATH += .

symbian:
{
    INCLUDEPATH += $$SYMBIAN_PATHS
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
    # Input
    HEADERS += $$SYMBIAN_HEADERS \
                ut_transformcontactdata.h
	
    SOURCES += $$SYMBIAN_SOURCES \
                ut_transformcontactdata.cpp

    CONFIG += mobility
    MOBILITY = contacts

    TARGET.CAPABILITY = ALL -TCB
  
    LIBS += $$SYMBIAN_LIBS
}
