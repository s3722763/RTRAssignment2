#version 460 core

layout (location = 0) in vec3 positionVert;
layout (location = 1) in vec2 texCoordVert;
layout (location = 2) in vec3 normalVert;

out vec2 texCoord;
out vec3 normal;
out vec3 pos;

uniform mat4 viewProj;
uniform mat4 model;

void main () {
	vec4 adjustedPos = viewProj * model * vec4(positionVert, 1.0);
	gl_Position = adjustedPos;

	texCoord = texCoordVert;
	normal = normalVert;
	pos = positionVert;
}