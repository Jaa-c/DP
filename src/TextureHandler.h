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
#include <unordered_map>
#include <queue>

#include "glm/glm.hpp"
#include "glm/core/type_mat3x3.hpp"
#include "io/BundlerParser.h"
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
	const uint ID;
	const Image image;
	const glm::ivec2 size;
	const CameraPosition camera;
	
	bool current;
	
	Photo(const uint ID, const Image image, const glm::ivec2 size, const CameraPosition camera) :
	ID(ID), image(image), size(size), camera(camera) {}
};

#include "Texture.h" //TODO!

class TextureHandler {
	std::vector<Photo> photos;
	
	std::vector<Texture> textures;
	
public:	
	TextureHandler(const std::vector<CameraPosition> &cameras, const std::vector<ImageData> &imgData) {
		
		photos.reserve(cameras.size());
		for(uint i = 0; i < cameras.size(); ++i) {
			const ImageData *img = &imgData.at(i);
			const CameraPosition *cp = &cameras.at(i);
			photos.push_back(Photo(i, img->image, img->size, *cp));
		}
		
	}
	
	const std::vector<Photo> & getPhotos() const {
		return photos;
	}
	
	//TODO SLOW, but not priority
	void updateTextures(const glm::vec3 & dir, const glm::mat4 &mvm, const uint count) {
		std::set<Photo*> currentPhotos = getClosestCameras(dir, mvm, count);
		
		for(auto it = currentPhotos.begin(); it != currentPhotos.end(); ) {
			bool erased = false;
			//compare textures and current photos
			//if there is a match - OK, remove photo, no need to change texture
			for(auto &tex : textures) {
				if((*it)->ID == tex.photo->ID) {
					(*it)->current = true;
					currentPhotos.erase(it++);
					erased = true;
					break;
				}
			}
			if(!erased) {
				++it;
			}
		}
		
		for(auto it = textures.begin(); it != textures.end(); ) {
			Texture *tex = &*it;
			//remove textures that are no longer used
			if(!tex->photo->current) {
				if(!currentPhotos.empty()) {
					tex->setImage(*currentPhotos.begin());
					currentPhotos.erase(currentPhotos.begin());
				}
				else { //lowered the number of textures
					textures.erase(it++);
					continue; //important, otherwise iterator is incremented 2x
				}
			}
			++it;
		}
		
		for(auto p : currentPhotos) {
			textures.push_back(Texture(GL_TEXTURE_RECTANGLE, textures.size(), p));
		}
	
	}
	
	std::vector<Texture> & getTextures() {
		return textures;
	}
	
private:
	//TODO SLOW, this is just a stupid version
	//but not major problem for now
	std::set<Photo*> getClosestCameras(const glm::vec3 & dir, const glm::mat4 &mvm, const uint count) {
		
		glm::vec2 ndir(dir.x, dir.z);
		ndir = glm::normalize(ndir);
		const glm::mat4 vecMat = glm::inverse(glm::transpose(mvm));
		
		auto comp = [vecMat, dir](const Photo* a, const Photo* b) {
			const glm::vec3 ta = glm::vec3(vecMat * glm::vec4(a->camera.getDirection(), 1.0f));
			const glm::vec3 tb = glm::vec3(vecMat * glm::vec4(b->camera.getDirection(), 1.0f));
			return glm::dot(ta, dir) > glm::dot(tb, dir);
		};
		
		std::set<Photo*, decltype(comp)> result(comp);
		for(Photo &p : photos) {
			p.current = false;
			result.insert(&p);
		}
		auto beg = result.begin();
		std::advance(beg, count);
		return std::set<Photo*>(result.begin(), beg);
	}


};

#endif	/* TEXTUREHANDLER_H */

