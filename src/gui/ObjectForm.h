/* 
 * File:   objectForm.h
 * Author: jaa
 *
 * Created on 20. březen 2014, 21:20
 */

#ifndef _OBJECTFORM_H
#define	_OBJECTFORM_H

#include "ui_ObjectForm.h"

class ObjectForm : public QDialog {
	Q_OBJECT
public:
	ObjectForm();
	virtual ~ObjectForm();
private:
	Ui::objectForm widget;
};

#endif	/* _OBJECTFORM_H */
