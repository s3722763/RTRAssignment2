#version 460 core

layout (location  = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 worldPosition;
layout (location = 2) in vec4 color;

out vec4 vecColor;

uniform mat4 viewProj;
uniform mat4 view;

void main() {
	vec3 cameraRight = {view[0][0], view[1][0], view[2][0]};
	vec3 cameraUp = {view[0][1], view[1][1], view[2][1]};

	vec3 pos = worldPosition + (cameraRight *  vertexPosition.x) + (cameraUp * vertexPosition.y);

	gl_Position = viewProj * vec4(pos, 1.0);
	vecColor = color;
}