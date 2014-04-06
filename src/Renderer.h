/* 
 * File:   Renderer.h
 * Author: jaa
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


class Renderer {
	GLuint planeVao;
	Camera& camera;
	
	std::vector<GLuint> *ulocs;
	
	
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
		if(texture.samplerID == GL_ID_NONE) {
			glGenSamplers(1, &texture.samplerID);
			glSamplerParameteri(texture.samplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glSamplerParameteri(texture.samplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glSamplerParameteri(texture.samplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glSamplerParameteri(texture.samplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		}
		
		assert(texture.samplerID);
		assert(texture.textureID);
		
		glCheckError();
		if(texture.updateImage) {
			glBindTexture(texture.target, texture.textureID);
			glTexSubImage2D(texture.target, 0, 0, 0, texture.getSizeWithPadding().x,
					texture.getSizeWithPadding().y, GL_RGB, GL_UNSIGNED_BYTE, texture.getImageStart());
			glCheckError();
			glBindTexture(texture.target, 0);
			texture.updateImage = false;
		}
		
		glBindSampler(texture.unit, texture.samplerID);
		glBindTexture(texture.target, texture.textureID);
	}

public:
	
	Renderer(Camera& camera) : planeVao(GL_ID_NONE), camera(camera) {
	}
	
	void setUniformLocations(std::vector<GLuint> * locs) {
		ulocs = locs;
	}
	
	void bindCameraMatrices() {
		const glm::mat4 * modelView = camera.getModelViewMatrix();
		const glm::mat4 * projection = camera.getProjectionMatrix();

		glUniformMatrix4fv(ulocs->at(RenderPass::MODELVIEW_MATRIX), 1, GL_FALSE, &(*modelView)[0][0]);
		glUniformMatrix4fv(ulocs->at(RenderPass::PROJECTION_MATRIX), 1, GL_FALSE, &(*projection)[0][0]);	
	}
	
	void drawTextures(std::vector<Texture> * textures) {
		std::vector<GLint> units;
		for(auto &tex : *textures)  {
			drawTexture(tex);
			units.push_back(tex.unit);
			int w;
			glGetTexLevelParameteriv(tex.target, 0, GL_TEXTURE_WIDTH, &w);
		}
		glUniform1iv(ulocs->at(RenderPass::TEXTURE0), textures->size(), &units[0]);
	}
	
	void drawPointData(ObjectData &data) {
		std::shared_ptr<PointData> points = data.pointData;
		if(points == nullptr) {
			return; 
		}
		
		if(!camera.isCameraStatic()) { //if we are moving
			glm::mat4 modelView =  *camera.getModelViewMatrix() * data.getMvm();
			glUniformMatrix4fv(ulocs->at(RenderPass::MODELVIEW_MATRIX), 1, GL_FALSE, &modelView[0][0]);		
		}
		
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
	
	void drawObject(ObjectData &data) {
		if(!data.isOK()) {
			return;
		}
		
		if(!camera.isCameraStatic()) { //if we are moving
			glm::mat4 modelView =  *camera.getModelViewMatrix() * data.getMvm();
			glUniformMatrix4fv(ulocs->at(RenderPass::MODELVIEW_MATRIX), 1, GL_FALSE, &modelView[0][0]);
			
			glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(data.getMvm()));
			glUniformMatrix3fv(ulocs->at(RenderPass::NORMAL_MATRIX), 1, GL_FALSE, &normalMatrix[0][0]);
		}
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
	
	void drawPlane() {
		if(planeVao == GL_ID_NONE) {
			glm::vec3 normal(0.0f, 0.0f, 1.0f);

			std::vector<glm::vec3> vertices;
			vertices.push_back(glm::vec3(-10.0f, -10.0f, -10.0f));
			vertices.push_back(glm::vec3(10.0f, -1.0f, -10.0f));
			vertices.push_back(glm::vec3(10.0f, 10.0f, -10.0f));
			vertices.push_back(glm::vec3(-10.0f, 10.0f, -10.0f));

			std::vector<glm::vec2> texCoords;
			texCoords.push_back(glm::vec2(0.0f, 0.0f));
			texCoords.push_back(glm::vec2(1.0f, 0.0f));
			texCoords.push_back(glm::vec2(1.0f, 1.0f));
			texCoords.push_back(glm::vec2(0.0f, 1.0f));

			glGenVertexArrays(1, &planeVao);
			glBindVertexArray(planeVao);

			GLuint vertexVBO, normalsVBO, texCoordsVBO;	
			glGenBuffers(1, &vertexVBO);
			glGenBuffers(1, &normalsVBO);
			glGenBuffers(1, &texCoordsVBO);

			glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &(*vertices.begin()), GL_STATIC_DRAW);  
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &normal, GL_STATIC_DRAW);  
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glBindBuffer(GL_ARRAY_BUFFER, texCoordsVBO);
			glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), &(*texCoords.begin()), GL_STATIC_DRAW);  
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}
		//draw plane
		glBindVertexArray(planeVao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glDrawArrays(GL_QUADS, 0, 4);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);
	}
	
	
	const Camera& getCamera() const {
		return camera;
	}
};


#endif	/* RENDERER_H */
