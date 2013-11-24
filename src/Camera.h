/* 
 * File:   Camera.h
 * Author: jaa
 *
 * Created on 24. listopad 2013, 0:42
 */

#ifndef CAMERA_H
#define	CAMERA_H

class Camera {
	
	//Transformation matrixes
	glm::mat4 g_CameraProjectionMatrix; // Camera projection transformation
	glm::mat4 g_CameraViewMatrix; // Camera view transformation

	glm::vec3 cameraPos;
	glm::vec3 cameraRot;
	glm::vec3 cameraPosLag;
	glm::vec3 cameraRotLag;
	
	bool freeWalk;
	
	const float inertia = 0.08f; //mouse inertia
	const float rotateSpeed = 0.2f; //mouse rotate speed (sensitivity)
	const float walkSpeed = 0.25f; //walking speed (wasd)
	
public:
	enum Move {
		FORWARD,
		BACK,
		LEFT,
		RIGHT
	};
	
	
	Camera(int winWidth, int winHeight) : freeWalk(true) {
		cameraPos = glm::vec3(0.0f, -2.0f, -4.0f);
		cameraRot = glm::vec3(0.0f, 250.0f, 0.0f);

		cameraPosLag = cameraPos;
		cameraRotLag = cameraRot;
		resizeWindow(winWidth, winHeight);
	}
	
	void resizeWindow(int winWidth, int winHeight) {
		g_CameraProjectionMatrix = glm::perspective(75.0f, GLfloat(winWidth) / GLfloat(winHeight), 0.10f, 1000.0f);
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
	
	void move(Move dir) {
		switch(dir) {
			case BACK:
				cameraPos[0] -= g_CameraViewMatrix[0][2] * walkSpeed;
				cameraPos[1] -= g_CameraViewMatrix[1][2] * walkSpeed;
				cameraPos[2] -= g_CameraViewMatrix[2][2] * walkSpeed;
				break;
			case FORWARD:
				cameraPos[0] += g_CameraViewMatrix[0][2] * walkSpeed;
				cameraPos[1] += g_CameraViewMatrix[1][2] * walkSpeed;
				cameraPos[2] += g_CameraViewMatrix[2][2] * walkSpeed;
				break;
			case LEFT:
				cameraPos[0] += g_CameraViewMatrix[0][0] * walkSpeed;
				cameraPos[1] += g_CameraViewMatrix[1][0] * walkSpeed;
				cameraPos[2] += g_CameraViewMatrix[2][0] * walkSpeed;
				break;
			case RIGHT:
				cameraPos[0] -= g_CameraViewMatrix[0][0] * walkSpeed;
				cameraPos[1] -= g_CameraViewMatrix[1][0] * walkSpeed;
				cameraPos[2] -= g_CameraViewMatrix[2][0] * walkSpeed;
				break;
		}
	}
	
	void rotate(float diffX, float diffY) {
		cameraRot[0] += diffY * rotateSpeed;
		cameraRot[1] += diffX * rotateSpeed;
	}
	
	void setCameraParams(glm::mat3 &rot, glm::vec3 &trans) {
		g_CameraViewMatrix[3] = glm::vec4(trans, 1.0f);
		
		g_CameraViewMatrix[0] = glm::vec4(rot[0], 0.0f);
		g_CameraViewMatrix[1] = glm::vec4(rot[1], 0.0f);
		g_CameraViewMatrix[2] = glm::vec4(rot[2], 0.0f);	
	}
	
	void switchFreewalk() {
		freeWalk = !freeWalk;
	}
	
	glm::mat4 * getModelViewMatrix() {
		return &g_CameraViewMatrix;
	}

	glm::mat4 * getProjectionMatrix() {
		return &g_CameraProjectionMatrix;
	}


};


#endif	/* CAMERA_H */

