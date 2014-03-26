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

#include <QThreadPool>
#include <QRunnable>

#include "globals.h"
#include "glm/glm.hpp"
#include "glm/core/type_mat3x3.hpp"
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
	
	bool loading; //possible union?
	bool current;
	
	Photo(
		const uint ID, 
		const std::string name,
		const glm::ivec2 size, 
		const uint rowPadding, 
		const CameraPosition camera
	) :	ID(ID), name(name), size(size), rowPadding(rowPadding), camera(camera) {
		current = false;
		loading = false;
	}
	
	int operator[] (const int i) const {
		return camera.position[i];
	}
};

#include "io/ImageLoader.h"
#include "io/BundlerParser.h"
#include "Texture.h" //TODO


class ImgLoader : public QObject, public QRunnable {
	Q_OBJECT

	Photo &p;

	virtual void run() {
		ImageLoader::loadImage(p);
		p.loading = false;
		emit result(&p);
	}

signals:
	void result(Photo *);

public:
	ImgLoader(Photo &p) 
		: QRunnable(), p(p) 
	{

	}

};

class TextureHandler : public QObject {
	Q_OBJECT
	
	///loads data into photos w/out exposing it as public
	friend class CalibrationLoader; 
	/// displays kdtree data, so let's give it access, mostly debug
	friend class RadarRenderPass;
	
	std::vector<Photo> photos;
	std::vector<Texture> textures;
	
	std::unordered_map<int, Photo *> nearPhotos;
	
	/// indices of the best textures in the texture array bound to the shader
	std::vector<int> bestTexIdx;
	
	KDTree<Photo> kdtree;
	
public slots:
	void getResult(Photo *p) {
		nearPhotos.insert(std::pair<int, Photo *>(p->ID, p));
	}
	
public:	
	TextureHandler() {
		QThreadPool::globalInstance()->setMaxThreadCount(40);
	}
	
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
		
		std::vector<Photo *> toLoad, toDelete;
		std::vector<Photo *> currNearPhotos = kdtree.kNearestNeighbors<glm::vec3>(&cameraPos, 20);
		
		std::unordered_map<int, Photo *> result; //TODO: test if better than vector lookup
		for(Photo *p : currNearPhotos) {
			result.insert(std::pair<int, Photo *>(p->ID, p));
			if(p->image.size() == 0 && !p->loading) {
				toLoad.push_back(p);
			}
		}
		
		for(std::pair<int, Photo *> p : nearPhotos) {
			if(result.find(p.first) == result.end()) { 
				toDelete.push_back(p.second);
			}
		}
		
		for(Photo *p : toLoad) {
			p->loading = true;
			ImgLoader *loader = new ImgLoader(*p);
			connect(loader, SIGNAL(result(Photo *)), SLOT(getResult(Photo *)), Qt::QueuedConnection);
			QThreadPool::globalInstance()->start(loader);
		}
		
		if(nearPhotos.size() == 0) {
			return;
		}
//		nearPhotos = result;
		
		for(Photo *p : toDelete) {
			nearPhotos.erase(p->ID);
			p->image.clear();
		}
		
		std::vector<Photo*> currentPhotos = getClosestCameras(viewDir, mvm, count);
		
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
		
		for(std::pair<int, Photo *> p : nearPhotos) {
			p.second->current = false;
			//if(p.second->image.size() != 0) {
				result.insert(p.second);
			//}
		}
		auto beg = result.begin();
		std::advance(beg, count);
		return std::vector<Photo*>(result.begin(), beg);
	}


};

#endif	/* TEXTUREHANDLER_H */

