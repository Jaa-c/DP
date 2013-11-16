#version 330 compatibility

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat3 u_NormalMatrix;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_textureCoords;

out block {
	vec4 v_position;
	vec3 v_viewPos;
	vec3 v_normal;
} Out;

void main () {
	
	vec4 viewPos = u_ModelViewMatrix * a_vertex;
	Out.v_Normal = u_NormalMatrix * gl_Normal;
	Out.v_Position =  u_ProjectionMatrix * viewPos;

	gl_Position = Out.v_Position;
}