#include "LightingSystem.h"
#include <iostream>

constexpr size_t SIZE = sizeof(LightData);

const std::vector<size_t>* LightingSystem::getLightEntities() {
	return &this->lightEntityIds;
}

void LightingSystem::createBuffer(unsigned int uniformBindingPoint) {
	lightDataUniformBlock.createBuffer(&this->lightData, SIZE, uniformBindingPoint);
}

void LightingSystem::update(const std::vector<PositionComponent>* positions) {
	for (auto i = 0; i < this->lightEntityIds.size(); i++) {
		auto id = this->lightEntityIds.at(i);

		this->lightData.lights.positions[i] = glm::vec4{ positions->at(id).WorldPosition, 1.0 };
	}

	this->lightDataUniformBlock.update(&this->lightData, SIZE);
}

void LightingSystem::addLight(LightInfo* info) {
	if (this->lightData.numberLights >= MAX_LIGHTS) {
		std::cerr << "LIGHT SYSTEM: UNABLE TO ADD LIGHT AS AT MAXIMUM AMOUNT OF LIGHTS" << std::endl;
	} else {
		size_t i = this->lightData.numberLights;

		this->lightEntityIds.push_back(std::move(info->entityId));

		this->lightData.lights.ambients[i] = std::move(info->ambient);
		this->lightData.lights.diffuses[i] = std::move(info->diffuse);
		this->lightData.lights.directions[i] = std::move(info->direction);
		this->lightData.lights.speculars[i] = std::move(info->specular);

		this->lightData.numberLights += 1;
	}
}

std::array<glm::mat4, MAX_LIGHTS> LightingSystem::generateLightSpaceTransforms(const std::vector<PositionComponent>* positionComponents) {
	std::array<glm::mat4, MAX_LIGHTS> lightSpaceTransforms{};
	
	const float near_plane = 1.0f;
	const float far_plane = 7.5f;

	glm::mat4 orthoProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	size_t i = 0;
	for (auto& lightId : this->lightEntityIds) {
		// TODO: Implement for point lights
		glm::mat4 lightView = glm::lookAt(positionComponents->at(lightId).WorldPosition, glm::vec3{ 0.0, 0.0, 0.0 }, glm::vec3{ 0.0, 1.0, 0.0 });
		lightSpaceTransforms[i] = orthoProj * lightView;
		i += 1;
	}

	return lightSpaceTransforms;
}

std::vector<LightTransformInformation> LightingSystem::generateLightSpaceTransformsPoint(const std::vector<PositionComponent>* positionComponents) {
	std::vector<LightTransformInformation> lightSpaceTransforms{};

	size_t j = 0;
	for (auto& lightId : this->lightEntityIds) {
		auto direction = this->lightData.lights.directions[j];
		if (direction.w == 0) {
			// Point light
			LightTransformInformation lightTransformInformation;
			std::array<glm::mat4, 6> transforms;

			transforms[0] = glm::lookAt(positionComponents->at(lightId).WorldPosition, positionComponents->at(lightId).WorldPosition + glm::vec3{ 1.0, 0.0, 0.0 }, glm::vec3{ 0.0, -1.0, 0.0 });
			transforms[1] = glm::lookAt(positionComponents->at(lightId).WorldPosition, positionComponents->at(lightId).WorldPosition + glm::vec3{ -1.0, 0.0, 0.0 }, glm::vec3{ 0.0, -1.0, 0.0 });
			transforms[2] = glm::lookAt(positionComponents->at(lightId).WorldPosition, positionComponents->at(lightId).WorldPosition + glm::vec3{ 0.0, 1.0, 0.0 }, glm::vec3{ 0.0, 0.0, 1.0 });
			transforms[3] = glm::lookAt(positionComponents->at(lightId).WorldPosition, positionComponents->at(lightId).WorldPosition + glm::vec3{ 0.0, -1.0, 0.0 }, glm::vec3{ 0.0, 0.0, -1.0 });
			transforms[4] = glm::lookAt(positionComponents->at(lightId).WorldPosition, positionComponents->at(lightId).WorldPosition + glm::vec3{ 0.0, 0.0, 1.0 }, glm::vec3{ 0.0, -1.0, 0.0 });
			transforms[5] = glm::lookAt(positionComponents->at(lightId).WorldPosition, positionComponents->at(lightId).WorldPosition + glm::vec3{ 0.0, 0.0, -1.0 }, glm::vec3{ 0.0, -1.0, 0.0 });

			lightTransformInformation.position = positionComponents->at(lightId).WorldPosition;
			lightTransformInformation.transforms = transforms;
			lightSpaceTransforms.push_back(lightTransformInformation);
		}

		j += 1;
	}

	return lightSpaceTransforms;
}
