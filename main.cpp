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

#include "cube.h"
#include "ShaderHandler.h"
#include "Controlls.h"
#include "BundlerParser.h"


// GLSL variables
GLuint g_WireMode = 0;

ShaderHandler *shaderHandler;
Controlls *controlls;

glm::mat4 g_CameraProjectionMatrix;

void printMat4(glm::mat4 &m) {
	std::cout << m[0][0] << " "<< m[0][1] << " "<< m[0][2] << " "<< m[0][3] << "\n";
	std::cout << m[1][0] << " "<< m[1][1] << " "<< m[1][2] << " "<< m[1][3] << "\n";
	std::cout << m[2][0] << " "<< m[2][1] << " "<< m[2][2] << " "<< m[2][3] << "\n";
	std::cout << m[3][0] << " "<< m[3][1] << " "<< m[3][2] << " "<< m[3][3] << "\n";
		
}


void initGL() {
	
    // Set OpenGL state variables
    glClearColor(0.4f, 0.4f, 0.7f, 0);
	
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	shaderHandler->compileShaderProgram(ShaderHandler::SHADER_TEST, true, false, true);
	
}

void main_loop() {
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);
	glDisable(GL_CULL_FACE);
	
	controlls->updateCameraViewMatrix();
	
	glm::mat4 * modelView = controlls->getModelViewMatrix();
	glm::mat4 * projection = controlls->getProjectionMatrix();
	
    // Turn on programmable pipeline
    if (true) {
        glUseProgram(shaderHandler->getProgramId());    // Active shader program
		
		glm::mat3 normalM = glm::inverseTranspose(glm::mat3(*modelView));
		
		glUniformMatrix4fv(glGetUniformLocation(shaderHandler->getProgramId(), "u_ModelViewMatrix"), 1, GL_FALSE, &(*modelView)[0][0]);
		
		glUniformMatrix4fv(glGetUniformLocation(shaderHandler->getProgramId(), "u_ProjectionMatrix"), 1, GL_FALSE, &(*projection)[0][0]);
    
		glUniformMatrix3fv(glGetUniformLocation(shaderHandler->getProgramId(), "u_NormalMatrix"), 1, GL_FALSE, &normalM[0][0]);
    
	}
	
	pgr2DrawCube();

    // Turn off programmable pipeline
    glUseProgram(0);

}

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
	
	BundlerParser bp;
	bp.parseFile("/home/jaa/Dokumenty/FEL/DP/data/bundle.rd.out");
	return 0;
	
	const int window_width = 800;
	const int window_height = 600;
	const char *window_title = "Titulek";
	
	shaderHandler = new ShaderHandler();
	controlls = new Controlls(window_width, window_height);
	
	
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
	
    return 0;
}

