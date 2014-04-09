
#include <GL/glew.h>


#include "src/Settings.h"


#include <set>
#include <map>
#include <thread>
#include <QThreadPool>
#include "glm/glm.hpp"
#include "glm/core/type_mat3x3.hpp"

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
//	std::vector<Photo*> currentPhotos = getBestCameras(viewDir, mvm, count);
	
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

std::vector<Photo*> TextureHandler::getBestCameras(const glm::vec3 & dir, const glm::mat4 &mvm, const uint count) {
	auto directions = rayCaster->getClusters();
	
	const glm::mat4 vecMat = glm::inverse(glm::transpose(mvm));
	int viewPhotos = 2; //temp todo
	
	typedef std::pair<float, Photo *> Entry;
	struct Data {
		uint size;
		std::map<float, Photo *> photos;
			glm::vec3 centroid;
		
		Data() : size(1) {}
	};
	std::vector<Data> data(1);
	int remaining = count;
	data[0].size = 2;
	data[0].centroid = glm::normalize(-dir);
	remaining -= viewPhotos;
	
	///initialize data, compute number of photos for each direction
	for(auto &d : directions) {
		if(d.vectors.size() > 0) {
			if(remaining > 0) {
				int c = std::min((int)((count - viewPhotos) * d.weight + .5f), remaining);
				assert(c >= 0);
				if(c == 0) c = remaining;
				remaining -= c;
				Data dta;
				dta.size = c;
				dta.centroid = d.centroid;
				data.push_back(dta);
			}
			else {
				break;
			}
		}
	}
	assert(remaining >= 0); //TODO, should == 0
	
	/// go thru all photos and choose best photos for each direction
	for(Photo &p : photos) {
		glm::vec3 cameraDir(vecMat * glm::vec4(p.camera.direction, 1.0f));
		cameraDir = glm::normalize(cameraDir);
		
		for(Data &d : data) {
			const float dotP = -1 * glm::dot(d.centroid, cameraDir);
			assert(dotP <= 1);
			if(dotP > 0) {
				if(d.photos.size() < d.size) { //initialization
					d.photos.insert(Entry(dotP, &p));
					continue;
				}
				if(d.photos.begin()->first < dotP) {
					d.photos.erase(d.photos.begin());
					d.photos.insert(Entry(dotP, &p));
				}
			}
		}	
	}
	
	std::vector<Photo *> result;
	result.reserve(count);
	
	for(uint i = 1; i < data.size(); ++i) {
		for(auto it = data[i].photos.rbegin(); it != data[i].photos.rend(); ++it) {
			auto &e = *it;
			//slow, but there will be only a few elements
			if(std::find(result.begin(), result.end(), e.second) == result.end()) {
				result.push_back(e.second);
			}
		}
	}
	for(auto &e : data[0].photos) {
		if(result.size() < count) {
			result.insert(result.begin(), e.second); //insert to the beginning
		}
		else {
			break;
		}
	}
	assert(result.size() <= count);
	return result;
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


void TextureHandler::setRayCaster(std::shared_ptr<RayCaster> r) {
	rayCaster = r;
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
