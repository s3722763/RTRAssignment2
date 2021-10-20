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

uniform sampler2D directionalDepthMap;
// I dont like this
uniform mat4 lightSpaceMatrix;

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
		vec3 lightDir = vec3(0.0, 0.0, 0.0);

		if (lightData.lights.directions[i].w == 0) {
			lightDir = normalize(pos - lightData.lights.positions[i].xyz);
		} else {
			lightDir = normalize(lightData.lights.directions[i].xyz);
		}

		float diff = max(dot(normal, -lightDir), 0.0);
		vec3 diffuse = diff * albedo * lightData.lights.diffuses[i].rgb;
		result += diffuse;
	}

	return result;
}

vec3 getSpecular(vec3 pos, vec3 normal, float specIn) {
	vec3 result = vec3(0);

	for (uint i = uint(0); i < lightData.numberLights; i++) {
		vec3 lightDir = vec3(0.0, 0.0, 0.0);

		if (lightData.lights.directions[i].w == 0) {
			lightDir = normalize(pos - lightData.lights.positions[i].xyz);
		} else {
			lightDir = normalize(lightData.lights.directions[i].xyz);
		}

		vec3 viewDir = normalize(viewPos - pos);
		vec3 halfwayDir = normalize(-lightDir + viewDir);

		// TODO: Change shininess from 64 to material value
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
		vec3 specular = spec * specIn * lightData.lights.speculars[i].rgb;

		result += specular;
	}

	return result;
}

float calculateDirectionalShadow(vec3 position, vec3 normal, uint index) {
	// Perform perspective divide
	vec4 fragPosLightSpace = lightSpaceMatrix * vec4(position, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(directionalDepthMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	vec3 lightDir = lightData.lights.directions[index].xyz;
	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float bias = 0.005;
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(directionalDepthMap, 0);

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			float pcfDepth = texture(directionalDepthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	if (projCoords.z > 1.0) {
		// Outside of light frustum
		shadow = 0.0;
	}

	return shadow / 9.0;
}

float calculatePointShadow(vec3 position, uint index) {
	float shadow = 0;
	float bias = 0.05;
	float samples = 4.0;
	float offset = 0.1;

	float lightShadow = 0;
	vec3 fragToLight = position - lightData.lights.positions[index].xyz;
	float currentDepth = length(fragToLight);

	// PCF
	for (float x = -offset; x < offset; x += offset / (samples * 0.5)) {
		for (float y = -offset; y < offset; y += offset / (samples * 0.5)) {
			for (float z = -offset; z < offset; z += offset / (samples * 0.5)) {
				float closestDepth = texture(pointDepthMap, fragToLight + vec3(x, y, z)).r;
				closestDepth *= far_plane;

				if (currentDepth - bias > closestDepth) {
					lightShadow += 1.0;
				}
			}
		}
	}

	return lightShadow / (samples * samples * samples);
}

float calculateShadows(vec3 fragPos, vec3 normal) {
	float shadow = 0;

	for (uint i = uint(0); i < lightData.numberLights; i++) {
		if (lightData.lights.directions[i].w == 0) {
			// Point light
			shadow += calculatePointShadow(fragPos, i);
		} else {
			// Directional light
			shadow += calculateDirectionalShadow(fragPos, normal, i);
		}
	}

	return shadow;
}

void main() {
	vec3 fragPos = texture(gPosition, texCoord).rgb;
	vec3 normal = texture(gNormal, texCoord).rgb;
	vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
	float spec = texture(gAlbedoSpec, texCoord).a;

	vec3 ambient = albedo * lightData.ambientStrength;
	vec3 diffuse = getDiffuse(fragPos, normal, albedo);
	vec3 specular = getSpecular(fragPos, normal, spec);

	float shadow = calculateShadows(fragPos, normal);

	color = vec4(ambient + ((1 - shadow) * (diffuse + specular)), 1.0);
}