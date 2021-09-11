#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

struct PositionComponent {
	glm::vec3 WorldPosition = { 0, 0, 0 };
	// TODO: Maybe seperate into "rotation" component
	glm::quat rotation;
};