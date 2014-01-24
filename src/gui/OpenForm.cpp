/*
 * File:   OpenForm.cpp
 * Author: jaa
 *
 * Created on 24. leden 2014, 16:45
 */

#include "OpenForm.h"
#include "GLWidget.h"
#include <QFileDialog>

OpenForm::OpenForm(GLWidget * glw) : glWidget(glw) {
	openForm.setupUi(this);
}

OpenForm::~OpenForm() {
}


void OpenForm::acceptCB() {
	if( openForm.geometryPath->text().length() > 0 && 
		openForm.bundlerPath->text().length() > 0 &&
		openForm.photosPath->text().length() > 0) 
	{
		this->close();
		glWidget->createScene(
			openForm.geometryPath->text().toStdString(), 
			openForm.bundlerPath->text().toStdString(), 
			openForm.photosPath->text().toStdString()
		);
	}
	else {
		openForm.statusLabel->setText("Select all required files!");
	}

}

void OpenForm::rejectCB() {
	this->close();
}

void OpenForm::openGeometryCB() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open geometry file"), "",tr("Geometry file"));
	if(!fileName.isEmpty()) {
		openForm.geometryPath->setText(fileName);
	}
}

void OpenForm::openBundlerCB() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open bundler file"), "",tr("Bundler out file (*.out)"));
	if(!fileName.isEmpty()) {
		openForm.bundlerPath->setText(fileName);
	}
}

void OpenForm::openPhotosCB() {
	QString path = QFileDialog::getExistingDirectory (this, tr("Choose folder with photos"));
	if(!path.isEmpty()) {
		openForm.photosPath->setText(path);
	}
}