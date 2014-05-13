/** @file 
 * File:   RenderPass.h
 * Author: Daniel Pinc <princdan@fel.cvut.cz>
 *
 * Created on 23. listopad 2013, 20:54
 */

#ifndef RENDERPASS_H
#define	RENDERPASS_H

class Renderer;

/**
 * Base class for all render passes
 */
class RenderPass {
		
public:	
	/**
	 * Types of render passes
	 * Note that the order in this enum is the order in which the passes
	 * are drawn. So the first types are drawn first.
	 */
	enum RenderPassType {
		BASIC_TEXTURING_PASS = 0,
		TEXTURING_PRE_PASS,
		TEXTURING_PASS,
		BUNDLER_POINTS_PASS,
		NORMALS_PASS,
		RADAR_PASS, ///< should be last
		PASS_SIZE	
	};
	
	/**
	 * Uniform locations for the common stuff
	 */
	enum UniformLocations {
		PROJECTION_MATRIX = 0, ///< projection matrix
		MODELVIEW_MATRIX, ///< final modelview matrix
		NORMAL_MATRIX, ///< normal matrix
		TEXTURE0, ///< texture location
		COLOR, ///< color if needed
		UNIFORM_LOC_SIZE
	};
	
	/**
	 * Draws the object
     * @param object
     */
	virtual void draw(std::shared_ptr<ObjectData> object) = 0;
	
	virtual ~RenderPass() {}
	
	/**
	 * Deletes shaders
     */
	virtual void reset() {
		shaderHandler.resetShader(shader);
		programID = GL_ID_NONE;
	}
	
protected:
	const RenderPassType type; //!< Type of this pass
	GLuint programID; //!< openGL program ID
	
	Renderer& renderer; //!< needed to render stuff
	ShaderHandler& shaderHandler; //!< provides program ID
	std::shared_ptr<TextureHandler> textureHandler; //!< needed for textures
	
	ShaderHandler::ShaderType shader; //!< which shader this pass uses
	
	std::vector<GLuint> uLocs; //!< the common locatins
	
	/**
	 * Initializes member variables
     * @param type
     * @param r
     * @param s
     * @param t
     */
	RenderPass(
		RenderPassType type, 
		Renderer& r, 
		ShaderHandler& s, 
		std::shared_ptr<TextureHandler> t
	) : type(type), programID(GL_ID_NONE),
		renderer(r), shaderHandler(s), textureHandler(t),
		shader(ShaderHandler::SIZE)
	{ 
	}
	
	/**
	 * Sets locations of the default uniforms
     */
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

