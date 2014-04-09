/* 
 * File:   TexturingPrePass.h
 * Author: jaa
 *
 * Created on 10. duben 2014, 0:34
 */

#ifndef TEXTURINGPREPASS_H
#define	TEXTURINGPREPASS_H

#include "RenderPass.h"

class TexturingPrePass : public RenderPass {
		
	GLuint loc_textureCount;
	GLuint loc_textureIndices;
	GLuint loc_viewDir;
	GLuint textureDataUB;
	
	GLuint frameBuffer;
	GLuint normalsTexture;
	
public:
	
	TexturingPrePass(
		Renderer& r, 
		ShaderHandler& s, 
		std::shared_ptr<TextureHandler> th
	) : 
		RenderPass(TEXTURING_PRE_PASS, r, s, th)
	{	
		shader = ShaderHandler::SHADER_TEXTURING_1;
		textureDataUB = GL_ID_NONE;
		frameBuffer = GL_ID_NONE;
		normalsTexture = GL_ID_NONE;
		
		loc_viewDir = GL_ID_NONE;
	}
		
	~TexturingPrePass() {

	}
	
	void draw(std::shared_ptr<ObjectData> object) {
		const uint sizeOfTextureData = sizeof(glm::mat4) + sizeof(glm::ivec2) + 2 * sizeof(float);
		if(programID == GL_ID_NONE) {
			programID = shaderHandler.getProgramId(shader);
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
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
		
		
		//1st pass:
		if(frameBuffer == GL_ID_NONE) {
			glGenFramebuffers(1, &frameBuffer);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		
		if(normalsTexture == GL_ID_NONE) {
			glGenTextures(1, &normalsTexture);
			glBindTexture(GL_TEXTURE_2D, normalsTexture);
			const glm::ivec2 winSize = renderer.getCamera().getWindowSize();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, winSize.x, winSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		}
		glBindTexture(GL_TEXTURE_2D, normalsTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		
		// Set "normalsTexture" as our colour attachement #0
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, normalsTexture, 0);

		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers); 
		
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Log::e("[Texturing render pass] framebuffer NOT OK");
			throw "framebuffer error, can't render textures!";
		}
		
		GLint texCount = textures->size();
		glUniform1i(loc_textureCount, texCount);
		
		std::vector<int> &indices = textureHandler->getBestTexIdx();
		glUniform1iv(loc_textureIndices, indices.size(), &indices[0]);
		
		glm::vec3 viewDir = object->getCentroidPosition() - renderer.getCamera().getCameraPosition();
		glUniform3fv(loc_viewDir, 1, &viewDir[0]);
		
		renderer.setUniformLocations(&uLocs);
		renderer.bindCameraMatrices();
		renderer.drawTextures(textures);
		renderer.drawObject(*object);
		
		glCheckError();
		glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

};

#endif	/* TEXTURINGPREPASS_H */

