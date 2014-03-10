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

in block {
	smooth vec4 v_position;
	smooth vec4 v_viewPos;
	smooth vec3 v_normal;
	smooth vec2 v_texCoords;
	flat   int	v_texIndex;
} In;

uniform sampler2DRect u_texture0[32];
uniform ivec2	u_TextureSize;

layout(location = 0) out vec4 a_FragColor;

void main() {

	vec4 light_pos = vec4(0.0f, .0f, -3.0f, 1.0f);
	vec3 lp = light_pos.xyz;

	vec3 N = normalize(In.v_normal);
	vec3 L = normalize(lp - In.v_viewPos.xyz);
	float diffuse = max(dot(N, L), 0.5f);

	vec3 E = normalize(-In.v_viewPos.xyz);
	vec3 R = normalize(-reflect(L, N));
	
	float specular = pow(max(dot(R, E), 0.0f), 256.0f);

	TextureData data = ub_texData[In.v_texIndex];
	vec2 c;

	vec3 coords = (data.u_TextureRt * In.v_position).xyz;
	c.x = -coords.x/coords.z * data.u_TextureFL + data.u_TextureSize.x * 0.5f;
	c.y =  coords.y/coords.z * data.u_TextureFL + data.u_TextureSize.y * 0.5f;

	vec3 col = texture2DRect(u_texture0[In.v_texIndex], c).rgb;
	
	vec3 color = min(col * diffuse + specular * .3f, 1.0f);

	//color.r = c.x / 2400;
	//color.g = c.y / 3200;
	//color.b = 0;
	if(In.v_texIndex == 0) color.r += 0.2f;
	if(In.v_texIndex == 1) color.g += 0.2f;
		
	a_FragColor = vec4(color, 1.0f);

}