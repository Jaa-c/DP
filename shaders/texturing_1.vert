#version 400 core

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat3 u_NormalMatrix;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_textureCoords;

out block {
	smooth vec4 v_position;
	smooth vec4 v_viewPos;
	smooth vec3 v_normal;
} Out;

void main () {
	Out.v_position =  vec4(a_position, 1.0f);
	Out.v_viewPos = u_ModelViewMatrix * Out.v_position;
	Out.v_normal = u_NormalMatrix * a_normal;

	gl_Position = u_ProjectionMatrix * Out.v_viewPos;
}