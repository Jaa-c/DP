#include "Texture.h"

GLuint Texture::samplerID = GL_ID_NONE;

Texture::Texture(const GLenum target, const GLuint unit, Photo * photo) : 
	photo(photo), target(target), unit(unit), 
	textureID(GL_ID_NONE),
	current(false)
{
	prevImgSize = 0;
}

Texture::~Texture() {
	if(glIsTexture(textureID)) { //TODO
		glDeleteTextures(1, &textureID);
	}
}

bool Texture::checkTexture() {
	if(!photo || photo->getImage().data.empty()) {
		return false;
	}
	if(prevImgSize != photo->getImage().data.size()) {
		prevImgSize = photo->getImage().data.size();
		updateImage = true;
	}
	return true;
}

void Texture::setImage(Photo * photo) {
	if(photo->ID != this->photo->ID) {
		this->photo = photo;
		prevImgSize = photo->getImage().data.size();
		updateImage = true;
	}
};

const uchar *Texture::getImageStart() const {
	const Image &im = photo->getImage();
	assert(im.data.size() == (uint) (im.size.x + im.rowPadding) * im.size.y * 3);
	return &im.data[0];
}

const glm::ivec2 Texture::getSizeWithPadding() const {
	if(!photo) {
		return glm::ivec2(0, 0);
	}
	const Image &im = photo->getImage();
	return glm::ivec2(im.size.x + im.rowPadding, im.size.y);
}

const glm::ivec2 Texture::getFullSizeWithPadding() const {
	if(!photo) {
		return glm::ivec2(0, 0);
	}
	const Image &im = photo->getFullSizeImage();
	return glm::ivec2(im.size.x + im.rowPadding, im.size.y);
}