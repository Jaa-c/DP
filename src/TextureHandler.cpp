
#include <GL/glew.h>


#include "src/Settings.h"


#include <set>
#include <thread>
#include <QThreadPool>
#include "glm/glm.hpp"
#include "glm/core/type_mat3x3.hpp"

#include "TextureHandler.h"
#include "globals.h"
#include "Settings.h"
#include "src/Photo.h"
	
TextureHandler::TextureHandler() {
	//I want low nober of threads I don't want to waint long a get 
	//all pictures at once, rather get them "sequentially"
	uint thr = std::thread::hardware_concurrency();
	thr = std::max((uint) 2, thr - 2);
	QThreadPool::globalInstance()->setMaxThreadCount(thr);
	Log::i("Using %d threads for image load.", thr);
	
	textures.reserve(Settings::maxTextures);
}

TextureHandler::~TextureHandler() {
	for(Texture &tex: textures) {
		tex.release();
	}
}

//TODO needs some optimizations...
void TextureHandler::updateTextures(
	const glm::vec3 &cameraPos, 
	const glm::vec3 &viewDir, 
	const glm::mat4 &mvm, 
	const uint count
) {
	if(Settings::useKDT) {
		std::vector<Photo *> toLoad, toDelete;
		std::vector<Photo *> currNearPhotos = kdtree.kNearestNeighbors<glm::vec3>(&cameraPos, 50);

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
			nearPhotos.erase(p->ID);
			releaseImage(*p);
		}
	}
	else {
		if(!nearPhotos.empty()) {
			nearPhotos.clear();
		}
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

	for(auto it = currentPhotos.begin(); it != currentPhotos.end(); ++i) {
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
				bestTexIdx[i] = tex.unit;
				break;
			}
		}
		if(!erased) {
			++it;
		}
	}
	
	i = 0;
	for(auto it = textures.begin(); it < textures.end(); ) {
		Texture &tex = *it;
		//remove textures that are no longer used
		if(!tex.current) {
			releaseImage(*tex.photo);
			if(!currentPhotos.empty()) {
				Photo *p = *currentPhotos.begin();
				tex.setImage(p);
				loadFullImage(*p);
				currentPhotos.erase(currentPhotos.begin());
				while(bestTexIdx[i] != -1) i++;
				bestTexIdx[i] = tex.unit;
			}
			else { //lowered the number of textures
				tex.release();
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
	bestTexIdx.resize(textures.size());
}

void TextureHandler::loadFullImage(Photo &p) {
	if(!p.loading) {
		p.loading = true;
		ImgThread *loader = new ImgThread(p);
		connect(loader, SIGNAL(result(Photo *)), SLOT(getResult(Photo *)), Qt::QueuedConnection); //Qt::DirectConnection
		QThreadPool::globalInstance()->start(loader);
	}
}

void TextureHandler::releaseImage(Photo &p) {
	p.loading = false;
	p.image.data.clear();
	p.image.data.shrink_to_fit();
}

void TextureHandler::getResult(Photo *p) {
	if(!p->loading) { //already deleted
		releaseImage(*p);
	}
	else {
		p->loading = false;
	}
}

void TextureHandler::buildTree() {
	kdtree.construct(&photos);	
}

const std::vector<Photo> & TextureHandler::getPhotos() const {
	return photos;
}

std::vector<Texture> & TextureHandler::getTextures() {
	return textures;
}

std::vector<int> & TextureHandler::getBestTexIdx() {
	return bestTexIdx;
}

std::vector<Photo*> TextureHandler::getClosestCameras(const glm::vec3 & dir, const glm::mat4 &mvm, const uint count) {

	const glm::mat4 vecMat = glm::inverse(glm::transpose(mvm));

	auto comp = [vecMat, dir](const Photo* a, const Photo* b) {
		const glm::vec3 ta = glm::vec3(vecMat * glm::vec4(a->camera.direction, 1.0f));
		const glm::vec3 tb = glm::vec3(vecMat * glm::vec4(b->camera.direction, 1.0f));
		return glm::dot(ta, dir) > glm::dot(tb, dir);
	};

	std::set<Photo*, decltype(comp)> result(comp);

	if(Settings::useKDT) {
		for(std::pair<int, Photo *> p : nearPhotos) {
			result.insert(p.second);
		}
	}
	else {
		for(Photo &p : photos) {
			result.insert(&p);
		}
	}
	auto beg = result.begin();
	std::advance(beg, count);
	return std::vector<Photo*>(result.begin(), beg);
}
