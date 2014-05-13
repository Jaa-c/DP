
#include "MainWindow.h"
#include "GLWidget.h"
#include "OpenForm.h"
#include "SettingsForm.h"
#include "ObjectForm.h"

#include <QApplication>
#include <QtGui/QtGui>

MainWindow::MainWindow(QApplication *app, int w, int h) : view(nullptr), init(false) {
	//Menu
	createActions();
	createMenus();
	
	statusBar = std::make_shared<QStatusBar>(this);
	setStatusBar(statusBar.get());
	statusBar->show();

	
	fpsLabel = std::make_shared<QLabel>("FPS: ", this);
	statusBar->addWidget(fpsLabel.get());
	fpsValue = std::make_shared<QLabel>("", this);
	statusBar->addWidget(fpsValue.get());
	
	texLabel = std::make_shared<QLabel>("| Using photos: ", this);
	statusBar->addWidget(texLabel.get());
	texValue = std::make_shared<QLabel>("", this);
	statusBar->addWidget(texValue.get());
		
	//OpenGL context format
	QGLFormat glFormat;
    glFormat.setVersion(4, 3);
    glFormat.setProfile(QGLFormat::CompatibilityProfile);

	//OpenGL widget:
    glWidget = std::make_shared<GLWidget>(glFormat, w, h - menuBar()->height(), this);
	glWidget->setAttribute(Qt::WA_NoMousePropagation);
	this->setCentralWidget(glWidget.get());
	
	app->installEventFilter(glWidget.get());	//opengl controlls
		
	initAppState();
}

void MainWindow::initAppState() {
	renderPass->setEnabled(false);
	settings->setEnabled(false);
}

void MainWindow::initScene() {
	if(texturingRP->isChecked()) {
		glWidget->addRenderPass(RenderPass::TEXTURING_PASS);
	}
	if(radarRP->isChecked()) {
		glWidget->addRenderPass(RenderPass::RADAR_PASS);
	}
	texturingRP->setChecked(true);
	radarRP->setChecked(true);
	renderPass->setEnabled(true);
	settings->setEnabled(true);
	
	//should be last, if nvidia not avaiable
	texturingPreRP->setChecked(true);
}

void MainWindow::createActions() {
	openAct = std::make_shared<QAction>(tr("&Open"), this);
	openAct->setStatusTip(tr("Open model"));
	openAct->setShortcut(Qt::Key_O | Qt::CTRL);
	connect(openAct.get(), SIGNAL(triggered()), this, SLOT(openCB()));
	
	quitAct = std::make_shared<QAction>(tr("&Quit"), this);
	quitAct->setStatusTip(tr("Exit application"));
	quitAct->setShortcut(Qt::Key_Escape);
	connect(quitAct.get(), SIGNAL(triggered()), this, SLOT(quitCB()));
	
	texGroup = std::make_shared<QActionGroup>(this);
	
	texturingPreRP = std::make_shared<QAction>(tr("Texturing &coverage Pass"), this);
	texturingPreRP->setCheckable(true);
	//texturingPreRP->setActionGroup(texGroup.get());
	connect(texturingPreRP.get(), SIGNAL(changed()), this, SLOT(texturingPrePassCB()));
	
	texturingRP = std::make_shared<QAction>(tr("&Texturing Pass"), this);
	texturingRP->setCheckable(true);
	texturingRP->setActionGroup(texGroup.get());
	connect(texturingRP.get(), SIGNAL(changed()), this, SLOT(texturingPassCB()));
		
	bundlerPointsRP = std::make_shared<QAction>(tr("&Display camera positions"), this);
	bundlerPointsRP->setCheckable(true);
	connect(bundlerPointsRP.get(), SIGNAL(changed()), this, SLOT(bundlerPointsPassCB()));
	
	normalsRP = std::make_shared<QAction>(tr("Display &normals"), this);
	normalsRP->setCheckable(true);
	connect(normalsRP.get(), SIGNAL(changed()), this, SLOT(normalsPassCB()));
	
	radarRP = std::make_shared<QAction>(tr("Display &radar"), this);
	radarRP->setCheckable(true);
	connect(radarRP.get(), SIGNAL(changed()), this, SLOT(radarPassCB()));
	
	programSettings = std::make_shared<QAction>(tr("Program &settings"), this);
	connect(programSettings.get(), SIGNAL(triggered()), this, SLOT(displaySettingsCB()));
	
	objectSettings = std::make_shared<QAction>(tr("&Object settings"), this);
	connect(objectSettings.get(), SIGNAL(triggered()), this, SLOT(displayObjectSettingsCB()));
		
	reloadShaders = std::make_shared<QAction>(tr("&Recompile shaders"), this);
	reloadShaders->setShortcut(Qt::Key_R | Qt::CTRL);
	connect(reloadShaders.get(), SIGNAL(triggered()), this, SLOT(reloadShadersCB()));
}

void MainWindow::createMenus() {
	file = menuBar()->addMenu(tr("File"));
	file->addAction(openAct.get());
	file->addSeparator();
	file->addAction(quitAct.get());
	
	renderPass = menuBar()->addMenu(tr("Render Passes"));
	renderPass->addAction(texturingPreRP.get());
	renderPass->addAction(texturingRP.get());
	renderPass->addAction(bundlerPointsRP.get());
	renderPass->addAction(normalsRP.get());
	renderPass->addAction(radarRP.get());
	
//	view = menuBar()->addMenu(tr("&View"));
//	view->addAction(displayRadar);
	
	settings = menuBar()->addMenu(tr("Settings"));
	settings->addAction(programSettings.get());
	settings->addAction(objectSettings.get());
	
	debug = menuBar()->addMenu(tr("Debug"));
	debug->addAction(reloadShaders.get());
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

void MainWindow::displayObjectSettingsCB() {
	ObjectForm form;
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

void MainWindow::texturingPrePassCB() {
	if(texturingPreRP->isChecked()) {
		glWidget->addRenderPass(RenderPass::TEXTURING_PRE_PASS);
	}
	else {
		glWidget->removeRenderPass(RenderPass::TEXTURING_PRE_PASS);
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

void MainWindow::normalsPassCB() {
	if(normalsRP->isChecked()) {
		glWidget->addRenderPass(RenderPass::NORMALS_PASS);
	}
	else {
		glWidget->removeRenderPass(RenderPass::NORMALS_PASS);
	}
}

void MainWindow::radarPassCB() {
	if(radarRP->isChecked()) {
		glWidget->addRenderPass(RenderPass::RADAR_PASS);
	}
	else {
		glWidget->removeRenderPass(RenderPass::RADAR_PASS);
	}
}

void MainWindow::setFps(float fps) {
	fpsValue->setText(QString::number(fps, 'f', 1));
}

void MainWindow::setPhotos(int p) {
	texValue->setText(QString::number(p));
}