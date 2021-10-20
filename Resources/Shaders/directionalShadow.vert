#version 460 core

layout (location = 0) in vec3 positionVert;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
	gl_Position = lightSpaceMatrix * model * vec4(positionVert, 1.0);
}