######################################################################
#
# Mobility API project - common QMake settings
#
######################################################################

include($$PWD/features/utils.pri)

CONFIG(debug, debug|release) {
    WAS_IN_DEBUG=debug
} else {
    WAS_IN_DEBUG=release
}

include(staticconfig.pri)

symbian:contains(symbian_symbols_unfrozen,1) {
    #see configure.bat for details
    MMP_RULES+="EXPORTUNFROZEN"
}

mac {
    contains(QT_CONFIG, qt_framework):contains(TEMPLATE, lib) {
        #MacOSX always builds debug and release libs when using mac framework
        CONFIG+=$$WAS_IN_DEBUG
        CONFIG += debug_and_release build_all
    } else {
        !contains(QT_CONFIG,debug)|!contains(QT_CONFIG,release) {
            CONFIG -= debug_and_release debug release
            contains(QT_CONFIG,debug): CONFIG+=debug
            contains(QT_CONFIG,release): CONFIG+=release
        }
    }
}

#In Windows we want to build libraries in debug and release mode if the user
#didn't select a version - if Qt is build in debug_and_release
#this avoids problems for third party as qmake build debug by default
#If mobility selected debug_and_release but Qt only supports
#one version but not the other we silently disable the impossible combination
win32:contains(CONFIG_WIN32,build_all) {
    contains(QT_CONFIG,debug):contains(QT_CONFIG,release) {
        contains(TEMPLATE,.*lib) {
            CONFIG += $$WAS_IN_DEBUG
            CONFIG += debug_and_release build_all
        }
    } else {
        CONFIG -= debug_and_release debug release
        contains(QT_CONFIG,debug): CONFIG+=debug
        contains(QT_CONFIG,release): CONFIG+=release
    }
}

# Make sure this goes everywhere we need it
symbian: load(data_caging_paths)


# Figure out the root of where stuff should go (this could be done via configure)
OUTPUT_DIR = $$QT_MOBILITY_BUILD_TREE
SOURCE_DIR = $$PWD

CONFIG(debug, debug|release) {
    SUBDIRPART=Debug
} else {
    SUBDIRPART=Release
}

contains(QT_CONFIG, reduce_exports):CONFIG+=hide_symbols

#export more symbols if we build the unit tests
contains(build_unit_tests, yes) {
    DEFINES+=QTM_BUILD_UNITTESTS
} else {
    contains(build_public_unit_tests,yes):DEFINES+=QTM_BUILD_PUBLIC_UNITTESTS
}


#test whether we have a unit test
!testcase:!testhelper {
    OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
    !plugin {
        contains(TEMPLATE,.*lib) {
            DESTDIR = $$OUTPUT_DIR/lib
            symbian:defFilePath=../s60installs
            symbian:DEF_FILE=../s60installs
            VERSION = 1.2.2
        } else {
            DESTDIR = $$OUTPUT_DIR/bin
        }
    } else {
        testplugin {
            DESTDIR = $$OUTPUT_DIR/build/tests/bin/plugins/$$PLUGIN_TYPE 
        } else {
            symbian:VERSION = 1.2.2
            #check that plugin_type is set or warn otherwise
            isEmpty(PLUGIN_TYPE) {
                message(PLUGIN_TYPE not specified - install rule may not work)
            } else {
                target.path=$${QT_MOBILITY_PLUGINS}/$${PLUGIN_TYPE}
                INSTALLS += target
            }
        }
    }

    MOC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/ui
    QMAKE_RPATHDIR += $$QT_MOBILITY_LIB
} else {
    # Unit test code (no plugins! test plugins are just normal plugins installed elsewhere)
    testcase:QT *= testlib
    CONFIG += console
    CONFIG -= app_bundle
    OBJECTS_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET
    DESTDIR = $$OUTPUT_DIR/build/tests/bin
    MOC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/ui
    QMAKE_RPATHDIR += $$OUTPUT_DIR/lib
}

contains(TEMPLATE,.*lib):DEFINES += QT_SHARED

maemo6|meego {
    maemo6:DEFINES+= Q_WS_MAEMO_6
    meego:DEFINES+= Q_WS_MEEGO
    contains(TEMPLATE,.*app.*): QMAKE_LIB_FLAGS+= -Wl,-rpath-link $$[QT_INSTALL_LIBS]
    QMAKE_LIB_FLAGS+= -Wl,-rpath-link $$[QT_INSTALL_LIBS]
    QMAKE_RPATHDIR += $$[QT_INSTALL_LIBS]
}
maemo5 {
    DEFINES+= Q_WS_MAEMO_5
}
meego {
    DEFINES+= Q_WS_MEEGO
}

wince* {
    ### Bearer Management
    BEARERLIB.sources = $$OUTPUT_DIR/lib/$$mobilityDeployFilename(QtBearer).dll
    BEARERLIB.path = .
    DEPLOYMENT += BEARERLIB

    ### Contacts
    # Main library
    CONTACTS_DEPLOYMENT.sources = $$OUTPUT_DIR/lib/$$mobilityDeployFilename(QtContacts).dll
    CONTACTS_DEPLOYMENT.path = /Windows

    # Plugins
    CONTACTS_PLUGINS_DEPLOYMENT.sources = $$OUTPUT_DIR/plugins/contacts/*.dll
    CONTACTS_PLUGINS_DEPLOYMENT.path = plugins/contacts
    DEPLOYMENT += CONTACTS_DEPLOYMENT CONTACTS_PLUGINS_DEPLOYMENT

    ### Service Framework    
    SFW_DEPLOYMENT.sources = $$OUTPUT_DIR/lib/$$mobilityDeployFilename(QtServiceFramework).dll
    SFW_DEPLOYMENT.path = .
    DEPLOYMENT += SFW_DEPLOYMENT
    
    ### Location
    LOCATION.sources = $$OUTPUT_DIR/lib/$$mobilityDeployFilename(QtLocation).dll
    LOCATION.path = .
    DEPLOYMENT += LOCATION

    ### Landmarks
    LANDMARKS.sources = $$OUTPUT_DIR/lib/$$mobilityDeployFilename(QtLandmarks).dll
    LANDMARKS.path = .
    DEPLOYMENT += LANDMARKS
}

symbian {
    #For some reason the default include path doesn't include MOC_DIR on symbian
    INCLUDEPATH += $$MOC_DIR
    
    #This is supposed to be defined in symbian_os.hrh
    #DEFINES += SYMBIAN_EMULATOR_SUPPORTS_PERPROCESS_WSD
}

# Add the output dirs to the front of the link path.
# They must be at the front in case some previous Mobility version is already
# installed on the system.
# Note that we must use QMAKE_LIBDIR/QMAKE_FRAMEWORKPATH for this, and not
# LIBS, because Qt will prepend the Qt library path to LIBS elsewhere.
mac:contains(QT_CONFIG,qt_framework) {
    #add framework option
    QMAKE_FRAMEWORKPATH = $$OUTPUT_DIR/lib
}
QMAKE_LIBDIR += $$OUTPUT_DIR/lib

linux*-g++*:QMAKE_LFLAGS += $$QMAKE_LFLAGS_NOUNDEF

DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += $$SOURCE_DIR/src/global

!symbian:!wince*:DEFINES += QTM_PLUGIN_PATH=\\\"$$replace(QT_MOBILITY_PLUGINS, \\\\, /)\\\"
