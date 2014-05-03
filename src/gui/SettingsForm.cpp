/*
 * File:   SettingsForm.cpp
 * Author: jaa
 *
 * Created on 4. březen 2014, 22:24
 */

#include "SettingsForm.h"
#include "GLWidget.h"
#include "../Settings.h"

SettingsForm::SettingsForm(std::shared_ptr<GLWidget> glw) : glWidget(glw)  {
	setAttribute(Qt::WA_NoMousePropagation); //not working TODO
	glWidget->setFocusPolicy(Qt::NoFocus);	
	widget.setupUi(this);
	
	
	widget.photosSlider->setMinimum(1);
	widget.photosSlider->setMaximum(Settings::maxTextures);
	widget.photosSlider->setValue(Settings::usingTextures);
	widget.photoSliderValue->setText(QString::number(widget.photosSlider->value()));
	widget.directionsCheckBox->setChecked(Settings::useRecomputedDirections);
	
	photoSliderPrevVal = widget.photosSlider->value();
}

SettingsForm::~SettingsForm() {
	glWidget->setFocusPolicy(Qt::StrongFocus);	
}

void SettingsForm::acceptCB() {
	Settings::usingTextures = widget.photosSlider->value();
	Settings::useRecomputedDirections = widget.directionsCheckBox->isChecked();
	this->close();
}

void SettingsForm::rejectCB() {
	
	widget.photosSlider->setValue(photoSliderPrevVal);
	this->close();
}

void SettingsForm::photoSliderChangedCB(int val) {
	widget.photoSliderValue->setText(QString::number(val));
}