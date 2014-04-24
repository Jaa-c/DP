/* 
 * File:   NormalsDebugPass.h
 * Author: jaa
 *
 * Created on 24. duben 2014, 15:07
 */

#ifndef NORMALSDEBUGPASS_H
#define	NORMALSDEBUGPASS_H

#include "RenderPass.h"

class NormalsDebugPass : public RenderPass {
			
public:
	
	NormalsDebugPass(
		Renderer& r, 
		ShaderHandler& s, 
		std::shared_ptr<TextureHandler> th
	) : 
		RenderPass(NORMALS_PASS, r, s, th)
	{	
		shader = ShaderHandler::SHADER_NORMALS;
	}
	
	void draw(std::shared_ptr<ObjectData> object) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glClearColor(0.4f, 0.4f, 0.7f, 0);
		
		if(programID == GL_ID_NONE) {
			programID = shaderHandler.getProgramId(shader);
			getDefaultUniformLocations();
		}
		
		assert(programID != GL_ID_NONE);
		glUseProgram(programID);
		
		
		renderer.setUniformLocations(&uLocs);
		renderer.bindCameraMatrices();
		renderer.drawObject(*object);
		
		glCheckError();
		glUseProgram(0);
	}

};


#endif	/* NORMALSDEBUGPASS_H */

