/* 
 * File:   Renderer.h
 * Author: jaa
 *
 * Created on 16. listopad 2013, 20:06
 */

#ifndef RENDERER_H
#define	RENDERER_H

#include <GL/glew.h> //stupid IDE :)

#include "ObjectData.h"
#include "Controlls.h"
#include "Texture.h"


class Renderer {
	GLuint planeVao;
	Camera *camera;

public:
	
	Renderer(Camera *camera) : planeVao(GL_ID_NONE), camera(camera) {
	
	}
	
	void bindCameraMatrices(const GLuint programID) {
		glm::mat4 * modelView = camera->getModelViewMatrix();
		glm::mat4 * projection = camera->getProjectionMatrix();

		glUniformMatrix4fv(glGetUniformLocation(programID, "u_ModelViewMatrix"), 1, GL_FALSE, &(*modelView)[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programID, "u_ProjectionMatrix"), 1, GL_FALSE, &(*projection)[0][0]);	
	}
	
	void drawTexture(const GLuint programID, Texture &texture) {
		if(texture.getImageStart() == NULL) {
			return; //no texture avaiable!
		}
		if(texture.textureID == GL_ID_NONE) {
				
			glGenTextures(1, &texture.textureID);
			glBindTexture(texture.target, texture.textureID);
			glTexImage2D(texture.target, 0, GL_RGB, texture.getSize().x, texture.getSize().y, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.getImageStart());
			glBindTexture(texture.target, 0);

			glGenSamplers(1, &texture.samplerID);
			glSamplerParameteri(texture.samplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glSamplerParameteri(texture.samplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glSamplerParameteri(texture.samplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glSamplerParameteri(texture.samplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		}
		
		assert(texture.samplerID);
		assert(texture.textureID);
		
		glUniform1i(glGetUniformLocation(programID, "texture0"), 0);
		
		glBindSampler(0, texture.samplerID);
		glActiveTexture(GL_TEXTURE0 + texture.unit);
		
		glBindTexture(texture.target, texture.textureID);	
	}
	
	void drawObject(ObjectData &data) {
		if(!data.isOK()) {
			return;
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
			
			Log::d("initializing data");
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
	
};


#endif	/* RENDERER_H */
