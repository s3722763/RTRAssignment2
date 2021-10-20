#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

flat in float[] vertexMaterialIndex;

out vec3 pos;

in VS_OUT {
	vec3 worldPosition;
} gs_in[];

out vec3 normal;

vec3 calculateNormal() {
	vec3 a = vec3(gs_in[1].worldPosition) - vec3(gs_in[0].worldPosition);
	vec3 b = vec3(gs_in[2].worldPosition) - vec3(gs_in[0].worldPosition);
	return normalize(cross(a, b));
}

void main() {
	normal = calculateNormal();

	pos = gs_in[0].worldPosition;
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	//gs_out.materialIndex = vertexMaterialIndex[1];
	pos = gs_in[1].worldPosition;
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	//gs_out.materialIndex = vertexMaterialIndex[2];
	pos = gs_in[2].worldPosition;
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}