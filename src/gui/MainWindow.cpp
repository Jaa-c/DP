
#include "MainWindow.h"
#include "GLWidget.h"
#include "OpenForm.h"
#include "SettingsForm.h"

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
	
	app->installEventFilter(glWidget);	//opengl controlls
		
	initAppState();
}

void MainWindow::initAppState() {

//	glWidget->createScene(
//		"/home/jaa/Documents/FEL/DP/data/statue/statue.obj",
//		"/home/jaa/Documents/FEL/DP/data/statue/bundle.out",
//		"/home/jaa/Documents/FEL/DP/data/statue/photos/"
//	);
}

void MainWindow::initScene() {
	basicTexturingRP->setChecked(true);
	displayRadar->setChecked(true);
}

void MainWindow::createActions() {
	openAct = new QAction(tr("&Open"), this);
	openAct->setStatusTip(tr("Open model"));
	openAct->setShortcut(Qt::Key_O | Qt::CTRL);
	connect(openAct, SIGNAL(triggered()), this, SLOT(openCB()));
	
	quitAct = new QAction(tr("&Quit"), this);
	quitAct->setStatusTip(tr("Exit application"));
	quitAct->setShortcut(Qt::Key_Escape);
	connect(quitAct, SIGNAL(triggered()), this, SLOT(quitCB()));
	
	texGroup = new QActionGroup(this);
	
	texturingRP = new QAction(tr("&Texturing Pass"), this);
	texturingRP->setCheckable(true);
	texturingRP->setActionGroup(texGroup);
	connect(texturingRP, SIGNAL(changed()), this, SLOT(texturingPassCB()));
	
	basicTexturingRP = new QAction(tr("B&asic Texturing Pass"), this);
	basicTexturingRP->setCheckable(true);
	basicTexturingRP->setActionGroup(texGroup);
	connect(basicTexturingRP, SIGNAL(changed()), this, SLOT(basicTexturingPassCB()));
	
	bundlerPointsRP = new QAction(tr("&Bundler Points Pass"), this);
	bundlerPointsRP->setCheckable(true);
	connect(bundlerPointsRP, SIGNAL(changed()), this, SLOT(bundlerPointsPassCB()));
	
	displayRadar = new QAction(tr("Display &radar"), this);
	displayRadar->setCheckable(true);
	connect(displayRadar, SIGNAL(changed()), this, SLOT(displayRadarCB()));
	
	programSettings = new QAction(tr("Program &settings"), this);
	connect(programSettings, SIGNAL(triggered()), this, SLOT(displaySettingsCB()));
	
	reloadShaders = new QAction(tr("&Recompile shaders"), this);
	reloadShaders->setShortcut(Qt::Key_R | Qt::CTRL);
	connect(reloadShaders, SIGNAL(triggered()), this, SLOT(reloadShadersCB()));
}

void MainWindow::createMenus() {
	file = menuBar()->addMenu(tr("&File"));
	file->addAction(openAct);
	file->addSeparator();
	file->addAction(quitAct);
	
	renderPass = menuBar()->addMenu(tr("&Render Passes"));
	renderPass->addAction(texturingRP);
	renderPass->addAction(basicTexturingRP);
	renderPass->addAction(bundlerPointsRP);
	
//	view = menuBar()->addMenu(tr("&View"));
//	view->addAction(displayRadar);
	
	settings = menuBar()->addMenu(tr("&Settings"));
	settings->addAction(displayRadar);
	settings->addAction(programSettings);
	
	debug = menuBar()->addMenu(tr("&Debug"));
	debug->addAction(reloadShaders);
}

void MainWindow::reloadShadersCB() {
	glWidget->shaderHandler.resetShaders();
	glWidget->renderPassHandler.resetShaderIDs();
}

void MainWindow::openCB() {
	OpenForm form(glWidget);
	form.exec();
	initScene();
}

void MainWindow::displaySettingsCB() {
	SettingsForm form(glWidget);
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

void MainWindow::basicTexturingPassCB() {
	if(basicTexturingRP->isChecked()) {
		glWidget->addRenderPass(RenderPass::BASIC_TEXTURING_PASS);
	}
	else {
		glWidget->removeRenderPass(RenderPass::BASIC_TEXTURING_PASS);
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

MainWindow::~MainWindow() {
	DELETE(glWidget);
	
	DELETE(file);
	DELETE(renderPass);
	DELETE(view);

	DELETE(openAct);
	DELETE(quitAct);
	DELETE(texturingRP);
	DELETE(basicTexturingRP);
	DELETE(bundlerPointsRP);
	DELETE(displayRadar);
	
	DELETE(texGroup);
	
	//TODO
}