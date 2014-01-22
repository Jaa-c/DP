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
	
	BundlerPointsRenderPass(Renderer *r, ShaderHandler *sh) : 
		RenderPass(TEXTURING_PASS, r, sh)
	{	
		shader = ShaderHandler::SHADER_POINTS;
	}
		
	~BundlerPointsRenderPass() {
		
	}
	
	void draw(ObjectData *object) {
		if(programID == GL_ID_NONE) {
			programID = shaderHandler->getProgramId(shader);
			getDefaultUniformLocations();
		}
		
		glEnable(GL_PROGRAM_POINT_SIZE );
		glUseProgram(programID);
		
		renderer->setUniformLocations(&uniformLocations);
		
		renderer->bindCameraMatrices();
		renderer->drawPointData(*object);
		
		glUseProgram(0);
		glDisable(GL_PROGRAM_POINT_SIZE );
	}
};

#endif	/* BUNDLERPOINTSRENDERPASS_H */

