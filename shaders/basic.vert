#version 330 core

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat3 u_NormalMatrix;

struct TextureData {
	mat4	u_TextureRt;
	ivec2	u_TextureSize;
	float	u_TextureFL;
};

uniform int u_textureCount;

layout(std140) uniform u_textureDataBlock {
	TextureData ub_texData[32];
};

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_textureCoords;

out block {
	smooth vec4 v_position;
	smooth vec4 v_viewPos;
	smooth vec3 v_normal;
	smooth vec2 v_texCoords;
} Out;

void main () {
	
	Out.v_viewPos = u_ModelViewMatrix * vec4(a_position, 1.0);
	Out.v_normal = a_normal;
	Out.v_position =  u_ProjectionMatrix * Out.v_viewPos;
	//Out.v_texCoords = a_textureCoords;
	
	TextureData data = ub_texData[0]; //tmp

	vec3 coords = (data.u_TextureRt * vec4(a_position, 1.0f)).xyz;
	coords = -coords / coords.z;
	Out.v_texCoords.x = coords.x * data.u_TextureFL + data.u_TextureSize.x * 0.5f;
	Out.v_texCoords.y = -coords.y * data.u_TextureFL + data.u_TextureSize.y * 0.5f;

	gl_Position = Out.v_position;
}