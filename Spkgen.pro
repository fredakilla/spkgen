#--------------------------------------------------------------------
# path to Urho3D and Urhox
#--------------------------------------------------------------------
URHO3D_HOME = /home/fred/Dev/Urho3D/BUILD
URHOX_HOME = /home/fred/Dev/Urhox/output

#--------------------------------------------------------------------
# project
#--------------------------------------------------------------------
QT += core gui widgets charts
TARGET = Spkgen
TEMPLATE = app
CONFIG += c++14
CONFIG -= console
CONFIG += windows
CONFIG += no_keywords

DESTDIR = $$PWD/bin
QMAKE_CLEAN += $$DESTDIR/$$TARGET

CONFIG(debug, debug|release):
    DEFINES += _DEBUG QT_NO_KEYWORDS

INCLUDEPATH += $$URHO3D_HOME/include
INCLUDEPATH += $$URHO3D_HOME/include/Urho3D/ThirdParty
INCLUDEPATH += $$URHOX_HOME/include/Urhox
INCLUDEPATH += $$URHOX_HOME/include/Urhox/ThirdParty

# nodes editor library
# build nodeeditor with :
# cmake .. -DCMAKE_PREFIX_PATH=/opt/Qt5.11.1/5.11.1/gcc_64 -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF
DEFINES += NODE_EDITOR_STATIC
INCLUDEPATH += 3rdparty/nodeeditor/include
LIBS += -L$$PWD/3rdparty/nodeeditor/BUILD/lib -lnodes

#--------------------------------------------------------------------
# platform specific
#--------------------------------------------------------------------
linux: {
    QT += x11extras
    DEFINES += __linux__    
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    PRE_TARGETDEPS += $$URHOX_HOME/lib/libUrhox.a
    LIBS += -L$$URHOX_HOME/lib -lUrhox
    LIBS += -L$$URHO3D_HOME/lib -lUrho3D
    LIBS += -lXi -ldl -lpthread -lGL -lGLU -lX11
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
    src/node-editor/common/BaseNode.cpp \
    src/node-editor/common/Color.cpp \
    src/node-editor/common/CommentGraphicsItem.cpp \
    src/node-editor/common/CustomFlowScene.cpp \
    src/node-editor/common/CustomWidgets.cpp \
    src/node-editor/common/Parameter.cpp \
    src/node-editor/common/ParamWidget.cpp \
    src/node-editor/common/Path.cpp \
    src/node-editor/common/TrackEdit.cpp \
    src/node-editor/common/Types.cpp \
    src/node-editor/spark-nodes/SpkEmitters.cpp \
    src/node-editor/spark-nodes/SpkInterpolators.cpp \
    src/node-editor/spark-nodes/SpkModifiers.cpp \
    src/node-editor/spark-nodes/SpkSystem.cpp \
    src/node-editor/spark-nodes/SpkZones.cpp \
    src/node-editor/spark-nodes/SparkNodesRegistry.cpp \
    src/GraphView.cpp \
    src/main.cpp \
    src/MainWindow.cpp \
    src/node-editor/spark-nodes/SpkUtils.cpp \
    src/node-editor/spark-nodes/SparkNodeRender.cpp \
    src/UrhoDevice.cpp \
    src/urho/GridGeometry.cpp \
    src/urho/CameraController.cpp \
    src/RenderWidget.cpp \
    src/urho/BaseRenderer.cpp \
    src/node-editor/common/Curve.cpp

HEADERS += \
    src/node-editor/common/BaseNode.h \
    src/node-editor/common/Color.h \
    src/node-editor/common/CommentGraphicsItem.h \
    src/node-editor/common/CustomFlowScene.h \
    src/node-editor/common/CustomWidgets.h \
    src/node-editor/common/Nodestyle.h \
    src/node-editor/common/Parameter.h \
    src/node-editor/common/ParamWidget.h \
    src/node-editor/common/Path.h \
    src/node-editor/common/Trackedit.h \
    src/node-editor/common/Types.h \
    src/node-editor/spark-nodes/SpkEmitters.h \
    src/node-editor/spark-nodes/SpkInterpolators.h \
    src/node-editor/spark-nodes/SpkModifiers.h \
    src/node-editor/spark-nodes/SpkSystem.h \
    src/node-editor/spark-nodes/SpkZones.h \
    src/node-editor/spark-nodes/SparkNodesRegistry.h \
    src/benchmark.h \
    src/GraphView.h \
    src/MainWindow.h \
    src/node-editor/spark-nodes/spark-nodes.h \
    src/node-editor/spark-nodes/SpkUtils.h \
    src/node-editor/spark-nodes/SparkNodeRender.h \
    src/UrhoDevice.h \
    src/urho/GridGeometry.h \
    src/urho/CameraController.h \
    src/RenderWidget.h \
    src/urho/BaseRenderer.h \
    src/node-editor/common/Curve.h



