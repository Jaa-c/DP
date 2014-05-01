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

uniform sampler2DRect u_texture0[32];

uniform vec3 u_viewDir;
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

	vec3 N = normalize(In.v_normal);
	vec3 L = normalize(-u_viewDir);
	float diffuse = min(dot(N, L), 1.0f);

	const float dirLimit = 0.4; //0 = perpendicular, 1 = same direction
	vec2 coords;
	float weight;
	
	vec3 bestCoords = vec3(.0, -1.0f, -1.0f);
	float bestWeight = 0.0f;
	for(int i = 0; i < u_textureCount; ++i) {
		weight = 1.f / (i + .1f);

		projectCoords(i, In.v_position, coords);
		weight *= float(inRange(i, coords));

		mat4 Rt = ub_texData[i].u_TextureRt;
		float dirDiff = dot(N, normalize(u_NormalMatrix *  vec3(Rt[0][2], Rt[1][2], Rt[2][2])));
		weight *= -(dirDiff + 1.f) / (1.f - dirLimit) + 1;

		if(weight > bestWeight) { //TODO
			bestWeight = weight;
			bestCoords.x = i;
			bestCoords.yz = coords;
		}
	}
	
	vec3 tex = texture2DRect(u_texture0[int(bestCoords.x)], bestCoords.yz).rgb;
	vec3 color = min(vec3(.2f, .2f, .2f) * diffuse + .2f, 1.0f); //mat
	if(length(tex) != 0) {
		color = tex * (color + 0.6f) + .2f;
	}

	//if(ub_texData[int(bestCoords.x)].u_TextureSize.x == 512) color.b = 0.4;
	//if(bestWeight == 0) color.r = 1;

	
	a_FragColor = vec4(color, 1.0f);
	//a_FragColor = vec4(-N, 1.0f);
}