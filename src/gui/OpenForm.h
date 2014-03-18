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

class GLWidget;

class OpenForm : public QDialog {
	Q_OBJECT
public:
	OpenForm(GLWidget * glw);
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
	GLWidget *glWidget;
};

#endif	/* _OPENFORM_H */
