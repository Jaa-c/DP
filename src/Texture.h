/* 
 * File:   Texture.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 22:03
 */

#ifndef TEXTURE_H
#define	TEXTURE_H

class Texture {
	const std::vector<unsigned char> *image;
	
public:
	const GLenum target;
	const GLuint unit;
	
	GLuint textureID;
	GLuint samplerID;
	
	glm::vec2 size;
	
	Texture(const GLenum target, const GLuint unit) : target(target), unit(unit), 
		textureID(GL_ID_NONE), samplerID(GL_ID_NONE) 
	{
	
	}
	
	void setImage(const std::vector<unsigned char> *image, const int w, const int h) {
		textureID = GL_ID_NONE;
		this->image = image;
		size.x = w;
		size.y = h;
	};
	
	const unsigned char *getImageStart() {
		return &(*image)[0];
	}
	
	

};



#endif	/* TEXTURE_H */

