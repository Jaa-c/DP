
#include "MainWindow.h"
#include "GLWidget.h"
#include "OpenForm.h"

#include <QApplication>
#include <QtGui/QtGui>

MainWindow::MainWindow(QApplication *app, int w, int h) {
	//Menu
	createActions();
	createMenus();
	
	//OpenGL context format
	QGLFormat glFormat;
    glFormat.setVersion(3, 2);
    glFormat.setProfile(QGLFormat::CompatibilityProfile);

	//OpenGL widget:
    glWidget = new GLWidget(glFormat, w, h - menuBar()->height(), this);
	glWidget->setAttribute(Qt::WA_NoMousePropagation);
	this->setCentralWidget(glWidget);
	
	app->installEventFilter(glWidget);	//only opengl controlls
	app->installEventFilter(this);		//other application controlls
		
	initAppState();
}

void MainWindow::initAppState() {
	//Initial state of application
	texturingRP->setChecked(true);
	displayRadar->setChecked(true);
}

void MainWindow::createActions() {
	openAct = new QAction(tr("&Open"), this);
	openAct->setStatusTip(tr("Open model"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(openCB()));
	
	quitAct = new QAction(tr("&Quit"), this);
	quitAct->setStatusTip(tr("Exit application"));
	connect(quitAct, SIGNAL(triggered()), this, SLOT(quitCB()));
	
	texturingRP = new QAction(tr("&Texturing Pass"), this);
	texturingRP->setCheckable(true);
	connect(texturingRP, SIGNAL(changed()), this, SLOT(texturingPassCB()));
	
	bundlerPointsRP = new QAction(tr("&Bundler Points Pass"), this);
	bundlerPointsRP->setCheckable(true);
	connect(bundlerPointsRP, SIGNAL(changed()), this, SLOT(bundlerPointsPassCB()));
	
	displayRadar = new QAction(tr("Display &Radar"), this);
	displayRadar->setCheckable(true);
	connect(displayRadar, SIGNAL(changed()), this, SLOT(displayRadarCB()));
}

void MainWindow::createMenus() {
	file = menuBar()->addMenu(tr("&File"));
	file->addAction(openAct);
	file->addSeparator();
	file->addAction(quitAct);
	
	renderPass = menuBar()->addMenu(tr("&Render Passes"));
	renderPass->addAction(texturingRP);
	renderPass->addAction(bundlerPointsRP);
	
	view = menuBar()->addMenu(tr("&View"));
	view->addAction(displayRadar);
}

void MainWindow::openCB() {
	OpenForm form;
	form.exec();
}

 void MainWindow::quitCB() {
	 QApplication::quit();
 }
 
void MainWindow::texturingPassCB() {
	if(texturingRP->isChecked()) {
		glWidget->addRenderPass(RenderPass::TEXTURING_PASS);
	}
	else {
		glWidget->removeRenderPass(RenderPass::TEXTURING_PASS);
	}
}

void MainWindow::bundlerPointsPassCB() {
	if(bundlerPointsRP->isChecked()) {
		glWidget->addRenderPass(RenderPass::BUNDLER_POINTS_PASS);
	}
	else {
		glWidget->removeRenderPass(RenderPass::BUNDLER_POINTS_PASS);
	}
}

void MainWindow::displayRadarCB() {
	glWidget->setDisplayRadar(displayRadar->isChecked());
}

 bool MainWindow::eventFilter(QObject *, QEvent *event) {
	switch(event->type()) {
		case QEvent::KeyPress:
		{
			QKeyEvent *e = (QKeyEvent *) event;
			switch(e->key()) {
				case Qt::Key_Q:
				case Qt::Key_Escape:
					quitCB();
					break;
			}
		}
		default:
			return false;
	}
	return false;
}
 
MainWindow::~MainWindow() {
	if(glWidget) delete glWidget;
	
	if(file) delete file;
	if(renderPass) delete renderPass;
	if(view) delete view;

	if(openAct) delete openAct;
	if(quitAct) delete quitAct;
	if(texturingRP) delete texturingRP;
	if(bundlerPointsRP) delete bundlerPointsRP;
	if(displayRadar) delete displayRadar;
}