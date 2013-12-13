/* 
 * File:   Texture.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 22:03
 */

#ifndef TEXTURE_H
#define	TEXTURE_H


struct ImageData {
	Image image;
	glm::ivec2 size;
};


class Texture {
	ImageData* image;
	
public:
	const GLenum target;
	const GLuint unit;
	
	GLuint textureID;
	GLuint samplerID;
	
	CameraPosition *cameraPosition;
	
	Texture(const GLenum target, const GLuint unit) : target(target), unit(unit), 
		textureID(GL_ID_NONE), samplerID(GL_ID_NONE) 
	{
	
	}
	
	void setImage(ImageData *image, CameraPosition *cp) {
		if(image && image != this->image) {
			if(glIsTexture(textureID)) { //TODO
				glDeleteTextures(1, &textureID);
			}
			textureID = GL_ID_NONE;
			this->image = image;
			cameraPosition = cp;
		}
	};
	
	const rgb *getImageStart() const {
		if(image->image.empty()) {
			return NULL;
		}
		assert(image->image.size() == (uint) image->size.x * image->size.y * 3);
		return &image->image[0];
	}
	
	const glm::ivec2 getSize() const {
		return image->size;
	}
	
};



#endif	/* TEXTURE_H */

