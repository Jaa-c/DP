/* 
 * File:   objectForm.h
 * Author: jaa
 *
 * Created on 20. březen 2014, 21:20
 */

#ifndef _OBJECTFORM_H
#define	_OBJECTFORM_H

#include <glm/glm.hpp>
#include "ui_ObjectForm.h"


class ObjectForm : public QDialog {
	Q_OBJECT
public:
	ObjectForm();
	
private slots:
	void acceptCB();
	void rejectCB();
	void rotSliderXChangedCB(int v);
	void rotSliderYChangedCB(int v);
	void rotSliderZChangedCB(int v);
	
private:
	Ui::objectForm widget;
	
	glm::vec3 prevRotationValue;
};

#endif	/* _OBJECTFORM_H */
