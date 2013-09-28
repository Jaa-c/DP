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


// GLSL variables
GLuint g_WireMode = 0;

float scene_rot = 0;

ShaderHandler *shaderHandler;

glm::mat4 g_CameraProjectionMatrix;





void initGL() {
	
    // Set OpenGL state variables
    glClearColor(0.4f, 0.4f, 0.7f, 0);
	
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	
	shaderHandler->compileShaderProgram(ShaderHandler::SHADER_TEST, true, false, true);
	
	g_CameraProjectionMatrix = glm::perspective(55.0f, GLfloat(800)/600, 0.010f, 100.0f);

}

void main_loop() {
	
	static GLfloat scene_rot = 0.0f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);
	glDisable(GL_CULL_FACE);

    // Setup camera
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(scene_rot, 0.0f, 1.0f, 0.0f);

    // Turn on programmable pipeline
    if (true) {
        glUseProgram(shaderHandler->getProgramId());    // Active shader program
		
		//nahrajeme modelview a projection matrix do shaderu
		GLfloat mvm[16] = {0};
		glGetFloatv(GL_MODELVIEW_MATRIX, mvm);
		glm::mat3 normalM = glm::inverseTranspose(glm::mat3(glm::make_mat4(mvm)));
		
		glUniformMatrix4fv(glGetUniformLocation(shaderHandler->getProgramId(), "u_ModelViewMatrix"), 1, GL_FALSE, mvm);
		
		glUniformMatrix4fv(glGetUniformLocation(shaderHandler->getProgramId(), "u_ProjectionMatrix"), 1, GL_FALSE, &g_CameraProjectionMatrix[0][0]);
    
		glUniformMatrix3fv(glGetUniformLocation(shaderHandler->getProgramId(), "u_NormalMatrix"), 1, GL_FALSE, &normalM[0][0]);
    
	}
	
	pgr2DrawCube();

    // Turn off programmable pipeline
    glUseProgram(0);

	scene_rot+=0.25;
    

}

/*
 * 
 */
int main(int argc, char** argv) {
	
	shaderHandler = new ShaderHandler();
	
	int window_width = 800;
	int window_height = 600;
	const char *window_title = "Titulek";
	
    bool bDebugOutput = false;

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

    // Enable OGL debug 
//    if (bDebugOutput) {
//        if (glfwExtensionSupported("GL_ARB_debug_output")) {
//            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
//            glDebugMessageCallbackARB(_cbPrintOGLDebugLog, NULL);
//            glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW_ARB, 0, NULL, GL_TRUE);
//        }
//    }
    // Init OGL
    if (initGL) {
        initGL();
    }

    // Set GLFW event callbacks
    //glfwSetWindowSizeCallback(_cbWindowSizeChanged);
    //glfwSetCharCallback(_cbKeyboardChanged);
    //glfwSetMouseButtonCallback(_cbMouseButtonChanged);
    //glfwSetMousePosCallback(_cbMousePositionChanged);
    //glfwSetKeyCallback(cbKeyboardChanged);
  
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
    return 0;
}

