#version 330 core

layout(location=0) out vec4 v_color;

uniform vec3 u_color;

void main(void) { 
    v_color =  vec4(u_color, 1.0f);
}
    