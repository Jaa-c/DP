/* 
 * File:   ModelLoader.h
 * Author: jaa
 *
 * Created on 16. listopad 2013, 14:30
 */

#ifndef MODELLOADER_H
#define	MODELLOADER_H

#include "../globals.h"
#include "../Log.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <jpeglib.h>

#include "glm/core/type.hpp"

class DataLoader {
	
	static inline void updateBB(const aiVector3D &vert, glm::vec3 &max, glm::vec3 &min) {
		if(vert.x > max.x) max.x = vert.x;
		if(vert.y > max.y) max.y = vert.y;
		if(vert.z > max.z) max.z = vert.z;
		
		if(vert.x < min.x) min.x = vert.x;
		if(vert.y < min.y) min.y = vert.y;
		if(vert.z < min.z) min.z = vert.z;
	}
		
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
		std::vector<glm::vec3> &outNormals,
		glm::vec3 &centroid,
		glm::vec3 &offset,
		glm::vec3 &min,
		glm::vec3 &max
	) {
		Assimp::Importer importer;
		importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 45.f);
		const aiScene* scene = importer.ReadFile(file,
				aiProcess_ImproveCacheLocality |
				aiProcess_Triangulate |
				aiProcess_SortByPType |
				aiProcess_FindInvalidData  |
				aiProcess_GenSmoothNormals |
				aiProcess_OptimizeMeshes |
				aiProcess_OptimizeGraph |
				0);
		
		if(scene == nullptr) {
			Log::e("[DataLoader] unable to load file: %s", importer.GetErrorString());
			throw "Error while loading file \n" + file + "\n" + importer.GetErrorString();
		}
		
		int faces = 0;
		glm::vec3 lmin = glm::vec3(
				scene->mMeshes[0]->mVertices[0].x, 
				scene->mMeshes[0]->mVertices[0].y,
				scene->mMeshes[0]->mVertices[0].z);
		glm::vec3 lmax = lmin;
		
		for(uint m = 0; m < scene->mNumMeshes; ++m) {
			const aiMesh* mesh = scene->mMeshes[m];
			outIndices.reserve(outIndices.size() + mesh->mNumFaces * 3);
			
			int indOffset = outVertices.size();
			for (uint i = 0; i < mesh->mNumFaces; ++i) {
				const aiFace &face = mesh->mFaces[i];
				assert(face.mNumIndices == 3);
				outIndices.push_back(indOffset + face.mIndices[0]);
				outIndices.push_back(indOffset + face.mIndices[1]);
				outIndices.push_back(indOffset + face.mIndices[2]);
			}
			
			outVertices.reserve(outVertices.size() + mesh->mNumVertices);
			outNormals.reserve(outNormals.size() + mesh->mNumVertices);
			for (uint i = 0; i < mesh->mNumVertices; ++i) {
				if (mesh->HasPositions()) {
					updateBB(mesh->mVertices[i], lmax, lmin);
					outVertices.push_back(glm::vec3(
							mesh->mVertices[i].x, 
							mesh->mVertices[i].y, 
							mesh->mVertices[i].z
					));
					centroid += outVertices.at(outVertices.size() - 1);
				}

				if (mesh->HasNormals()) {
					outNormals.push_back(glm::vec3(
							mesh->mNormals[i].x, 
							mesh->mNormals[i].y, 
							mesh->mNormals[i].z
					));
				}
			}
			faces += mesh->mNumFaces;
		}
		
		centroid /= outVertices.size();
		min = lmin;
		max = lmax;
		offset = -centroid;
		Log::i("[DataLoader] Loaded mesh with %d faces and %d vertices.", faces, outVertices.size());
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
		RGBData &raw,
		int &width,
		int &height,
		uint &rowPadding,
		int scaleDenom = 1
	) {
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;

        JSAMPROW row[1];
        
        FILE *infile = fopen(filename.c_str(), "rb");
        
        if (!infile) {
			Log::e("Error opening jpeg image: " + filename);
			return false;
        }
		
        cinfo.err = jpeg_std_error(&jerr);

        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, infile);				
        jpeg_read_header(&cinfo, TRUE);
		
		cinfo.scale_denom = scaleDenom;

        jpeg_start_decompress(&cinfo);
		width = cinfo.output_width;
		height = cinfo.output_height;
		
		rowPadding = width % 4;

        raw.reserve((width+rowPadding) * height * cinfo.num_components);

        row[0] = new unsigned char[width * cinfo.num_components];

        while(cinfo.output_scanline < (uint) height)
        {
			jpeg_read_scanlines(&cinfo, row, 1);
			for(int i = 0; i < width * cinfo.num_components; i++) {
				raw.push_back(row[0][i]);
			}
			for(uint i = 0; i < rowPadding * cinfo.num_components; ++i) {
				raw.push_back((unsigned char) 0);
			}
        }
		//width += rowPadding; //TODO !?
		
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        delete [] row[0];
        return true;
	}
	
	static void loadRawInfo(
		const std::string &filename,
		int &width, 
		int &height,
		uint &rowPadding
	) {
		// Read input file
		if (fileExists(filename)) {
			std::ifstream input(filename, std::ios::in | std::ifstream::binary);
			input.read(reinterpret_cast<char*> (&width), sizeof(int));
			input.read((char*) (&height), sizeof(int));
			input.read((char*) (&rowPadding), sizeof(int));
						
			Log::i("[DataLoader] Loaded image info: " + filename + " %d x %d", width, height);
		}
		else {
			Log::e("[DataLoader] File does not exist: " + filename);
		}
	}
	
	/**
	 * Loads raw image to rgb array
     * @param filename path to the image
     * @param reference to vector of rgb data
	 * @return true if loaded OK
     */
	static void loadRAWData (
		const std::string &filename, 
		RGBData &raw, 
		const int &width, 
		const int &height,
		const uint &rowPadding
	) {
		// Read input file
		if (fileExists(filename)) {
			std::ifstream input(filename, std::ios::in | std::ifstream::binary);
			if(width * height == 0) {
				input.read((char*) (&width), sizeof(int));
				input.read((char*) (&height), sizeof(int));
				input.read((char*) (&rowPadding), sizeof(int));
			} 
			else {
				input.seekg(3 * sizeof(int));
			}
			
			const size_t size = (width + rowPadding) * height * 3;
			raw.resize(size);
			input.read((char*) &raw[0], raw.size() * sizeof(uchar));
			//Log::i("[DataLoader] Loaded image: " + filename + " %d x %d", width, height);
		}
		else {
			//Log::e("[DataLoader] File does not exist: " + filename);
		}
	}
	
	static inline bool fileExists (const std::string& name) {
		if (FILE *file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
		} else {
			return false;
		}   
	}

};

#endif	/* MODELLOADER_H */
