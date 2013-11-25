/* 
 * File:   RenderPass.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 20:54
 */

#ifndef RENDERPASS_H
#define	RENDERPASS_H

#include <GL/glew.h> //ide bug

class Renderer;

class RenderPass {
public:	
	enum RenderPassType {
		TEXTURING_PASS = 0,
		BUNDLER_POINTS_PASS,
		PASS_SIZE	
	};
	
	enum UniformLocations {
		PROJECTION_MATRIX = 0,
		MODELVIEW_MATRIX,
		TEXTURE0,
		COLOR,
		UNIFORM_LOC_SIZE
	};
	
	virtual void draw(ObjectData *object) = 0;
	
	virtual ~RenderPass() {}
	
protected:
	const RenderPassType type;
	ShaderHandler *shaderHandler;
	Renderer *renderer;
	
	ShaderHandler::ShaderType shader;
	
	std::vector<GLuint> uniformLocations;
	
	GLuint programID;
	
	RenderPass(RenderPassType type, Renderer *r, ShaderHandler *s) : type(type), programID(GL_ID_NONE) {
		shaderHandler = s;
		renderer = r;
	}
	
	void getDefaultUniformLocations() {
		assert(programID != GL_ID_NONE);
		
		uniformLocations.resize(UNIFORM_LOC_SIZE);
		uniformLocations[PROJECTION_MATRIX] = glGetUniformLocation(programID, "u_ProjectionMatrix");
		uniformLocations[MODELVIEW_MATRIX] = glGetUniformLocation(programID, "u_ModelViewMatrix");
		uniformLocations[TEXTURE0] = glGetUniformLocation(programID, "u_texture0");
		uniformLocations[COLOR] = glGetUniformLocation(programID, "u_color");
	}
	
};


#endif	/* RENDERPASS_H */

