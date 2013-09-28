#version 330 core

uniform mat4 u_ModelViewMatrix;

in block{
	vec4 v_Position;
	vec3 v_ViewPos;
	vec3 v_Normal;
} In;

layout(location = 0) out vec4 a_FragColor;

void main() {

	vec4 light_pos = vec4(0.0f, .0f, 3.0f, 1.0f);
	vec3 lp = light_pos.xyz;

	vec3 N = normalize(In.v_Normal);
	vec3 L = normalize(lp - In.v_ViewPos);
	float diffuse = max(dot(N, L), 0.0);

	//vec3 E = normalize(-v_Vertex);
	//vec3 R = normalize(-reflect(L, N));
	
	//float specular = pow(max(dot(R, E), 0.0), 32.0);
	
	vec3 color = min(vec3(0.2f, 0.2f, 0.8f) * diffuse + 0, 1.0f);
	
	a_FragColor = vec4(color, 1.0);

}