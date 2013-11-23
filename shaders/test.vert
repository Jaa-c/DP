#version 330 compatibility

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat3 u_NormalMatrix;

in vec4 a_vertex;
//in vec3 a_normal;

out block {
	vec4 v_Position;
	vec3 v_ViewPos;
	vec3 v_Normal;
} Out;

void main () {
	
	vec4 viewPos = u_ModelViewMatrix * a_vertex;
	Out.v_Normal = u_NormalMatrix * gl_Normal;
	//Out.v_ViewPos = viewPos.xyz / viewPos.w;
	
	Out.v_Position =  u_ProjectionMatrix * viewPos;

	gl_Position = Out.v_Position;

}