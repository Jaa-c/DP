/* 
 * File:   Renderer.h
 * Author: jaa
 *
 * Created on 16. listopad 2013, 20:06
 */

#ifndef RENDERER_H
#define	RENDERER_H

#include "ObjectData.h"


class Renderer {
	
	GLuint planeVao;

public:
	
	Renderer() : planeVao(GL_ID_NONE) {
	
	}
	
	void draw(ObjectData &data) {
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
