#include "LightingSystem.h"
#include <iostream>

constexpr size_t SIZE = sizeof(LightData);

void LightingSystem::createBuffer(unsigned int uniformBindingPoint) {
	lightDataUniformBlock.createBuffer(&this->lightData, SIZE, uniformBindingPoint);
}

void LightingSystem::update() {
	this->lightDataUniformBlock.update(&this->lightData, SIZE);
}

void LightingSystem::addLight(LightInfo* info) {
	if (this->lightData.numberLights >= MAX_LIGHTS) {
		std::cerr << "LIGHT SYSTEM: UNABLE TO ADD LIGHT AS AT MAXIMUM AMOUNT OF LIGHTS" << std::endl;
	} else {
		size_t i = this->lightData.numberLights;

		this->lightData.lights.ambients[i] = std::move(info->ambient);
		this->lightData.lights.positions[i] = std::move(info->position);
		this->lightData.lights.diffuses[i] = std::move(info->diffuse);
		this->lightData.lights.directions[i] = std::move(info->direction);
		this->lightData.lights.speculars[i] = std::move(info->specular);

		this->lightData.numberLights += 1;
	}
}
