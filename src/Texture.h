/* 
 * File:   Texture.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 22:03
 */

#ifndef TEXTURE_H
#define	TEXTURE_H

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <QtGlobal>

#include "glm/glm.hpp"
#include "Photo.h"

class CameraPosition;

class Texture {	
	uint prevImgSize;
	
public:	
	Photo * photo;
	
	GLenum target; //todo: should be const
	GLuint unit; //todo: should be const
	
	GLuint textureID;
	static GLuint samplerID;//TODO: make this static, I only need one sampler
	
	bool current; // flag for loading
	bool updateImage;

	Texture(const GLenum target, const GLuint unit, Photo * photo);
	~Texture();
	bool checkTexture();
	void setImage(Photo * photo);
	const uchar *getImageStart() const;
	const glm::ivec2 getSizeWithPadding() const;
	const glm::ivec2 getFullSizeWithPadding() const;
	
	friend std::ostream & operator << (std::ostream& os, const Texture &tex) {
		os  << "Texture{unit=" << tex.unit << ", textureID=" << tex.textureID 
			<< ", samplerId=" << tex.samplerID << ", photo.ID=" << tex.photo->ID 
			<< "}\n";
		return os;
	}

};


#endif	/* TEXTURE_H */

