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

in block {
	smooth vec4 v_position;
	smooth vec4 v_viewPos;
	smooth vec3 v_normal;
	//flat   int	v_texIndex;
} In;

layout(location = 0) out vec4 a_FragColor;

// -----------

void projectCoords(in int index, in vec4 pos, out vec2 coords) {
	TextureData data = ub_texData[index];
	vec3 c = (data.u_TextureRt * pos).xyz;
	coords.x = -c.x/c.z * data.u_TextureFL + data.u_TextureSize.x * 0.5f;
	coords.y =  c.y/c.z * data.u_TextureFL + data.u_TextureSize.y * 0.5f;
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
	float diffuse = max(dot(N, L), 0.5f);

	vec3 E = normalize(-In.v_viewPos.xyz);
	vec3 R = normalize(-reflect(L, N));
	
	float specular = pow(max(dot(R, E), 0.0f), 256.0f);

	vec2 coords;
	int index = -1;
	do {
		index++;
		projectCoords(u_textureIndices[index], In.v_position, coords);
	} while(index + 1 < u_textureCount && !inRange(u_textureIndices[index], coords));

	vec3 col = texture2DRect(u_texture0[u_textureIndices[index]], coords).rgb;
	vec3 color = min(col * diffuse + specular * .3f, 1.0f);

	//color.r = coords.x / 2400;
	//color.g = coords.y / 3200;
	//color.b = 0;
	//if(In.v_texIndex == 0) color.r += .2f;
	//if(In.v_texIndex == 1) color.g += .2f;
		
	a_FragColor = vec4(color, 1.0f);
}