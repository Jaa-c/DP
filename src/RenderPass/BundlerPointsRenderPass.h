/* 
 * File:   BundlerPointsRenderPass.h
 * Author: jaa
 *
 * Created on 24. listopad 2013, 14:14
 */

#ifndef BUNDLERPOINTSRENDERPASS_H
#define	BUNDLERPOINTSRENDERPASS_H

#include "RenderPass.h"

class BundlerPointsRenderPass : public RenderPass {
		
public:
	
	BundlerPointsRenderPass(Renderer *r, ShaderHandler *sh, BundlerParser *bp) : 
		RenderPass(TEXTURING_PASS, r, sh)
	{	
		shader = ShaderHandler::SHADER_POINTS;
	}
		
	~BundlerPointsRenderPass() {
		
	}
	
	void draw(ObjectData *object) {
		GLuint programID = shaderHandler->getProgramId(shader);
		
		glEnable(GL_PROGRAM_POINT_SIZE );
		glUseProgram(programID);
		renderer->bindCameraMatrices(programID);
		renderer->drawPointData(programID, *object);
		glUseProgram(0);
	}
};

#endif	/* BUNDLERPOINTSRENDERPASS_H */

