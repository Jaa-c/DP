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

public:
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
		}
		
		//draw geometry from VBO
		glBindVertexArray(data.vaoID);
		glEnableVertexAttribArray(0);

		if (data.normalsID != GL_ID_NONE) {
			glEnableVertexAttribArray(1);
		}

		if (data.texCoordsID != GL_ID_NONE) {
			glEnableVertexAttribArray(2);
		}

		glDrawElements(GL_TRIANGLES, data.getIndices().size(), GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);	
	}

};


#endif	/* RENDERER_H */
