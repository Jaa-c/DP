#version 330 core

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat3 u_NormalMatrix;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_textureCoords;

out block {
	vec4 v_position;
	vec4 v_viewPos;
	vec3 v_normal;
	vec2 v_texCoords;
} Out;

void main () {
	
	Out.v_viewPos = u_ModelViewMatrix * vec4(a_position, 1.0);
	Out.v_normal = a_normal;
	Out.v_position =  u_ProjectionMatrix * Out.v_viewPos;
	Out.v_texCoords = a_textureCoords;

	gl_Position = Out.v_position;
}