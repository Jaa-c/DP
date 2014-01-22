/* 
 * File:   main.cpp
 * Author: jaa
 *
 * Created on 27. září 2013, 19:23
 */

#include <GL/glew.h>
#include "globals.h"

#include <QtGlobal>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLContext>
#include <QtGui/QMouseEvent>
#include <QtGui/QtGui>

#include "GLWidget.h"


#include "glm/glm.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <cassert>
#include <ctime>
#include <sys/time.h>
#include <vector>


#include "RenderPass/RenderPass.h"
#include "RenderPass/TexturingRenderPass.h"
#include "RenderPass/BundlerPointsRenderPass.h"





// GLSL variables todo
GLuint g_WireMode = 0;

GLWidget::GLWidget(const QGLFormat& format, int w, int h, QWidget* parent) : 
	QGLWidget( format, parent ),
	camera(w, h), 
	renderer(&camera), 
	textureHandler("/home/jaa/Documents/FEL/DP/data/statue/photos/") 
{		
	const int defaultCameraID = 20;

	bp.parseFile("/home/jaa/Documents/FEL/DP/data/statue/bundle.out");
	controlls = &Controlls::getInstance();
	controlls->setPointers(&bp, &camera, &shaderHandler);
	controlls->setCameraId(defaultCameraID);

	renderPassHandler.add(RenderPass::TEXTURING_PASS, new TexturingRenderPass(&renderer, &shaderHandler));
//		renderPassHandler.add(RenderPass::BUNDLER_POINTS_PASS, new BundlerPointsRenderPass(&renderer, &shaderHandler));

	object = new ObjectData("/home/jaa/Documents/FEL/DP/data/statue/statue.obj");
	object->mvm = glm::rotate(object->mvm, 180.f, glm::vec3(1.0f, 0.0f, 0.0f));
	object->pointData = new PointData(&bp, object->getCentroid());
	object->texture = new Texture(GL_TEXTURE_RECTANGLE, 0);

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
	glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);

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

	drawRadar(10, 10, 250, 250);
	
	
	this->update(); //TODO
}

bool GLWidget::eventFilter(QObject *obj, QEvent *event) {
	
	switch(event->type()) {
		case QEvent::MouseMove:
		{
			QMouseEvent *e = (QMouseEvent *) event;
			controlls->mousePositionChangedImpl(e->x(), e->y());
			break;
		}
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent *e = (QMouseEvent *) event;
			controlls->mouseButtonChangedImpl(e->buttons());
		}
		case QEvent::KeyPress:
		{
			QKeyEvent *e = (QKeyEvent *) event;
			controlls->keyboardActionImpl(e->key(), e->type());
		}
		default:
			return false;
	}
	return false;
}


void GLWidget::resizeGL(int w, int h) {
	controlls->windowSizeChangedImpl(w, h);
}


int main(int argc, char** argv) {
	const char *window_title = "Titulek";
	const int width = 1000;
	const int height = 800;
		
	QApplication app(argc, argv );

    QGLFormat glFormat;
    glFormat.setVersion(3, 2);
    glFormat.setProfile(QGLFormat::CompatibilityProfile);
    glFormat.setSampleBuffers(true);

    GLWidget window(glFormat, width, height);
	window.setWindowTitle(window_title);
	window.resize(width, height);
    window.show();
	
	app.installEventFilter(&window);
		
	GLint texture_units = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
	Log::i("Avaiable texture units for FS: %d", texture_units);
	
    return app.exec();
			
//	// Intialize GLFW   
//	if(!glfwInit()) {
//		Log::e("Unable to init GLFW.");
//		return 1;
//	}
//
//    // Create a window
//    GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);
//    glfwSetWindowPos(window, 100, 100);
//    /* Make the window's context current */
//    glfwMakeContextCurrent(window);
//
//	GLuint err = glewInit();
//    if (err != GLEW_OK) {
//        Log::e("Unable to init glew.");
//        return 1;
//    }
//	

//	
//    // Set GLFW event callbacks
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//    glfwSetWindowSizeCallback(window, Controlls::windowSizeChanged);
//    //glfwSetCharCallback(window, Controlls::keyboardAction);
//    glfwSetKeyCallback(window, Controlls::keyboardAction);
//    glfwSetMouseButtonCallback(window, Controlls::mouseButtonChanged);
//    glfwSetCursorPosCallback(window, Controlls::mousePositionChanged);
//	
//	// Set OpenGL state variables
//    glClearColor(0.4f, 0.4f, 0.7f, 0);
//	
//	struct timeval start, end;
//	int fps = 0;
//    srand((unsigned)std::time(0)); 
//	gettimeofday(&start, NULL);
//	
//   // Main loop
//    while(!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
//
//		main.main_loop();
//
//		gettimeofday(&end, NULL);
//		if(((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000.0) > 5000) {
//			Log::i("fps ~ %d", (int)(fps/5.0f + .5f));
//			fps = 0;
//			gettimeofday(&start, NULL);
//		}
//		fps++;
//        
//        // Present frame buffer
//        glfwSwapBuffers(window);
//		/// Poll for and process events
//        glfwPollEvents();
//    }
//	
//    glfwTerminate();    // Terminate GLFW
//    return 0;
}

