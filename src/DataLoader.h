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

#include <stdio.h>
#include <jpeglib.h>

#include "glm/core/type.hpp"

typedef unsigned char rgb;

class DataLoader {
	
public:	
	/**
	 * Imports model from varios formats
     * @param file path to file
     * @param outIndices list of model indices
     * @param outVertices list of vertices
     * @param outNormals list of normals (if avaiable)
     */
	static void importModel
	(
		const std::string &file,
		std::vector<unsigned int> &outIndices,
		std::vector<glm::vec3> &outVertices,
		std::vector<glm::vec3> &outNormals
	) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(file,
				 aiProcessPreset_TargetRealtime_Quality & ~aiProcess_FindDegenerates);
		
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

	/**
	 * Loads JPEG image to rgb array
     * @param filename path to the image
     * @param reference to vector of rgb data
     * @param width image width
     * @param height image height
	 * @return true if loaded OK
     */
	static bool loadJPEG(
		const std::string filename,
		std::vector<rgb> &raw,
		int &width,
		int &height
	) {
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;

        JSAMPROW row[1];
        
        FILE *infile = fopen(filename.c_str(), "rb");
        
        if (!infile) {
			Log::e("Error opening jpeg image: %s", filename.c_str());
			return false;
        }
		
        cinfo.err = jpeg_std_error(&jerr);

        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, infile);
        jpeg_read_header(&cinfo, TRUE);

        jpeg_start_decompress(&cinfo);
		width = cinfo.output_width;
		height = cinfo.output_height;

        raw.reserve(width * height * cinfo.num_components);

        row[0] = new unsigned char[width * cinfo.num_components];

        while(cinfo.output_scanline < height)
        {
			jpeg_read_scanlines(&cinfo, row, 1);
			for(int i = 0; i < width * cinfo.num_components; i++) {
				raw.push_back(row[0][i]);
			}
        }
		
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        delete [] row[0];
		Log::i("Loaded image " + filename);
        return true;
	}

};

#endif	/* MODELLOADER_H */