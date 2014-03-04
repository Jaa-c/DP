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
		
		const uint sizeOfTextureData = sizeof(glm::mat4) + sizeof(glm::ivec2) + sizeof(float);
		if(programID == GL_ID_NONE) {
			programID = shaderHandler->getProgramId(shader);
			getDefaultUniformLocations();
			textureCount = glGetUniformLocation(programID, "u_textureCount");
			textureDataUB = GL_ID_NONE;
			
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
				glBufferData(GL_UNIFORM_BUFFER, 32 * sizeOfTextureData, NULL, GL_STATIC_DRAW);
				glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, textureDataUB);
			}
		}
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
		assert(programID != GL_ID_NONE);
		//assert(textureDataUB != GL_ID_NONE); //no need to crash on shader error :)
		
		glUseProgram(programID);
		
		std::vector<Texture> * textures = object->textures;
		
		//this needs optimization later!!
		if(textureDataUB != GL_ID_NONE) {
			glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
			int offset = 0;
			for(uint i = 0; i < textures->size(); ++i) { //slooooooooow
				const Photo *p = textures->at(i).photo;
				//int offset = i * sizeOfTextureData;
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), &p->camera.Rt[0][0]);
				offset += 16 * sizeof(float);
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::ivec2) , &p->size);
				offset += 2 * sizeof(int);
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &p->camera.focalL);
				offset += 2 * sizeof(float); //note that this is std140 alignment!
			}			
		}
		
		GLint texCount = textures->size();
		glUniform1i(textureCount, texCount);
		
		renderer->setUniformLocations(&uniformLocations, &textureLocations);
		
		renderer->bindCameraMatrices();
		for(auto &tex : *textures) {	//slooow ??
			renderer->drawTexture(tex);
		}
//		renderer->bindTextures(textures->size());
		renderer->drawObject(*object);
		
		glUseProgram(0);
	}

};

#endif	/* TEXTURINGRENDERPASS_H */
