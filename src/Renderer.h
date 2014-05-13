/** @file 
 * File:   Renderer.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
 *
 * Created on 16. listopad 2013, 20:06
 */

#ifndef RENDERER_H
#define	RENDERER_H

#include <GL/glew.h>

#include "ObjectData.h"
#include "Controlls.h"
#include "Texture.h"
#include "Camera.h"
#include "RenderPass/RenderPass.h"

#include "glm/gtc/matrix_inverse.hpp"

/**
 * Draws everything
 */
class Renderer {
	Camera& camera; //!< reference to virutal camera
	
	std::vector<GLuint> *ulocs; //!< some default uniform locations
	
	/**
	 * DSraws single texture
     * @param texture
     */
	void drawTexture(Texture &texture) {
		if(!texture.checkTexture()) {
			return; //no texture avaiable!
		}
		glActiveTexture(GL_TEXTURE0 + texture.unit);
		
		if(texture.textureID == GL_ID_NONE) {
			glGenTextures(1, &texture.textureID);
			glBindTexture(texture.target, texture.textureID);
			glTexImage2D(texture.target, 0, GL_RGB8, texture.getFullSizeWithPadding().x, 
					texture.getFullSizeWithPadding().y, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *) NULL);
			
			glBindTexture(texture.target, 0);
		}
		if(texture.nearestSamplerID == GL_ID_NONE) {
			glGenSamplers(1, &texture.nearestSamplerID);
			glSamplerParameteri(texture.nearestSamplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glSamplerParameteri(texture.nearestSamplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glSamplerParameteri(texture.nearestSamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glSamplerParameteri(texture.nearestSamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			
			glGenSamplers(1, &texture.linearSamplerID);
			glSamplerParameteri(texture.linearSamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glSamplerParameteri(texture.linearSamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glSamplerParameteri(texture.linearSamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glSamplerParameteri(texture.linearSamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		}
		
		assert(texture.nearestSamplerID);
		assert(texture.linearSamplerID);
		assert(texture.textureID);
		
		glCheckError();
		if(texture.updateImage && texture.checkTexture()) {
			glBindTexture(texture.target, texture.textureID);
				glTexSubImage2D(texture.target, 0, 0, 0, texture.getSizeWithPadding().x,
					texture.getSizeWithPadding().y, GL_RGB, GL_UNSIGNED_BYTE, texture.getImageStart());
			glCheckError();
			glBindTexture(texture.target, 0);
			texture.updateImage = false;
		}
		if(Settings::billinearFiltering) {
			glBindSampler(texture.unit, texture.linearSamplerID);
		}
		else {
			glBindSampler(texture.unit, texture.nearestSamplerID);
		}
		
		glBindTexture(texture.target, texture.textureID);
	}

public:
	
	Renderer(Camera& camera) : camera(camera), ulocs(nullptr) {
	}
	
	/**
	 * Sets default uniform locations
     * @param locs
     */
	void setUniformLocations(std::vector<GLuint> * locs) {
		ulocs = locs;
	}
	
	/**
	 * Binds default camera matrices
     */
	void bindCameraMatrices() {
		const glm::mat4 &modelView = camera.getModelViewMatrix();
		const glm::mat4 &projection = camera.getProjectionMatrix();

		glUniformMatrix4fv(ulocs->at(RenderPass::MODELVIEW_MATRIX), 1, GL_FALSE, &modelView[0][0]);
		glUniformMatrix4fv(ulocs->at(RenderPass::PROJECTION_MATRIX), 1, GL_FALSE, &projection[0][0]);	
	}
	
	/**
	 * Draws all textures according to given indices
     * @param textures
     * @param indices
     */
	void drawTextures(std::vector<Texture>& textures, const std::unordered_map<uint, uint> & indices) {
		std::vector<GLint> units(textures.size());
		for(auto &tex : textures)  {
			drawTexture(tex);
			uint index = indices.at(tex.photo->ID);
			units[index] = tex.unit;
		}
		glUniform1iv(ulocs->at(RenderPass::TEXTURE0), textures.size(), &units[0]);
	}
	
	/**
	 * Draws points
     * @param data
     */
	void drawPointData(ObjectData &data) {
		std::shared_ptr<PointData> points = data.pointData;
		if(points == nullptr) {
			return; 
		}
		
		glm::mat4 modelView =  camera.getModelViewMatrix() * data.getMvm();
		glUniformMatrix4fv(ulocs->at(RenderPass::MODELVIEW_MATRIX), 1, GL_FALSE, &modelView[0][0]);		

		
		if(points->pointsVBO == GL_ID_NONE) {
			glGenBuffers(1, &points->pointsVBO);
			glBindBuffer(GL_ARRAY_BUFFER, points->pointsVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points->getPointData().size(), &points->getPointData()[0], GL_STATIC_DRAW); 
			
			glGenBuffers(1, &points->camPosVBO);
			glBindBuffer(GL_ARRAY_BUFFER, points->camPosVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points->getCameraPositions().size(), &points->getCameraPositions()[0], GL_STATIC_DRAW); 
		
		}
		glm::vec3 color(1.0f, 0.0f, 0.0f);
		glUniform3fv(ulocs->at(RenderPass::COLOR), 1, &color[0]);
		
		
		glBindBuffer(GL_ARRAY_BUFFER, points->pointsVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //index 0, 3 floats per vertex
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_POINTS, 0, points->getPointData().size());
		
		color = glm::vec3(1.0f, 1.0f, 0.0f);
		glUniform3fv(ulocs->at(RenderPass::COLOR), 1, &color[0]);
		
		glBindBuffer(GL_ARRAY_BUFFER, points->camPosVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //index 0, 3 floats per vertex
		glDrawArrays(GL_POINTS, 0, points->getCameraPositions().size());
		
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
	}
	
	/**
	 * Dras the object
     * @param data
     */
	void drawObject(ObjectData &data) {
		if(!data.isOK()) {
			return;
		}
		
		glm::mat4 modelView =  camera.getModelViewMatrix() * data.getMvm();
		glUniformMatrix4fv(ulocs->at(RenderPass::MODELVIEW_MATRIX), 1, GL_FALSE, &modelView[0][0]);

		glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(data.getMvm()));
		glUniformMatrix3fv(ulocs->at(RenderPass::NORMAL_MATRIX), 1, GL_FALSE, &normalMatrix[0][0]);
		
		//initialize buffers
		if (data.vaoID == GL_ID_NONE) {
			glGenVertexArrays(1, &data.vaoID);
			glBindVertexArray(data.vaoID);
	
			glGenBuffers(1, &data.verticesID);
			glBindBuffer(GL_ARRAY_BUFFER, data.verticesID);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.getVertices().size(), &data.getVertices()[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glGenBuffers(1, &data.indicesID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.indicesID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * data.getIndices().size(), &data.getIndices()[0], GL_STATIC_DRAW);

			if (!data.getNormals().empty())
			{
				glGenBuffers(1, &data.normalsID);
				glBindBuffer(GL_ARRAY_BUFFER, data.normalsID);
				glBufferData(GL_ARRAY_BUFFER, data.getNormals().size() * sizeof(glm::vec3), &data.getNormals()[0], GL_STATIC_DRAW);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}
			
			Log::d("[Renderer] initializing data");
		}
		
		//draw geometry from VBO
		glBindVertexArray(data.vaoID);
		glEnableVertexAttribArray(0);

		if (data.normalsID != GL_ID_NONE) {
			glEnableVertexAttribArray(1);
		}
		
		glDrawElements(GL_TRIANGLES, data.getIndices().size(), GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);	
	}
		
	const Camera& getCamera() const {
		return camera;
	}
};


#endif	/* RENDERER_H */
