/* 
 * File:   TextureHandler.h
 * Author: jaa
 *
 * Created on 23. únor 2014, 22:42
 */

#ifndef TEXTUREHANDLER_H
#define	TEXTUREHANDLER_H

#include <vector>
#include <unordered_map>
#include <QObject>
#include <QRunnable>

#include "kdtree/KDTree.h"
#include "Photo.h"
#include "Texture.h"
#include "io/ImageLoader.h"

class RayCaster;


struct Cluster {
	int id;
	glm::vec3 centroid;
	float weight;
	int size;
};

class ImgThread : public QObject, public QRunnable {
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
	ImgThread(Photo &p) 
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
	
	std::shared_ptr<RayCaster> rayCaster;
	
	std::vector<Photo> photos;
	std::stack<GLuint> units;
	std::vector<Texture> textures;
	std::unordered_map<int, Photo *> nearPhotos;
	
	KDTree<Photo> kdtree;
	
	std::vector<Cluster> clusters;
	
public slots:
	void getResult(Photo *p);
	
public:	
	TextureHandler();
	~TextureHandler();
	
	void buildTree();
	void loadFullImage(Photo &p);
	void releaseImage(Photo &p);
	//TODO needs some optimizations...
	void updateTextures(
		const glm::vec3 &cameraPos, 
		const glm::vec3 &viewDir,
		const uint count
	);
	
	void setRayCaster(std::shared_ptr<RayCaster> r);
			
	const std::vector<Photo> & getPhotos() const;
	std::vector<Texture> & getTextures();
	
	void setClusters(std::vector<Cluster> c);
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

