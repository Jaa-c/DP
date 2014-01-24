/*
 * File:   OpenForm.cpp
 * Author: jaa
 *
 * Created on 24. leden 2014, 16:45
 */

#include "OpenForm.h"
#include <QFileDialog>

OpenForm::OpenForm() {
	widget.setupUi(this);
}

OpenForm::~OpenForm() {
}


void OpenForm::acceptCB() {

}

void OpenForm::rejectCB() {
	this->close();
}

void OpenForm::openGeometryCB() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open geometry file"), "",tr("Geometry file"));
	if(!fileName.isEmpty()) {
		widget.geometryPath->setText(fileName);
	}
}

void OpenForm::openBundlerCB() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open bundler file"), "",tr("Bundler out file (*.out)"));
	if(!fileName.isEmpty()) {
		widget.bundlerPath->setText(fileName);
	}
}

void OpenForm::openPhotosCB() {
	QString path = QFileDialog::getExistingDirectory (this, tr("Choose folder with photos"));
	if(!path.isEmpty()) {
		widget.photosPath->setText(path);
	}
}