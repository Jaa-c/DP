#############################################################################
# Makefile for building: dist/debug-no-optirun/GNU-Linux-x86/qtDP
# Generated by qmake (2.01a) (Qt 4.8.5) on: st �no 19 11:47:45 2014
# Project:  nbproject/qt-debug-no-optirun.pro
# Template: app
# Command: /usr/bin/qmake VPATH=. -o qttmp-debug-no-optirun.mk nbproject/qt-debug-no-optirun.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_WEBKIT -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -std=c++11 -g -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/default -Inbproject -I/usr/include/QtCore -I/usr/include/QtGui -I/usr/include/QtOpenGL -I/usr/include -Ilib/ -I/usr/X11R6/include -I. -I. -Inbproject -I.
LINK          = g++
LFLAGS        = 
LIBS          = $(SUBLIBS)  -L/usr/lib64 -L/usr/X11R6/lib -lX11 -lGLEW -lglfw -lassimp -ljpeg -lQtOpenGL -L/usr/lib64 -L/usr/X11R6/lib -lQtGui -lQtCore -lGL -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = build/debug-no-optirun/GNU-Linux-x86/

####### Files

SOURCES       = ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/globals.cpp \
		../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/GLWidget.cpp \
		../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/MainWindow.cpp \
		../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.cpp \
		src/main.cpp moc_OpenForm.cpp \
		moc_GLWidget.cpp \
		moc_MainWindow.cpp
OBJECTS       = build/debug-no-optirun/GNU-Linux-x86/globals.o \
		build/debug-no-optirun/GNU-Linux-x86/GLWidget.o \
		build/debug-no-optirun/GNU-Linux-x86/MainWindow.o \
		build/debug-no-optirun/GNU-Linux-x86/OpenForm.o \
		build/debug-no-optirun/GNU-Linux-x86/main.o \
		build/debug-no-optirun/GNU-Linux-x86/moc_OpenForm.o \
		build/debug-no-optirun/GNU-Linux-x86/moc_GLWidget.o \
		build/debug-no-optirun/GNU-Linux-x86/moc_MainWindow.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_phonon.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		nbproject/qt-debug-no-optirun.pro
QMAKE_TARGET  = qtDP
DESTDIR       = dist/debug-no-optirun/GNU-Linux-x86/
TARGET        = dist/debug-no-optirun/GNU-Linux-x86/qtDP

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: qttmp-debug-no-optirun.mk $(TARGET)

$(TARGET): ui_OpenForm.h $(OBJECTS)  
	@$(CHK_DIR_EXISTS) dist/debug-no-optirun/GNU-Linux-x86/ || $(MKDIR) dist/debug-no-optirun/GNU-Linux-x86/ 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)
	{ test -n "$(DESTDIR)" && DESTDIR="$(DESTDIR)" || DESTDIR=.; } && test $$(gdb --version | sed -e 's,[^0-9]\+\([0-9]\)\.\([0-9]\).*,\1\2,;q') -gt 72 && gdb --nx --batch --quiet -ex 'set confirm off' -ex "save gdb-index $$DESTDIR" -ex quit '$(TARGET)' && test -f $(TARGET).gdb-index && objcopy --add-section '.gdb_index=$(TARGET).gdb-index' --set-section-flags '.gdb_index=readonly' '$(TARGET)' '$(TARGET)' && rm -f $(TARGET).gdb-index || true

qttmp-debug-no-optirun.mk: nbproject/qt-debug-no-optirun.pro  /usr/share/qt4/mkspecs/default/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_phonon.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/opengl.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib64/libQtOpenGL.prl \
		/usr/lib64/libQtGui.prl \
		/usr/lib64/libQtCore.prl
	$(QMAKE) VPATH=. -o qttmp-debug-no-optirun.mk nbproject/qt-debug-no-optirun.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_phonon.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/shared.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/opengl.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib64/libQtOpenGL.prl:
/usr/lib64/libQtGui.prl:
/usr/lib64/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) VPATH=. -o qttmp-debug-no-optirun.mk nbproject/qt-debug-no-optirun.pro

dist: 
	@$(CHK_DIR_EXISTS) build/debug-no-optirun/GNU-Linux-x86/qtDP1.0.0 || $(MKDIR) build/debug-no-optirun/GNU-Linux-x86/qtDP1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) build/debug-no-optirun/GNU-Linux-x86/qtDP1.0.0/ && $(COPY_FILE) --parents ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/BundlerParser.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Camera.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Controlls.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/DataLoader.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Log.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/ObjectData.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Radar.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/RenderPass/BundlerPointsRenderPass.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/RenderPass/RenderPass.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/RenderPass/TexturingRenderPass.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/RenderPassHandler.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Renderer.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/ShaderHandler.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/Texture.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/TextureHandler.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/globals.h ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.h src/gui/GLWidget.h src/gui/MainWindow.h build/debug-no-optirun/GNU-Linux-x86/qtDP1.0.0/ && $(COPY_FILE) --parents ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/globals.cpp ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/GLWidget.cpp ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/MainWindow.cpp ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.cpp src/main.cpp build/debug-no-optirun/GNU-Linux-x86/qtDP1.0.0/ && $(COPY_FILE) --parents ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.ui build/debug-no-optirun/GNU-Linux-x86/qtDP1.0.0/ && (cd `dirname build/debug-no-optirun/GNU-Linux-x86/qtDP1.0.0` && $(TAR) qtDP1.0.0.tar qtDP1.0.0 && $(COMPRESS) qtDP1.0.0.tar) && $(MOVE) `dirname build/debug-no-optirun/GNU-Linux-x86/qtDP1.0.0`/qtDP1.0.0.tar.gz . && $(DEL_FILE) -r build/debug-no-optirun/GNU-Linux-x86/qtDP1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) qttmp-debug-no-optirun.mk


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_OpenForm.cpp moc_GLWidget.cpp moc_MainWindow.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_OpenForm.cpp moc_GLWidget.cpp moc_MainWindow.cpp
moc_OpenForm.cpp: src/globals.h \
		src/Log.h \
		ui_OpenForm.h \
		../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.h
	/usr/bin/moc $(DEFINES) $(INCPATH) ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.h -o moc_OpenForm.cpp

