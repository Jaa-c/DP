#version 400 core
#extension GL_ARB_texture_rectangle : enable

#define MAX_TEXTURES 32

struct TextureData {
	mat4	u_Rt;			//matice R|t
	vec3	u_cameraViewDir;//opraveny smer kamery
	ivec2	u_textureSize;	//rozmery fotografie
	float	u_textureFL;	//ohniskova vzdalenost
	float	u_coveredArea;	//normalizovana plocha
};
layout(std140) uniform u_textureDataBlock {
	TextureData ub_texData[MAX_TEXTURES];
};
uniform int u_textureCount;
uniform int u_texuresBasic;

uniform sampler2DRect u_texture0[MAX_TEXTURES];

uniform vec3 u_viewDir;
uniform mat3 u_NormalMatrix;

in block {
	smooth vec4 v_position;
	smooth vec3 v_normal;
} In;

layout(location = 0) out vec4 a_FragColor;

const float dirLimit = 0.4; //cos(a), 0 = perpendicular, 1 = same direction
// -----------

void projectCoords(in int index, in vec4 pos, out vec2 coords) {
	TextureData data = ub_texData[index];
	vec3 c = (data.u_Rt * pos).xyz;
	coords =  c.xy/c.z * data.u_textureFL + data.u_textureSize.xy * 0.5f;
}

bool inRange(in int index, in vec2 coords) {
	ivec2 s = ub_texData[index].u_textureSize;
	return coords.x >= 0 && coords.x < s.x && coords.y >= 0 && coords.y < s.y; 
}

float computeWeight(in int index, in vec3 N, out vec2 coords, in float dl = dirLimit) {
	TextureData data = ub_texData[index];
	float weight = data.u_coveredArea;

	projectCoords(index, In.v_position, coords);
	weight *= float(inRange(index, coords));

	float dirDiff = dot(N, data.u_cameraViewDir);
	weight *= -(dirDiff + 1.f) / (1.f - dl) + 1;
	return weight;
}

void main() {

	vec3 N = normalize(In.v_normal);
	vec3 L = normalize(-u_viewDir);
	float diffuse = min(dot(N, L), 1.0f);
	
	vec2 coords;
	float weight;
	vec3 bestCoords = vec3(.0, -1.0f, -1.0f);
	float bestWeight = 0.0f;

	//use only the inital photos for texturing
	for(int i = 0; i < u_texuresBasic; ++i) {
		weight = computeWeight(i, N, coords);
		weight *= dot(u_viewDir, ub_texData[i].u_cameraViewDir);
		if(weight > bestWeight) {
			bestWeight = weight;
			bestCoords.x = i;
			bestCoords.yz = coords;
		}
	}

	//now fill empty areas with additional textures
	if(bestWeight == 0) { //this condition is neccessary :(
		for(int i = u_texuresBasic; i < u_textureCount; ++i) {
			weight = computeWeight(i, N, coords);
			if(weight > bestWeight) {
				bestWeight = weight;
				bestCoords.x = i;
				bestCoords.yz = coords;
			}
		}
	}

	//now, depending on the number of textures, there can still be empty spaces
	//we don't want that, even if we can't texture them properly
	//let's just texture them with what we have, 
	if(bestWeight == 0) {
		for(int i = 0; i < u_textureCount; ++i) {
			weight = computeWeight(i, N, coords, 0.0f);
			if(weight > bestWeight) {
				bestWeight = weight;
				bestCoords.x = i;
				bestCoords.yz = coords;
			}
		}
	}
	
	vec3 tex = texture2DRect(u_texture0[int(bestCoords.x)], bestCoords.yz).rgb;
	vec3 color = min(vec3(.2f, .2f, .2f) * diffuse + .2f, 1.0f); //mat
	if(length(tex) != 0) {
		color = tex * (color + 0.6f) + .2f;
	}
	//if(ub_texData[int(bestCoords.x)].u_textureSize == 512) color.b += .4f;
	a_FragColor = vec4(color, 1.0f);
}