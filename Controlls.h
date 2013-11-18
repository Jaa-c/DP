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
	int cameraId;
	
	bool freeWalk;
	
	//Transformation matrixes
	glm::mat4 g_CameraProjectionMatrix; // Camera projection transformation
	glm::mat4 g_CameraViewMatrix; // Camera view transformation

	glm::vec3 cameraPos;
	glm::vec3 cameraRot;
	glm::vec3 cameraPosLag;
	glm::vec3 cameraRotLag;

	bool mouseRotationEnabled;

	int ox, oy;
	const float inertia = 0.08f; //mouse inertia
	const float rotateSpeed = 0.2f; //mouse rotate speed (sensitivity)
	const float walkSpeed = 0.25f; //walking speed (wasd)

public:

	Controlls(int winWidth, int winHeight, BundlerParser * bp) : cameraId(0), mouseRotationEnabled(false), freeWalk(true) {
		this->bp = bp;
		cameraPos = glm::vec3(0.0f, -2.0f, -4.0f);
		cameraRot = glm::vec3(0.0f, 250.0f, 0.0f);

		cameraPosLag = cameraPos;
		cameraRotLag = cameraRot;

		g_CameraProjectionMatrix = glm::perspective(65.0f, GLfloat(winWidth) / GLfloat(winHeight), 0.10f, 1000.0f);
	}

	/**
	 * this code is inspired by NVIDIA CUDA samples v5.0 (Smoke Particles)
	 */
	void updateCameraViewMatrix() {
		if(!freeWalk) {
			return;
		}
		//camera inertia
		cameraPosLag += (cameraPos - cameraPosLag) * inertia;
		cameraRotLag += (cameraRot - cameraRotLag) * inertia;
		
		// view transform
		g_CameraViewMatrix = glm::rotate(glm::mat4(1.0f), cameraRotLag[0], glm::vec3(1.0, 0.0, 0.0));
		g_CameraViewMatrix = glm::rotate(g_CameraViewMatrix, cameraRotLag[1], glm::vec3(0.0, 1.0, 0.0));
		g_CameraViewMatrix = glm::translate(g_CameraViewMatrix, cameraPosLag);
	}

	/**
	 * Callback for keyboard actions
	 */
	void keyboardAction(int key, int action) {
		if(action != GLFW_PRESS) {
			return;
		}
		switch (key) {
			case 'S':// backwards
				cameraPos[0] -= g_CameraViewMatrix[0][2] * walkSpeed;
				cameraPos[1] -= g_CameraViewMatrix[1][2] * walkSpeed;
				cameraPos[2] -= g_CameraViewMatrix[2][2] * walkSpeed;
				break;
			case 'W':// forwards
				cameraPos[0] += g_CameraViewMatrix[0][2] * walkSpeed;
				cameraPos[1] += g_CameraViewMatrix[1][2] * walkSpeed;
				cameraPos[2] += g_CameraViewMatrix[2][2] * walkSpeed;
				break;
			case 'A'://left
				cameraPos[0] += g_CameraViewMatrix[0][0] * walkSpeed;
				cameraPos[1] += g_CameraViewMatrix[1][0] * walkSpeed;
				cameraPos[2] += g_CameraViewMatrix[2][0] * walkSpeed;
				break;
			case 'D'://right
				cameraPos[0] -= g_CameraViewMatrix[0][0] * walkSpeed;
				cameraPos[1] -= g_CameraViewMatrix[1][0] * walkSpeed;
				cameraPos[2] -= g_CameraViewMatrix[2][0] * walkSpeed;
				break;
			case 'P':
				freeWalk = !freeWalk;
				setCameraParams();
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
	void mouseButtonChanged(int button, int action) {
		mouseRotationEnabled = ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS));
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
			cameraRot[0] += dy * rotateSpeed;
			cameraRot[1] += dx * rotateSpeed;
		}
	}

	glm::mat4 * getModelViewMatrix() {
		return &g_CameraViewMatrix;
	}

	glm::mat4 * getProjectionMatrix() {
		return &g_CameraProjectionMatrix;
	}
	
	void setCameraParams() {
		Camera * cam = &bp->getCameras()->at(cameraId);
		
		g_CameraViewMatrix[3][0] = cam->translate[0];
		g_CameraViewMatrix[3][1] = cam->translate[1];
		g_CameraViewMatrix[3][2] = cam->translate[2];
		
		g_CameraViewMatrix[0][0] = cam->rotate[0][0];
		g_CameraViewMatrix[0][1] = cam->rotate[0][1];
		g_CameraViewMatrix[0][2] = cam->rotate[0][2];
		
		g_CameraViewMatrix[1][0] = cam->rotate[1][0];
		g_CameraViewMatrix[1][1] = cam->rotate[1][1];
		g_CameraViewMatrix[1][2] = cam->rotate[1][2];
		
		g_CameraViewMatrix[2][0] = cam->rotate[2][0];
		g_CameraViewMatrix[2][1] = cam->rotate[2][1];
		g_CameraViewMatrix[2][2] = cam->rotate[2][2];
	}

};



#endif	/* CONTROLLS_H */

