/* 
 * File:   TextureHandler.h
 * Author: jaa
 *
 * Created on 23. únor 2014, 22:42
 */

#ifndef TEXTUREHANDLER_H
#define	TEXTUREHANDLER_H


#include <vector>
#include <set>

#include "glm/glm.hpp"
#include "glm/core/type_mat3x3.hpp"
#include "io/BundlerData.h"
#include "io/ImageLoader.h"

struct CameraPosition {
	glm::mat3 rotate;
	glm::vec3 translate;
	glm::mat4 Rt;
	
	float focalL;
	float d1, d2;
	
	//maybe just save this
	glm::vec3 getDirection() const {
		return glm::vec3(-rotate[2]);
	}
};

struct Photo {
	const Image image;
	const glm::ivec2 size;
	const CameraPosition camera;
	
	Photo(const Image image, const glm::ivec2 size, const CameraPosition camera) :
	image(image), size(size), camera(camera) {}
};

class TextureHandler {
	std::vector<Photo> photos;
	
public:	
	TextureHandler(const std::vector<CameraPosition> &cameras, const std::vector<ImageData> &imgData) {
		
		photos.reserve(cameras.size());
		for(int i = 0; i < cameras.size(); ++i) {
			const ImageData *img = &imgData.at(i);
			const CameraPosition *cp = &cameras.at(i);
			photos.push_back(Photo(img->image, img->size, *cp));
		}
		
	}
	
	const std::vector<Photo> & getPhotos() const {
		return photos;
	}
	
	const Photo * getClosestCamera(const glm::vec3 & dir, const glm::mat4 &mvm) const {
		glm::vec2 ndir(dir.x, dir.z);
		ndir = glm::normalize(ndir);
		const glm::mat4 vecMat = glm::inverse(glm::transpose(mvm));
		auto max = std::max_element(photos.begin(), photos.end(), 
			[ndir, vecMat] (const Photo &a, const Photo &b) -> bool {
				const glm::vec4 ta = vecMat * glm::vec4(a.camera.getDirection(), 1.0f);
				const glm::vec4 tb = vecMat * glm::vec4(b.camera.getDirection(), 1.0f);
				const glm::vec2 v1(ta.x, ta.z);
				const glm::vec2 v2(tb.x, tb.z);
				return glm::dot(glm::normalize(v1), ndir) < glm::dot(glm::normalize(v2), ndir);
			}
		);
		return &(*max);
	}
	
	//TODO, this is just a simple version
	std::set<const Photo*> getClosestCameras(const glm::vec3 & dir, const glm::mat4 &mvm, const uint count) const {
		std::set<const Photo*> result;
		glm::vec2 ndir(dir.x, dir.z);
		ndir = glm::normalize(ndir);
		const glm::mat4 vecMat = glm::inverse(glm::transpose(mvm));
		
		for(auto &p : photos) {
			if(result.empty()) {
				result.insert(&p);
				continue;
			}
			
			const glm::vec4 ta = vecMat * glm::vec4(p.camera.getDirection(), 1.0f);
			const glm::vec4 tb = vecMat * glm::vec4((*result.begin())->camera.getDirection(), 1.0f);
			const glm::vec2 v1(ta.x, ta.z);
			const glm::vec2 v2(tb.x, tb.z);
			
			if (result.size() < count || glm::dot(glm::normalize(v1), ndir) < glm::dot(glm::normalize(v2), ndir)) {
				result.insert(&p);
				while (result.size() > count) {
					if (result.size() == count) {
						break;
					}
					result.erase(result.begin());
				}
			}
		 }
		
		return result;
	}


};

#endif	/* TEXTUREHANDLER_H */

