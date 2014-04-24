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
		v_color = vec4(0.f, 0.f, 0.f, 1.f);
		EmitVertex();

		gl_Position = mvp * vec4(In[i].v_position.xyz + In[i].v_normal, 1.0f);
		v_color = vec4(1.f, 1.f, 1.f, 1.f);
		EmitVertex();
		EndPrimitive();
	}

	vec3 V0 = In[0].v_position.xyz - In[1].v_position.xyz;
	vec3 V1 = In[2].v_position.xyz - In[1].v_position.xyz;
	vec3 N = normalize(cross(V1, V0));
	vec3 P = (In[0].v_position.xyz+In[1].v_position.xyz+In[2].v_position.xyz) / 3.0f;

	gl_Position = mvp * vec4(P, 1.0f);
	v_color = vec4(N, 1.f);
	EmitVertex();

	gl_Position = mvp * vec4(P + N, 1.0f);
	v_color = vec4(N, 1.f);
	EmitVertex();
	EndPrimitive();
}


