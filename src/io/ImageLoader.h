/* 
 * File:   TextureHandler.h
 * Author: jaa
 *
 * Created on 24. listopad 2013, 14:59
 */

#ifndef IMAGELOADER_H
#define	IMAGELOADER_H

#include <dirent.h>
#include <sys/types.h>
#include <fstream>

#include "DataLoader.h"

struct ImageData {
	Image image;
	glm::ivec2 size;
};

class ImageLoader {
	std::vector<ImageData> data;
	std::vector<std::string> jpgPaths;
	std::vector<std::string> rawPaths;
	
	const std::string folder;
	
	const std::string RAW = "raw";
	const std::string JPG = "jpg";
	
	void readDirectory(const std::string& path) {
		dirent* de;
		DIR* dp;
		dp = opendir(path.c_str());
		if (dp) {
			while ((de = readdir(dp)) != NULL) {
				std::string name(de->d_name);
					if(name.length() > 3) {
					std::string sfx = name.substr(name.length()-3, 3);
					std::transform(sfx.begin(), sfx.end(), sfx.begin(), ::tolower);
					if(sfx == JPG) {
						jpgPaths.push_back(name.substr(0, name.length()-3));
					}
					if(sfx == RAW) {
						rawPaths.push_back(name.substr(0, name.length()-3));
					}
				}
			}
			closedir(dp);
			std::sort(jpgPaths.begin(), jpgPaths.end());
			std::sort(rawPaths.begin(), rawPaths.end());
		}
	}
	
	void createRaws(std::function<void(int)> progress = NULL) {
		if(rawPaths.size() != jpgPaths.size()) {
			for(uint i = 0; i < jpgPaths.size(); ++i) {
				const std::string name = folder + jpgPaths[i];
				ImageData& id = data[i];
				if(!DataLoader::fileExists(name + RAW)) {
					DataLoader::loadJPEG(name + JPG, id.image, id.size.x, id.size.y);
					assert(id.image.size() == (uint) id.size.x * id.size.y * 3);
					
					//this should be probbably elsewhere
					std::fstream binaryFile(name + RAW, std::ios::out | std::ios::binary);
					binaryFile.write((char *) &id.size.x, sizeof(id.size));
					binaryFile.write((char *) &id.image[0], 3 * sizeof(rgb) * id.image.size());
					binaryFile.close();
					Log::i("Created file: " + name + RAW);
					if(progress) {
						progress((int) (100 / (float) jpgPaths.size() + .5f));
					}
				}
			}
		}
		rawPaths = jpgPaths;
	}
	
	/// this expects that every jpg has raw equivalent
	void loadImages(std::function<void(int)> progress = NULL) {
		assert(rawPaths.size() == jpgPaths.size());
		for(uint i = 0; i < rawPaths.size(); ++i) {
			const std::string name = folder + rawPaths[i] + RAW;
			ImageData& id = data[i];
			if(id.image.empty()) {
				DataLoader::loadRAW(name, id.image, id.size.x, id.size.y);
				if(progress) {
					progress((int) (100 / (float) rawPaths.size() + .5f));
				}
			}
		}
	}
	
public:
	ImageLoader(const std::string folder, std::function<void(int)> progress = NULL) : folder(folder + "/") {
		readDirectory(folder);
		data.resize(jpgPaths.size()); //same number of images as cameras
		
		if(data.empty()) {
			throw "No jpg or raw images in folder:\n " + folder;
		}
		
		/// convert all the jpegs to raws for faster loading
		createRaws(progress); 
		
		/// preload all images
		/// this is not neccessary, image can be loaded on demand
		loadImages(progress); 
	}
	
	const std::vector<ImageData> & getData() const {
		return data;
	}
	
	ImageData * getImage(uint camID) {
		if(camID > data.size()) {
			return NULL;
		}
		ImageData& id = data[camID];
		if(id.image.empty()) {
			DataLoader::loadJPEG(folder + jpgPaths[camID] + JPG, id.image, id.size.x, id.size.y);
			assert(id.image.size() == (uint) id.size.x * id.size.y * 3);
		}
		return &data[camID];
	}

};

#endif	/* IMAGELOADER_H */