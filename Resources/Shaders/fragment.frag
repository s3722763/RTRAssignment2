#version 460 core

#define MAX_LIGHTS 10

in vec2 texCoord;
in vec3 normal;
in vec3 pos;

out vec4 color;

uniform vec3 viewPos;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

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

vec4 getSpecular() {
	vec4 result = vec4(0);

	for (uint i = uint(0); i < lightData.numberLights; i++) {
		vec3 lightDir = normalize(lightData.lights.positions[i].xyz - pos);

		vec3 viewDir = normalize(viewPos - pos);
		vec3 halfwayDir = normalize(lightDir + viewDir);

		// TODO: Change shininess from 64 to material value
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
		vec4 specular = (texture(specularTexture, texCoord) * spec) * lightData.lights.speculars[i];

		result += specular;
	}

	return result;
}

void main() {
	vec4 ambient = texture(diffuseTexture, texCoord) * lightData.ambientStrength;
	vec4 diffuse = getDiffuse();
	vec4 specular = getSpecular();
	color = diffuse + ambient + specular;
}