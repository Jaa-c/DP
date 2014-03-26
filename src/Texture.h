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
	Photo * photo;
	
	GLenum target; //todo: should be const
	GLuint unit; //todo: should be const
	
	GLuint textureID;
	GLuint samplerID;
	
	Texture(const GLenum target, const GLuint unit) : 
		photo(NULL), target(target), unit(unit), 
		textureID(GL_ID_NONE), samplerID(GL_ID_NONE) 
	{
	
	}
	
	Texture(const GLenum target, const GLuint unit, Photo * photo) : 
		photo(photo), target(target), unit(unit), 
		textureID(GL_ID_NONE), samplerID(GL_ID_NONE) 
	{
	
	}
	
	void setImage(Photo * photo) {
		if(photo->ID != this->photo->ID) {
			if(glIsTexture(textureID)) { //TODO
				glDeleteTextures(1, &textureID);
			}
			textureID = GL_ID_NONE;
			this->photo = photo;
		}
	};
	
	const rgb *getImageStart() const {
		if(!photo || photo->image.empty()) {
			return NULL;
		}
		assert(photo->image.size() == (uint) (photo->size.x + photo->rowPadding) * photo->size.y * 3);
		return &photo->image[0];
	}
	
	const glm::ivec2 getRealSize() const {
		if(!photo) {
			return glm::ivec2(0, 0);
		}
		return photo->size;
	}
	
	const glm::ivec2 getSizeWithPadding() const {
		if(!photo) {
			return glm::ivec2(0, 0);
		}
		return glm::ivec2(photo->size.x + photo->rowPadding, photo->size.y);
	}
	
	friend std::ostream & operator << (std::ostream& os, const Texture &tex) {
		os  << "Texture{unit=" << tex.unit << ", textureID=" << tex.textureID 
			<< ", samplerId=" << tex.samplerID << ", photo.ID=" << tex.photo->ID 
			<< "}\n";
		return os;
	}
	
};



#endif	/* TEXTURE_H */

