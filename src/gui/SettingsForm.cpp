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
	widget.setupUi(this);
	
	widget.photosSlider->setMinimum(1);
	widget.photosSlider->setMaximum(Settings::maxTextures);
	widget.photosSlider->setValue(Settings::usingTextures);
	widget.photoSliderValue->setText(QString::number(widget.photosSlider->value()));
	widget.directionsCheckBox->setChecked(Settings::useRecomputedDirections);
	widget.filteringCheckBox->setChecked(Settings::billinearFiltering);

	photoSliderPrevVal = widget.photosSlider->value();
}

void SettingsForm::acceptCB() {
	Settings::usingTextures = widget.photosSlider->value();
	Settings::useRecomputedDirections = widget.directionsCheckBox->isChecked();
	Settings::billinearFiltering = widget.filteringCheckBox->isChecked();
	this->close();
}

void SettingsForm::rejectCB() {
	widget.photosSlider->setValue(photoSliderPrevVal);
	this->close();
}

void SettingsForm::photoSliderChangedCB(int val) {
	widget.photoSliderValue->setText(QString::number(val));
}