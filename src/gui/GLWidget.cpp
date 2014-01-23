
#include "GLWidget.h"

#include "../RenderPass/TexturingRenderPass.h"
#include "../RenderPass/BundlerPointsRenderPass.h"

#include <QtGui/QMouseEvent>

#include <ctime>
#include <sys/time.h>

GLWidget::GLWidget(const QGLFormat& format, int w, int h, QWidget* parent) : 
	QGLWidget(format, parent),
	camera(w, h), 
	renderer(&camera), 
	textureHandler("/home/jaa/Documents/FEL/DP/data/statue/photos/"),
	displayRadar(false)
{		
	const int defaultCameraID = 20;

	bp.parseFile("/home/jaa/Documents/FEL/DP/data/statue/bundle.out");
	controlls = &Controlls::getInstance();
	controlls->setPointers(&bp, &camera, &shaderHandler);
	controlls->setCameraId(defaultCameraID);

//	renderPassHandler.add(RenderPass::TEXTURING_PASS, new TexturingRenderPass(&renderer, &shaderHandler));
//	renderPassHandler.add(RenderPass::BUNDLER_POINTS_PASS, new BundlerPointsRenderPass(&renderer, &shaderHandler));

	object = new ObjectData("/home/jaa/Documents/FEL/DP/data/statue/statue.obj");
	object->mvm = glm::rotate(object->mvm, 180.f, glm::vec3(1.0f, 0.0f, 0.0f));
	object->pointData = new PointData(&bp, object->getCentroid());
	object->texture = new Texture(GL_TEXTURE_RECTANGLE, 0);
	
	fps = 0;
    srand((unsigned)std::time(0)); 
	gettimeofday(&start, NULL);

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

GLWidget::~GLWidget() {
	if(object) delete object;
}

void GLWidget::initializeGL()
{
	GLuint err = glewInit();
	if (err != GLEW_OK) {
		Log::e("Unable to init glew: %s", glewGetErrorString(err));
		return;
	}

	// Set OpenGL state variables
	glClearColor(0.4f, 0.4f, 0.7f, 0);
}

void GLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	camera.updateCameraViewMatrix();

	if(!camera.isCameraStatic()) {
		glm::vec3 viewDir = object->getCentroidPosition() + camera.getCameraPosition();
		const int cam = bp.getClosestCamera(viewDir, object->mvm);
		controlls->setCameraId(cam);
	}

	const int camID = controlls->getCameraId();
	object->texture->setImage(textureHandler.getImage(camID), &bp.getCameras()->at(camID));

	renderPassHandler.draw(object);

	glUseProgram(0);

	if(displayRadar) {
		drawRadar(10, 10, 250, 250);
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

bool GLWidget::eventFilter(QObject *obj, QEvent *event) {
	
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


void GLWidget::resizeGL(int w, int h) {
	controlls->windowSizeChangedImpl(w, h);
}

void GLWidget::setDisplayRadar(bool value) {
	displayRadar = value;
}