TEMPLATE = app
DESTDIR = dist/Release
TARGET = qtDP
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += release 
PKGCONFIG +=
QT = core gui widgets opengl
SOURCES += src/Settings.cpp src/Texture.cpp src/TextureHandler.cpp src/globals.cpp src/gui/GLWidget.cpp src/gui/MainWindow.cpp src/gui/ObjectForm.cpp src/gui/OpenForm.cpp src/gui/SettingsForm.cpp src/main.cpp
HEADERS += src/Camera.h src/Controlls.h src/GLTimer.h src/Log.h src/ObjectData.h src/Photo.h src/RayCaster.h src/RenderPass/BasicTexturingRenderPass.h src/RenderPass/RadarRenderPass.h src/RenderPass/RenderPass.h src/RenderPass/TexturingRenderPass.h src/RenderPassHandler.h src/Renderer.h src/Settings.h src/ShaderHandler.h src/Texture.h src/TextureHandler.h src/globals.h src/gui/ObjectForm.h src/gui/OpenForm.h src/gui/SettingsForm.h src/io/BundlerParser.h src/io/CalibrationLoader.h src/io/DataLoader.h src/io/ImageBB.h src/io/ImageLoader.h src/io/parser.h src/RenderPass/CameraPointsRenderPass.h src/RenderPass/NormalsDebugPass.h src/RenderPass/TexturingPrePass.h src/gui/GLWidget.h src/gui/MainWindow.h src/io/Rz3Parser.h
FORMS += src/gui/ObjectForm.ui src/gui/OpenForm.ui src/gui/SettingsForm.ui
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Release
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += lib/ 
LIBS += -lGL -lX11 -lGLEW -lglfw -lassimp -ljpeg
equals(QT_MAJOR_VERSION, 4) {
QMAKE_CXXFLAGS += -std=c++11
}
equals(QT_MAJOR_VERSION, 5) {
CONFIG += c++11
}
