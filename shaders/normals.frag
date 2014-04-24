#version 400 core

in vec4 v_color;

layout(location = 0) out vec4 a_FragColor;

void main() {
	a_FragColor = v_color;
}