/* 
 * File:   Controlls.h
 * Author: jaa
 *
 * Created on 28. září 2013, 13:46
 */

#ifndef CONTROLLS_H
#define	CONTROLLS_H

#include "BundlerParser.h"
#include "glm/core/type_mat4x4.hpp"

class Controlls {
private:
	BundlerParser * bp;
	Camera *camera;
	ShaderHandler *shaderHandler;
	
	bool mouseRotationEnabled;
	int ox, oy;
	
	int cameraId;
	
	Controlls() : mouseRotationEnabled(false), cameraId(0) {}
    Controlls(Controlls const&);
    void operator=(Controlls const&);
	
public:
	static Controlls& getInstance() {
		static Controlls instance;
		return instance;
	}
	
	static void keyboardAction(GLFWwindow* window, int key, int scancode, int action, int mods) {
		getInstance().keyboardActionImpl(key, action);
	}
	
	static void mouseButtonChanged(GLFWwindow* window, int button, int action, int mods) {
		getInstance().mouseButtonChangedImpl(button, action);
	}
	
	static void mousePositionChanged(GLFWwindow* window, double x, double y) {
		getInstance().mousePositionChangedImpl(floor(x), floor(y));
	}
	
	static void windowSizeChanged(GLFWwindow* window, int width, int height) {
		getInstance().windowSizeChangedImpl(width, height);
	}	
	
	void setPointers(BundlerParser * bp, Camera * cam, ShaderHandler *shaderHandler) {
		this->bp = bp;
		this->camera = cam;
		this->shaderHandler = shaderHandler;
	}
	
	/**
	 * Callback for keyboard actions
	 */
	void keyboardActionImpl(int key, int action) {
		if(action == GLFW_RELEASE) {
			return;
		}
		switch (key) {
			case 'S':// backwards
				camera->move(Camera::BACK);
				break;
			case 'W':// forwards
				camera->move(Camera::FORWARD);
				break;
			case 'A'://left
				camera->move(Camera::LEFT);
				break;
			case 'D'://right
				camera->move(Camera::RIGHT);
				break;
			case 'P':
				camera->switchFreewalk();
				setCameraParams();
				break;
			case 'R':// recompile shaders
				shaderHandler->resetShaders();
				break;
			case GLFW_KEY_LEFT:
				cameraId--;
				if(cameraId < 0) {
					cameraId = bp->getCameras()->size()-1;
				}
				setCameraParams();
				break;
			case GLFW_KEY_RIGHT:
				cameraId = (cameraId + 1) % bp->getCameras()->size();
				setCameraParams();
				break;
		}
	}
	
	/**
	 * Callback for mouse click, rotation enabled while holding left button
	 */
	void mouseButtonChangedImpl(int button, int action) {
		mouseRotationEnabled = ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS));
	}
	
	/**
	 * Handles rotation with mouse
	 */
	void mousePositionChangedImpl(int x, int y) {
		float dx, dy;
		dx = (float) (x - ox);
		dy = (float) (y - oy);

		ox = x;
		oy = y;

		if (mouseRotationEnabled) {
			camera->rotate(dx, dy);
		}
	}
	
	void windowSizeChangedImpl(int width, int height) {
		 glViewport(0, 0, width, height);
		 camera->resizeWindow(width, height);
	}	
	
	void setCameraParams() {
		if(bp) {
			Log::d("using camera: %d", cameraId);
			CameraPosition * cam = &bp->getCameras()->at(cameraId);
			camera->setCameraParams(cam->rotate, cam->translate);	
		}
	}
	
	void getProjectionMatrixForCamera(int cameraID, glm::mat4x3 &projection) const {
		if(bp) {
			CameraPosition * cam = &bp->getCameras()->at(cameraID);
			projection = glm::mat4x3(cam->rotate);
			projection[3] = cam->translate;
		}
	}
	
	int getCameraId() const {
		return cameraId;
	}
	
	void setCameraId(const int id) {
		cameraId = id % bp->getCameras()->size();
	}
	

};



#endif	/* CONTROLLS_H */

