/*
 * File:   objectForm.cpp
 * Author: jaa
 *
 * Created on 20. březen 2014, 21:20
 */

#include "ObjectForm.h"
#include "../Settings.h"

ObjectForm::ObjectForm() {
	widget.setupUi(this);
	
	prevRotationValue = Settings::objectRotate;
	
	widget.rotValueX->setText(QString::number(Settings::objectRotate.x));
	widget.rotSliderX->setValue(Settings::objectRotate.x);
	
	widget.rotValueY->setText(QString::number(Settings::objectRotate.y));
	widget.rotSliderY->setValue(Settings::objectRotate.y);
	
	widget.rotValueZ->setText(QString::number(Settings::objectRotate.z));
	widget.rotSliderZ->setValue(Settings::objectRotate.z);
}

ObjectForm::~ObjectForm() {
	
}

void ObjectForm::acceptCB() {
	this->close();
}

void ObjectForm::rejectCB() {
	Settings::objectRotate = prevRotationValue;
	this->close();
}

void ObjectForm::rotSliderXChangedCB(int v) {
	widget.rotValueX->setText(QString::number(v));
	Settings::objectRotate.x = v;
	
}

void ObjectForm::rotSliderYChangedCB(int v) {
	widget.rotValueY->setText(QString::number(v));
	Settings::objectRotate.y = v;
}


void ObjectForm::rotSliderZChangedCB(int v) {
	widget.rotValueZ->setText(QString::number(v));
	Settings::objectRotate.z = v;
}
	
	