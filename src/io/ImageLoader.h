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
#include <sstream>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h> // for windows mkdir
#endif

#include "DataLoader.h"

struct ImageData {
	std::string path;
	Image image;
	glm::ivec2 size;
	
	ImageData(const std::string &path) : path(path) {}
};

class ImageLoader {
	std::function<void(int)> progress;
	int expectedCount;
		
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
					if(loadImage(path, name, id)) {
						data.push_back(id);
					}
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
	
public:
	ImageLoader(std::function<void(int)> progress = NULL) {
		this->progress = progress;
		expectedCount = 0;
	}
	
	const std::vector<ImageData> loadAllImages(const std::string path) {
		if(path.substr(path.length()-1, 1) != "/")
			return readDirectory(path + "/");
		else
			return readDirectory(path);
			
	}

	bool loadImage(const std::string &folder,const std::string &name, ImageData& id) {
		std::string raw = name.substr(0, name.length()-3);
		raw += RAW;
		std::string rawFile = folder + RAW + "/" + raw;
		if(!DataLoader::fileExists(rawFile)) {
			std::string sfx = name.substr(name.length()-3, 3);
			std::transform(sfx.begin(), sfx.end(), sfx.begin(), ::tolower);
			if(sfx != JPG) {
				return false; //some other file
			}
			DataLoader::loadJPEG(folder + name, id.image, id.size.x, id.size.y);
			assert(id.image.size() == (uint) id.size.x * id.size.y * 3);

			//this should be probbably elsewhere 
			#if defined(_WIN32)
			_mkdir(string(folder + RAW).c_str());
			#else 
			mkdir(string(folder + RAW).c_str(), 0755);
			#endif
			std::fstream binaryFile(rawFile, std::ios::out | std::ios::binary);
			binaryFile.write((char *) &id.size.x, sizeof(id.size));
			binaryFile.write((char *) &id.image[0], 3 * sizeof(rgb) * id.image.size());
			binaryFile.close();
			Log::i("Created file: " + rawFile);
		}
		else {
			DataLoader::loadRAW(rawFile, id.image, id.size.x, id.size.y);
		}
		if(progress && expectedCount != 0) {
			progress((int) (100 / (float) expectedCount + .5f));
		}
		return true;
	}
	
	void setExpectedCount(const int c) {
		expectedCount = c;
	}
	

};

#endif	/* IMAGELOADER_H */
