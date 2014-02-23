/* 
 * File:   ObjectData.h
 * Author: jaa
 *
 * Created on 16. listopad 2013, 19:13
 */

#ifndef OBJECTDATA_H
#define	OBJECTDATA_H

#include "DataLoader.h"
#include "Texture.h"
#include "BundlerData.h"

struct PointData {
private:
	/// Positions of cameras
	Points cameraPos;
	
	/// viewing directions of cameras
	Points cameraDirections;
	
	/// Point data from bundler
	Points *pointData;
public:
	GLuint pointsVBO;
	GLuint camPosVBO;
	PointData(BundlerData *bp, glm::vec3 centroid) : pointsVBO(GL_ID_NONE), camPosVBO(GL_ID_NONE) {
		for(auto &cam : *bp->getCameras()) {
			glm::vec3 v = -1 * glm::transpose(cam.rotate) * cam.translate;
			cameraPos.push_back(v);
			cameraDirections.push_back(cam.getDirection());
		}
		cameraPos.push_back(centroid);
		pointData = bp->getPoints();
	}
	
	const Points &getPointData() const {
		return *pointData;
	}
	
	const Points &getCameraDirections() const {
		return cameraDirections;
	}
	
	const Points &getCameraPositions() const {
		return cameraPos;
	}
	
};

struct ObjectData {
private:
	std::vector<GLuint> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	glm::vec3 centroid;
		
	bool ok;
	
public:
	GLuint vaoID;
	GLuint texCoordsID;
	GLuint verticesID;
	GLuint normalsID;
	GLuint indicesID;
	
	Texture *texture;
	PointData * pointData;
	
	glm::mat4 mvm;
	
	ObjectData(std::string file) : ok(true),
		vaoID(GL_ID_NONE), texCoordsID(GL_ID_NONE), verticesID(GL_ID_NONE), 
		normalsID(GL_ID_NONE), indicesID(GL_ID_NONE), 
		texture(NULL), pointData(NULL)
	{
		glm::vec3 offset;
		try {
			DataLoader::importModel(file, indices, vertices, normals, centroid, offset);
		}
		catch(const char* c) {
			ok = false;
		}
		///move center to the origin
		mvm = glm::translate(glm::mat4(1.0f), offset);
		
	}
	
	~ObjectData() {
		if(texture) delete texture;
		if(pointData) delete pointData;
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
	
	bool isOK() const {
		return ok;
	}
	
	const glm::vec3 & getCentroid() const {
		return centroid;
	}
	
	const glm::vec3 getCentroidPosition() const {
		glm::vec4 c(centroid, 1.0);
		glm::vec4 v = mvm * c;
		return glm::vec3(v / v.w);
	}


};

#endif	/* OBJECTDATA_H */
