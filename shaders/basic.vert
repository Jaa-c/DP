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
	smooth vec2 v_texCoords;
	flat   int	v_texIndex;
} Out;

void main () {
	
	Out.v_viewPos = u_ModelViewMatrix * vec4(a_position, 1.0);
	Out.v_normal = a_normal;
	Out.v_position =  u_ProjectionMatrix * Out.v_viewPos;
	
	TextureData data;
	Out.v_texIndex = 0;
	float bestDot = 0.0f;
	for(int i = 0; i < u_textureCount; i++) {
		data = ub_texData[i];
		float dotp = dot(a_normal, -vec3(data.u_TextureRt[0][2], data.u_TextureRt[1][2], data.u_TextureRt[2][2]));
		if(dotp > bestDot) {
			bestDot = dotp;
			Out.v_texIndex = i;
		}
	}
	Out.v_texIndex = 0;
	data = ub_texData[Out.v_texIndex];

	vec3 coords = (data.u_TextureRt * vec4(a_position, 1.0f)).xyz;
	Out.v_texCoords.x = -coords.x/coords.z * data.u_TextureFL + data.u_TextureSize.x * 0.5f;
	Out.v_texCoords.y = coords.y/coords.z * data.u_TextureFL + data.u_TextureSize.y * 0.5f;

	gl_Position = Out.v_position;
}