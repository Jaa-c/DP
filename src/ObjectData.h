/* 
 * File:   ObjectData.h
 * Author: jaa
 *
 * Created on 16. listopad 2013, 19:13
 */

#ifndef OBJECTDATA_H
#define	OBJECTDATA_H

#include "glm/glm.hpp"
#include "io/DataLoader.h"
#include "Texture.h"
#include "TextureHandler.h"
#include "Settings.h"

struct PointData {
private:
	/// Positions of cameras
	Points cameraPos;
	
	/// viewing directions of cameras
	Points cameraDirections;
	Points fixedCameraDirections;
	
	/// Point data from bundler
	Points pointData;
public:
	GLuint pointsVBO;
	GLuint camPosVBO;
	PointData(const std::vector<Photo> &photos, const Points *bundlerPoints = nullptr) : pointsVBO(GL_ID_NONE), camPosVBO(GL_ID_NONE) {
		for(auto &photo : photos) {
			cameraPos.push_back(photo.camera.position);
			fixedCameraDirections.push_back(photo.camera.fixedDirection);
			cameraDirections.push_back(photo.camera.direction);
		}
		if(bundlerPoints) {
			pointData = *bundlerPoints;
		}
	}
	
	const Points &getPointData() const {
		return pointData;
	}
	
	const Points &getCameraDirections() const {
		return Settings::useRecomputedDirections ? fixedCameraDirections : cameraDirections;
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
	
	//AABB
	glm::vec3 min;
	glm::vec3 max;
		
	bool ok;
	
	glm::mat4 mvm;
	glm::vec3 rotation;
	glm::vec3 translation;
	
	void updateMvm() {
		mvm = glm::translate(glm::mat4(1.0f), translation);
		mvm = glm::rotate(mvm, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		mvm = glm::rotate(mvm, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		mvm = glm::rotate(mvm, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	
public:
	const std::string fileName;
	
	GLuint vaoID;
	GLuint texCoordsID;
	GLuint verticesID;
	GLuint normalsID;
	GLuint indicesID;
	
	/// deprecated
	std::vector<Texture> * textures;
	std::shared_ptr<PointData> pointData;
	
	
	ObjectData(std::string file) : ok(true), fileName(file),	
		vaoID(GL_ID_NONE), texCoordsID(GL_ID_NONE), verticesID(GL_ID_NONE), 
		normalsID(GL_ID_NONE), indicesID(GL_ID_NONE), 
		textures(NULL), pointData(NULL)
	{
		try {
			DataLoader::importModel(file, indices, vertices, normals, centroid, translation, min, max);
		}
		catch(const char* c) {
			ok = false;
		}
		///move center to the origin
		updateMvm();		
	}
	
	~ObjectData() {
		pointData.reset();
	}
	
	void rotate(glm::vec3 &rot) {
		rotation = rot;
		updateMvm();
	}
	
	void translate(glm::vec3 &trans) {
		translation = trans;
		updateMvm();
	}
	
	const glm::mat4 & getMvm() const {
		return mvm;
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
	
	std::pair<glm::vec3, glm::vec3> getAABB() const {
		return std::pair<glm::vec3, glm::vec3>(min, max);
	}
	
	bool insideAABB(const glm::vec3 &p) const {
		return p.x < max.x && p.y < max.y && p.z < max.z &&
				p.x > min.x && p.y > min.y && p.z > min.z;
	}


};

#endif	/* OBJECTDATA_H */
