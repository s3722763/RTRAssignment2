#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 texCoord;
in vec3 normal;
in vec3 pos;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

void main() {
	gPosition = pos;
	gNormal = normalize(normal);
	gAlbedoSpec.rgb = texture(diffuseTexture, texCoord).rgb;
	// Specular intensity
	gAlbedoSpec.a = texture(specularTexture, texCoord).r;
}
