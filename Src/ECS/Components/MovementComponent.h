#pragma once
#include <glm/vec3.hpp>
#include <vector>

struct MovementComponents {
	std::vector<glm::vec3> velocitys;
	std::vector<glm::vec3> rotationVelocities;
};

struct MovementComponent {
	glm::vec3 velocity;
	glm::vec3 rotationVelocity;
};