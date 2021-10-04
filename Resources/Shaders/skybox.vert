#version 460 core
layout (location = 0) in vec3 vertexPos;

out vec3 texCoord;

uniform mat4 viewProj;

void main() {
	texCoord = vertexPos;
	
	vec4 pos = viewProj * vec4(vertexPos, 1.0);
	gl_Position = pos.xyww;
}