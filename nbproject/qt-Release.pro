# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Release/GNU-Linux-x86
TARGET = qtDP
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += release 
PKGCONFIG +=
QT = core gui widgets opengl
SOURCES += ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Settings.cpp ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/globals.cpp ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/GLWidget.cpp ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/MainWindow.cpp ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/ObjectForm.cpp ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.cpp ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/SettingsForm.cpp src/main.cpp
HEADERS += ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Camera.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Controlls.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Log.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/ObjectData.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/RenderPass/BasicTexturingRenderPass.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/RenderPass/BundlerPointsRenderPass.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/RenderPass/RadarRenderPass.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/RenderPass/RenderPass.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/RenderPass/TexturingRenderPass.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/RenderPassHandler.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Renderer.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Settings.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/ShaderHandler.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Texture.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/TextureHandler.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/globals.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/ObjectForm.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/SettingsForm.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/io/BundlerParser.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/io/CalibrationLoader.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/io/DataLoader.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/io/ImageLoader.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/io/parser.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/kdtree/KDTree.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/kdtree/KDTreeNodes.h src/gui/GLWidget.h src/gui/MainWindow.h src/io/Rz3Parser.h
FORMS += ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/ObjectForm.ui ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.ui ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/SettingsForm.ui
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Release/GNU-Linux-x86
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += lib/ 
LIBS += 
equals(QT_MAJOR_VERSION, 4) {
QMAKE_CXXFLAGS += -std=c++11
}
equals(QT_MAJOR_VERSION, 5) {
CONFIG += c++11
}
"LIBS += -lGL -lX11 -lGLEW -lglfw -lassimp -ljpeg"
