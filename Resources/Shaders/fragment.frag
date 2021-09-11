#version 460 core

#define MAX_LIGHTS 10

in vec2 texCoord;

out vec4 color;

uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

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

vec3 getDiffuse(vec3 pos, vec3 normal, vec3 albedo) {
	vec3 result = vec3(0);
	
	for (uint i = uint(0); i < lightData.numberLights; i++) {
		// Right now only supports point lights
		vec3 lightDir = normalize(lightData.lights.positions[i].xyz - pos);

		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * albedo * lightData.lights.diffuses[i].rgb;
		result += diffuse;
	}

	return result;
}

/*vec4 getSpecular() {
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
}*/

void main() {
	vec3 fragPos = texture(gPosition, texCoord).rgb;
	vec3 normal = texture(gNormal, texCoord).rgb;
	vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
	float specular = texture(gAlbedoSpec, texCoord).a;

	vec3 ambient = albedo * lightData.ambientStrength;
	vec3 diffuse = getDiffuse(fragPos, normal, albedo);
	//vec4 specular = getSpecular();
	color = vec4(diffuse + ambient, 1.0);
}