#version 400 core
#extension GL_ARB_texture_rectangle : enable

struct TextureData {
	mat4	u_TextureRt;
	vec3	u_cameraViewDir;
	ivec2	u_TextureSize;
	float	u_TextureFL;
	float	u_coveredArea; //uninitialized here!!
};
layout(std140) uniform u_textureDataBlock {
	TextureData ub_texData[32];
};
uniform int u_textureCount;
uniform int u_textureIndices[32];

uniform vec3 u_viewDir;
uniform mat3 u_NormalMatrix;

in block {
	smooth vec4 v_position;
	smooth vec4 v_viewPos;
	smooth vec3 v_normal;
	smooth vec3 v_origNormal;
} In;

layout(location = 0) out vec3 a_data;

const float dirLimit = 0.4; //cos(a), 0 = perpendicular, 1 = same direction
// -----------

void projectCoords(in int index, in vec4 pos, out vec2 coords) {
	TextureData data = ub_texData[index];
	vec3 c = (data.u_TextureRt * pos).xyz;
	coords =  c.xy/c.z * data.u_TextureFL + data.u_TextureSize.xy * 0.5f;
}

bool inRange(in int index, in vec2 coords) {
	ivec2 s = ub_texData[index].u_TextureSize;
	return coords.x >= 0 && coords.x < s.x && coords.y >= 0 && coords.y < s.y; 
}

void main() {

	vec3 N = normalize(In.v_normal);
	vec2 coords;
	float weight;
	vec3 bestCoords = vec3(.0, -1.0f, -1.0f);
	float bestWeight = 0.0f;
	for(int i = 0; i < u_textureCount; ++i) {
		weight = 1.f;

		projectCoords(i, In.v_position, coords);
		weight *= float(inRange(i, coords));

		float dirDiff = dot(N, ub_texData[i].u_cameraViewDir);
		weight *= -(dirDiff + 1.f) / (1.f - dirLimit) + 1;
		
		if(weight > 0) {
			bestWeight = weight;
			break; // not that it matters...
		}
	}

	a_data = vec3(0.0f, 0.0f, 0.0f);

	if(bestWeight == 0) {
		a_data = normalize(In.v_origNormal);//N
	}
}