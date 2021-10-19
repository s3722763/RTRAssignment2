#version 460 core

layout (location = 0) in vec3 positionVert;

uniform mat4 model;

void main() {
	gl_Position = model * vec4(positionVert, 1.0);
}