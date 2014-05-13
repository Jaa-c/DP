/** @file 
 * File:   Controlls.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
 *
 * Created on 28. září 2013, 13:46
 */

#ifndef CONTROLLS_H
#define	CONTROLLS_H

#include "io/BundlerParser.h"
#include "Settings.h"
#include "glm/core/type_mat4x4.hpp"

#include <Qt> 

/**
 * Handles user inputs (keyboard, mouse)
 */
class Controlls {
private:
	Camera *camera;
	ShaderHandler *shaderHandler;
	const std::vector<Photo> *photos;
	
	bool mouseRotationEnabled; //!< true if left button is held
	int ox, oy; //!< for mouse rotation
	
	int cameraId; //!< camera ID if preffered camera is used, DEBUG
	
	Controlls() : mouseRotationEnabled(false), cameraId(0) {}
    Controlls(Controlls const&);
    void operator=(Controlls const&);
	
public:
	static Controlls& getInstance() {
		static Controlls instance;
		return instance;
	}
	
	/**
	 * Initializers
     * @param cam
     * @param shaderHandler
     */
	void setPointers(Camera * cam, ShaderHandler *shaderHandler) {
		this->camera = cam;
		this->shaderHandler = shaderHandler;
	}
	
	void setPhotos(const std::vector<Photo> * photos) {
		this->photos = photos;
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
				Settings::usePrefferedCamera = !Settings::usePrefferedCamera;
				if(Settings::usePrefferedCamera) {
					Log::i("Using preffered camera mode.");
				}
				break;
			case 'C':
				Settings::circle = !Settings::circle;
			case 'R':// recompile shaders
				shaderHandler->resetShaders();
				break;
			case Qt::Key_Left:
				if(photos) {
					cameraId--;
					if(cameraId < 0) {
						cameraId = photos->size()-1;
					}
					setCameraParams();
					Settings::prefferedCamera = cameraId;
				}
				break;
			case Qt::Key_Right:
				if(photos) {
					cameraId = (cameraId + 1) % photos->size();
					setCameraParams();
					Settings::prefferedCamera = cameraId;
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
	
	/**
	 * Callback if window size has changed
     */
	void windowSizeChangedImpl(int width, int height) {
		 glViewport(0, 0, width, height);
		 camera->resizeWindow(width, height);
	}	
	
	void setCameraParams() {
		if(photos) {
			Log::d("using camera: %d", cameraId);
			const CameraPosition * cam = &photos->at(cameraId).camera;
			camera->setCameraParams(cam->Rt);
		}
	}
	
	const Photo * getCurrentPhoto() {
		return &photos->at(cameraId);
	}
	
};


#endif	/* CONTROLLS_H */
