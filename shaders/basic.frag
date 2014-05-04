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
uniform vec3 u_viewDir;

uniform sampler2DRect u_texture0[32];

in block {
	smooth vec4 v_position;
	smooth vec4 v_viewPos;
	smooth vec3 v_normal;
	flat   int	v_texIndex;
} In;

layout(location = 0) out vec4 a_FragColor;

// -----------

struct Pair {
	int i;
	float dotp;
};

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

	vec3 E = normalize(-In.v_viewPos.xyz);
	vec3 R = normalize(-reflect(L, N));
	
	float specular = pow(max(dot(R, E), 0.0f), 256.0f);

	vec2 coords;
	projectCoords(In.v_texIndex, In.v_position, coords);
	
	vec3 col = texture2DRect(u_texture0[In.v_texIndex], coords).rgb;
	
	if(!inRange(In.v_texIndex, coords)) {
		Pair p[32];
		for(int i = 0; i < u_textureCount; ++i) {
			TextureData data = ub_texData[i];
			vec3 camDir = vec3(data.u_TextureRt[0][2], data.u_TextureRt[1][2], data.u_TextureRt[2][2]);
			p[i].dotp = dot(In.v_normal, camDir);
			p[i].i = i;
		}

		for (int n = u_textureCount; n > 0; --n) {
			for (int i = 0; i < n; ++i) {
				if(p[i].dotp < p[i+1].dotp) { //ugly stuff
					Pair tmp = p[i];
					p[i] = p[i+1];
					p[i+1] = tmp;
				}
			}
		}
		
		int index = 0;
		do {
			index++;
			projectCoords(p[index].i, In.v_position, coords);
		}
		while(index < u_textureCount && !inRange(p[index].i, coords));

		col = texture2DRect(u_texture0[p[index].i], coords).rgb;
	}

	vec3 color = min(vec3(.2f, .2f, .2f) * diffuse + .2f, 1.0f); //mat
	if(length(col) != 0) {
		color = col * (color + 0.6f) + .2f;
	}

	a_FragColor = vec4(color, 1.0f);
}