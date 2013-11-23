#version 330 compatibility

layout(location=0) in vec3 v_position;

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;

void main(void) {
    gl_PointSize = 2.0f;
    gl_Position = u_ProjectionMatrix * u_ModelViewMatrix * vec4(v_position, 1.0f);
}
