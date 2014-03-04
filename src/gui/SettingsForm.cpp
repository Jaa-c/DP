/*
 * File:   SettingsForm.cpp
 * Author: jaa
 *
 * Created on 4. březen 2014, 22:24
 */

#include "SettingsForm.h"
#include "GLWidget.h"

SettingsForm::SettingsForm(GLWidget * glw) : glWidget(glw)  {
	setAttribute(Qt::WA_NoMousePropagation); //not working
	widget.setupUi(this);
	
	
	widget.photosSlider->setMinimum(1);
	widget.photosSlider->setMaximum(32);
	widget.photosSlider->setValue(6);
	widget.photoSliderValue->setText(QString::number(widget.photosSlider->value()));
	
	photoSliderPrevVal = widget.photosSlider->value();
}

SettingsForm::~SettingsForm() {
}

void SettingsForm::acceptCB() {
	glWidget->tmpCameras = widget.photosSlider->value();
	this->close();
}

void SettingsForm::rejectCB() {
	
	widget.photosSlider->setValue(photoSliderPrevVal);
	this->close();
}

void SettingsForm::photoSliderChangedCB(int val) {
	widget.photoSliderValue->setText(QString::number(val));
}