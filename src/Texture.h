/** @file 
 * File:   Texture.h
 * Author:  Daniel Pinc <princdan@fel.cvut.cz>
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

/**
 * Represents OpenGL texture
 */
class Texture {	
	uint prevImgSize;
	
public:	
	Photo * photo; //!< pointer to current photo
	
	GLenum target; //!< texture target
	GLuint unit; //!< texture unit
	
	GLuint textureID; //!< texture id
	static GLuint nearestSamplerID; //!< GL_NEAREST sampler
	static GLuint linearSamplerID; //!< GL_LINEAR sampler
		
	bool current; //!< flag for loading image
	bool updateImage; //!< if image has changed

	Texture(const GLenum target, const GLuint unit, Photo * photo);
	
	/// Deletes texture on device
	void release();
	/// thue if image present and has data
	bool checkTexture();
	void setImage(Photo * photo);
	const uchar *getImageStart() const;
	const glm::ivec2 getSizeWithPadding() const;
	const glm::ivec2 getFullSizeWithPadding() const;
	
	friend std::ostream & operator << (std::ostream& os, const Texture &tex) {
		os  << "Texture{unit=" << tex.unit << ", textureID=" << tex.textureID 
			<< ", samplerId=" << tex.nearestSamplerID << ", photo.ID=" << tex.photo->ID 
			<< "}\n";
		return os;
	}

};

#endif	/* TEXTURE_H */
