/*
 * File:   OpenForm.cpp
 * Author: jaa
 *
 * Created on 24. leden 2014, 16:45
 */

#include "OpenForm.h"
#include "GLWidget.h"
#include <QFileDialog>
#include <QtGui/qsplashscreen.h>

OpenForm::OpenForm(GLWidget * glw) : glWidget(glw) {
	openForm.setupUi(this);
	openForm.tabWidget->setCurrentIndex(0);
	//DEBUG TEMP
	switch(1) {
		case 0:
			openForm.geometryPath->setText("/home/jaa/Documents/FEL/DP/data/dum/dil09.obj");
			openForm.photosPath->setText("/home/jaa/Documents/FEL/DP/data/dum/09");	
			openForm.rz3Path->setText("/home/jaa/Documents/FEL/DP/data/dum/09_done.rz3");
			openForm.rz3Images->setText("/home/jaa/Documents/FEL/DP/data/dum/09_pictures0_OK.txt");
			openForm.tabWidget->setCurrentIndex(1);
			break;
		case 1:
			openForm.geometryPath->setText("/home/jaa/Documents/FEL/DP/data/statue/statue.obj");
			openForm.bundlerPath->setText("/home/jaa/Documents/FEL/DP/data/statue/bundle.out");
			openForm.photosPath->setText("/home/jaa/Documents/FEL/DP/data/statue/photos/");	
			break;
		case 2:
			openForm.geometryPath->setText("/home/jaa/Documents/FEL/DP/data/debug/statue.obj");
			openForm.bundlerPath->setText("/home/jaa/Documents/FEL/DP/data/debug/bundle.out");
			openForm.photosPath->setText("/home/jaa/Documents/FEL/DP/data/debug/photos/");
			break;
	}
}

OpenForm::~OpenForm() {
}


void OpenForm::acceptCB() {
	if( openForm.geometryPath->text().length() > 0 && 
		openForm.photosPath->text().length() > 0 && (
		openForm.bundlerPath->text().length() > 0 || (
		openForm.rz3Path->text().length() > 0 &&
		openForm.rz3Images->text().length() > 0
		)))
	{
		this->close();
		
		QApplication::setOverrideCursor(Qt::WaitCursor);
		
		CalibrationLoader::FileType type = (CalibrationLoader::FileType) openForm.tabWidget->currentIndex();
		
		std::string file1, file2("");
		if(type == CalibrationLoader::FileType::BUNDLER) {
			file1 = openForm.bundlerPath->text().toStdString();
		}
		else if(type == CalibrationLoader::FileType::RZ3) {
			file1 = openForm.rz3Path->text().toStdString();
			file2 = openForm.rz3Images->text().toStdString();
		}
		
		glWidget->deleteScene();
		glWidget->createScene(
			openForm.geometryPath->text().toStdString(), 
			openForm.photosPath->text().toStdString(),
			type,
			file1,
			file2
		);
		
		QApplication::restoreOverrideCursor();
	}
	else {
		openForm.statusLabel->setText("Select all required files!");
	}

}

void OpenForm::rejectCB() {
	this->close();
}

void OpenForm::openGeometryCB() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open geometry file"), "",
		tr("3D geometry file (*.obj *.dae *.blend *.3ds *.ase *.ifc *.xgl *.zgl *.ply *.dxf *.lwo *.lws *.lxo *.stl *.x *.ac *.ms3d *.cob *.scn)"));
	
	if(!fileName.isEmpty()) {
		openForm.geometryPath->setText(fileName);
	}
}

void OpenForm::openBundlerCB() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open bundler file"), "", tr("Bundler out file (*.out)"));
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


void OpenForm::openRz3FileCB() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open rz3 file"), "", tr("rz3 (*.rz3)"));
	if(!fileName.isEmpty()) {
		openForm.rz3Path->setText(fileName);
	}

}
void OpenForm::openRz3ImageFileCB() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open images file"), "", tr("Text file (*.*)"));
	if(!fileName.isEmpty()) {
		openForm.rz3Images->setText(fileName);
	}

}