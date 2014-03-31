/* 
 * File:   TexturingRenderPass.h
 * Author: jaa
 *
 * Created on 14. březen 2014, 22:39
 */

#ifndef TEXTURINGRENDERPASS_H
#define	TEXTURINGRENDERPASS_H

#include "RenderPass.h"

class TexturingRenderPass : public RenderPass {
		
	GLuint loc_viewDir;
	GLuint loc_textureCount;
	GLuint loc_textureIndices;
	GLuint textureDataUB;
	
public:
	
	TexturingRenderPass(Renderer *r, ShaderHandler *sh, TextureHandler *th) : 
		RenderPass(TEXTURING_PASS, r, sh, th)
	{	
		shader = ShaderHandler::SHADER_TEXTURING;
		textureDataUB = GL_ID_NONE;
	}
		
	~TexturingRenderPass() {

	}
	
	void draw(ObjectData *object) {
				
		const uint sizeOfTextureData = sizeof(glm::mat4) + sizeof(glm::ivec2) + sizeof(float);
		if(programID == GL_ID_NONE) {
			programID = shaderHandler->getProgramId(shader);
			getDefaultUniformLocations();
			
			loc_textureCount = glGetUniformLocation(programID, "u_textureCount");
			loc_textureIndices = glGetUniformLocation(programID, "u_textureIndices");
			loc_viewDir = glGetUniformLocation(programID, "u_viewDir");
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
		
		assert(textureHandler);
		std::vector<Texture> * textures = &textureHandler->getTextures();
		
		//this needs optimization later!!
		if(textureDataUB != GL_ID_NONE) {
			glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
			int offset = 0;
			for(uint i = 0; i < textures->size(); ++i) { //slooooooooow
				const Photo *p = textures->at(i).photo;
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), &p->camera.Rt[0][0]);
				offset += 16 * sizeof(float);
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::ivec2) , &p->getImage().size);
				offset += 2 * sizeof(int);
				float focalL = p->camera.focalL / p->getImageScale(); //changing focal length for thumbnails
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &focalL);
				offset += 2 * sizeof(float); //note that this is std140 alignment!
			}
		}
		
		GLint texCount = textures->size();
		glUniform1i(loc_textureCount, texCount);
		
		std::vector<int> &indices = textureHandler->getBestTexIdx();
		glUniform1iv(loc_textureIndices, indices.size(), &indices[0]);
		
		glm::vec3 viewDir = object->getCentroidPosition() - renderer->getCamera()->getCameraPosition();
		glUniform3fv(loc_viewDir, 1, &viewDir[0]);
		
		renderer->setUniformLocations(&uLocs);
		
		renderer->bindCameraMatrices();
		renderer->drawTextures(textures);
		renderer->drawObject(*object);
		
		glUseProgram(0);
	}

};

#endif	/* TEXTURINGRENDERPASS_H */

 