/* 
 * File:   TextureHandler.h
 * Author: jaa
 *
 * Created on 23. únor 2014, 22:42
 */

#ifndef TEXTUREHANDLER_H
#define	TEXTUREHANDLER_H

#include <vector>
#include <stack>
#include <unordered_map>
#include <QObject>
#include <QRunnable>
#include <QThreadPool>

#include "Photo.h"
#include "Texture.h"
#include "io/ImageLoader.h"
#include "Settings.h"


struct Cluster {
	int id;
	glm::vec3 centroid;
	float weight;
	int size;
};

class ImgThread : public QRunnable {
	
	Photo &p;
	virtual void run() {
		if(p.loading) { //It could have been canceled
			ImageLoader::loadImage(p);;
		}
		if(!p.loading) {
			p.image.data.clear();
			p.image.data.shrink_to_fit();
		}
		p.loading = false;
	}

public:
	ImgThread(Photo &p) 
		: QRunnable(), p(p) 
	{
	}
};

class CleanThread : public QRunnable {
	
	Photo &p;
	virtual void run() {
		if(!p.image.data.empty()) {
			p.image.data.clear();
			p.image.data.shrink_to_fit();
		}
	}

public:
	CleanThread(Photo &p) 
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
	
	
	QThreadPool pool;
		
	std::vector<Photo> photos;
	std::stack<GLuint> units;
	std::vector<Texture> textures;
	std::unordered_map<uint, uint> photoIndices;
	
	std::vector<Cluster> clusters;
		
public:	
	TextureHandler();
	~TextureHandler();
	
	void loadFullImage(Photo &p);
	void releaseImage(Photo &p);
	//TODO needs some optimizations...
	void updateTextures(
		const glm::vec3 &viewDir,
		const uint count
	);
				
	const std::vector<Photo> & getPhotos() const;
	std::vector<Texture> & getTextures();
	const std::unordered_map<uint, uint> & getIndices() const;
	
	void setClusters(std::vector<Cluster> c);
	const std::vector<Cluster>& getClusters() const;
	void emptyClusters();
//private:
	//TODO SLOW, this is just a stupid version
	//but not major problem for now
	//only based on current direction
	std::vector<Photo*> getClosestCameras(const glm::vec3 & dir, const uint count);
	
	///based on directions from RayCaster
	std::vector<Photo*> getBestCameras(const glm::vec3 & dir, const uint count);

};

#endif	/* TEXTUREHANDLER_H */

