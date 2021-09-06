#version 460 core

in vec2 texCoord;

out vec4 color;

uniform sampler2D diffuseTexture;

void main() {
	color = texture(diffuseTexture, texCoord);
}