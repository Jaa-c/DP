/** @file 
 * File:   TexturingRenderPass.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
 *
 * Created on 14. březen 2014, 22:39
 */

#ifndef TEXTURINGRENDERPASS_H
#define	TEXTURINGRENDERPASS_H

#include "RenderPass.h"
#include "TexturingPrePass.h"

/**
 * The main pass for texturing
 */
class TexturingRenderPass : public RenderPass {
		
	GLuint loc_viewDir;
	GLuint loc_textureCount;
	GLuint loc_texturesBasic;
	GLuint textureDataUB;
	
	std::shared_ptr<TexturingPrePass> prePass; //!< pointer to pre pass if on
	
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
		loc_textureCount = GL_ID_NONE;
		loc_texturesBasic = GL_ID_NONE;
	}
		
	~TexturingRenderPass() {
		glDeleteBuffers(1, &textureDataUB);
	}
	
	void draw(std::shared_ptr<ObjectData> object) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.4f, 0.4f, 0.7f, 0);
		
		const uint sizeOfTextureData = sizeof(glm::mat4) + 8 * sizeof(GLfloat);
		
		if(programID == GL_ID_NONE) {
			programID = shaderHandler.getProgramId(shader);
			getDefaultUniformLocations();
			
			loc_textureCount = glGetUniformLocation(programID, "u_textureCount");
			loc_texturesBasic = glGetUniformLocation(programID, "u_texuresBasic");
			loc_viewDir = glGetUniformLocation(programID, "u_viewDir");
			if(prePass) {
				textureDataUB = prePass->textureDataUB;
			}
			
			// the binding point must be smaller than GL_MAX_UNIFORM_BUFFER_BINDINGS
			GLuint bindingPoint = 1, blockIndex; 
			blockIndex = glGetUniformBlockIndex(programID, "u_textureDataBlock");
			if(blockIndex == GL_INVALID_INDEX) { //block not found
				Log::e("TexturingRenderPass: invalid u_textureDataBlock index");
			}
			else {
				glUniformBlockBinding(programID, blockIndex, bindingPoint);
				if(textureDataUB == GL_ID_NONE) {
					glGenBuffers(1, &textureDataUB);
					glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
					glBufferData(GL_UNIFORM_BUFFER, 32 * sizeOfTextureData, NULL, GL_STATIC_DRAW);
				}
				glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
				glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, textureDataUB);
			}
		}
		
		assert(programID != GL_ID_NONE);
		assert(textureHandler);
		glUseProgram(programID);
		
		const Camera &cam = renderer.getCamera();
		
		const glm::mat4 im = glm::inverse(object->getMvm());
		const glm::mat3 invMvm = glm::transpose(glm::mat3(im));
		
		glm::vec3 viewDir;
		//this is actually really difficult to do right, there can be so many 
		//different cases, I'll leave it for now...
		if(false && object->insideAABB(glm::vec3(im * glm::vec4(cam.getCameraPosition(), 1)))) {
			viewDir = -cam.getCameraViewDirection();
		}
		else {
			viewDir = glm::normalize(object->getCentroidPosition() - cam.getCameraPosition());
		}
		glm::vec3 viewDirObjSpace(glm::normalize(glm::transpose(object->getMvm()) * glm::vec4(viewDir, 1.0f)));
		
		textureHandler->updateTextures(viewDirObjSpace, Settings::usingTextures);
		std::vector<Texture> &textures = textureHandler->getTextures();
		const std::unordered_map<uint, uint> & indices = textureHandler->getIndices();
		
		if(textureDataUB != GL_ID_NONE) {
			glBindBuffer(GL_UNIFORM_BUFFER, textureDataUB);
			for(uint i = 0; i < textures.size(); ++i) { //slooooooooow
				const Photo *p = textures.at(i).photo;
				uint offset = indices.at(p->ID) * sizeOfTextureData;

				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), &p->camera.Rt[0][0]);
				offset += 16 * sizeof(float);
				glm::vec3 fd = glm::normalize(invMvm *  p->camera.fixedDirection);
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), &fd);
				offset += 4 * sizeof(float);
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::ivec2) , &p->getImage().size);
				offset += 2 * sizeof(int);
				float focalL = p->camera.focalL / p->getImageScale(); //changing focal length for thumbnails
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &focalL);
				offset += sizeof(float);
				glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &p->camera.relativeArea);
				offset += sizeof(float);
			}
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		GLint texCount = textures.size();
		glUniform1i(loc_textureCount, texCount);
		GLint basicTex = textureHandler->getClusters().empty() ? texCount : ceil(texCount/2.f);
		glUniform1i(loc_texturesBasic, basicTex);
				
		glUniform3fv(loc_viewDir, 1, &viewDir[0]);
		
		renderer.setUniformLocations(&uLocs);
		renderer.bindCameraMatrices();
		renderer.drawTextures(textures, indices);
		renderer.drawObject(*object);
		
		glCheckError();
		glUseProgram(0);
	}

};

#endif	/* TEXTURINGRENDERPASS_H */

 