moc_GLWidget.cpp: src/BundlerParser.h \
		src/Log.h \
		src/ShaderHandler.h \
		src/Camera.h \
		src/globals.h \
		src/ObjectData.h \
		src/DataLoader.h \
		src/Texture.h \
		src/TextureHandler.h \
		src/Renderer.h \
		src/Controlls.h \
		src/RenderPass/RenderPass.h \
		src/Radar.h \
		src/RenderPassHandler.h \
		src/gui/GLWidget.h
	/usr/bin/moc $(DEFINES) $(INCPATH) src/gui/GLWidget.h -o moc_GLWidget.cpp

moc_MainWindow.cpp: src/gui/MainWindow.h
	/usr/bin/moc $(DEFINES) $(INCPATH) src/gui/MainWindow.h -o moc_MainWindow.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_OpenForm.h
compiler_uic_clean:
	-$(DEL_FILE) ui_OpenForm.h
ui_OpenForm.h: ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.ui
	/usr/bin/uic ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.ui -o ui_OpenForm.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

####### Compile

build/debug-no-optirun/GNU-Linux-x86/globals.o: ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/globals.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/debug-no-optirun/GNU-Linux-x86/globals.o ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/globals.cpp

build/debug-no-optirun/GNU-Linux-x86/GLWidget.o: ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/GLWidget.cpp src/gui/GLWidget.h \
		src/BundlerParser.h \
		src/Log.h \
		src/ShaderHandler.h \
		src/Camera.h \
		src/globals.h \
		src/ObjectData.h \
		src/DataLoader.h \
		src/Texture.h \
		src/TextureHandler.h \
		src/Renderer.h \
		src/Controlls.h \
		src/RenderPass/RenderPass.h \
		src/Radar.h \
		src/RenderPassHandler.h \
		src/RenderPass/TexturingRenderPass.h \
		src/RenderPass/BundlerPointsRenderPass.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/debug-no-optirun/GNU-Linux-x86/GLWidget.o ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/GLWidget.cpp

build/debug-no-optirun/GNU-Linux-x86/MainWindow.o: ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/MainWindow.cpp src/gui/MainWindow.h \
		src/gui/GLWidget.h \
		src/BundlerParser.h \
		src/Log.h \
		src/ShaderHandler.h \
		src/Camera.h \
		src/globals.h \
		src/ObjectData.h \
		src/DataLoader.h \
		src/Texture.h \
		src/TextureHandler.h \
		src/Renderer.h \
		src/Controlls.h \
		src/RenderPass/RenderPass.h \
		src/Radar.h \
		src/RenderPassHandler.h \
		src/gui/OpenForm.h \
		ui_OpenForm.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/debug-no-optirun/GNU-Linux-x86/MainWindow.o ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/MainWindow.cpp

build/debug-no-optirun/GNU-Linux-x86/OpenForm.o: ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.cpp src/gui/OpenForm.h \
		src/globals.h \
		src/Log.h \
		ui_OpenForm.h \
		src/gui/GLWidget.h \
		src/BundlerParser.h \
		src/ShaderHandler.h \
		src/Camera.h \
		src/ObjectData.h \
		src/DataLoader.h \
		src/Texture.h \
		src/TextureHandler.h \
		src/Renderer.h \
		src/Controlls.h \
		src/RenderPass/RenderPass.h \
		src/Radar.h \
		src/RenderPassHandler.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/debug-no-optirun/GNU-Linux-x86/OpenForm.o ../../../../../Dokumenty/FEL/DP/trunk/qtDP/src/gui/OpenForm.cpp

build/debug-no-optirun/GNU-Linux-x86/main.o: src/main.cpp src/globals.h \
		src/Log.h \
		src/gui/GLWidget.h \
		src/BundlerParser.h \
		src/ShaderHandler.h \
		src/Camera.h \
		src/ObjectData.h \
		src/DataLoader.h \
		src/Texture.h \
		src/TextureHandler.h \
		src/Renderer.h \
		src/Controlls.h \
		src/RenderPass/RenderPass.h \
		src/Radar.h \
		src/RenderPassHandler.h \
		src/gui/MainWindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/debug-no-optirun/GNU-Linux-x86/main.o src/main.cpp

build/debug-no-optirun/GNU-Linux-x86/moc_OpenForm.o: moc_OpenForm.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/debug-no-optirun/GNU-Linux-x86/moc_OpenForm.o moc_OpenForm.cpp

build/debug-no-optirun/GNU-Linux-x86/moc_GLWidget.o: moc_GLWidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/debug-no-optirun/GNU-Linux-x86/moc_GLWidget.o moc_GLWidget.cpp

build/debug-no-optirun/GNU-Linux-x86/moc_MainWindow.o: moc_MainWindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o build/debug-no-optirun/GNU-Linux-x86/moc_MainWindow.o moc_MainWindow.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

