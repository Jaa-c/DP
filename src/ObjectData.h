/* 
 * File:   ObjectData.h
 * Author: jaa
 *
 * Created on 16. listopad 2013, 19:13
 */

#ifndef OBJECTDATA_H
#define	OBJECTDATA_H

#include "DataLoader.h"

struct ObjectData {
	std::vector<GLuint> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
		
	bool ok;
	
public:
	GLuint vaoID;
	GLuint texCoordsID;
	GLuint verticesID;
	GLuint normalsID;
	GLuint indicesID;
	
	ObjectData(std::string file) : 
		vaoID(GL_ID_NONE), texCoordsID(GL_ID_NONE), verticesID(GL_ID_NONE), 
		normalsID(GL_ID_NONE), indicesID(GL_ID_NONE), ok(true)
	{
		try {
			DataLoader::importModel(file, indices, vertices, normals);
		}
		catch(const char* c) {
			ok = false;
		}
	}
	
	const std::vector<GLuint> & getIndices() const {
		return indices;
	}
	
	const std::vector<glm::vec3> & getVertices() const {
		return vertices;
	}
	
	const std::vector<glm::vec3> & getNormals() const {
		return normals;
	}
	
	const bool isOK() const {
		return ok;
	}


};

#endif	/* OBJECTDATA_H */
