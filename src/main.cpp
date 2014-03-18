/* 
 * File:   main.cpp
 * Author: jaa
 *
 * Created on 27. září 2013, 19:23
 */

#include "globals.h"
#include "gui/GLWidget.h"
#include "gui/MainWindow.h"
#include "io/Rz3Parser.h"

#include <QApplication>
#include <QtGui/qerrormessage.h>

class MyApplication : public QApplication {

public:
	QErrorMessage *err;
	MyApplication(int & argc, char ** argv) : QApplication(argc, argv) {
		err = new QErrorMessage();
		err->setModal(true);
	}
	~MyApplication() {
		DELETE(err);
	}
	
	void handleError(std::exception_ptr eptr) {
		std::string msg;
		try {
			if (eptr != std::exception_ptr()) {
				std::rethrow_exception(eptr);
			}
		} catch(const std::exception& e) {
			msg = std::string(e.what());
		}
		catch(const char* e) {
			msg = e;
		}
		catch(...) {
			msg = "unknow exception";			
		}
		msg = "Unexpected error: " + msg;
		err->showMessage(msg.c_str());
		Log::e(msg);
	}
	
	virtual bool notify(QObject * receiver, QEvent * event) {
		try {
			return QApplication::notify(receiver, event);
		} catch (...) {
			handleError(std::current_exception());
		}
		return false;
	}

};

int main(int argc, char** argv) {
	
//	Rz3Parser p("/home/jaa/Documents/FEL/DP/data/dum/09_done.rz3", 
//			"/home/jaa/Documents/FEL/DP/data/dum/09_pictures0_OK.txt");
//	
//	p.parseFile();
//	return 0;
	
	
	const char *window_title = "View dependent texturing";
	const int width = 1000;
	const int height = 800;
		
	MyApplication app(argc, argv);
    
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
