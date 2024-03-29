#version 460 core
in vec4 fragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main() {
	float lightDistance = length(fragPos.xyz - lightPos);

	lightDistance = lightDistance / far_plane;

	gl_FragDepth = lightDistance;
}