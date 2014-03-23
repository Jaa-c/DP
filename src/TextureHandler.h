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

#include "globals.h"
#include "glm/glm.hpp"
#include "glm/core/type_mat3x3.hpp"
#include "io/BundlerParser.h"
#include "kdtree/KDTree.h"

struct CameraPosition {
	glm::mat3 rotate;
	glm::vec3 translate;
	glm::mat4 Rt;
	
	glm::vec3 position;
	
	float focalL;
	float d1, d2;
	
	//maybe just save this
	glm::vec3 getDirection() const {
		return glm::vec3(-rotate[2]);
	}
	
	CameraPosition() {
		focalL = 0;
		d1 = 0;
		d2 = 0;
	}
};

struct Photo {
	const uint ID;
	const std::string name;
	const glm::ivec2 size; 
	const uint rowPadding;
	const CameraPosition camera;
	
	Image image;
	
	bool current;
	
	Photo(
		const uint ID, 
		const std::string name,
		const glm::ivec2 size, 
		const uint rowPadding, 
		const CameraPosition camera
	) :	ID(ID), name(name), size(size), rowPadding(rowPadding), camera(camera) {
		current = false;
	}
	
	int operator[] (const int i) const {
		return camera.position[i];
	}
};

#include "io/ImageLoader.h"
#include "Texture.h" //TODO!

class TextureHandler {
	///loads data into photos w/out exposing it as public
	friend class CalibrationLoader; 
	/// displays kdtree data, so let's give it access, mostly debug
	friend class RadarRenderPass;
	
	std::vector<Photo> photos;
	std::vector<Texture> textures;
	
	std::vector<Photo *> nearPhotos;
	
	/// indices of the best textures in the texture array bound to the shader
	std::vector<int> bestTexIdx;
	
	KDTree<Photo> kdtree;
	
public:	
	TextureHandler() {}
	
	void buildTree() {
		kdtree.construct(&photos);	
	}
		
	const std::vector<Photo> & getPhotos() const {
		return photos;
	}
	
	//TODO SLOW, but not priority
	void updateTextures(
		const glm::vec3 &cameraPos, 
		const glm::vec3 &viewDir, 
		const glm::mat4 &mvm, 
		const uint count
	) {
		nearPhotos = kdtree.kNearestNeighbors<glm::vec3>(&cameraPos, 5); //TODO
//		nearPhotos.clear();
//		nearPhotos.push_back(kdtree.nearestNeighbor<glm::vec3>(&cameraPos));
		std::vector<Photo*> currentPhotos = getClosestCameras(viewDir, mvm, count);
		//std::cout << "\nbest photo id : " << (*currentPhotos.begin())->ID << "\n";
		
		int i = 0;
		bestTexIdx.clear();
		bestTexIdx.resize(std::max(count, (uint) textures.size()));
		
		for(auto it = currentPhotos.begin(); it != currentPhotos.end(); ) {
			bool erased = false;
			bestTexIdx[i] = -1;
			//compare textures and current photos
			//if there is a match - OK, remove photo, no need to change texture
			for(auto &tex : textures) {
				if((*it)->ID == tex.photo->ID) {
					(*it)->current = true;
					currentPhotos.erase(it);
					erased = true;
					bestTexIdx[i] = tex.unit;//TODO: check!!
					i++;
					break;
				}
			}
			if(!erased) {
				++it;
			}
		}
		
		for(auto it = textures.begin(); it < textures.end(); ) {
			Texture &tex = *it;
			//remove textures that are no longer used
			if(!tex.photo->current) {
				if(!currentPhotos.empty()) {
					tex.setImage(*currentPhotos.begin());
					currentPhotos.erase(currentPhotos.begin());
				}
				else { //lowered the number of textures
					textures.erase(it++);
					continue; //important, otherwise iterator is incremented 2x
				}
			}
			++it;
		}
		
		i = 0;
		for(auto p : currentPhotos) {
			textures.push_back(Texture(GL_TEXTURE_RECTANGLE, textures.size(), p));
			while(bestTexIdx[i] != -1) i++;
			bestTexIdx[i] = textures.at(textures.size()-1).unit;
		}	
	}
	
	std::vector<Texture> & getTextures() {
		return textures;
	}
	
	std::vector<int> & getBestTexIdx() {
		return bestTexIdx;
	}
	
private:
	//TODO SLOW, this is just a stupid version
	//but not major problem for now
	std::vector<Photo*> getClosestCameras(const glm::vec3 & dir, const glm::mat4 &mvm, const uint count) {
		
		const glm::mat4 vecMat = glm::inverse(glm::transpose(mvm));
		
		auto comp = [vecMat, dir](const Photo* a, const Photo* b) {
			const glm::vec3 ta = glm::vec3(vecMat * glm::vec4(a->camera.getDirection(), 1.0f));
			const glm::vec3 tb = glm::vec3(vecMat * glm::vec4(b->camera.getDirection(), 1.0f));
			return glm::dot(ta, dir) > glm::dot(tb, dir);
		};
		
		std::set<Photo*, decltype(comp)> result(comp);
//		for(Photo &p : photos) {
//			p.current = false;
//			result.insert(&p);
//		}
		for(Photo *p : nearPhotos) {
			p->current = false;
			result.insert(p);
		}
		auto beg = result.begin();
		std::advance(beg, count);
		return std::vector<Photo*>(result.begin(), beg);
	}


};

#endif	/* TEXTUREHANDLER_H */

