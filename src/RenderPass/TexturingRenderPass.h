/* 
 * File:   TexturingRenderPass.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 21:07
 */

#ifndef TEXTURINGRENDERPASS_H
#define	TEXTURINGRENDERPASS_H

#include "RenderPass.h"

class TexturingRenderPass : public RenderPass {
	
//	/// R * t matrix
//	GLuint textureRTLoc;
//	/// texture dimenstions
//	GLuint textureSizeLoc;
//	/// focal length
//	GLuint textureFLLoc;
	
	GLuint textureCount;
	GLuint textureDataUB;
	
public:
	
	TexturingRenderPass(Renderer *r, ShaderHandler *sh) : 
		RenderPass(TEXTURING_PASS, r, sh)
	{	
		shader = ShaderHandler::SHADER_BASIC;
		textureDataUB = GL_ID_NONE;
	}
		
	~TexturingRenderPass() {

	}
	
	void draw(ObjectData *object) {
		
		if(programID == GL_ID_NONE) {
			programID = shaderHandler->getProgramId(shader);
			getDefaultUniformLocations();
//			textureRTLoc = glGetUniformLocation(programID, "u_TextureRt");
//			textureSizeLoc = glGetUniformLocation(programID, "u_TextureSize");
//			textureFLLoc = glGetUniformLocation(programID, "u_TextureFL");
			textureCount = glGetUniformLocation(programID, "u_textureCount");
		}
		
		const uint sizeOfTextureData = sizeof(glm::mat4) + sizeof(glm::ivec2) + sizeof(float);
		if(textureDataUB == GL_ID_NONE) {
			// the binding point must be smaller than GL_MAX_UNIFORM_BUFFER_BINDINGS
			GLuint bindingPoint = 1, blockIndex; 
			blockIndex = glGetUniformBlockIndex(programID, "u_textureDataBlock");
			if(blockIndex == GL_INVALID_INDEX) { //block not found
				Log::e("TexturingRenderPass: invalid u_textureDataBlock index");
			}
			else {
				glUniformBlockBinding(programID, blockIndex, bindingPoint);
				glGenBuffers(1, &textureDataUB);
				glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
				glBufferData(GL_UNIFORM_BUFFER, 32 * sizeOfTextureData + sizeof(int), NULL, GL_STATIC_DRAW);
				glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, textureDataUB);
			}
		}
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
		assert(programID != GL_ID_NONE);
		glUseProgram(programID);
		
		std::vector<Texture> * textures = object->textures;
		
		//this needs optimization later!!
		if(textureDataUB != GL_ID_NONE) {
			glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
			
			for(uint i = 0; i < textures->size(); ++i) { //slooooooooow
				const Photo *p = textures->at(i).photo;
				int offset = i * sizeOfTextureData;
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), &p->camera.Rt[0][0]);
				offset += sizeof(glm::mat4);
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::ivec2) , &p->size);
				offset += sizeof(glm::ivec2);
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &p->camera.focalL);
			}
			
			
		}
		
//		const CameraPosition * c = &object->texture->photo->camera;
//		glUniformMatrix4fv(textureRTLoc, 1, GL_FALSE, &c->Rt[0][0]);
//		glUniform2iv(textureSizeLoc, 1, &object->texture->getSize()[0]);
//		glUniform1f(textureFLLoc, c->focalL);
		GLint texCount = textures->size();
		glUniform1i(textureCount, texCount);
		
		renderer->setUniformLocations(&uniformLocations);
		
		renderer->bindCameraMatrices();
		for(uint i = 0; i < textures->size(); ++i) {	//slooow ??
			renderer->drawTexture(textures->at(i), i);
		}
		//renderer->bindTextures(textures->size());
		renderer->drawObject(*object);
		
		glUseProgram(0);
	}

};

#endif	/* TEXTURINGRENDERPASS_H */
