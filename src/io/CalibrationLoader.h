/* 
 * File:   CalibrationLoader.h
 * Author: jaa
 *
 * Created on 17. březen 2014, 14:50
 */

#ifndef CALIBRATIONLOADER_H
#define	CALIBRATIONLOADER_H

#include "ImageLoader.h"
#include "Rz3Parser.h"
#include "ScannerParser.h"
#include "ImageBB.h"


class CalibrationLoader {
	std::vector<Photo> &outPhotos;
	//only from bundler data
	std::shared_ptr<PointData> pointData;
	const std::function<void(int)> progress;
	const ObjectData& object;
public:
	enum FileType {
		BUNDLER = 0, //this is expected to be in the same order as GUI tabWidget
		RZ3,
		SCANNER
	};
	
	CalibrationLoader(
			std::shared_ptr<TextureHandler> th, 
			const ObjectData& object, 
			std::function<void(int)> progress = nullptr
	)
		: outPhotos(th->photos), pointData(nullptr), progress(progress), object(object) {}
		
	~CalibrationLoader() {
		pointData.reset();
	}
	
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

