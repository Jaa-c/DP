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
typedef glm::vec3 Point;
typedef glm::vec3 Vector;
typedef std::vector<Point> Points;
typedef std::vector<Vector> Vectors;

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
		renderPassHandler.add(RenderPass::BUNDLER_POINTS_PASS, new BundlerPointsRenderPass(&renderer, &shaderHandler, &bp));

		object = new ObjectData("/home/jaa/Documents/FEL/DP/data/statue/statue.obj");
		object->mvm = glm::rotate(object->mvm, 180.f, glm::vec3(1.0f, 0.0f, 0.0f));
		object->pointData = new PointData(&bp, object->getCentroid());
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
			glm::vec3 viewDir = object->getCentroidPosition() - camera.getCameraPosition();
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
		
		
		pgr2ShowTexture(0, 10, 10, 250, 250);

	}
	
	inline void pgr2ShowTexture(GLuint tex_id, GLint x, GLint y, GLsizei width, GLsizei height)
	{
		GLint viewport[4] = {0};
		glGetIntegerv(GL_VIEWPORT, viewport);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, viewport[2], 0, viewport[3], -1.0, 1.0);
		//glOrtho(10.f, 260.f, 10.f, 260.f, -1.f, 1.f);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable( GL_BLEND );
		
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		
		glBegin(GL_QUADS);
			glVertex2i(x, y);
			glVertex2i(x+width, y);
			glVertex2i(x+width, y+height);
			glVertex2i(x, y+height);
		glEnd();
		
		glEnable(GL_POINT);
		glEnable(GL_POINT_SIZE);
		glPointSize( 4.0 );
				
		glm::vec2 c, k;
		c.x= object->getCentroidPosition().x;	
		c.y= object->getCentroidPosition().z;
				
		k.x = -camera.getCameraPosition().x;
		k.y = -camera.getCameraPosition().z;
		
		const Points &cameras = object->pointData->getCameraPositions();
		const Vectors &cameraDirections = bp.getCamerDirections();
		
		glBegin(GL_LINES);
			for(uint i = 0; i < cameraDirections.size(); ++i) {
				glm::vec4 tmp =  object->mvm * glm::vec4(cameras.at(i), 1.0f);
				glm::vec2 p1(tmp.x, tmp.z);
				tmp = glm::inverse(glm::transpose(object->mvm)) * glm::vec4(cameraDirections.at(i), 1.0f);
				glm::vec2 dir(tmp.x, tmp.z);
				dir = glm::normalize(dir);
				dir *= 3;
				glm::vec2 p2 = p1 + dir;
				if(computeCoordinates(p1) && computeCoordinates(p2)) {
					glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
					glVertex2f(p1.x, p1.y);
					glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
					glVertex2f(p2.x, p2.y);
				}
			}
		
			const Vector cv = camera.getCameraViewDirection();
			glm::vec2 dir(cv.x, cv.z);
			dir = glm::normalize(dir);
			dir *= 3;
			glm::vec2 p2 = k - dir;
			glm::vec2 kam(k);
			if(computeCoordinates(kam) && computeCoordinates(p2)) {
				glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
				glVertex2f(kam.x, kam.y);
				glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
				glVertex2f(p2.x, p2.y);
			}
		
		glEnd();
		
		glBegin(GL_POINTS);
			glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
			for(auto it = cameras.begin(); it != cameras.end(); ++it) {
				glm::vec4 tmp = object->mvm * glm::vec4((*it), 1.0f);
				glm::vec2 v;
				v.x = tmp.x;
				v.y = tmp.z;
				if(computeCoordinates(v)) glVertex2f(v.x, v.y);
			}
			
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			if(computeCoordinates(k)) glVertex2f(k.x, k.y);

			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
			if(computeCoordinates(c)) glVertex2f(c.x, c.y);
			
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			const int camID = controlls->getCameraId();
			glm::vec4 tmp =  object->mvm * glm::vec4(cameras[camID], 1.0f);
			glm::vec2 v;
			v.x = tmp.x;
			v.y = tmp.z;
			if(computeCoordinates(v)) glVertex2f(v.x, v.y);
		glEnd();
		
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	
	bool computeCoordinates(glm::vec2 &p) {
		const glm::vec2 xr(-20.f, 20.f);
		const glm::vec2 yr(5.f, 45.f);
		
		p.x = (p.x - xr.x) * (260.f - 10.f) / (xr.y - xr.x) + 10.f;
		p.y = (p.y - yr.x) * (260.f - 10.f) / (yr.y - yr.x) + 260.f;
		
		if(p.x < 10 || p.x > 260 || p.y < 10 || p.y > 260) 
			return false;
		return true;
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
	Log::i("Avaiable texture units for FS: %d", texture_units);
	
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

