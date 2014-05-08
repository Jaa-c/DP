/* 
 * File:   BundlerPointsRenderPass.h
 * Author: jaa
 *
 * Created on 24. listopad 2013, 14:14
 */

#ifndef BUNDLERPOINTSRENDERPASS_H
#define	BUNDLERPOINTSRENDERPASS_H

#include "RenderPass.h"

class CameraPointsRenderPass : public RenderPass {
		
public:
	
	CameraPointsRenderPass(
		Renderer& r, 
		ShaderHandler& s, 
		std::shared_ptr<TextureHandler> th
	) : 
		RenderPass(BASIC_TEXTURING_PASS, r, s, th)
	{	
		shader = ShaderHandler::SHADER_POINTS;
	}
		
	~CameraPointsRenderPass() {
		
	}
	
	void draw(std::shared_ptr<ObjectData> object) {
		if(programID == GL_ID_NONE) {
			programID = shaderHandler.getProgramId(shader);
			getDefaultUniformLocations();
		}
		
		glEnable(GL_PROGRAM_POINT_SIZE );
		glUseProgram(programID);
		
		renderer.setUniformLocations(&uLocs);
		
		renderer.bindCameraMatrices();
		renderer.drawPointData(*object);
		
		glUseProgram(0);
		glDisable(GL_PROGRAM_POINT_SIZE );
	}
};

#endif	/* BUNDLERPOINTSRENDERPASS_H */

