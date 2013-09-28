/* 
 * File:   Controlls.h
 * Author: jaa
 *
 * Created on 28. září 2013, 13:46
 */

#ifndef CONTROLLS_H
#define	CONTROLLS_H

class Controlls {
private:
	//Transformation matrixes
	glm::mat4 g_CameraProjectionMatrix;     // Camera projection transformation
	glm::mat4 g_CameraViewMatrix;           // Camera view transformation
	
	glm::vec3 cameraPos;
	glm::vec3 cameraRot;
	glm::vec3 cameraPosLag;
	glm::vec3 cameraRotLag;
	
	bool mouseRotationEnabled;
	
	int ox, oy;
	const float inertia			= 0.1f;  //mouse inertia
	const float rotateSpeed		= 0.4f;  //mouse rotate speed (sensitivity)
	const float walkSpeed		= 0.05f; //walking speed (wasd)
	
public:
	Controlls() {
		cameraPos = glm::vec3(0.0f, -1.0f, -1.0f);
		cameraRot = glm::vec3(0.0f, 0.0f, 0.0f);
		
		cameraPosLag = cameraPos;
		cameraRotLag = cameraRot;
		
		mouseRotationEnabled = false;
				
		glm::translate(g_CameraViewMatrix, glm::vec3(0.0f, 0.0f, -5.0f));
		
		g_CameraProjectionMatrix = glm::perspective(55.0f, GLfloat(800)/600, 0.010f, 1000.0f);
	}

	/**
	 * this code is inspired by NVIDIA CUDA samples v5.0 (Smoke Particles)
	 */
	void updateCameraViewMatrix() {
		//camera alway above the ground
		if (cameraPos.y > -0.1f) {
			cameraPos.y = -0.1f;
		}

		if(cameraPos.x > 5.0f) {
			cameraPos.x = -5.0f;
			cameraPosLag.x = -5.0f - inertia;
		}
		if(cameraPos.x < -5.0f) {
			cameraPos.x = 5.0f;
			cameraPosLag.x = 5.0f + inertia;
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
	void keyboardAction(int key, int action)
	{
		switch (key)
		{
			case 's' : // backwards
				cameraPos[0] -= g_CameraViewMatrix[0][2] * walkSpeed;
				cameraPos[1] -= g_CameraViewMatrix[1][2] * walkSpeed;
				cameraPos[2] -= g_CameraViewMatrix[2][2] * walkSpeed;
				break;
			case 'w' : // forwards
				cameraPos[0] += g_CameraViewMatrix[0][2] * walkSpeed;
				cameraPos[1] += g_CameraViewMatrix[1][2] * walkSpeed;
				cameraPos[2] += g_CameraViewMatrix[2][2] * walkSpeed;
				break;
			case 'a' : //left
				cameraPos[0] += g_CameraViewMatrix[0][0] * walkSpeed;
				cameraPos[1] += g_CameraViewMatrix[1][0] * walkSpeed;
				cameraPos[2] += g_CameraViewMatrix[2][0] * walkSpeed;
				break;
			case 'd' : //right
				cameraPos[0] -= g_CameraViewMatrix[0][0] * walkSpeed;
				cameraPos[1] -= g_CameraViewMatrix[1][0] * walkSpeed;
				cameraPos[2] -= g_CameraViewMatrix[2][0] * walkSpeed;
				break;
		}
	}

	/**
	 * Callback for mouse click, rotation enabled while holding left button
	 */
	void mouseButtonChanged(int button, int action)
	{
		mouseRotationEnabled = ((button == GLFW_MOUSE_BUTTON_LEFT) && (action == GLFW_PRESS));
	}

	/**
	 * Handles rotation with mouse
	 */
	void mousePositionChanged(int x, int y)
	{
		float dx, dy;
		dx = (float)(x - ox);
		dy = (float)(y - oy);

		ox = x;
		oy = y;

		if (mouseRotationEnabled)
		{
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

};



#endif	/* CONTROLLS_H */

