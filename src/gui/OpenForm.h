/* 
 * File:   OpenForm.h
 * Author: jaa
 *
 * Created on 24. leden 2014, 16:45
 */

#ifndef _OPENFORM_H
#define	_OPENFORM_H

#include "../globals.h"
#include "ui_OpenForm.h"
#include <memory>

class GLWidget;

class OpenForm : public QDialog {
	Q_OBJECT
public:
	OpenForm(std::shared_ptr<GLWidget> glw);
	virtual ~OpenForm();
	
private slots:
	void acceptCB();
	void rejectCB();
	void openGeometryCB();
	void openBundlerCB();
	void openPhotosCB();
	void openRz3FileCB();
	void openRz3ImageFileCB();
	
private:
	Ui::OpenForm openForm;
	std::shared_ptr<GLWidget> glWidget;
};

#endif	/* _OPENFORM_H */
