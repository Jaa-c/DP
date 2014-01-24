/* 
 * File:   main.cpp
 * Author: jaa
 *
 * Created on 27. září 2013, 19:23
 */

#include <GL/glew.h>
#include "globals.h"
#include "gui/GLWidget.h"
#include "gui/MainWindow.h"

#include <QApplication>
	 
int main(int argc, char** argv) {
	const char *window_title = "View dependent texturing";
	const int width = 1000;
	const int height = 800;
		
	QApplication app(argc, argv);
    
	MainWindow window(&app, width, height);
	window.setWindowTitle(window_title);
	window.resize(width, height);
	window.move(200, 0);
    window.show();
		
	GLint texture_units = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
	Log::i("Avaiable texture units for FS: %d", texture_units);
	
    return app.exec();
}
