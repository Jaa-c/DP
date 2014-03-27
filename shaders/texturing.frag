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

	vec2 coords;
	int index = -1;
	float dotProd;
	do {
		index++;
		projectCoords(u_textureIndices[index], In.v_position, coords);
		//mat4 Rt = ub_texData[u_textureIndices[index]].u_TextureRt;
		//dotProd = dot(N, vec3(Rt[2][0], Rt[2][1], Rt[2][2]));
	} while(
		//dotProd > -1 &&
		index + 1 < u_textureCount && 
		!inRange(u_textureIndices[index], coords)
	);

	vec3 col = texture2DRect(u_texture0[u_textureIndices[index]], coords).rgb;
	vec3 color = min((.2f + col) * diffuse + specular * .3f, 1.0f);

	mat4 Rt = ub_texData[u_textureIndices[index]].u_TextureRt;
	
	vec3 camDir = u_NormalMatrix *  vec3(Rt[0][2], Rt[1][2], Rt[2][2]);

	dotProd = dot(N, normalize(camDir));
	if(dotProd > -0.5)
		color.r = 1;
	//color.r = (1 + dotProd) /2.f;
	//color.g = (1 + dotProd) /2.f;
	//color.b = (1 + dotProd) /2.f;
	
	//color = N;

	a_FragColor = vec4(color, 1.0f);
}