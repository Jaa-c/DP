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
#include <thread>

#include <QThreadPool>
#include <QRunnable>

#include "glm/glm.hpp"
#include "glm/core/type_mat3x3.hpp"
#include "globals.h"
#include "Settings.h"
#include "kdtree/KDTree.h"

#include "Photo.h"
#include "io/ImageLoader.h"
#include "io/BundlerParser.h"
#include "Texture.h" //TODO


class ImgLoader : public QObject, public QRunnable {
	Q_OBJECT

	Photo &p;

	virtual void run() {
		if(p.loading) { //It could have been canceled
			ImageLoader::loadImage(p);
		}
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
		if(!p->loading) { //already deleted
			p->image.data.clear();
			p->image.data.shrink_to_fit();
		}
		else {
			p->loading = false;
		}
	}
	
public:	
	TextureHandler() {
		//I want low nober of threads I don't want to waint long a get 
		//all pictures at once, rather get them "sequentially"
		uint thr = std::thread::hardware_concurrency();
		thr = std::max((uint) 2, thr - 2);
		QThreadPool::globalInstance()->setMaxThreadCount(thr);
		Log::i("Using %d threads for image load.", thr);
	}
	
	void buildTree() {
		kdtree.construct(&photos);	
	}
	
	void loadFullImage(Photo &p) {
		p.loading = true;
		ImgLoader *loader = new ImgLoader(p);
		connect(loader, SIGNAL(result(Photo *)), SLOT(getResult(Photo *)), Qt::QueuedConnection); //Qt::DirectConnection
		QThreadPool::globalInstance()->start(loader);
	}
	
	//TODO needs some optimizations...
	void updateTextures(
		const glm::vec3 &cameraPos, 
		const glm::vec3 &viewDir, 
		const glm::mat4 &mvm, 
		const uint count
	) {
		
		std::vector<Photo *> toLoad, toDelete;
		std::vector<Photo *> currNearPhotos = kdtree.kNearestNeighbors<glm::vec3>(&cameraPos, 30);
		
		std::unordered_map<int, Photo *> result; //TODO: test if better than vector lookup
		for(Photo *p : currNearPhotos) {
			result.insert(std::pair<int, Photo *>(p->ID, p));
			if(p->image.data.size() == 0 && !p->loading) {
				toLoad.push_back(p);
			}
		}
		
		for(std::pair<int, Photo *> p : nearPhotos) {
			if(result.find(p.first) == result.end()) { 
				toDelete.push_back(p.second);
			}
		}
		
		for(Photo *p : toLoad) {
			//add it right now, since we have thubnails
			nearPhotos.insert(std::pair<int, Photo *>(p->ID, p));
		}
				
		if(nearPhotos.size() == 0) {
			return;
		}
		
		for(Photo *p : toDelete) {
			p->loading = false;
			nearPhotos.erase(p->ID);
			p->image.data.clear();
			p->image.data.shrink_to_fit();
		}
		
		std::vector<Photo*> currentPhotos = getClosestCameras(viewDir, mvm, count);
		
		///mostly DEBUG
		if(Settings::usePrefferedCamera) {
			Photo *p = &photos[Settings::prefferedCamera];
			if(p->image.data.size() == 0) {
				loadFullImage(*p);
			}
			auto it = std::find(currentPhotos.begin(), currentPhotos.end(), p);
			if(it != currentPhotos.end()) {
				std::iter_swap(currentPhotos.begin(), it);
			}
			else {
				currentPhotos.insert(currentPhotos.begin(), p);
				currentPhotos.pop_back();
			}
		}
		
		uint i = 0;
		bestTexIdx.clear();
		bestTexIdx.resize(std::max(count, (uint) textures.size()));
		
		for(auto it = currentPhotos.begin(); it != currentPhotos.end(); ) {
			Photo *p = *it;
			bool erased = false;
			bestTexIdx[i] = -1;
			//compare textures and current photos
			//if there is a match - OK, remove photo, no need to change texture
			for(Texture &tex : textures) {
				if(p->ID == tex.photo->ID) {
					tex.current = true;
					erased = true;
					currentPhotos.erase(it);
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
			if(!tex.current) {
				if(!currentPhotos.empty()) {
					Photo *p = *currentPhotos.begin();
					tex.setImage(p);
					loadFullImage(*p);
					currentPhotos.erase(currentPhotos.begin());
				}
				else { //lowered the number of textures
					textures.erase(it++);
					continue; //important, otherwise iterator is incremented 2x
				}
			}
			tex.current = false;
			++it;
		}
		
		i = 0;
		for(auto p : currentPhotos) {
			textures.push_back(Texture(GL_TEXTURE_RECTANGLE, textures.size(), p));
			loadFullImage(*p);
			while(bestTexIdx[i] != -1) i++;
			bestTexIdx[i] = textures.at(textures.size()-1).unit;
		}
	}
			
	const std::vector<Photo> & getPhotos() const {
		return photos;
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
			const glm::vec3 ta = glm::vec3(vecMat * glm::vec4(a->camera.direction, 1.0f));
			const glm::vec3 tb = glm::vec3(vecMat * glm::vec4(b->camera.direction, 1.0f));
			return glm::dot(ta, dir) > glm::dot(tb, dir);
		};
		
		std::set<Photo*, decltype(comp)> result(comp);
		
		for(std::pair<int, Photo *> p : nearPhotos) {
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

