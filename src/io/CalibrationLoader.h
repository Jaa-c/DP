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


class CalibrationLoader {
	std::vector<Photo> &outPhotos;
	//only from bundler data
	std::shared_ptr<PointData> pointData;
	const std::function<void(int)> progress;
	
public:
	enum FileType {
		BUNDLER = 0, //this is expected to be in the same order as GUI tabWidget
		RZ3
	};
	
	CalibrationLoader(TextureHandler *th, std::function<void(int)> progress = nullptr)
		: outPhotos(th->photos), pointData(nullptr), progress(progress) {}
		
	~CalibrationLoader() {
		pointData.reset();
	}
	
	void loadData(
			const FileType type, 
			const std::string photosFolder, 
			const std::string calibrationFile,
			const std::string rz3images = "") {
		
		ImageLoader imgLoader(progress);
		
		switch(type) {
			case BUNDLER:
			{ //expected bundler file
				BundlerParser bundlerData(calibrationFile);
				const std::vector<CameraPosition> cameras = bundlerData.parseFile();
				imgLoader.setExpectedCount(cameras.size());
				const std::vector<ImageData> imgData = imgLoader.loadAllImages(photosFolder);

				outPhotos.reserve(cameras.size());
				for(uint i = 0; i < cameras.size(); ++i) {
					const ImageData &img = imgData.at(i);
					const CameraPosition &cp = cameras.at(i);
					outPhotos.push_back(Photo(i, img.image, img.size, cp));
				}

				pointData = std::shared_ptr<PointData>(new PointData(outPhotos, bundlerData.getPoints()));
				break;
			}
			case RZ3:
			{
				Rz3Parser rz3Parser(imgLoader, calibrationFile, rz3images, photosFolder);
				outPhotos = rz3Parser.parseFile();
			}
		} 
	
	}
	
	std::shared_ptr<PointData> getPointData() {
		return pointData;
	}


};


#endif	/* CALIBRATIONLOADER_H */

