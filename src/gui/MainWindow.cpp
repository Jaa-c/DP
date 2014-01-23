
#include "MainWindow.h"
#include "GLWidget.h"

#include <QApplication>
#include <QtGui/QtGui>


MainWindow::MainWindow(QApplication *app, int w, int h) {
	
	//Menu
	quitAct = new QAction(tr("&Quit"), this);
	quitAct->setStatusTip(tr("Exit application"));
	connect(quitAct, SIGNAL(triggered()), this, SLOT(quit()));
	
	texturingRP = new QAction(tr("&Texturing Pass"), this);
	texturingRP->setCheckable(true);
	connect(texturingRP, SIGNAL(changed()), this, SLOT(texturingPass()));
	
	bundlerPointsRP = new QAction(tr("&Bundler Points Pass"), this);
	bundlerPointsRP->setCheckable(true);
	connect(bundlerPointsRP, SIGNAL(changed()), this, SLOT(bundlerPointsPass()));

	file = menuBar()->addMenu(tr("&File"));
	file->addAction(quitAct);
	
	renderPass = menuBar()->addMenu(tr("&Render Passes"));
	renderPass->addAction(texturingRP);
	renderPass->addAction(bundlerPointsRP);
	
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
	
	
	//Initial state of application
	texturingRP->setChecked(true);
}

MainWindow::~MainWindow() {
	if(glWidget) delete glWidget;
	if(file) delete file;
	if(renderPass) delete renderPass;

	if(quitAct) delete quitAct;
	if(texturingRP) delete texturingRP;
	if(bundlerPointsRP) delete bundlerPointsRP;
}

 void MainWindow::quit() {
	 QApplication::quit();
 }
 
void MainWindow::texturingPass() {
	if(texturingRP->isChecked()) {
		glWidget->addRenderPass(RenderPass::TEXTURING_PASS);
	}
	else {
		glWidget->removeRenderPass(RenderPass::TEXTURING_PASS);
	}
}

void MainWindow::bundlerPointsPass() {
	if(bundlerPointsRP->isChecked()) {
		glWidget->addRenderPass(RenderPass::BUNDLER_POINTS_PASS);
	}
	else {
		glWidget->removeRenderPass(RenderPass::BUNDLER_POINTS_PASS);
	}
}

 bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
	switch(event->type()) {
		case QEvent::KeyPress:
		{
			QKeyEvent *e = (QKeyEvent *) event;
			switch(e->key()) {
				case Qt::Key_Q:
				case Qt::Key_Escape:
					quit();
					break;
			}
		}
		default:
			return false;
	}
	return false;
}