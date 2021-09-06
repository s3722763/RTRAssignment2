#pragma once
#include <array>
#include <glm/vec4.hpp>
#include <glad/glad.h>
#include "../../../Render/UniformBlock.h"

constexpr unsigned int MAX_LIGHTS = 10;

struct LightInfo {
	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 direction = glm::vec4{ 0.0f };
};

struct Lights {
	std::array<glm::vec4, MAX_LIGHTS> positions;
	std::array<glm::vec4, MAX_LIGHTS> ambients;
	std::array<glm::vec4, MAX_LIGHTS> diffuses;
	std::array<glm::vec4, MAX_LIGHTS> speculars;
	std::array<glm::vec4, MAX_LIGHTS> directions;
};

struct LightData {
	Lights lights;
	GLfloat ambientStrength{ 0.1f };
	GLuint numberLights{ 0 };
	// Need these for memory alignment as floats must be in groups of 16 bytes
	GLfloat temp1 = { 0.0f };
	GLfloat temp2 = { 0.0f };
};

class LightingSystem {
	LightData lightData;
	UniformBlock<LightData> lightDataUniformBlock;

public:
	void createBuffer(unsigned int uniformBindingPoint);
	void update();
	void addLight(LightInfo* info);
};