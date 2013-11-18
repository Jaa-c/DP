/* 
 * File:   ModelLoader.h
 * Author: jaa
 *
 * Created on 16. listopad 2013, 14:30
 */

#ifndef MODELLOADER_H
#define	MODELLOADER_H


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "lib/glm/core/type.hpp"

class ModelLoader {

public:	
	static void importModel
	(
		const std::string &file,
		std::vector<unsigned int> &outIndices,
		std::vector<glm::vec3> &outVertices,
		std::vector<glm::vec3> &outNormals
	) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(file, aiProcess_JoinIdenticalVertices);
		
		if(scene == NULL) {
			Log::e("[ModelLoader] unable to load file: %s", importer.GetErrorString());
			throw importer.GetErrorString();
		}
		
		const aiMesh* mesh = scene->mMeshes[0];
		outIndices.resize(mesh->mNumFaces * 3);

		for (int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace &face = mesh->mFaces[i];
			assert(face.mNumIndices == 3);
			outIndices[i*3] = face.mIndices[0];
			outIndices[i*3 + 1] = face.mIndices[1];
			outIndices[i*3 + 2] = face.mIndices[2];
		}

		outVertices.resize(mesh->mNumVertices);
		outNormals.resize(mesh->mNumVertices);
		for (int i = 0; i < mesh->mNumVertices; ++i) {
			if (mesh->HasPositions()) {
				outVertices[i] = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			}

			if (mesh->HasNormals()) {
				outNormals[i] = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			}
		}
		
		Log::i("[ModelLoader] Loaded mesh with %d faces and %d vertices.", mesh->mNumFaces, mesh->mNumVertices);
	}




};

#endif	/* MODELLOADER_H */
