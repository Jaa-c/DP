/** @file 
 * File:   TextureHandler.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
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
#include "../Photo.h"
#include "../TextureHandler.h"

/**
 * Storage for image-related data
 */
struct ImageData {
	std::string path; //!< File path
	RGBData image; //!< The image
	glm::ivec2 size; //!< Size of the image
	uint rowPadding; //!< Padding of the image (4-byte alignment)
	
	RGBData thumbImage; //!< Thumbnail
	glm::ivec2 thumbSize; //!< Thumnail size
	uint thumbRowPadding; //!< Thumbnail padding (4-byte alignment)
};

/**
 * Handles image loading,
 * creates raws and thumbnails
 */
class ImageLoader {
	std::function<void(int)> progress; //!< Progress if loading thw whole folder at once
	float prgVal; //!< progress value
	int expectedCount; //!< Expected number of images to be loaded
		
	const std::string RAW = "raw"; //!< RAW file extension
	const std::string JPG = "jpg"; //!< jpg file extension
	const std::string THUMB = "thumb."; //!< thumbnail suffix
	static const int thumbExpectedSize = 512; //!< thumbnail size 
	
	/*
	 * Reads all JPGs from given directory
	 */
	std::vector<ImageData> readDirectory(const std::string& path) {
		std::vector<ImageData> data;
		dirent* de;
		DIR* dp;
		dp = opendir(path.c_str());
		if (dp) {
			while ((de = readdir(dp)) != NULL) {
				std::string name(de->d_name);
				if(name.length() > 3) {
					ImageData id;
					if(checkImage(path, name, id)) {
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
		prgVal = 0;
	}
	
	/**
	 * Reads info about all images in given directory.)
	 * Thumbnails and raws are created if neccessary
     * @param path dir with images
     * @return vector of image info
     */
	const std::vector<ImageData> checkAllImages(const std::string &path) {
		if(path.substr(path.length()-1, 1) != "/")
			return readDirectory(path + "/");
		else
			return readDirectory(path);
	}
	
	/**
	 * Loads given image from HDD
     * @param p photo to be loaded
     */
	static void loadImage(Photo &p) {
		DataLoader::loadRAWData(p.name, p.image.data, p.image.size.x, p.image.size.y, p.image.rowPadding);
	} 

	/**
	 * reads image info, creates raw and thumbnail if not exist
     * @param folder folder with file
     * @param name file name
     * @param id output data about the image
     * @return true if file exists
     */
	bool checkImage(const std::string &folder,const std::string &name, ImageData& id) {
		std::string fileName = name.substr(0, name.length()-3);
		std::string rawFile = folder + RAW + "/" + fileName + RAW;
		std::string rawThumb = folder + RAW + "/" + fileName + THUMB +  RAW;
		
		id.path = rawFile;
		if(!DataLoader::fileExists(rawFile)) {
			std::string sfx = name.substr(name.length()-3, 3);
			std::transform(sfx.begin(), sfx.end(), sfx.begin(), ::tolower);
			if(sfx != JPG) {
				return false; //some other file
			}
			DataLoader::loadJPEG(folder + name, id.image, id.size.x, id.size.y, id.rowPadding);
			assert(id.image.size() == (uint) (id.size.x + id.rowPadding) * id.size.y * 3);

			//this should be probbably elsewhere 
			#if defined(_WIN32)
			_mkdir(string(folder + RAW).c_str());
			#else 
			mkdir(string(folder + RAW).c_str(), 0755);
			#endif
			std::fstream binaryFile(rawFile, std::ios::out | std::ios::binary);
			binaryFile.write((char *) &id.size.x, sizeof(id.size));
			binaryFile.write((char *) &id.rowPadding, sizeof(id.rowPadding));
			binaryFile.write((char *) &id.image[0], 3 * sizeof(uchar) * id.image.size());
			binaryFile.close();
			Log::i("Created file: " + rawFile);
			
			//create thumb:
			uint targetLvl = 0;
			uint val = id.size.x / thumbExpectedSize;
			while(val >>= 1) ++targetLvl;
			uint scaleDen = std::min(1 << targetLvl, 8);
			
			DataLoader::loadJPEG(folder + name, id.thumbImage, 
					id.thumbSize.x, id.thumbSize.y, id.thumbRowPadding, scaleDen);
			assert(id.thumbImage.size() == (uint) (id.thumbSize.x + id.thumbRowPadding) * id.thumbSize.y * 3);
			
			std::fstream thumbFile(rawThumb, std::ios::out | std::ios::binary);
			thumbFile.write((char *) &id.thumbSize.x, sizeof(id.size));
			thumbFile.write((char *) &id.thumbRowPadding, sizeof(id.thumbRowPadding));
			thumbFile.write((char *) &id.thumbImage[0], 3 * sizeof(uchar) * id.thumbImage.size());
			thumbFile.close();
			
		}
		else {
			DataLoader::loadRawInfo(rawFile, id.size.x, id.size.y, id.rowPadding);
			DataLoader::loadRAWData(rawThumb, id.thumbImage, id.thumbSize.x, id.thumbSize.y, id.thumbRowPadding);
		}
		
		if(progress && expectedCount != 0) {
			prgVal += 100 / (float) expectedCount;
			if(prgVal > 1) {
				progress(floor(prgVal));
				prgVal -= floor(prgVal);
			}
		}
		return true;
	}
	
	void setExpectedCount(const int c) {
		expectedCount = c;
		prgVal = 0;
	}
	

};

#endif	/* IMAGELOADER_H */
