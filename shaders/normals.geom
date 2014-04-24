#version 400 core

layout(triangles) in;
layout(line_strip, max_vertices=6) out;

uniform mat4 u_ProjectionMatrix;

in block {
	smooth vec4 v_viewPos;
	smooth vec3 v_normal;
	smooth vec3 v_origNormal;
} In[];

void main() {
	for(int i=0; i<gl_in.length(); i++) {
		gl_Position = u_ProjectionMatrix * In[i].v_viewPos;
		EmitVertex();

		gl_Position = u_ProjectionMatrix * vec4(In[i].v_viewPos.xyz + In[i].v_normal * 1.f, 1.0);
		EmitVertex();
		EndPrimitive();
	}
}


