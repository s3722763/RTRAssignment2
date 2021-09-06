#version 460 core

layout (location = 0) in vec3 positionVert;
layout (location = 1) in vec2 texCoordVert;

out vec2 texCoord;

uniform mat4 viewProj;

void main () {
	vec4 adjustedPos = viewProj * vec4(positionVert, 1.0);
	gl_Position = adjustedPos;

	texCoord = texCoordVert;
}