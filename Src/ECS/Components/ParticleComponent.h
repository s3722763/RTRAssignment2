#pragma once
#include <vector>
#include <glm/vec4.hpp>

struct ParticleComponents {
	std::vector<float> sizes;
	std::vector<glm::vec4> colors;
	std::vector<float> timeToLives;
};

struct ParticleComponent {
	float size;
	glm::vec4 color;
	float timeToLive;
};