/* 
 * File:   SettingsForm.h
 * Author: jaa
 *
 * Created on 4. březen 2014, 22:24
 */

#ifndef _SETTINGSFORM_H
#define	_SETTINGSFORM_H

#include "ui_SettingsForm.h"

class GLWidget;

class SettingsForm : public QDialog {
	Q_OBJECT
public:
	SettingsForm(GLWidget * glw);
	virtual ~SettingsForm();
	
private slots:
	void acceptCB();
	void rejectCB();
	void photoSliderChangedCB(int v);
	
private:
	Ui::SettingsForm widget;
	GLWidget *glWidget;
	
	int photoSliderPrevVal;
};

#endif	/* _SETTINGSFORM_H */
