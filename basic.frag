#version 330 core
#extension GL_ARB_texture_rectangle : enable

in block{
	vec4 v_position;
	vec4 v_viewPos;
	vec3 v_normal;
	vec2 v_texCoords;
} In;

uniform sampler2DRect texture0;

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

	vec3 col = texture2DRect(texture0, In.v_texCoords).rgb;
	//col = vec3(0.6f, 0.6f, 0.6f);
	
	vec3 color = min(col * diffuse + specular, 1.0f);
	
	a_FragColor = vec4(color, 1.0f);

}