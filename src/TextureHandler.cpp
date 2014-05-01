
#include <GL/glew.h>


#include "src/Settings.h"


#include <set>
#include <map>
#include <thread>
#include <QThreadPool>
#include "glm/glm.hpp"
#include "glm/core/type_mat3x3.hpp"
#include "glm/gtc/matrix_inverse.hpp"

#include "TextureHandler.h"
#include "globals.h"
#include "Settings.h"
#include "src/Photo.h"
#include "RayCaster.h"
	
TextureHandler::TextureHandler() {
	//I want low nober of threads I don't want to waint long a get 
	//all pictures at once, rather get them "sequentially"
	uint thr = std::thread::hardware_concurrency();
	thr = std::max((uint) 2, thr - 2);
	QThreadPool::globalInstance()->setMaxThreadCount(thr);
	Log::i("Using %d threads for image load.", thr);
	
	textures.reserve(Settings::maxTextures);
	
	for(int i = Settings::maxTextures-1; i >=0; --i) {
		units.push(i);
	}
}

TextureHandler::~TextureHandler() {
	for(Texture &tex: textures) {
		tex.release();
	}
}

//TODO needs some optimizations...
void TextureHandler::updateTextures(
	const glm::vec3 &viewDir, //in object space!!
	const uint count
) {
	if(!nearPhotos.empty()) {
		nearPhotos.clear();
	}
	
	std::vector<Photo*> currentPhotos = getBestCameras(viewDir, count);
	
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
	for(auto it = currentPhotos.begin(); it != currentPhotos.end(); ++i) {
		Photo *p = *it;
		bool erased = false;
		//compare textures and current photos
		//if there is a match - OK, remove photo, no need to change texture
		for(Texture &tex : textures) {
			if(p->ID == tex.photo->ID) {
				tex.current = true;
				erased = true;
				currentPhotos.erase(it);
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
			}
			else { //lowered the number of textures
				units.push(tex.unit);
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
		if(!units.empty()) {
			textures.push_back(Texture(GL_TEXTURE_RECTANGLE, units.top(), p));	
			units.pop();
			loadFullImage(*p);
		}
	}
}

std::vector<Photo*> TextureHandler::getBestCameras(const glm::vec3 & dir, const uint count) {
	int remaining = count;
	std::set<Photo *> result;
	int use = clusters.empty() ? count : ceil(count/2.f);
	remaining -= use; //how moch more photos to add
	std::vector<Photo *> r = getClosestCameras(dir, use);
	result.insert(r.begin(), r.end());
	if(clusters.size() > 0) {
		for(Cluster &c : clusters) {
			if(remaining > 0) {
				use = ceil(floor(count/2.f) * c.weight); //how many pictures to use for this direction
				std::vector<Photo *> tmp = getClosestCameras(c.centroid, count); //need to get more if duplicite
				for(Photo * p : tmp) {
					if(result.find(p) == result.end()) {
						use--;
						remaining--;
						result.insert(p);
						if(use == 0 || remaining == 0) {
							break;
						}
					}
				}
			}
		}
	}
	assert(result.size() <= count);
	return std::vector<Photo*>(result.begin(), result.end());
}

std::vector<Photo*> TextureHandler::getClosestCameras(const glm::vec3 &d, const uint count) {

	const glm::vec3 dir = glm::normalize(d);

	auto comp = [dir](const Photo* a, const Photo* b) {
		return glm::dot(a->camera.direction, dir) > glm::dot(b->camera.direction, dir);
	};

	std::set<Photo*, decltype(comp)> result(comp);

	for(Photo &p : photos) {
		result.insert(&p);
	}
	
	auto beg = result.begin();
	std::advance(beg, count);
	return std::vector<Photo*>(result.begin(), beg);
}

void TextureHandler::setClusters(std::vector<Cluster> c) {
	clusters = c;
}

void TextureHandler::emptyClusters() {
	clusters.clear();
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


void TextureHandler::setRayCaster(std::shared_ptr<RayCaster> r) {
	rayCaster = r;
}

const std::vector<Photo> & TextureHandler::getPhotos() const {
	return photos;
}

std::vector<Texture> & TextureHandler::getTextures() {
	return textures;
}
