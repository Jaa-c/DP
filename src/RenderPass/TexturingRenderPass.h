/* 
 * File:   TexturingRenderPass.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 21:07
 */

#ifndef TEXTURINGRENDERPASS_H
#define	TEXTURINGRENDERPASS_H

#include "RenderPass.h"

class TexturingRenderPass : RenderPass {
	
	
public:
	
	TexturingRenderPass(Renderer *r, ShaderHandler *sh) : RenderPass(TEXTURING_PASS, r, sh) {	
		programID = shaderHandler->getProgramId(ShaderHandler::SHADER_BASIC);
	
	}
	
	void draw(ObjectData *object) {
	
	}



};



#endif	/* TEXTURINGRENDERPASS_H */

