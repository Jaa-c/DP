/* 
 * File:   Texture.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 22:03
 */

#ifndef TEXTURE_H
#define	TEXTURE_H

#include <GL/glew.h>

#include "TextureHandler.h"


class CameraPosition;

class Texture {	
public:	
	const Photo * photo;
	
	const GLenum target;
	const GLuint unit;
	
	GLuint textureID;
	GLuint samplerID;
	
	Texture(const GLenum target, const GLuint unit) : target(target), unit(unit), 
		textureID(GL_ID_NONE), samplerID(GL_ID_NONE) 
	{
	
	}
	
	void setImage(const Photo * photo) {
		if(&photo->image != &this->photo->image) {
			if(glIsTexture(textureID)) { //TODO
				glDeleteTextures(1, &textureID);
			}
			textureID = GL_ID_NONE;
			this->photo = photo;
		}
	};
	
	const rgb *getImageStart() const {
		if(photo->image.empty()) {
			return NULL;
		}
		assert(photo->image.size() == (uint) photo->size.x * photo->size.y * 3);
		return &photo->image[0];
	}
	
	const glm::ivec2 getSize() const {
		return photo->size;
	}
	
};



#endif	/* TEXTURE_H */

