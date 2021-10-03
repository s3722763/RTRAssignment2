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
