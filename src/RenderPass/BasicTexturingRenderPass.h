/* 
 * File:   TexturingRenderPass.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 21:07
 */

#ifndef BASICTEXTURINGRENDERPASS_H
#define	BASICTEXTURINGRENDERPASS_H

#include "RenderPass.h"

class BasicTexturingRenderPass : public RenderPass {
		
	GLuint textureCount;
	GLuint textureDataUB;
	GLuint loc_viewDir;
	
public:
	
	BasicTexturingRenderPass(
		Renderer& r, 
		ShaderHandler& s, 
		std::shared_ptr<TextureHandler> th
	) : 
		RenderPass(BASIC_TEXTURING_PASS, r, s, th)
	{	
		shader = ShaderHandler::SHADER_BASIC;
		textureDataUB = GL_ID_NONE;
		loc_viewDir = GL_ID_NONE;
	}
		
	~BasicTexturingRenderPass() {

	}
	
	void draw(std::shared_ptr<ObjectData> object) {
				
		const uint sizeOfTextureData = sizeof(glm::mat4) + sizeof(glm::ivec2) + sizeof(float);
		if(programID == GL_ID_NONE) {
			programID = shaderHandler.getProgramId(shader);
			getDefaultUniformLocations();
			
			textureCount = glGetUniformLocation(programID, "u_textureCount");
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
		const glm::mat4 invMvm = glm::transpose(object->getMvm());
		glm::vec3 viewDir = glm::normalize(object->getCentroidPosition() - renderer.getCamera().getCameraPosition());
		glm::vec3 viewDirObjSpace(glm::normalize(invMvm * glm::vec4(viewDir, 1.0f)));
		
		textureHandler->updateTextures(viewDirObjSpace, Settings::usingTextures);
		std::vector<Texture> &textures = textureHandler->getTextures();
		const std::unordered_map<uint, uint> & indices = textureHandler->getIndices();
		
		//this needs optimization later!!
		if(textureDataUB != GL_ID_NONE) {
			glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
			for(uint i = 0; i < textures.size(); ++i) { //slooooooooow
				const Photo *p = textures.at(i).photo;
				uint offset = indices.at(p->ID) * sizeOfTextureData;
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), &p->camera.Rt[0][0]);
				offset += 16 * sizeof(float);
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::ivec2) , &p->getImage().size);
				offset += 2 * sizeof(int);
				float focalL = p->camera.focalL / p->getImageScale(); //changing focal length for thumbnails
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &focalL);
				offset += 2 * sizeof(float); //note that this is std140 alignment!
			}			
		}
		
		GLint texCount = textures.size();
		glUniform1i(textureCount, texCount);
		glUniform3fv(loc_viewDir, 1, &viewDir[0]);
		
		renderer.setUniformLocations(&uLocs);
		
		renderer.bindCameraMatrices();
		renderer.drawTextures(textures, textureHandler->getIndices());
		renderer.drawObject(*object);
		
		glUseProgram(0);
	}

};

#endif	/* BASICTEXTURINGRENDERPASS_H */
