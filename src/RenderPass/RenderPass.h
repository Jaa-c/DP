/* 
 * File:   RenderPass.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 20:54
 */

#ifndef RENDERPASS_H
#define	RENDERPASS_H

#include <GL/glew.h> //ide bug

class RenderPass {
public:	
	enum RenderPassType {
		TEXTURING_PASS,
		BUNDLER_POINTS_PASS,
		PASS_SIZE	
	};
	
	virtual void draw(ObjectData *object) const = 0;
	
	virtual ~RenderPass() {}
	
protected:
	const RenderPassType type;
	ShaderHandler *shaderHandler;
	Renderer *renderer;
	
	ShaderHandler::ShaderType shader;
	
	RenderPass(RenderPassType type, Renderer *r, ShaderHandler *s) : type(type) {
		shaderHandler = s;
		renderer = r;
	}
	
};


#endif	/* RENDERPASS_H */

