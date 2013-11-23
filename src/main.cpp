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
#include "cube.h"
#include "ObjectData.h"
#include "ShaderHandler.h"
#include "Renderer.h"
#include "Controlls.h"
#include "BundlerParser.h"

#include "RenderPass/RenderPass.h"
#include "RenderPass/TexturingRenderPass.h"
#include "RenderPassHandler.h"

//TODO
RenderPassHandler *renderPassHandler;
ObjectData *object;
Controlls *controlls;
ShaderHandler *shaderHandler;

// GLSL variables
GLuint g_WireMode = 0;
GLuint pointsVBO = 0;
GLuint camPosVBO = 0;
GLuint pboId = 0;

float * cameraPos;
float depth_data[1000*800];

void printMat(glm::mat4 m) {
	std::cout <<m[0][0]<<" "<<m[0][1]<<" "<<m[0][2]<<" "<<m[0][3]<<"\n";
	std::cout <<m[1][0]<<" "<<m[1][1]<<" "<<m[1][2]<<" "<<m[1][3]<<"\n";
	std::cout <<m[2][0]<<" "<<m[2][1]<<" "<<m[2][2]<<" "<<m[2][3]<<"\n";
	std::cout <<m[3][0]<<" "<<m[3][1]<<" "<<m[3][2]<<" "<<m[3][3]<<"\n\n";
	
}


void main_loop() {
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);
	
	controlls->updateCameraViewMatrix();

	renderPassHandler->draw(object);
	
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
	
	
	
//	
//	programID = shaderHandler->getProgramId(ShaderHandler::SHADER_POINTS);
//	glEnable(GL_PROGRAM_POINT_SIZE );
//	glUseProgram(programID);
//	renderer->bindCameraMatrices(programID);
//	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * bp.getPoints()->size(), &bp.getPoints()->at(0).x, GL_STATIC_DRAW); 
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //index 0, 3 floats per vertex
//    glEnableVertexAttribArray(0);//Enable attribute index 0 as being used 
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	
//	glDrawArrays(GL_POINTS, 0, bp.getPoints()->size());
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	
//	shader = ShaderHandler::SHADER_CAMERAS;
//	glUseProgram(shaderHandler->getProgramId(shader));    // Active shader program
//	renderer->bindCameraMatrices(programID);
//	
//	glGenBuffers(1, &camPosVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, camPosVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) *  bp.getCameras()->size() * 3, cameraPos, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //index 0, 3 floats per vertex
//    glEnableVertexAttribArray(0);//Enable attribute index 0 as being used 
//	
//	glDrawArrays(GL_POINTS, 0, bp.getCameras()->size());
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glUseProgram(0);

}

//-----------------------------------------------------------------------------
// Name: cbWindowSizeChanged()
// Desc: 
//-----------------------------------------------------------------------------
//void cbWindowSizeChanged(int width, int height)
//{
//    glViewport(0, 0, width, height);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(45.0f, GLfloat(width)/height, 0.1f, 1000.0f);
//    glMatrixMode(GL_MODELVIEW);
//
//    g_WindowWidth  = width;
//    g_WindowHeight = height;
//}

void keyBoardCallback(int key, int action) {
	controlls->keyboardAction(key, action);
	if(action != GLFW_PRESS) {
		return;
	}
	switch (key) {
		case 'R':// recompile shaders
			shaderHandler->resetShaders();
			break;
	}
}

void mouseButtonCallback(int button, int action) {
	controlls->mouseButtonChanged(button, action);
}

void mousePositionCallback(int x, int y) {
	controlls->mousePositionChanged(x, y);
}


int main(int argc, char** argv) {
	
	const int window_width = 1000;
	const int window_height = 800;
	const char *window_title = "Titulek";
	
	BundlerParser bp;
	bp.parseFile("/home/jaa/Dokumenty/FEL/DP/data/bundle.rd.out");
	controlls = new Controlls (window_width, window_height, &bp);
	
    // Intialize GLFW   
    glfwInit();
	
	Renderer renderer(controlls);
	shaderHandler = new ShaderHandler();
	
	renderPassHandler = new RenderPassHandler();
	renderPassHandler->add(RenderPass::TEXTURING_PASS, new TexturingRenderPass(&renderer, shaderHandler, &bp));
	
	object = new ObjectData(std::string("/home/jaa/Documents/FEL/DP/data/statue.obj"));
	std::vector<rgb> image;
	int width, height;
	DataLoader::loadJPEG("/home/jaa/Documents/FEL/DP/data/visualize/00000000.jpg", image, width, height);
	assert(image.size() == width * height * 3);
	object->texture = new Texture(GL_TEXTURE_RECTANGLE, 0);
	object->texture->setImage(&image, width, height);
	
    // Create a window
    glfwOpenWindow(window_width, window_height, 0, 0, 0, 0, 32, 0, GLFW_WINDOW);
    glfwSetWindowTitle(window_title);
    glfwSetWindowPos(100, 100);

    glfwEnable(GLFW_MOUSE_CURSOR);
    glfwEnable(GLFW_KEY_REPEAT);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        printf("Error: %s\n", glewGetErrorString(err));
        assert(0);
        return 1;
    }
	
	// Set OpenGL state variables
    glClearColor(0.4f, 0.4f, 0.7f, 0);
	
	//TODO
	glGenBuffers(1, &pointsVBO);
	glGenBuffers(1, &camPosVBO);
	
	int cams = bp.getCameras()->size();
	cameraPos = new float[cams * 3];
	
	for(int i = 0; i < cams; i++) {
		Camera * c = &bp.getCameras()->at(i);
		glm::vec3 v = -1 * glm::transpose(c->rotate) * c->translate;
		cameraPos[i*3] = v[0];
		cameraPos[i*3+1] = v[1];
		cameraPos[i*3+2] = v[2];
	}

    // Set GLFW event callbacks
    //glfwSetWindowSizeCallback(_cbWindowSizeChanged);
    glfwSetCharCallback(keyBoardCallback);
    glfwSetKeyCallback(keyBoardCallback);
    glfwSetMouseButtonCallback(mouseButtonCallback);
    glfwSetMousePosCallback(mousePositionCallback);
	
	struct timeval start, end;
	int fps = 0;
    srand((unsigned)std::time(0)); 
	gettimeofday(&start, NULL);
	
   // Main loop
    while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC)) {
        if (main_loop) {
			
            main_loop();
			
			gettimeofday(&end, NULL);
			if(((end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000.0) > 5000) {
				Log::i("fps ~ %d", (int)(fps/5.0f + .5f));
				fps = 0;
				gettimeofday(&start, NULL);
			}
			fps++;
        }
        // Present frame buffer
        glfwSwapBuffers();
    }
	
    glfwTerminate();    // Terminate GLFW
	
	delete controlls;
	delete cameraPos;
	delete shaderHandler;
	delete renderPassHandler;
    return 0;
}

