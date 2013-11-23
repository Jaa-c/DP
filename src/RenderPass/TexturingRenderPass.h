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
	BundlerParser * bundlerParser;
	
public:
	
	TexturingRenderPass(Renderer *r, ShaderHandler *sh, BundlerParser *bp) : 
		RenderPass(TEXTURING_PASS, r, sh), bundlerParser(bp)
	{	
		shader = ShaderHandler::SHADER_BASIC;
	}
		
	~TexturingRenderPass() {

	}
	
	void draw(ObjectData *object) const {
		GLuint programID = shaderHandler->getProgramId(shader);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
		glUseProgram(programID);
		
		Camera * c = &bundlerParser->getCameras()->at(renderer->getControlls()->getCameraId());
		glUniformMatrix4fv(glGetUniformLocation(programID, "u_TextureRt"), 1, GL_FALSE, &c->Rt[0][0]);
		glUniform2fv(glGetUniformLocation(programID, "u_TextureSize"), 1, &object->texture->size[0]);
		glUniform1f(glGetUniformLocation(programID, "u_TextureFL"), c->focalL);
		
		
		
		renderer->bindCameraMatrices(programID);
		renderer->drawTexture(programID, *object->texture);
		renderer->drawObject(*object);
		glUseProgram(0);
	}



};



#endif	/* TEXTURINGRENDERPASS_H */

