#version 460 core

#define MAX_LIGHTS 10

in vec2 texCoord;
in vec3 normal;
in vec3 pos;

out vec4 color;

uniform sampler2D diffuseTexture;

struct Lights {
	vec4[MAX_LIGHTS] positions;
	vec4[MAX_LIGHTS] ambients;
	vec4[MAX_LIGHTS] diffuses;
	vec4[MAX_LIGHTS] speculars;
	vec4[MAX_LIGHTS] directions;
};

layout (std140) uniform LightData {
	Lights lights;
	float ambientStrength;
	uint numberLights;
} lightData;

vec4 getDiffuse() {
	vec4 result = vec4(0);
	
	for (uint i = uint(0); i < lightData.numberLights; i++) {
		// Right now only supports point lights
		vec3 lightDir = normalize(lightData.lights.positions[i].xyz - pos);

		float diff = max(dot(normal, lightDir), 0.0);
		vec4 diffuse = (diff  * texture(diffuseTexture, texCoord)) * lightData.lights.diffuses[i];
		result += diffuse;
	}

	return result;
}

void main() {
	vec4 ambient = texture(diffuseTexture, texCoord) * lightData.ambientStrength;
	vec4 diffuse = getDiffuse();
	color = diffuse + ambient;
}