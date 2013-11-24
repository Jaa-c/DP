/* 
 * File:   main.cpp
 * Author: jaa
 *
 * Created on 27. září 2013, 19:23
 */

#include <GL/glew.h>
#include <GL/glfw.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <cassert>
#include <ctime>
#include <sys/time.h>

const unsigned GL_ID_NONE = (unsigned)~(unsigned(0));

#include "Log.h"
#include "BundlerParser.h"
#include "ObjectData.h"
#include "ShaderHandler.h"
#include "Camera.h"
#include "Renderer.h"
#include "Controlls.h"

#include "RenderPass/RenderPass.h"
#include "RenderPass/TexturingRenderPass.h"
#include "RenderPass/BundlerPointsRenderPass.h"
#include "RenderPassHandler.h"


// GLSL variables
GLuint g_WireMode = 0;
GLuint pointsVBO = 0;
GLuint camPosVBO = 0;
GLuint pboId = 0;


void printMat(glm::mat4 m) {
	std::cout <<m[0][0]<<" "<<m[0][1]<<" "<<m[0][2]<<" "<<m[0][3]<<"\n";
	std::cout <<m[1][0]<<" "<<m[1][1]<<" "<<m[1][2]<<" "<<m[1][3]<<"\n";
	std::cout <<m[2][0]<<" "<<m[2][1]<<" "<<m[2][2]<<" "<<m[2][3]<<"\n";
	std::cout <<m[3][0]<<" "<<m[3][1]<<" "<<m[3][2]<<" "<<m[3][3]<<"\n\n";
}

class Main {
	
	BundlerParser bp;
	RenderPassHandler renderPassHandler;
	Camera camera;
	ShaderHandler shaderHandler;
	Renderer renderer;
	
	ObjectData *object;
	
public:
	Main(int windowWidth, int windowHeight) : camera(windowWidth, windowHeight), renderer(&camera)	{		
		
		bp.parseFile("/home/jaa/Dokumenty/FEL/DP/data/bundle.rd.out");
		Controlls::getInstance().setPointers(&bp, &camera, &shaderHandler);
		
		renderPassHandler.add(RenderPass::TEXTURING_PASS, new TexturingRenderPass(&renderer, &shaderHandler));
		renderPassHandler.add(RenderPass::BUNDLER_POINTS_PASS, new BundlerPointsRenderPass(&renderer, &shaderHandler, &bp));

		object = new ObjectData(std::string("/home/jaa/Documents/FEL/DP/data/statue.obj"));
		
		std::vector<rgb> image;
		int width, height;
		DataLoader::loadJPEG("/home/jaa/Documents/FEL/DP/data/visualize/00000000.jpg", image, width, height);
		assert(image.size() == width * height * 3);
		
		object->texture = new Texture(GL_TEXTURE_RECTANGLE, 0);
		object->texture->setImage(&image, width, height, &bp.getCameras()->at(0));

	}
	
	~Main() {
		if(object) delete object;
	}

	void main_loop() {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);

		camera.updateCameraViewMatrix();

		renderPassHandler.draw(object);

	//	glDepthFunc(GL_LESS);     // We want to get the nearest pixels
	//	glColorMask(0,0,0,0);     // Disable color, it's useless, we only want depth.
	//	glDepthMask(GL_TRUE);     // Ask z writing
	//	
	//	renderer.draw(*object);
	//	
	//	int visible = 0;
	//
	//	glReadPixels(0, 0, 1000, 800, GL_DEPTH_COMPONENT, GL_FLOAT, &depth_data);
	//	typedef  std::vector<glm::vec3>::const_iterator points;
	//	glm::mat4 mvp = *projection * *modelView;
	//	for(points it = object->getVertices().begin(); it != object->getVertices().end(); ++it) {
	//		glm::vec4 v =  mvp * glm::vec4(*it, 1.0);
	//		v /= v.w;
	//		int x = (int) (500 * (v.x + 1));
	//		int y = (int) (400 * (v.y + 1));
	//		//Log::d("c: %f, depth: %f", (v.z + 1)/2.0f, depth_data[x + 1000 * y]);
	//		if(x >= 0 && y >= 0 && x < 1000 && y < 800) {
	//			if((v.z + 1)/2.0f <= depth_data[x + 1000 * y])
	//				visible++;
	//		}
	//	}
	//	Log::i("visible: %d", visible);
	//	
	//	glDepthFunc(GL_LEQUAL);
	//	glColorMask(1,1,1,1);     // We want color this time
	//	glDepthMask(GL_FALSE);

		//renderer.draw(*object);

		glUseProgram(0);

	}
	
};

int main(int argc, char** argv) {
	
	const char *window_title = "Titulek";
	int width = 1000;
	int height = 800;
	
	Main main(width, height);
	
	// Intialize GLFW   
	glfwInit();

    // Create a window
    glfwOpenWindow(width, height, 0, 0, 0, 0, 32, 0, GLFW_WINDOW);
    glfwSetWindowTitle(window_title);
    glfwSetWindowPos(100, 100);

    glfwEnable(GLFW_MOUSE_CURSOR);
    glfwEnable(GLFW_KEY_REPEAT);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        Log::e("Error: %s", glewGetErrorString(err));
        assert(0);
        return 1;
    }
	
	// Set OpenGL state variables
    glClearColor(0.4f, 0.4f, 0.7f, 0);
	
    // Set GLFW event callbacks
    glfwSetWindowSizeCallback(Controlls::windowSizeChanged);
    glfwSetCharCallback(Controlls::keyboardAction);
    glfwSetKeyCallback(Controlls::keyboardAction);
    glfwSetMouseButtonCallback(Controlls::mouseButtonChanged);
    glfwSetMousePosCallback(Controlls::mousePositionChanged);
	
	struct timeval start, end;
	int fps = 0;
    srand((unsigned)std::time(0)); 
	gettimeofday(&start, NULL);
	
   // Main loop
    while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC)) {

		main.main_loop();

		gettimeofday(&end, NULL);
		if(((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000.0) > 5000) {
			Log::i("fps ~ %d", (int)(fps/5.0f + .5f));
			fps = 0;
			gettimeofday(&start, NULL);
		}
		fps++;
        
        // Present frame buffer
        glfwSwapBuffers();
    }
	
    glfwTerminate();    // Terminate GLFW
	
    return 0;
}

