#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

struct PositionComponents {
	std::vector<glm::vec3> WorldPositions;
	// TODO: Maybe seperate into "rotation" component
	std::vector<glm::quat> rotations;
};

struct PositionComponent {
	glm::vec3 WorldPosition;
	glm::vec3 rotation;
};