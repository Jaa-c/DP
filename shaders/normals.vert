#version 400 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

out block {
	smooth vec4 v_position;
	smooth vec3 v_normal;
} Out;

void main () {
	Out.v_position = vec4(a_position, 1.0f);
	Out.v_normal = normalize(a_normal);
}