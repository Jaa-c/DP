/* 
 * File:   TexturingRenderPass.h
 * Author: jaa
 *
 * Created on 14. březen 2014, 22:39
 */

#ifndef TEXTURINGRENDERPASS_H
#define	TEXTURINGRENDERPASS_H

#include "RenderPass.h"
#include "TexturingPrePass.h"

class TexturingRenderPass : public RenderPass {
		
	GLuint loc_viewDir;
	GLuint loc_textureCount;
	GLuint textureDataUB;
	
	std::shared_ptr<TexturingPrePass> prePass;
	
public:
	
	TexturingRenderPass(
		Renderer& r, 
		ShaderHandler& s, 
		std::shared_ptr<TextureHandler> th,
		std::shared_ptr<TexturingPrePass> prePass
	) : 
		RenderPass(TEXTURING_PASS, r, s, th), prePass(prePass)
	{	
		shader = ShaderHandler::SHADER_TEXTURING_2;
		textureDataUB = GL_ID_NONE;
	}
		
	~TexturingRenderPass() {

	}
	
	void draw(std::shared_ptr<ObjectData> object) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glClearColor(0.4f, 0.4f, 0.7f, 0);
		
		if(programID == GL_ID_NONE) {
			programID = shaderHandler.getProgramId(shader);
			getDefaultUniformLocations();
			
			loc_textureCount = glGetUniformLocation(programID, "u_textureCount");
			loc_viewDir = glGetUniformLocation(programID, "u_viewDir");
			textureDataUB = prePass->textureDataUB;
			
			// the binding point must be smaller than GL_MAX_UNIFORM_BUFFER_BINDINGS
			GLuint bindingPoint = 1, blockIndex; 
			blockIndex = glGetUniformBlockIndex(programID, "u_textureDataBlock");
			if(blockIndex == GL_INVALID_INDEX) { //block not found
				Log::e("TexturingRenderPass: invalid u_textureDataBlock index");
			}
			else {
				glUniformBlockBinding(programID, blockIndex, bindingPoint);
				glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
				glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, textureDataUB);
			}
		}
		
		assert(programID != GL_ID_NONE);
		glUseProgram(programID);
		
		assert(textureHandler);
		std::vector<Texture> * textures = &textureHandler->getTextures();
		
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
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		GLint texCount = textures->size();
		glUniform1i(loc_textureCount, texCount);
				
		glm::vec3 viewDir = object->getCentroidPosition() - renderer.getCamera().getCameraPosition();
		glUniform3fv(loc_viewDir, 1, &viewDir[0]);
		
		renderer.setUniformLocations(&uLocs);
		renderer.bindCameraMatrices();
		renderer.drawTextures(textures);
		renderer.drawObject(*object);
		
		glCheckError();
		glUseProgram(0);
	}

};

#endif	/* TEXTURINGRENDERPASS_H */

 