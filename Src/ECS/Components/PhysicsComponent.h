#pragma once
#include <cstdint>
#include <array>
#include <glm/vec3.hpp>

struct OBBFeatures {
	glm::vec3 centre;
	glm::vec3 halfWidthVector;
	glm::vec3 halfHeightVector;
	glm::vec3 halfLengthVector;
};

struct AABBPoints {
	glm::vec3 min;
	glm::vec3 max;
};

enum PhysicsComponentFlags {
	CircleBoundingBox = 1 << 0,
	// Axis aligned bounding box
	AABB = 1 << 1,
	// Oriented Bounding box
	OBB = 1 << 2,
	AffectedByGravity = 1 << 3,
	Static = 1 << 4
};

struct PhysicsComponent {
	uint64_t flags;
	AABBPoints aabbPoints;
	OBBFeatures obbFeatures;
	// For circle bounding box
	float radius;
	float mass;
};