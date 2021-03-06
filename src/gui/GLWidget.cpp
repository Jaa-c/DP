#include "../globals.h"
#include "GLWidget.h"

#include "../RenderPass/BasicTexturingRenderPass.h"
#include "../RenderPass/TexturingRenderPass.h"
#include "../RenderPass/TexturingPrePass.h"
#include "../RenderPass/CameraPointsRenderPass.h"
#include "../RenderPass/RadarRenderPass.h"
#include "../RenderPass/NormalsDebugPass.h"
#include "../io/CalibrationLoader.h"
#include "../io/ImageLoader.h"
#include "../Settings.h"
#include "../RayCaster.h"
#include "src/Settings.h"

#include <QtGui/QMouseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QProgressDialog>
#include <QApplication>

#include <ctime>
#include <sys/time.h>

const std::string GLWidget::settingsExt = "-settings.conf";

void GLWidget::paintGL() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(object) {
		camera.updateCameraViewMatrix();
		if(Settings::circle) camera.circle(object->getCentroidPosition());
		
		object->rotate(Settings::objectRotate);
		textureHandler->emptyClusters();
		
		renderPassHandler.draw(object);
		
		glUseProgram(0);
		
		mainWin.setPhotos(textureHandler->getTextures().size());
	}
	
	float time = gltimer->end();
	gltimer->start();
	if(Settings::circle) std::cout << time << ";" << Settings::temp << "\n";
	Settings::temp = 0;
	if(++f == 10) {
		mainWin.setFps(1000.f / time);
		f = 0;
	}
	this->update(); //TODO
}

void GLWidget::deleteScene() {
	if(object) {
		std::string file = object->fileName + settingsExt;
		Settings::serialize(file);
	}
	
	camera.resetView();
	renderPassHandler.removeAll();
	
	glClearColor(0.7f, 0.7f, 0.7f, 0);
}

void GLWidget::createScene(
		std::string geom, 
		std::string photos, 
		CalibrationLoader::FileType type,
		std::string file1,
		std::string file2)
{
	object.reset();
	textureHandler.reset();
	if(object) {
		throw std::string("Error while creating scene!");
	}
	QProgressDialog progress("Loading data", "", 0, 100, this);
	progress.setValue(0);
	progress.show();
	
	auto prgcb = [&progress] (int p) {
		progress.setValue(progress.value() + p); 
		QApplication::processEvents();
	};
	
	try {
		Settings::deserialize(geom + settingsExt);
		textureHandler = std::make_shared<TextureHandler>();
		object = std::make_shared<ObjectData>(geom);
		
		CalibrationLoader cl(textureHandler, *object, prgcb);
		cl.loadData(type, photos, file1, file2);
				
		object->pointData = cl.getPointData();
		object->rotate(Settings::objectRotate);
		
		controlls.setPhotos(&textureHandler->getPhotos());
				
		glClearColor(0.4f, 0.4f, 0.7f, 0);
	}
	catch(std::string &msg) {
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
	
	GLint texture_units = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
	Log::i("Avaiable texture units for FS: %d", texture_units);
	if(texture_units > 0) {
		Settings::maxTextures = texture_units;
	}

	// Set OpenGL state variables
	glClearColor(0.7f, 0.7f, 0.7f, 0);
	gltimer = std::make_shared<GLTimer>();
}

bool GLWidget::eventFilter(QObject *, QEvent *event) {
	if(!mainWin.isActiveWindow()) {
		return false;
	}
	switch(event->type()) {
		case QEvent::MouseMove:
		{
			QMouseEvent *e = (QMouseEvent *) event;
			controlls.mousePositionChanged(e->x(), e->y());
			break;
		}
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent *e = (QMouseEvent *) event;
			controlls.mouseButtonChanged(e->buttons());
			break;
		}
		case QEvent::KeyPress:
		{
			QKeyEvent *e = (QKeyEvent *) event;
			controlls.keyboardAction(e->key());
		}
		default:
			return false;
	}
	return false;
}

void GLWidget::addRenderPass(RenderPass::RenderPassType pass) {
	assert(textureHandler); //otherwise logic is wrong
	switch(pass) {
		case RenderPass::BASIC_TEXTURING_PASS:
			renderPassHandler.add(
				RenderPass::BASIC_TEXTURING_PASS, 
				std::make_shared<BasicTexturingRenderPass>(renderer, shaderHandler, textureHandler)
			);
			break;
		case RenderPass::TEXTURING_PRE_PASS:
		{
			if(!GLEW_NV_shader_atomic_float) {
				throw std::string(
						"This texturing pass requires nVidia card " 
						"with GL_NV_shader_atomic_float extension.");
			}
			
			renderPassHandler.remove(RenderPass::TEXTURING_PASS);
			
			std::shared_ptr<TexturingPrePass> pp = std::make_shared<TexturingPrePass>(renderer, shaderHandler, textureHandler);
			renderPassHandler.add(RenderPass::TEXTURING_PRE_PASS, pp);
			
			renderPassHandler.add(
				RenderPass::TEXTURING_PASS, 
				std::make_shared<TexturingRenderPass>(renderer, shaderHandler, textureHandler, pp)
			);
			break;
		}
		case RenderPass::TEXTURING_PASS:
		{
			renderPassHandler.add(
				RenderPass::TEXTURING_PASS, 
				std::make_shared<TexturingRenderPass>(renderer, shaderHandler, textureHandler, nullptr)
			);
			break;
		}
		case RenderPass::BUNDLER_POINTS_PASS:
			renderPassHandler.add(
				RenderPass::BUNDLER_POINTS_PASS,
				std::make_shared<CameraPointsRenderPass>(renderer, shaderHandler, textureHandler)
			);
			break;
		case RenderPass::RADAR_PASS:
			renderPassHandler.add(
				RenderPass::RADAR_PASS,
				std::make_shared<RadarRenderPass>(renderer, shaderHandler, textureHandler, camera)
			);
			break;
		case RenderPass::NORMALS_PASS:
			renderPassHandler.add(
				RenderPass::NORMALS_PASS,
				std::make_shared<NormalsDebugPass>(renderer, shaderHandler, textureHandler)
			);
			break;
		default:
			return;
	}
}


void GLWidget::removeRenderPass(RenderPass::RenderPassType pass) {
	renderPassHandler.remove(pass);
	switch(pass) {
		case RenderPass::TEXTURING_PASS:
			renderPassHandler.remove(RenderPass::TEXTURING_PRE_PASS);
			break;
		default:
			break;
	}
}


void GLWidget::resizeGL(int w, int h) {
	controlls.windowSizeChangedImpl(w, h);
}

GLWidget::GLWidget(const QGLFormat& format, int w, int h, QWidget* parent) :
	QGLWidget(format, parent),
	camera(w, h), 
	renderer(camera),
	controlls(Controlls::getInstance()),
	textureHandler(nullptr), 
	object(nullptr),
	gltimer(nullptr),
	mainWin(*dynamic_cast<MainWindow *>(parent))
{
	controlls.setPointers(&camera, &shaderHandler);
	f = 0;
}

GLWidget::~GLWidget() {
	if(object) {
		Settings::serialize(object->fileName + settingsExt);
	}
	
}
