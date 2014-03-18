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
	std::string path;
	Image image;
	glm::ivec2 size;
	
	ImageData(const std::string &path) : path(path) {}
};

class ImageLoader {
	std::function<void(int)> progress;
		
	const std::string RAW = "raw";
	const std::string JPG = "jpg";
	
	std::vector<ImageData> readDirectory(const std::string& path) {
		std::vector<ImageData> data;
		dirent* de;
		DIR* dp;
		dp = opendir(path.c_str());
		if (dp) {
			while ((de = readdir(dp)) != NULL) {
				std::string name(de->d_name);
				if(name.length() > 3) {
					ImageData id(name);
					loadImage(path + name, id);
					data.push_back(id);
	//				if(progress) {
	//					progress((int) (100 / (float) jpgPaths.size() + .5f));
	//				}
				}
			}
			closedir(dp);
			std::sort(data.begin(), data.end(), 
				[] (const ImageData &a, const ImageData &b) {
					return a.path < b.path;
				}
			);
		}
		return data;
	}
	
	void loadImage(const std::string path, ImageData& id) {
		std::string raw = path.substr(0, path.length()-3);
		raw += RAW;
		if(!DataLoader::fileExists(raw)) {
			std::string sfx = path.substr(path.length()-3, 3);
			std::transform(sfx.begin(), sfx.end(), sfx.begin(), ::tolower);
			if(sfx != JPG) {
				throw string("Unsupported image format: ") + path;
			}
			DataLoader::loadJPEG(path, id.image, id.size.x, id.size.y);
			assert(id.image.size() == (uint) id.size.x * id.size.y * 3);

			//this should be probbably elsewhere
			std::fstream binaryFile(raw, std::ios::out | std::ios::binary);
			binaryFile.write((char *) &id.size.x, sizeof(id.size));
			binaryFile.write((char *) &id.image[0], 3 * sizeof(rgb) * id.image.size());
			binaryFile.close();
			Log::i("Created file: " + raw);
			
		}
		else {
			DataLoader::loadRAW(raw, id.image, id.size.x, id.size.y);
		}	
	
	}
	
public:
	ImageLoader(std::function<void(int)> progress = NULL) {
		this->progress = progress;
	}
	
	const std::vector<ImageData> loadAllImages(const std::string path) {
		if(path.substr(path.length()-1, 1) != "/")
			return readDirectory(path + "/");
		else
			return readDirectory(path);
			
	}
	
//	ImageData * getImage(uint camID) {
//		if(camID > data.size()) {
//			return NULL;
//		}
//		ImageData& id = data[camID];
//		if(id.image.empty()) {
//			DataLoader::loadJPEG(folder + jpgPaths[camID] + JPG, id.image, id.size.x, id.size.y);
//			assert(id.image.size() == (uint) id.size.x * id.size.y * 3);
//		}
//		return &data[camID];
//	}

};

#endif	/* IMAGELOADER_H */
