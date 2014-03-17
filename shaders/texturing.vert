#version 400 core

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat3 u_NormalMatrix;

struct TextureData {
	mat4	u_TextureRt;
	ivec2	u_TextureSize;
	float	u_TextureFL;
};
layout(std140) uniform u_textureDataBlock {
	TextureData ub_texData[32];
};
uniform int u_textureCount;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_textureCoords;

out block {
	smooth vec4 v_position;
	smooth vec4 v_viewPos;
	smooth vec3 v_normal;
	//flat   int	v_texIndex;
} Out;

void main () {
	
	Out.v_position =  vec4(a_position, 1.0f);
	Out.v_viewPos = u_ModelViewMatrix * Out.v_position;
	Out.v_normal = a_normal;
	/*
	TextureData data;
	Out.v_texIndex = 0;
	float bestDot = -1.0f;
	for(int i = 0; i < u_textureCount; ++i) {
		data = ub_texData[i];
		vec3 camDir = vec3(data.u_TextureRt[0][2], data.u_TextureRt[1][2], data.u_TextureRt[2][2]);
		float dotp = dot(a_normal, camDir);
		if(dotp > bestDot) {
			bestDot = dotp;
			Out.v_texIndex = i;
		}
	}*/

	gl_Position = u_ProjectionMatrix * Out.v_viewPos;
}