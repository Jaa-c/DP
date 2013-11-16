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

const unsigned GL_ID_NONE = (unsigned)~(unsigned(0));

#include "cube.h"
#include "ShaderHandler.h"
#include "Controlls.h"
#include "BundlerParser.h"
#include "ObjectData.h"


// GLSL variables
GLuint g_WireMode = 0;
GLuint pointsVBO = 0;
GLuint camPosVBO = 0;

ShaderHandler *shaderHandler;
Controlls *controlls;
BundlerParser bp;

float * cameraPos;

void printMat(glm::mat4 &m) {
	std::cout <<m[0][0]<<" "<<m[0][1]<<" "<<m[0][2]<<" "<<m[0][3]<<"\n";
	std::cout <<m[1][0]<<" "<<m[1][1]<<" "<<m[1][2]<<" "<<m[1][3]<<"\n";
	std::cout <<m[2][0]<<" "<<m[2][1]<<" "<<m[2][2]<<" "<<m[2][3]<<"\n";
	std::cout <<m[3][0]<<" "<<m[3][1]<<" "<<m[3][2]<<" "<<m[3][3]<<"\n\n";
	
}

void initGL() {
	
    // Set OpenGL state variables
    glClearColor(0.4f, 0.4f, 0.7f, 0);
	
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	shaderHandler->compileShaderProgram(ShaderHandler::SHADER_TEST, true, false, true);
	shaderHandler->compileShaderProgram(ShaderHandler::SHADER_POINTS, true, false, true);
	shaderHandler->compileShaderProgram(ShaderHandler::SHADER_CAMERAS, true, false, true);

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


}

void main_loop() {
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);
	
	controlls->updateCameraViewMatrix();
	
	glm::mat4 * modelView = controlls->getModelViewMatrix();
	glm::mat4 * projection = controlls->getProjectionMatrix();
		
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE );
		
	ShaderHandler::ShaderList shader = ShaderHandler::SHADER_POINTS;
	glUseProgram(shaderHandler->getProgramId(shader));    // Active shader program
	
	glUniformMatrix4fv(glGetUniformLocation(shaderHandler->getProgramId(shader), "u_ModelViewMatrix"), 1, GL_FALSE, &(*modelView)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderHandler->getProgramId(shader), "u_ProjectionMatrix"), 1, GL_FALSE, &(*projection)[0][0]);
	
	
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * bp.getPoints()->size(), &bp.getPoints()->at(0).x, GL_STATIC_DRAW); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //index 0, 3 floats per vertex
    glEnableVertexAttribArray(0);//Enable attribute index 0 as being used 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glDrawArrays(GL_POINTS, 0, bp.getPoints()->size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	shader = ShaderHandler::SHADER_CAMERAS;
	glUseProgram(shaderHandler->getProgramId(shader));    // Active shader program
	
	glUniformMatrix4fv(glGetUniformLocation(shaderHandler->getProgramId(shader), "u_ModelViewMatrix"), 1, GL_FALSE, &(*modelView)[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderHandler->getProgramId(shader), "u_ProjectionMatrix"), 1, GL_FALSE, &(*projection)[0][0]);
	
	
	glGenBuffers(1, &camPosVBO);
	glBindBuffer(GL_ARRAY_BUFFER, camPosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) *  bp.getCameras()->size() * 3, cameraPos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //index 0, 3 floats per vertex
    glEnableVertexAttribArray(0);//Enable attribute index 0 as being used 
	
	glDrawArrays(GL_POINTS, 0, bp.getCameras()->size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
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
	
	bp.parseFile("/home/jaa/Dokumenty/FEL/DP/data/bundle.rd.out");
	shaderHandler = new ShaderHandler();
	controlls = new Controlls(window_width, window_height, &bp);
	
    // Intialize GLFW   
    glfwInit();

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
	
	initGL();

    // Set GLFW event callbacks
    //glfwSetWindowSizeCallback(_cbWindowSizeChanged);
    glfwSetCharCallback(keyBoardCallback);
    glfwSetKeyCallback(keyBoardCallback);
    glfwSetMouseButtonCallback(mouseButtonCallback);
    glfwSetMousePosCallback(mousePositionCallback);
  
   // Main loop
    while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC)) {
        if (main_loop) {
            main_loop();
        }
        // Present frame buffer
        glfwSwapBuffers();
    }
	
    glfwTerminate();    // Terminate GLFW
	
	delete shaderHandler;
	delete controlls;
	delete cameraPos;
    return 0;
}

