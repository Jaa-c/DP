/* 
 * File:   Photo.h
 * Author: jaa
 *
 * Created on 31. březen 2014, 14:53
 */

#ifndef PHOTO_H
#define	PHOTO_H

#include "glm/glm.hpp"
#include "globals.h"

/**
 * Contains projection matrix of a camera
 * + camera position, viewing direction, focal length
 */
struct CameraPosition {
	glm::mat4 Rt;
	
	glm::vec3 position;
	/// this is the direction of the camera
	glm::vec3 direction; 
	/// this is computed direction to the center of visible part of an object.
	glm::vec3 fixedDirection;
	/// area of photo, that contains the object. 1 = object covers the whole photo, 0= there is no object
	float relativeArea;
	
	float focalL;
	float d1, d2; //currently not used
	
	CameraPosition() {
		relativeArea = focalL = d1 = d2 = 0;
	}
};

/**
 * Image data (rgb) + image size + width padding
 */
struct Image {
	RGBData data;
	const glm::ivec2 size; 
	const uint rowPadding;
	
	Image(const glm::ivec2 size, const uint rowPadding) :
		size(size), rowPadding(rowPadding) { }
	
	Image(RGBData image, const glm::ivec2 size, const uint rowPadding) :
		data(image), size(size), rowPadding(rowPadding) {}

};

/**
 * Photo contains image data + thumbnail
 * + camera data
 */
class Photo {
	friend class TextureHandler;
	friend class ImageLoader;
	friend class ImgThread;
	
	Image image;
	Image thumbnail;
	
public:
	const uint ID;
	const std::string name;
	const CameraPosition camera;
	float thumbScale;
		
	bool loading;
	
	Photo(
		const uint ID, 
		const std::string &name,
		const CameraPosition &camera,
		const glm::ivec2 size, 
		const uint rowPadding,
		RGBData thumb,
		const glm::ivec2 tSize, 
		const uint tRowPadding
	) :	image(size, rowPadding),
		thumbnail(thumb, tSize, tRowPadding),
		ID(ID), name(name), camera(camera), loading(false) 
	{ 
		thumbScale = image.size.x / (float) thumbnail.size.x;
	}
	
	/// for KD-Tree
	int operator[] (const int i) const {
		return camera.position[i];
	}
	
	const Image& getImage() const {
//		if(!loading && image.data.size() != 0)
//			return image;
//		else
			return thumbnail;	
	}
	
	float getImageScale() const {
//		if(!loading && image.data.size() != 0)
//			return 1.0f;
//		else
			return thumbScale;
	}
	
	const Image& getFullSizeImage() const {
		return image;
	}	
};


#endif	/* PHOTO_H */

