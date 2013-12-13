/* 
 * File:   main.cpp
 * Author: jaa
 *
 * Created on 27. září 2013, 19:23
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "AntTweakBar/AntTweakBar.h"

#include <iostream>
#include <cassert>
#include <ctime>
#include <sys/time.h>
#include <vector>

const unsigned GL_ID_NONE = (unsigned)~(unsigned(0));

typedef unsigned char rgb;
typedef std::vector<rgb> Image;
typedef std::vector<glm::vec3> Points;

#include "Log.h"
#include "BundlerParser.h"
#include "ObjectData.h"
#include "ShaderHandler.h"
#include "Camera.h"
#include "Controlls.h"
#include "TextureHandler.h"
#include "Renderer.h"

#include "RenderPass/RenderPass.h"
#include "RenderPass/TexturingRenderPass.h"
#include "RenderPass/BundlerPointsRenderPass.h"
#include "RenderPassHandler.h"


// GLSL variables todo
GLuint g_WireMode = 0;

class Main {
	
	BundlerParser bp;
	RenderPassHandler renderPassHandler;
	Camera camera;
	ShaderHandler shaderHandler;
	Renderer renderer;
	TextureHandler textureHandler;
	
	ObjectData *object;
	Controlls * controlls;
	
public:
	Main(int windowWidth, int windowHeight) : 
		camera(windowWidth, windowHeight), renderer(&camera), 
		textureHandler("/home/jaa/Documents/FEL/DP/data/statue/photos/") 
	{
		const int defaultCameraID = 20;
		
		
		bp.parseFile("/home/jaa/Documents/FEL/DP/data/statue/bundle.out");
		controlls = &Controlls::getInstance();
		controlls->setPointers(&bp, &camera, &shaderHandler);
		controlls->setCameraId(defaultCameraID);
		
		renderPassHandler.add(RenderPass::TEXTURING_PASS, new TexturingRenderPass(&renderer, &shaderHandler));
		//renderPassHandler.add(RenderPass::BUNDLER_POINTS_PASS, new BundlerPointsRenderPass(&renderer, &shaderHandler, &bp));

		object = new ObjectData("/home/jaa/Documents/FEL/DP/data/statue/statue.obj");
		object->mvm = glm::rotate(object->mvm, 180.f, glm::vec3(1.0f, 0.0f, 0.0f));
		object->pointData = new PointData(&bp);
		object->texture = new Texture(GL_TEXTURE_RECTANGLE, 0);
		
	}
	
	~Main() {
		if(object) delete object;
	}

	void main_loop() {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);

		camera.updateCameraViewMatrix();
		
		if(!camera.isCameraStatic()) {
			glm::vec3 viewDir = -object->getCentroidPosition() + camera.getCameraPosition();
//			viewDir.x = (*camera.getModelViewMatrix())[0][2];
//			viewDir.y = (*camera.getModelViewMatrix())[1][2];
//			viewDir.z = (*camera.getModelViewMatrix())[2][2];
			const int cam = bp.getClosestCamera(viewDir);
			controlls->setCameraId(cam);
		}
		
		const int camID = controlls->getCameraId();
		object->texture->setImage(textureHandler.getImage(camID), &bp.getCameras()->at(camID));
		
		renderPassHandler.draw(object);
		
		glUseProgram(0);

	}
	
};

int main(int argc, char** argv) {
	const char *window_title = "Titulek";
	const int width = 1000;
	const int height = 800;
	
	Main main(width, height);
			
	// Intialize GLFW   
	if(!glfwInit()) {
		Log::e("Unable to init GLFW.");
		return 1;
	}

    // Create a window
    GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);
    glfwSetWindowPos(window, 100, 100);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

	GLuint err = glewInit();
    if (err != GLEW_OK) {
        Log::e("Unable to init glew.");
        return 1;
    }
	
	GLint texture_units = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
	Log::i("Avaiable texture units: %d", texture_units);
	
    // Set GLFW event callbacks
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetWindowSizeCallback(window, Controlls::windowSizeChanged);
    //glfwSetCharCallback(window, Controlls::keyboardAction);
    glfwSetKeyCallback(window, Controlls::keyboardAction);
    glfwSetMouseButtonCallback(window, Controlls::mouseButtonChanged);
    glfwSetCursorPosCallback(window, Controlls::mousePositionChanged);
	
	// Set OpenGL state variables
    glClearColor(0.4f, 0.4f, 0.7f, 0);
	
	struct timeval start, end;
	int fps = 0;
    srand((unsigned)std::time(0)); 
	gettimeofday(&start, NULL);
	
   // Main loop
    while(!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {

		main.main_loop();

		gettimeofday(&end, NULL);
		if(((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000.0) > 5000) {
			Log::i("fps ~ %d", (int)(fps/5.0f + .5f));
			fps = 0;
			gettimeofday(&start, NULL);
		}
		fps++;
        
        // Present frame buffer
        glfwSwapBuffers(window);
		/// Poll for and process events
        glfwPollEvents();
    }
	
    glfwTerminate();    // Terminate GLFW
    return 0;
}

