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
	
	GLuint textureRTLoc;
	GLuint textureSizeLoc;
	GLuint textureFLLoc;
	
public:
	
	TexturingRenderPass(Renderer *r, ShaderHandler *sh) : 
		RenderPass(TEXTURING_PASS, r, sh)
	{	
		shader = ShaderHandler::SHADER_BASIC;
	}
		
	~TexturingRenderPass() {

	}
	
	void draw(ObjectData *object) {
		
		if(programID == GL_ID_NONE) {
			programID = shaderHandler->getProgramId(shader);
			getDefaultUniformLocations();
			textureRTLoc = glGetUniformLocation(programID, "u_TextureRt");
			textureSizeLoc = glGetUniformLocation(programID, "u_TextureSize");
			textureFLLoc = glGetUniformLocation(programID, "u_TextureFL");
		}
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
		assert(programID != GL_ID_NONE);
		glUseProgram(programID);
		
		const CameraPosition * c = &object->texture->photo->camera;
		glUniformMatrix4fv(textureRTLoc, 1, GL_FALSE, &c->Rt[0][0]);
		glUniform2iv(textureSizeLoc, 1, &object->texture->getSize()[0]);
		glUniform1f(textureFLLoc, c->focalL);
		
		renderer->setUniformLocations(&uniformLocations);
		
		renderer->bindCameraMatrices();
		
		renderer->drawTexture(*object->texture);
		renderer->drawObject(*object);
		
		glUseProgram(0);
	}

};

#endif	/* TEXTURINGRENDERPASS_H */
