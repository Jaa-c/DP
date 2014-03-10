#include "../globals.h"
#include "GLWidget.h"

#include "../RenderPass/TexturingRenderPass.h"
#include "../RenderPass/BundlerPointsRenderPass.h"
#include "../io/ImageLoader.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QProgressDialog>
#include <QApplication>

#include <ctime>
#include <sys/time.h>

void GLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	if(object) {
		camera.updateCameraViewMatrix();
		glm::vec3 viewDir = object->getCentroidPosition() + camera.getCameraPosition();		
		textureHandler->updateTextures(viewDir, object->mvm, tmpCameras);
		
		if(camera.isCameraStatic()) {
			//todo!!
		}
		else {
			object->textures = &textureHandler->getTextures();
		}

		renderPassHandler.draw(object);

		glUseProgram(0);

		if(displayRadar) {
			radar->draw(&textureHandler->getTextures());
		}
	}
	
	gettimeofday(&end, NULL);
	if(((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000.0) > 5000) {
		Log::i("fps ~ %d", (int)(fps/5.0f + .5f));
		fps = 0;
		gettimeofday(&start, NULL);
	}
	fps++;
	
	this->update(); //TODO
}

void GLWidget::createScene(std::string geom, std::string bundler, std::string photos) {
	DELETE(textureHandler);
	DELETE(object);
	DELETE(radar);
	
	camera.resetView();
	
	QProgressDialog progress("Loading data", "", 0, 100, this);
	progress.show();
	
	auto prgcb = [&progress] (int p) {
		progress.setValue(progress.value() + p); 
		QApplication::processEvents();
	};
	
	try {
		ImageLoader imgLoader(photos, prgcb);
		BundlerParser bundlerData(bundler);
		textureHandler = new TextureHandler(bundlerData.parseFile(), imgLoader.getData());
		
		object = new ObjectData(geom);
		object->mvm = glm::rotate(object->mvm, 180.f, glm::vec3(1.0f, 0.0f, 0.0f));
		object->pointData = new PointData(textureHandler->getPhotos(), bundlerData.getPoints(), object->getCentroid());

		radar = new Radar(object, &camera, controlls);
		radar->setPosition(10, 10, 250, 250);
		
		controlls->setPhotos(&textureHandler->getPhotos());

		glClearColor(0.4f, 0.4f, 0.7f, 0);
	}
	catch(std::string msg) {
		QMessageBox messageBox;
		messageBox.critical(0, "Error!", msg.c_str());
	}
	progress.close();
}

void GLWidget::initializeGL() {
	GLuint err = glewInit();
	if (err != GLEW_OK) {
		Log::e("Unable to init glew: %s", glewGetErrorString(err));
		return;
	}

	// Set OpenGL state variables
	glClearColor(0.7f, 0.7f, 0.7f, 0);
}

bool GLWidget::eventFilter(QObject *, QEvent *event) {
	
	switch(event->type()) {
		case QEvent::MouseMove:
		{
			QMouseEvent *e = (QMouseEvent *) event;
			controlls->mousePositionChanged(e->x(), e->y());
			break;
		}
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent *e = (QMouseEvent *) event;
			controlls->mouseButtonChanged(e->buttons());
		}
		case QEvent::KeyPress:
		{
			QKeyEvent *e = (QKeyEvent *) event;
			controlls->keyboardAction(e->key());
		}
		default:
			return false;
	}
	return false;
}

void GLWidget::addRenderPass(RenderPass::RenderPassType pass) {
	switch(pass) {
		case RenderPass::TEXTURING_PASS:
			renderPassHandler.add(RenderPass::TEXTURING_PASS, new TexturingRenderPass(&renderer, &shaderHandler));
			break;
		case RenderPass::BUNDLER_POINTS_PASS:
			renderPassHandler.add(RenderPass::BUNDLER_POINTS_PASS, new BundlerPointsRenderPass(&renderer, &shaderHandler));
			break;
		default:
			return;
	}
}


void GLWidget::removeRenderPass(RenderPass::RenderPassType pass) {
	renderPassHandler.remove(pass);
}


void GLWidget::resizeGL(int w, int h) {
	controlls->windowSizeChangedImpl(w, h);
}

void GLWidget::setDisplayRadar(bool value) {
	displayRadar = value;
}


GLWidget::GLWidget(const QGLFormat& format, int w, int h, QWidget* parent) : 
	QGLWidget(format, parent),
	camera(w, h), 
	renderer(&camera),
	textureHandler(NULL), 
	radar(NULL), 
	object(NULL),
	displayRadar(false)
{		
	controlls = &Controlls::getInstance();
	controlls->setPointers(&camera, &shaderHandler);
	
	fps = 0;
    srand((unsigned)std::time(0)); 
	gettimeofday(&start, NULL);
	tmpCameras = 2;
}

GLWidget::~GLWidget() {
	DELETE(textureHandler);
	DELETE(object);
	DELETE(radar);
}
