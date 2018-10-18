#--------------------------------------------------------------------
# path to gplay engine
#--------------------------------------------------------------------
PRE_TARGETDEPS += $$PWD/../GPlayEngine/setup.pri
include($$PWD/../GPlayEngine/setup.pri)

#--------------------------------------------------------------------
# project
#--------------------------------------------------------------------
QT += core gui widgets charts
TARGET = spkgen
TEMPLATE = app
CONFIG += c++14
CONFIG -= console
CONFIG += windows
CONFIG += no_keywords

DESTDIR = $$GPLAY_OUTPUT_DIR/bin
QMAKE_CLEAN += $$DESTDIR/$$TARGET

CONFIG(debug, debug|release):
    DEFINES += _DEBUG QT_NO_KEYWORDS
    #DEFINES += GP_USE_MEM_LEAK_DETECTION

INCLUDEPATH += $$GPLAY_OUTPUT_DIR/include/gplayengine/
INCLUDEPATH += $$GPLAY_OUTPUT_DIR/include/gplayengine/thirdparty

# nodes editor library
# build nodeeditor with : cmake .. -DCMAKE_PREFIX_PATH=/opt/Qt5.11.1/5.11.1/gcc_64
DEFINES += NODE_EDITOR_STATIC
INCLUDEPATH += 3rdparty/nodeeditor/include
LIBS += -L$$PWD/3rdparty/nodeeditor/BUILD/lib -lnodes

#--------------------------------------------------------------------
# platform specific
#--------------------------------------------------------------------
linux: {
    DEFINES += __linux__
    PRE_TARGETDEPS += $$GPLAY_OUTPUT_DIR/lib/libgplay-deps.a
    PRE_TARGETDEPS += $$GPLAY_OUTPUT_DIR/lib/libgplay.a
    LIBS += -L$$GPLAY_OUTPUT_DIR/lib/ -lgplay
    LIBS += -L$$GPLAY_OUTPUT_DIR/lib/thirdparty/ -lgplay-deps
    LIBS += -lm -lGL -lrt -ldl -lX11 -lpthread -lsndio
    QMAKE_CXXFLAGS += -Wno-unused-parameter
}

# force qmake
#qmakeforce.target = dummy
#qmakeforce.commands = rm -f Makefile ##to force rerun of qmake
#qmakeforce.depends = FORCE
#PRE_TARGETDEPS += $$qmakeforce.target
#QMAKE_EXTRA_TARGETS += qmakeforce



#--------------------------------------------------------------------
# files
#--------------------------------------------------------------------
SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/RenderViewWidget.cpp \
    src/GPDevice.cpp \
    src/BaseNode.cpp \
    src/ParamWidget.cpp \
    src/CustomWidgets.cpp \
    src/TrackEdit.cpp \
    src/Types.cpp \
    src/SpkEmitters.cpp \
    src/SpkZones.cpp \
    src/SpkSystem.cpp \
    src/Parameter.cpp \
    src/Color.cpp \
    src/path.cpp \
    src/GraphView.cpp

HEADERS += \
    src/MainWindow.h \
    src/RenderViewWidget.h \
    src/GPDevice.h \
    src/BaseNode.h \
    src/ParamWidget.h \
    src/Parameter.h \
    src/Types.h \
    src/CustomWidgets.h \
    src/Trackedit.h \
    src/SpkEmitters.h \
    src/SpkZones.h \
    src/SpkSystem.h \
    src/Color.h \
    src/benchmark.h \
    src/path.hpp \
    src/GraphView.h \
    src/Nodestyle.h


