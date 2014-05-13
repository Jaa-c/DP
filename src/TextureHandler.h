/** @file
 * File:   TextureHandler.h
 * Author:  Daniel Pinc <princdan@fel.cvut.cz>
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

/// cluster for k-menas
struct Cluster {
	int id; //!< unique ID
	glm::vec3 centroid; //!< vector representing the cluster
	float weight; //!< normalized weight of the cluster
	int size; //!< number of normals in cluster
};

/**
 * Thread for image loading
 */
class ImgThread : public QRunnable {
	
	Photo &p;
	virtual void run() {
		if(p.loading) { //It could have been canceled
			ImageLoader::loadImage(p);
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

/**
 * Thread for clearin memory
 */
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


/**
 * This class handles texture logic, chooses photos and initializes load from HDD
 * @return 
 */
class TextureHandler : public QObject {
	Q_OBJECT
	
	/// loads data into photos w/out exposing it as public
	friend class CalibrationLoader; 
	/// displays kdtree data, so let's give it access, mostly debug
	friend class RadarRenderPass;
	/// needs current best photos
	friend class TexturingPrePass;
	
	
	QThreadPool loadPool; //!< thread pool for data loading
	QThreadPool clearPool; //!< thread pool for data deletion
		
	std::vector<Photo> photos; //!< thread pool for data loading
	std::stack<GLuint> units; //!< contains avaiable texture units
	std::vector<Texture> textures; //!< list of current textures
	std::unordered_map<uint, uint> photoIndices; //!< indices to textures (draw in correct order)
	
	std::vector<Cluster> clusters; //!< clusters from pre pass
		
public:	
	TextureHandler();
	~TextureHandler();
	/// initializes load from HDD
	void loadFullImage(Photo &p);
	/// releases image from RAM
	void releaseImage(Photo &p);
	
	/**
	 * Finds best textures for given direction
     * @param viewDir view direction in object space!
     * @param count required number of textures
     */
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
	
private:
	/**
	 * Chooses cameras cloeses to given direction
     * @param dir camera direction in object space
     * @param count required number of photos
     * @return 
     */
	std::vector<Photo*> getClosestCameras(const glm::vec3 & dir, const uint count);
	
	/**
	 * Chooses best cameras considering clusters from pre pass
     * @param dir camera direction in object space
     * @param count required number of photos
     * @return 
     */
	std::vector<Photo*> getBestCameras(const glm::vec3 & dir, const uint count);

};

#endif	/* TEXTUREHANDLER_H */

