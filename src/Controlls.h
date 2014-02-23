/* 
 * File:   Controlls.h
 * Author: jaa
 *
 * Created on 28. září 2013, 13:46
 */

#ifndef CONTROLLS_H
#define	CONTROLLS_H

#include "BundlerData.h"
#include "glm/core/type_mat4x4.hpp"

#include <Qt> 

class Controlls {
private:
	BundlerData * bp;
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

	void setPointers(BundlerData * bp, Camera * cam, ShaderHandler *shaderHandler) {
		this->bp = bp;
		this->camera = cam;
		this->shaderHandler = shaderHandler;
	}
	
	/**
	 * Callback for keyboard actions
	 */
	void keyboardAction(int key) {
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
			case Qt::Key_Left:
				if(bp && !bp->getCameras()->empty()) {
					cameraId--;
					if(cameraId < 0) {
						cameraId = bp->getCameras()->size()-1;
					}
					setCameraParams();
					}
				break;
			case Qt::Key_Right:
				if(bp && !bp->getCameras()->empty()) {
					cameraId = (cameraId + 1) % bp->getCameras()->size();
					setCameraParams();
				}
				break;
		}
	}
	
	/**
	 * Callback for mouse click, rotation enabled while holding left button
	 */
	void mouseButtonChanged(int button) {
		mouseRotationEnabled = (button == Qt::LeftButton);
	}
	
	/**
	 * Handles rotation with mouse
	 */
	void mousePositionChanged(int x, int y) {
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
		if(bp && !bp->getCameras()->empty()) {
			Log::d("using camera: %d", cameraId);
			CameraPosition * cam = &bp->getCameras()->at(cameraId);
			camera->setCameraParams(cam->rotate, cam->translate);	
		}
	}
	
	void getProjectionMatrixForCamera(int cameraID, glm::mat4x3 &projection) const {
		if(bp && !bp->getCameras()->empty()) {
			CameraPosition * cam = &bp->getCameras()->at(cameraID);
			projection = glm::mat4x3(cam->rotate);
			projection[3] = cam->translate;
		}
	}
	
	int getCameraId() const {
		return cameraId;
	}
	
	void setCameraId(const int id) {
		if(bp && bp->getCameras()->size() == 0) {
			cameraId = 0;
		}
		else {
			cameraId = id % bp->getCameras()->size();
		}
	}
	
};


#endif	/* CONTROLLS_H */
