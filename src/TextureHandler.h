/* 
 * File:   TextureHandler.h
 * Author: jaa
 *
 * Created on 24. listopad 2013, 14:59
 */

#ifndef TEXTUREHANDLER_H
#define	TEXTUREHANDLER_H

#include <dirent.h>
#include <sys/types.h>

class TextureHandler {
	std::vector<ImageData> data;
	std::vector<std::string> filePaths;
	
	const std::string folder;
	
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
					if(sfx == "jpg") {
						filePaths.push_back(name);
					}
				}
			}
			closedir(dp);
			std::sort(filePaths.begin(), filePaths.end());
		}
	}
	
public:
	TextureHandler(const std::string folder) : folder(folder) {
		readDirectory(folder);
		data.resize(filePaths.size()); //same number of images as cameras
	}
		
	ImageData * getImage(int camID) {
		if(camID > data.size()) {
			return NULL;
		}
		ImageData& id = data[camID];
		if(id.image.empty()) {
			DataLoader::loadJPEG(folder + filePaths[camID], id.image, id.size.x, id.size.y);
			assert(id.image.size() == id.size.x * id.size.y * 3);
		}
		return &data[camID];
	}

};

#endif	/* TEXTUREHANDLER_H */

