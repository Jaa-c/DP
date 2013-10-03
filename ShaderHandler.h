/* 
 * File:   ShaderHandler.h
 * Author: jaa
 *
 * Created on 28. září 2013, 12:48
 */

#ifndef SHADERHANDLER_H
#define	SHADERHANDLER_H

#include <iostream>

class ShaderHandler {
private:
	GLuint g_ProgramId;		// Shader program id
	
	const std::string SHADER_TEST_SRC = "test";
	
public:

	enum ShaderList {
		SHADER_TEST,
	};
	
	ShaderHandler() {
		g_ProgramId = 0;
	}
	
	~ShaderHandler() {}
	
	void compileShaderProgram(ShaderList shader, bool vert, bool geom, bool frag) {
		std::string name;
		switch(shader) {
			case SHADER_TEST:
				name = SHADER_TEST_SRC;
				break;
		}
		
		// Delete shader program if exists
		if (g_ProgramId) {
			glDeleteProgram(g_ProgramId);
		}

		// Create shader program object
		g_ProgramId = glCreateProgram();

		if (vert) {
			std::string n = name + ".vert";
			GLuint id = createShaderFromFile(GL_VERTEX_SHADER, n.c_str());
			glAttachShader(g_ProgramId, id);
			glDeleteShader(id);
		}

		if (geom) {
			std::string n = name + ".geom";
			GLuint id = createShaderFromFile(GL_GEOMETRY_SHADER, n.c_str());
			glAttachShader(g_ProgramId, id);
			glDeleteShader(id);
		}

		if (frag) {
			std::string n = name + ".frag";
			GLuint id = createShaderFromFile(GL_FRAGMENT_SHADER, n.c_str());
			glAttachShader(g_ProgramId, id);
			glDeleteShader(id);
		}

		// Link shader program
		glLinkProgram(g_ProgramId);

		if (!checkProgramLinkStatus(g_ProgramId)) {
			checkProgramInfoLog(g_ProgramId);
			printf("Shader program creation failed.\n\n");
			glDeleteProgram(g_ProgramId);
			g_ProgramId  = 0;
			return;
		} else {
			printf("Shader program compiled successfully.\n\n");
		}
	}
	
	GLuint getProgramId() {
		return g_ProgramId;
	}

private:
	
	GLuint createShaderFromSource(GLenum shader_type, const char* source) {
		if (source == NULL) {
			return 0;
		}

		switch (shader_type) {
			case GL_VERTEX_SHADER: std::cout << "vertex shader creation ... ";
				break;
			case GL_FRAGMENT_SHADER: std::cout << "fragment shader creation ... ";
				break;
			case GL_GEOMETRY_SHADER: std::cout << "geometry shader creation ... ";
				break;
			default: return 0;
		}

		GLuint shader_id = glCreateShader(shader_type);
		if (shader_id == 0) {
			return 0;
		}

		glShaderSource(shader_id, 1, &source, NULL);
		glCompileShader(shader_id);

		if (checkShaderCompileStatus(shader_id) != GL_TRUE) {
			std::cout << "failed.\n";
			checkShaderInfoLog(shader_id);
			glDeleteShader(shader_id);
			return 0;
		} else {
			std::cout << "successfull.\n";
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
