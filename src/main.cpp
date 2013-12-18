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
/// std::vector < unsigned char > 
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
//		renderPassHandler.add(RenderPass::BUNDLER_POINTS_PASS, new BundlerPointsRenderPass(&renderer, &shaderHandler, &bp));

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
			glm::vec3 viewDir = object->getCentroidPosition() + camera.getCameraPosition();
			const int cam = bp.getClosestCamera(viewDir, object->mvm);
			controlls->setCameraId(cam);
		}
		
		const int camID = controlls->getCameraId();
		object->texture->setImage(textureHandler.getImage(camID), &bp.getCameras()->at(camID));
		
		renderPassHandler.draw(object);
		
		glUseProgram(0);
		
		
		drawRadar(0, 10, 10, 250, 250);

	}
	
	inline void drawRadar(GLuint tex_id, GLint x, GLint y, GLsizei width, GLsizei height)
	{
		GLint viewport[4] = {0};
		glGetIntegerv(GL_VIEWPORT, viewport);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, viewport[2], 0, viewport[3], -1.0, 1.0);
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
		glPointSize(4.0);
		
		glm::vec4 tmp, p1, p2, dir;
		glm::vec3 c, k;
		c = object->getCentroidPosition();
		k = -camera.getCameraPosition();
		
		const Points &cameras = object->pointData->getCameraPositions();
		const Vectors &cameraDirections = bp.getCamerDirections();
		const glm::mat4 vecMat = glm::inverse(glm::transpose(object->mvm));
		
		glBegin(GL_LINES);
			glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
			for(uint i = 0; i < cameraDirections.size(); ++i) {
				p1 =  object->mvm * glm::vec4(cameras.at(i), 1.0f);
				dir = vecMat * glm::vec4(cameraDirections.at(i), 1.0f);
				drawLine(p1, dir);
			}
			
			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
			glm::vec3 viewDir = -camera.getCameraViewDirection();
			drawLine(k, viewDir);
			
			glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
			drawPoint(k);
			drawPoint(c);
		glEnd();
		
		
		glColor4f(0.3f, 0.3f, 0.3f, 1.0f);
		glBegin(GL_POINTS);
			for(auto it = cameras.begin(); it != cameras.end(); ++it) {
				tmp = object->mvm * glm::vec4((*it), 1.0f);
				drawPoint(tmp);
			}
		glEnd();
			
		glPointSize(5.0);
		glBegin(GL_POINTS);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			drawPoint(k);

			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
			drawPoint(c);
			
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			const int camID = controlls->getCameraId();
			tmp =  object->mvm * glm::vec4(cameras[camID], 1.0f);
			drawPoint(tmp);
		glEnd();
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	
	void drawPoint(glm::vec4 &p) {
		glm::vec2 tmp(p.x, p.z);
		drawPoint(tmp);
	}
	void drawPoint(glm::vec3 &p) {
		glm::vec2 tmp(p.x, p.z);
		drawPoint(tmp);
	}
	void drawPoint(glm::vec2 &p) {
		if(cmpCoords(p)) {
			glVertex2f(p.x, p.y);
		}
	}
	void drawLine(glm::vec4 &point, glm::vec4 &dir) {
		glm::vec3 p(point);
		glm::vec3 d(dir);
		drawLine(p, d);
	}
	void drawLine(glm::vec3 &point, glm::vec3 &dir) {
		glm::vec2 d(dir.x, dir.z);
		d = glm::normalize(d);
		d *= 3;
		glm::vec2 p1(point.x, point.z);
		glm::vec2 p2 = p1 + d;
		if(cmpCoords(p1) && cmpCoords(p2)) {
			glVertex2f(p1.x, p1.y);
			glVertex2f(p2.x, p2.y);
		}	
	}
	
	bool cmpCoords(glm::vec2 &p) {
		const glm::vec2 xr(-22.f, 18.f);
		const glm::vec2 yr(5.f, 45.f);
		
		p.x = (p.x - xr.x) * (260.f - 10.f) / (xr.y - xr.x) + 10.f;
		p.y = (p.y - yr.x) * (260.f - 10.f) / (yr.y - yr.x) + 260.f;
		
		if(p.x < 10 || p.x > 260 || p.y < 10 || p.y > 260) {
			return false;
		}
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

