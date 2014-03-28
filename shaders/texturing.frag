#version 400 core
#extension GL_ARB_texture_rectangle : enable

struct TextureData {
	mat4	u_TextureRt;
	ivec2	u_TextureSize;
	float	u_TextureFL;
};
layout(std140) uniform u_textureDataBlock {
	TextureData ub_texData[32];
};
uniform int u_textureCount;

uniform int u_textureIndices[32];
uniform sampler2DRect u_texture0[32];

uniform vec3 u_viewDir; //debug
uniform mat3 u_NormalMatrix;

in block {
	smooth vec4 v_position;
	smooth vec4 v_viewPos;
	smooth vec3 v_normal;
} In;

layout(location = 0) out vec4 a_FragColor;

// -----------

void projectCoords(in int index, in vec4 pos, out vec2 coords) {
	TextureData data = ub_texData[index];
	vec3 c = (data.u_TextureRt * pos).xyz;
	coords =  c.xy/c.z * data.u_TextureFL + data.u_TextureSize.xy * 0.5f;
}

bool inRange(in int index, in vec2 coords) {
	ivec2 s = ub_texData[index].u_TextureSize;
	return coords.x >= 0 && coords.x < s.x && coords.y > 0 && coords.y < s.y; 
}

void main() {

	vec4 light_pos = vec4(0.0f, .0f, -3.0f, 1.0f);
	vec3 lp = light_pos.xyz;

	vec3 N = normalize(In.v_normal);
	vec3 L = normalize(lp - In.v_viewPos.xyz);
	float diffuse = max(dot(N, L), 0.8f);

	vec3 E = normalize(-In.v_viewPos.xyz);
	vec3 R = normalize(-reflect(L, N));
	
	float specular = pow(max(dot(R, E), 0.0f), 256.0f);

	const float dirLimit = 0.4; //0 = perpendicular, 1 = same direction
	vec2 coords;
	float weight;
	
	vec3 bestCoords = vec3(0.0, -1.0f, -1.0f);
	float bestWeight = 0.0f;
	for(int i = 0; i < u_textureCount; ++i) {
		weight = 1.f / float(i); //TODO

		projectCoords(u_textureIndices[i], In.v_position, coords);
		weight *= float(inRange(u_textureIndices[i], coords));

		mat4 Rt = ub_texData[u_textureIndices[i]].u_TextureRt;
		float dirDiff = dot(N, normalize(u_NormalMatrix *  vec3(Rt[0][2], Rt[1][2], Rt[2][2])));
		weight *= -(dirDiff + 1.f) / (1.f - dirLimit) + 1;

		if(weight > bestWeight) { //TODO
			bestWeight = weight;
			bestCoords.x = i;
			bestCoords.yz = coords;
		}
	}

	vec3 col = texture2DRect(u_texture0[u_textureIndices[int(bestCoords.x)]], bestCoords.yz).rgb;
	vec3 color = min((.2f + col) * diffuse + specular * .2f, 1.0f);

	if(bestWeight == 0) color.r = 1;

	a_FragColor = vec4(color, 1.0f);
}