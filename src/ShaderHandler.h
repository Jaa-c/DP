/* 
 * File:   ShaderHandler.h
 * Author: jaa
 *
 * Created on 28. září 2013, 12:48
 */

#ifndef SHADERHANDLER_H
#define	SHADERHANDLER_H

#include <GL/glew.h>
#include <iostream>


class ShaderHandler {
public:
	/// list of avaiable shaders
	enum ShaderType {
		SHADER_BASIC = 0,
		SHADER_TEXTURING_1,
		SHADER_TEXTURING_2,
		SHADER_POINTS,
		SIZE,
	};
	
	
private:
	const std::string SHADER_PATH = "shaders/";
	/// shader file names
	const std::string SHADER_SRC[SIZE] = {
		"basic",
		"texturing_1",
		"texturing_2",
		"points"
	};
	
	/// Shader program ids
	GLuint g_ProgramId[SIZE];		
	
public:

	ShaderHandler() {
		resetShaders();
	}
	
	~ShaderHandler() {}
	
	
	GLuint getProgramId(ShaderType shader) {
		if(g_ProgramId[shader] == 0) {
			compileShaderProgram(shader, true, false, true); //TODO
		}
		return g_ProgramId[shader];
	}
	
	void resetShaders() {
		for(int i = 0; i < SIZE; i++) {
			g_ProgramId[i] = 0;
		}
	}

private:
	
	void compileShaderProgram(ShaderType shader, bool vert, bool geom, bool frag) {
		std::string name = SHADER_PATH + SHADER_SRC[shader];
		
		// Delete shader program if exists
		if (g_ProgramId[shader] != 0) {
			glDeleteProgram(g_ProgramId[shader]);
		}

		// Create shader program object
		g_ProgramId[shader] = glCreateProgram();
		std::string n;
		if (vert) {
			n = name + ".vert";
			Log::d("Compiling " + n + ": ");
			GLuint id = createShaderFromFile(GL_VERTEX_SHADER, n.c_str());
			glAttachShader(g_ProgramId[shader], id);
			glDeleteShader(id);
		}

		if (geom) {
			n = name + ".geom";
			Log::d("Compiling " + n + ": ");
			GLuint id = createShaderFromFile(GL_GEOMETRY_SHADER, n.c_str());
			glAttachShader(g_ProgramId[shader], id);
			glDeleteShader(id);
		}

		if (frag) {
			n = name + ".frag";
			Log::d("Compiling " + n + ": ");
			GLuint id = createShaderFromFile(GL_FRAGMENT_SHADER, n.c_str());
			glAttachShader(g_ProgramId[shader], id);
			glDeleteShader(id);
		}

		// Link shader program
		glLinkProgram(g_ProgramId[shader]);

		if (!checkProgramLinkStatus(g_ProgramId[shader])) {
			checkProgramInfoLog(g_ProgramId[shader]);
			Log::e("Shader program creation failed.");
			glDeleteProgram(g_ProgramId[shader]);
			g_ProgramId[shader]  = 0;
			return;
		} else {
			Log::i("Shader program compiled successfully.");
		}
	}
	
	GLuint createShaderFromSource(GLenum shader_type, const char* source) {
		if (source == NULL) {
			return 0;
		}
		std::string log;
		switch (shader_type) {
			case GL_VERTEX_SHADER: log = "vertex shader creation %s";
				break;
			case GL_FRAGMENT_SHADER: log = "fragment shader creation %s";
				break;
			case GL_GEOMETRY_SHADER: log = "geometry shader creation %s";
				break;
			default: 
				return 0;
		}

		GLuint shader_id = glCreateShader(shader_type);
		if (shader_id == 0) {
			return 0;
		}

		glShaderSource(shader_id, 1, &source, NULL);
		glCompileShader(shader_id);

		if (checkShaderCompileStatus(shader_id) != GL_TRUE) {
			Log::e(log, "failed");
			checkShaderInfoLog(shader_id);
			glDeleteShader(shader_id);
			return 0;
		} else {
			Log::d(log, "successfull.");
			return shader_id;
		}
	}

	GLuint createShaderFromFile(GLenum shader_type, const char* file_name) {
		if (file_name == NULL) {
			return 0;
		}

		char* buffer = NULL;

		// Read input file
		if (file_name != NULL) {
			FILE* fin = fopen(file_name, "rb");
			if (fin != NULL) {
				fseek(fin, 0, SEEK_END);
				long file_size = ftell(fin);
				rewind(fin);

				if (file_size > 0) {
					buffer = new char[file_size + 1];
					int count = fread(buffer, sizeof (char), file_size, fin);
					//              assert(count == file_size);
					buffer[count] = '\0';
				}
				fclose(fin);
			} else {
				return 0;
			}
		}

		assert(buffer != NULL);

		GLuint shader_id = createShaderFromSource(shader_type, buffer);
		delete [] buffer;

		return shader_id;
	}

	void checkProgramInfoLog(GLuint program_id) {
		if (program_id == 0) {
			return;
		}

		int log_length = 0;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

		if (log_length > 0) {
			char* buffer = new char[log_length];
			int written = 0;
			glGetProgramInfoLog(program_id, log_length, &written, buffer);
			printf("%s\n", buffer);
			delete [] buffer;
		}
	}
	
	GLint checkShaderCompileStatus(GLuint shader_id) {
		GLint status = GL_FALSE;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
		return status;
	}

	GLint checkProgramLinkStatus(GLuint program_id) {
		GLint status = GL_FALSE;
		glGetProgramiv(program_id, GL_LINK_STATUS, &status);
		return status;
	}

	void checkShaderInfoLog(GLuint shader_id) {
		if (shader_id == 0) {
			return;
		}

		int log_length = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

		if (log_length > 0) {
			char* buffer = new char[log_length];
			int written = 0;
			glGetShaderInfoLog(shader_id, log_length, &written, buffer);
			printf("%s\n", buffer);
			delete [] buffer;
		}
	}

};

#endif	/* SHADERHANDLER_H */
