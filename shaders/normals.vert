#version 400 core

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat3 u_NormalMatrix;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

out block {
	smooth vec4 v_viewPos;
	smooth vec3 v_normal;
	smooth vec3 v_origNormal;
} Out;

void main () {
	Out.v_viewPos = u_ModelViewMatrix * vec4(a_position, 1.0f);
	Out.v_normal = u_NormalMatrix * a_normal;
	Out.v_origNormal = a_normal;
}