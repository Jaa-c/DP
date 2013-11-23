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
	enum Type {
		TEXTURING_PASS,
		BUNDLER_POINTS_PASS,
		PASS_SIZE	
	};
	
	RenderPass(Type type, Renderer *r, ShaderHandler *s) : type(type) {
		shaderHandler = s;
		renderer = r;
	}
	virtual void draw(ObjectData *object);
	
protected:
	const Type type;
	ShaderHandler *shaderHandler;
	Renderer *renderer;
	
	GLuint programID;
};


#endif	/* RENDERPASS_H */

