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
	uint prevImgSize;
	
public:	
	Photo * photo;
	
	GLenum target; //todo: should be const
	GLuint unit; //todo: should be const
	
	GLuint textureID;
	GLuint samplerID;//TODO: make this static, I only need one sampler
	
	bool current; // flag for loading
	bool updateImage;
	
	Texture(const GLenum target, const GLuint unit) : 
		photo(NULL), target(target), unit(unit), 
		textureID(GL_ID_NONE), samplerID(GL_ID_NONE),
		current(false), updateImage(false)
	{
		prevImgSize = 0;
	}
	
	Texture(const GLenum target, const GLuint unit, Photo * photo) : 
		photo(photo), target(target), unit(unit), 
		textureID(GL_ID_NONE), samplerID(GL_ID_NONE),
		current(false), updateImage(false)
	{
		prevImgSize = 0;	
	}
	
	~Texture() {
		if(glIsTexture(textureID)) { //TODO
			glDeleteTextures(1, &textureID);
		}
	}
	
	bool checkTexture() {
		if(!photo || photo->getImage().data.empty()) {
			return false;
		}
		if(prevImgSize != photo->getImage().data.size()) {
			prevImgSize = photo->getImage().data.size();
			updateImage = true;
		}
		return true;
	}
	
	void setImage(Photo * photo) {
		if(photo->ID != this->photo->ID) {
			this->photo = photo;
			prevImgSize = photo->getImage().data.size();
			updateImage = true;
		}
	};
	
	const rgb *getImageStart() const {
		const Image &im = photo->getImage();
		assert(im.data.size() == (uint) (im.size.x + im.rowPadding) * im.size.y * 3);
		return &im.data[0];
	}
	
	const glm::ivec2 getSizeWithPadding() const {
		if(!photo) {
			return glm::ivec2(0, 0);
		}
		const Image &im = photo->getImage();
		return glm::ivec2(im.size.x + im.rowPadding, im.size.y);
	}
	
	const glm::ivec2 getFullSizeWithPadding() const {
		if(!photo) {
			return glm::ivec2(0, 0);
		}
		const Image &im = photo->getFullSizeImage();
		return glm::ivec2(im.size.x + im.rowPadding, im.size.y);
	}
	
	friend std::ostream & operator << (std::ostream& os, const Texture &tex) {
		os  << "Texture{unit=" << tex.unit << ", textureID=" << tex.textureID 
			<< ", samplerId=" << tex.samplerID << ", photo.ID=" << tex.photo->ID 
			<< "}\n";
		return os;
	}
	
};


#endif	/* TEXTURE_H */

