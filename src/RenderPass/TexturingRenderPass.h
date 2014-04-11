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
	GLuint loc_textureIndices;
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
			loc_textureIndices = glGetUniformLocation(programID, "u_textureIndices");
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
	}

};

#endif	/* TEXTURINGRENDERPASS_H */

 