#version 460 core

#define MAX_LIGHTS 10

in vec2 texCoord;

out vec4 color;

uniform vec3 viewPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform float far_plane;
uniform samplerCube pointDepthMap; 

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

vec3 getSpecular(vec3 pos, vec3 normal, float specIn) {
	vec3 result = vec3(0);

	for (uint i = uint(0); i < lightData.numberLights; i++) {
		vec3 lightDir = normalize(lightData.lights.positions[i].xyz - pos);

		vec3 viewDir = normalize(viewPos - pos);
		vec3 halfwayDir = normalize(lightDir + viewDir);

		// TODO: Change shininess from 64 to material value
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
		vec3 specular = spec * specIn * lightData.lights.speculars[i].rgb;

		result += specular;
	}

	return result;
}

float calculateShadow(vec3 position) {
	float shadow = 0;
	float bias = 0.05;
	float samples = 4.0;
	float offset = 0.1;

	vec3 fragToLight = position - lightData.lights.positions[0].xyz;
	float currentDepth = length(fragToLight);

	for (float x = -offset; x < offset; x += offset / (samples * 0.5)) {
		for (float y = -offset; y < offset; y += offset / (samples * 0.5)) {
			for (float z = -offset; z < offset; z += offset / (samples * 0.5)) {
				float closestDepth = texture(pointDepthMap, fragToLight + vec3(x, y, z)).r;
				closestDepth *= far_plane;

				if (currentDepth - bias > closestDepth) {
					shadow += 1.0;
				}
			}
		}
	}

	return shadow / (samples * samples * samples);
}

void main() {
	vec3 fragPos = texture(gPosition, texCoord).rgb;
	vec3 normal = texture(gNormal, texCoord).rgb;
	vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
	float spec = texture(gAlbedoSpec, texCoord).a;

	vec3 ambient = albedo * lightData.ambientStrength;
	vec3 diffuse = getDiffuse(fragPos, normal, albedo);
	vec3 specular = getSpecular(fragPos, normal, spec);

	float shadow = calculateShadow(fragPos);

	color = vec4(ambient + ((1 - shadow) * (diffuse + specular)), 1.0);
}