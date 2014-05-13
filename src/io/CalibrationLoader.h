/** @file 
 * File:   CalibrationLoader.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
 *
 * Created on 17. březen 2014, 14:50
 */

#ifndef CALIBRATIONLOADER_H
#define	CALIBRATIONLOADER_H

#include "ImageLoader.h"
#include "Rz3Parser.h"
#include "ScannerParser.h"
#include "ImageBB.h"

/**
 * Handles loading of calibration files and matching camera data with images
 * 
 */
class CalibrationLoader {
	std::vector<Photo> &outPhotos; //!< reference to vector where the loaded data are stored
	//only from bundler data
	std::shared_ptr<PointData> pointData; //!< Point data from bundler
	const std::function<void(int)> progress; //"< function that reports loading progres
	const ObjectData& object; //!< The loaded object
public:
	/**
	 * Type of calibration data
	 * 
	 * This is expected to be in the same order as GUI tabWidget in OpenForm
	 */
	enum FileType {
		BUNDLER = 0,
		RZ3,
		SCANNER
	};
	
	/** 
     * @param th reference to texture handler
     * @param object loaded object
     * @param progress function that reports progress of loading
     */
	CalibrationLoader(
			std::shared_ptr<TextureHandler> th, 
			const ObjectData& object, 
			std::function<void(int)> progress = nullptr
	)
		: outPhotos(th->photos), pointData(nullptr), progress(progress), object(object) {}
		
	~CalibrationLoader() {
		pointData.reset();
	}
	
	/**
	 * Loads data to texture handler
     * @param type type of calibration data
     * @param photos folder folder with photos
     * @param calibration file file with calibration data
     * @param rz3images indices to photos (rz3 only)
     */
	void loadData(
			const FileType type, 
			const std::string &photosFolder, 
			const std::string &calibrationFile,
			const std::string &rz3images = "") {
		
		ImageLoader imgLoader(progress);
		ImageBB imageBB(object.getVertices());
		
		switch(type) {
			case BUNDLER:
			{ //expected bundler file
				BundlerParser bundlerData(calibrationFile);
				std::vector<CameraPosition> cameras = bundlerData.parseFile();
				imgLoader.setExpectedCount(cameras.size());
				const std::vector<ImageData> imgData = imgLoader.checkAllImages(photosFolder);
				progress(0);
				outPhotos.reserve(cameras.size());
				for(uint i = 0; i < cameras.size(); ++i) {
					if(imgData.size() <= i) {
						cameras.erase(cameras.begin() + i, cameras.end());
						break;
					}
					const ImageData &img = imgData.at(i);
					CameraPosition &cp = cameras.at(i);
					
					imageBB.computeCameraParams(cp, img);
					progress(floor(100.f / cameras.size()));
					outPhotos.push_back(
						Photo(i, img.path, cp, 
							img.size, img.rowPadding, 
							img.thumbImage, img.thumbSize, img.thumbRowPadding));
				}

				pointData = std::shared_ptr<PointData>(new PointData(outPhotos, &bundlerData.getPoints()));
				break;
			}
			case RZ3:
			{
				Rz3Parser rz3Parser(imgLoader, calibrationFile, rz3images, photosFolder);
				outPhotos = rz3Parser.parseFile(imageBB);
				pointData = std::shared_ptr<PointData>(new PointData(outPhotos));
				break;
			}
			case SCANNER: 
			{
				ScannerParser scParser(imgLoader, calibrationFile, photosFolder);
				std::vector<CameraPosition> cameras = scParser.parseFile();
				imgLoader.setExpectedCount(cameras.size());
				const std::vector<ImageData> imgData = imgLoader.checkAllImages(photosFolder);
				progress(0);
				outPhotos.reserve(cameras.size());
				for(uint i = 0; i < cameras.size(); ++i) {
					if(imgData.size() <= i) {
						cameras.erase(cameras.begin() + i, cameras.end());
						break;
					}
					const ImageData &img = imgData.at(i);
					CameraPosition &cp = cameras.at(i);
					
					imageBB.computeCameraParams(cp, img);
					progress(floor(100.f / cameras.size()));
					outPhotos.push_back(
						Photo(i, img.path, cp, 
							img.size, img.rowPadding, 
							img.thumbImage, img.thumbSize, img.thumbRowPadding));
				}
				
				pointData = std::shared_ptr<PointData>(new PointData(outPhotos));
			}
		} 
		progress(100);
	}
	
	std::shared_ptr<PointData> getPointData() {
		return pointData;
	}


};


#endif	/* CALIBRATIONLOADER_H */

