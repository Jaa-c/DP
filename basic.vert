#version 330 core

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat3 u_NormalMatrix;

uniform mat3	u_TextureRot;
uniform vec3	u_TextureTrans;
uniform vec2	u_TextureSize;
uniform float	u_TextureFL;

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
	//Out.v_texCoords = a_textureCoords;

	vec3 coords =  u_TextureRot * a_position + u_TextureTrans;
	coords /= coords.z;
	Out.v_texCoords.x = coords.x * u_TextureFL + u_TextureSize.x * 0.5f;
	Out.v_texCoords.y = coords.y * u_TextureFL + u_TextureSize.y * 0.5f;

	gl_Position = Out.v_position;
}