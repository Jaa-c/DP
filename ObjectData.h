/* 
 * File:   ObjectData.h
 * Author: jaa
 *
 * Created on 16. listopad 2013, 19:13
 */

#ifndef OBJECTDATA_H
#define	OBJECTDATA_H

#include "ModelLoader.h"

class ObjectData {
	std::vector<unsigned int> indices;
	std::vector<float> vertices;
	std::vector<float> normals;
		
	bool ok;
	
public:
	GLuint vaoID;
	GLuint texCoordsID;
	GLuint verticesID;
	GLuint normalsID;
	GLuint indicesID;
	
	ObjectData(std::string &file) : 
		vaoID(GL_ID_NONE), texCoordsID(GL_ID_NONE), verticesID(GL_ID_NONE), 
		normalsID(GL_ID_NONE), indicesID(GL_ID_NONE), ok(true)
	{
		try {
			ModelLoader::importModel(file, indices, vertices, normals);
		}
		catch(const char* c) {
			ok = false;
		}
	
	}
	
	bool isOK() {
		return ok;
	}


};

#endif	/* OBJECTDATA_H */

