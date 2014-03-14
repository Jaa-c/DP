/*
 * File:   SettingsForm.cpp
 * Author: jaa
 *
 * Created on 4. březen 2014, 22:24
 */

#include "SettingsForm.h"
#include "GLWidget.h"

SettingsForm::SettingsForm(GLWidget * glw) : glWidget(glw)  {
	setAttribute(Qt::WA_NoMousePropagation); //not working TODO
	glWidget->setFocusPolicy(Qt::NoFocus);	
	widget.setupUi(this);
	
	
	widget.photosSlider->setMinimum(1);
	widget.photosSlider->setMaximum(32);
	widget.photosSlider->setValue(glWidget->tmpCameras);
	widget.photoSliderValue->setText(QString::number(widget.photosSlider->value()));
	
	photoSliderPrevVal = widget.photosSlider->value();
}

SettingsForm::~SettingsForm() {
	glWidget->setFocusPolicy(Qt::StrongFocus);	
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