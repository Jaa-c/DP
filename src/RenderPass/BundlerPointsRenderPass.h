/* 
 * File:   BundlerPointsRenderPass.h
 * Author: jaa
 *
 * Created on 24. listopad 2013, 14:14
 */

#ifndef BUNDLERPOINTSRENDERPASS_H
#define	BUNDLERPOINTSRENDERPASS_H

#include "RenderPass.h"

class BundlerPointsRenderPass : public RenderPass {
	
	GLuint pointsVBO;
	GLuint camPosVBO;
	/// Positions of cameras
	std::vector<glm::vec3> cameraPos;
	/// Point data from bundler
	std::vector<glm::vec3> *pointData;
	
public:
	
	BundlerPointsRenderPass(Renderer *r, ShaderHandler *sh, BundlerParser *bp) : 
		RenderPass(TEXTURING_PASS, r, sh)
	{	
		shader = ShaderHandler::SHADER_POINTS;
		pointsVBO = GL_ID_NONE;
		camPosVBO = GL_ID_NONE;

		for(std::vector<CameraPosition>::iterator it = bp->getCameras()->begin(); it != bp->getCameras()->end(); ++it) {
			glm::vec3 v = -1 * glm::transpose((*it).rotate) * (*it).translate;
			cameraPos.push_back(v);
		}
		
		pointData = bp->getPoints();
	}
		
	~BundlerPointsRenderPass() {
		
	}
	
	void draw(ObjectData *object) {
		GLuint programID = shaderHandler->getProgramId(shader);
		
		if(pointsVBO == GL_ID_NONE) {
			glGenBuffers(1, &pointsVBO);
			glGenBuffers(1, &camPosVBO);
		}
		
		glEnable(GL_PROGRAM_POINT_SIZE );
		glUseProgram(programID);
		renderer->bindCameraMatrices(programID);
		
		if(!renderer->getCamera()->isCameraStatic()) { //if we are moving
			glm::mat4 modelView =  *renderer->getCamera()->getModelViewMatrix() * object->mvm;
			glUniformMatrix4fv(glGetUniformLocation(programID, "u_ModelViewMatrix"), 1, GL_FALSE, &modelView[0][0]);		
		}
		
		///TODO: use renderer to draw this
		glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * pointData->size(), &pointData->at(0).x, GL_STATIC_DRAW); 
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //index 0, 3 floats per vertex
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_POINTS, 0, pointData->size());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	}
};

#endif	/* BUNDLERPOINTSRENDERPASS_H */

