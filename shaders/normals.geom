#version 400 core

layout(triangles) in;
layout(line_strip, max_vertices=8) out;

uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;

in block {
	smooth vec4 v_position;
	smooth vec3 v_normal;
} In[];

out vec4 v_color;

void main() {
	mat4 mvp = u_ProjectionMatrix * u_ModelViewMatrix;
	for(int i=0; i<gl_in.length(); i++) {
		vec4 p = mvp * In[i].v_position;
		gl_Position = p;
		v_color = vec4(0, 0, 0, 1);
		EmitVertex();

		gl_Position = mvp * vec4(In[i].v_position.xyz + In[i].v_normal, 1.0);
		v_color = vec4(1, 1, 1, 1);
		EmitVertex();
		EndPrimitive();
	}

	
	//------ One line for the face normal
	vec3 P0 = In[0].v_position.xyz;
	vec3 P1 = In[1].v_position.xyz;
	vec3 P2 = In[2].v_position.xyz;
	vec3 V0 = P0 - P1;
	vec3 V1 = P2 - P1;
	vec3 N = normalize(cross(V1, V0));
	vec3 P = (P0+P1+P2) / 3.0;

	gl_Position = mvp * vec4(P, 1.0);
	v_color = vec4(1, 0, 0, 1);
	EmitVertex();

	gl_Position = mvp * vec4(P + N, 1.0);
	v_color = vec4(1, 1, 1, 1);
	EmitVertex();
	EndPrimitive();
}


