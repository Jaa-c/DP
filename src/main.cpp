/* 
 * File:   main.cpp
 * Author: jaa
 *
 * Created on 27. září 2013, 19:23
 */

#include "globals.h"
#include "Log.h"
#include "gui/MainWindow.h"

#include <QApplication>
#include <QtGui/qerrormessage.h>
#include <memory>

class MyApplication : public QApplication {

public:
	std::shared_ptr<QErrorMessage> err;
	MyApplication(int & argc, char ** argv) : QApplication(argc, argv) {
		err = std::make_shared<QErrorMessage>();
		err->setModal(true);
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
		catch(const std::string &e) {
			msg = e;
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
	const char *window_title = "View dependent texturing";
	const int width = 1000;
	const int height = 800;
		
	MyApplication app(argc, argv);
    
	MainWindow window(&app, width, height);
	window.setWindowTitle(window_title);
	window.resize(width, height);
	window.move(200, 0);
    window.show();

    return app.exec();
}
