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

class ModelLoader {

public:	
	static void importModel
	(
		const std::string &file,
		std::vector<unsigned int> &outIndices,
		std::vector<float> &outVertices,
		std::vector<float> &outNormals
	) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(file, aiProcess_JoinIdenticalVertices);
		if(scene == NULL) {
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

		const int vertices = mesh->mNumVertices;
		outVertices.resize(vertices * 3);
		outNormals.resize(vertices * 3);
		for (int i = 0; i < vertices; ++i) {
			if (mesh->HasPositions()) {
				outVertices[i*3] = mesh->mVertices[i].x;
				outVertices[i*3 + 1] = mesh->mVertices[i].y;
				outVertices[i*3 + 2] = mesh->mVertices[i].z;
			}

			if (mesh->HasNormals()) {
				outNormals[i*3] = mesh->mNormals[i].x;
				outNormals[i*3 + 1] = mesh->mNormals[i].x;
				outNormals[i*3 + 2] = mesh->mNormals[i].x;
			}
		}
	}




};

#endif	/* MODELLOADER_H */
