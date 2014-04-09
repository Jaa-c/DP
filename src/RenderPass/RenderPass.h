/* 
 * File:   RenderPass.h
 * Author: jaa
 *
 * Created on 23. listopad 2013, 20:54
 */

#ifndef RENDERPASS_H
#define	RENDERPASS_H

class Renderer;

class RenderPass {
	
	friend class Rendered;
	
public:	
	/**
	 * Note that the order in this enum is the order in which the passes
	 * are drawn. So the first types are drawn first.
	 */
	enum RenderPassType {
		BASIC_TEXTURING_PASS = 0,
		TEXTURING_PRE_PASS,
		TEXTURING_PASS,
		BUNDLER_POINTS_PASS,
		RADAR_PASS, //should be last
		PASS_SIZE	
	};
	
	enum UniformLocations {
		PROJECTION_MATRIX = 0,
		MODELVIEW_MATRIX,
		NORMAL_MATRIX,
		TEXTURE0,
		COLOR,
		UNIFORM_LOC_SIZE
	};
	
	virtual void draw(std::shared_ptr<ObjectData> object) = 0;
	
	virtual ~RenderPass() {}
	
	
	void reset() {
		programID = GL_ID_NONE;
	}
	
protected:
	const RenderPassType type;
	GLuint programID;
	
	Renderer& renderer;
	ShaderHandler& shaderHandler;
	std::shared_ptr<TextureHandler> textureHandler;
	
	ShaderHandler::ShaderType shader;
	
	std::vector<GLuint> uLocs;
	
	
	RenderPass(
		RenderPassType type, 
		Renderer& r, 
		ShaderHandler& s, 
		std::shared_ptr<TextureHandler> t
	) : type(type), programID(GL_ID_NONE),
		renderer(r), shaderHandler(s), textureHandler(t) 
	{ 
	}
	
	void getDefaultUniformLocations() {
		assert(programID != GL_ID_NONE);
		
		uLocs.resize(UNIFORM_LOC_SIZE);
		uLocs[PROJECTION_MATRIX] = glGetUniformLocation(programID, "u_ProjectionMatrix");
		uLocs[MODELVIEW_MATRIX] = glGetUniformLocation(programID, "u_ModelViewMatrix");
		uLocs[NORMAL_MATRIX] = glGetUniformLocation(programID, "u_NormalMatrix");
		uLocs[TEXTURE0] = glGetUniformLocation(programID, "u_texture0");
		uLocs[COLOR] = glGetUniformLocation(programID, "u_color");
	}
	
};


#endif	/* RENDERPASS_H */

