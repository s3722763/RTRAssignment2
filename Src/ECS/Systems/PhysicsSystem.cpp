#include "PhysicsSystem.h"
#include <iostream>
#include <glm/gtx/norm.hpp>

constexpr glm::vec3 GRAVITY = { 0.0f,  0.0f, 0.0f };

void PhysicsSystem::updateSpatialStructure(const std::vector<PositionComponent>* positionComponents) {

}

bool PhysicsSystem::testForCollision(const PhysicsComponent* physicsComponent1, const PhysicsComponent* physicsComponent2, 
									 const PositionComponent* positionComponent1, const PositionComponent* positionComponent2) {
	auto& firstFlags = physicsComponent1->flags;
	auto& secondFlags = physicsComponent2->flags;

	if ((firstFlags & PhysicsComponentFlags::CircleBoundingBox) && (secondFlags & PhysicsComponentFlags::CircleBoundingBox)) {
		// Circle-Circle collision
		return this->testCircleCircleCollision(physicsComponent1, physicsComponent2, positionComponent1, positionComponent2);
	} else if ((firstFlags & PhysicsComponentFlags::CircleBoundingBox) && (secondFlags & PhysicsComponentFlags::AABB)) {
		// Circle-AABB collision
		return this->testCircleAABBCollision(physicsComponent1, physicsComponent2, positionComponent1, positionComponent2);
	} else if ((firstFlags & PhysicsComponentFlags::AABB) && (secondFlags & PhysicsComponentFlags::CircleBoundingBox)) {
		// Circle-AABB collision
		return this->testCircleAABBCollision(physicsComponent2, physicsComponent1, positionComponent2, positionComponent1);
	} else if ((firstFlags & PhysicsComponentFlags::OBB) && (secondFlags & PhysicsComponentFlags::CircleBoundingBox)) {
		// Circle-OBB collision
		// this->testCircleOBBCollision()
	} else if ((firstFlags & PhysicsComponentFlags::CircleBoundingBox) && (secondFlags & PhysicsComponentFlags::OBB)) {
		// Circle-OBB collision
	}
	// AABB-OBB are not tested as the ball is the only thing moving, therefore these will not collide
	return false;
}

bool PhysicsSystem::testCircleCircleCollision(const PhysicsComponent* physicsComponent1, const PhysicsComponent* physicsComponent2, 
											  const PositionComponent* positionComponent1, const PositionComponent* positionComponent2) {
	float xDiff = positionComponent1->WorldPosition.x - positionComponent2->WorldPosition.x;
	float yDiff = positionComponent1->WorldPosition.y - positionComponent2->WorldPosition.y;
	float zDiff = positionComponent1->WorldPosition.z - positionComponent2->WorldPosition.z;
	float distSquared = std::powf(xDiff, 2) + std::powf(yDiff, 2) + std::powf(zDiff, 2);

	float radius = std::powf(physicsComponent1->radius + physicsComponent2->radius, 2);

	if (distSquared < radius) {
		// Collision
		return true;
	} else {
		return false;
	}
}

bool PhysicsSystem::testCircleAABBCollision(const PhysicsComponent* circlePhysicsComponent, const PhysicsComponent* aabbPhysicsComponent, 
											const PositionComponent* circlePositionComponent, const PositionComponent* aabbPositionComponent) {
	float distanceSqaured = 0;
	glm::vec3 aabbMinPos = aabbPositionComponent->WorldPosition + aabbPhysicsComponent->aabbPoints.min;
	glm::vec3 aabbMaxPos = aabbPositionComponent->WorldPosition + aabbPhysicsComponent->aabbPoints.max;

	if (circlePositionComponent->WorldPosition.x < aabbMinPos.x) {
		distanceSqaured = distanceSqaured + std::powf(circlePositionComponent->WorldPosition.x - aabbMinPos.x, 2);
	} else if (circlePositionComponent->WorldPosition.x > aabbMaxPos.x) {
		distanceSqaured = distanceSqaured + std::powf(circlePositionComponent->WorldPosition.x - aabbMaxPos.x, 2);
	}

	if (circlePositionComponent->WorldPosition.y < aabbMinPos.y) {
		distanceSqaured = distanceSqaured + std::powf(circlePositionComponent->WorldPosition.y - aabbMinPos.y, 2);
	} else if (circlePositionComponent->WorldPosition.y > aabbMaxPos.y) {
		distanceSqaured = distanceSqaured + std::powf(circlePositionComponent->WorldPosition.y - aabbMaxPos.y, 2);
	}

	if (circlePositionComponent->WorldPosition.z < aabbMinPos.z) {
		distanceSqaured = distanceSqaured + std::powf(circlePositionComponent->WorldPosition.z - aabbMinPos.z, 2);
	} else if (circlePositionComponent->WorldPosition.z > aabbMaxPos.z) {
		distanceSqaured = distanceSqaured + std::powf(circlePositionComponent->WorldPosition.z - aabbMaxPos.z, 2);
	}

	if (distanceSqaured < std::powf(circlePhysicsComponent->radius, 2)) {
		// Collision
		return true;
	} else {
		return false;
	}
}

bool PhysicsSystem::testCircleOBBCollision(const PhysicsComponent* circlePhysicsComponent, const PhysicsComponent* obbPhysicsComponent, 
										   const PositionComponent* circlePositionComponent, const PositionComponent* obbPositionComponent) {
	// TODO: Game Physics Cookbook
	return false;
}

PhysicsComponent PhysicsSystem::createCircleBoundingBox(CreatePhysicsComponentOptions* options) {
	PhysicsComponent result{};
	result.flags = options->flags;
	result.mass = 1.0f;
	float distanceSquared = 0;
	
	auto& modelsVertices = options->component->meshes.vertices;

	for (auto modelI = 0; modelI < modelsVertices.size(); modelI++) {
		for (auto vertexI = 0; vertexI < modelsVertices[modelI].size(); vertexI++) {
			// ASSUME OBJECT IS CENTRED AROUND ORIGIN
			auto distance = glm::distance2({0, 0, 0}, modelsVertices[modelI][vertexI]);
			
			if (distance > distanceSquared) {
				distanceSquared = distance;
			}
		}
	}

	result.radius = std::sqrtf(distanceSquared);
	//result.radius = 0.01;

	return result;
}

PhysicsComponent PhysicsSystem::createAABB(CreatePhysicsComponentOptions* options) {
	PhysicsComponent result{};
	result.flags = options->flags;
	result.mass = 1.0f;

	AABBPoints points;
	glm::vec3 min{ 0.0f, 0.0f , 0.0f };
	glm::vec3 max{ 0.0f, 0.0f , 0.0f };

	for (auto& modelVertices : options->component->meshes.vertices) {
		for (auto& vertex : modelVertices) {
			if (vertex.x < min.x) {
				min.x = vertex.x;
			} else if (vertex.x > max.x) {
				max.x = vertex.x;
			}

			if (vertex.y < min.y) {
				min.y = vertex.y;
			} else if (vertex.y > max.y) {
				max.y = vertex.y;
			}

			if (vertex.z < min.z) {
				min.z = vertex.z;
			} else if (vertex.z > max.z) {
				max.z = vertex.z;
			}
		}
	}

	points.min = min;
	points.max = max;
	result.aabbPoints = std::move(points);

	return result;
	
	return result;
}

void PhysicsSystem::resolveCollision(const PhysicsComponent* physicsComponent1, const PhysicsComponent* physicsComponent2, 
									 const PositionComponent* positionComponent1, const PositionComponent* positionComponent2,
									 MovementComponent* movementComponent1, MovementComponent* movementComponent2) {
	//std::cout << "Collision" << std::endl;
	auto& firstFlags = physicsComponent1->flags;
	auto& secondFlags = physicsComponent2->flags;

	glm::vec3 normal{};
	
	if ((firstFlags & PhysicsComponentFlags::CircleBoundingBox) && (secondFlags & PhysicsComponentFlags::CircleBoundingBox)) {
		// Circle-Circle collision
		
	} else if ((firstFlags & PhysicsComponentFlags::CircleBoundingBox) && (secondFlags & PhysicsComponentFlags::AABB)) {
		// Circle-AABB collision
		normal = this->calculateCircleAABBNormal(positionComponent1, positionComponent2, physicsComponent2);

		// ASSUME: Only the ball movces
		movementComponent1->velocity = glm::reflect(movementComponent1->velocity, normal);

	} else if ((firstFlags & PhysicsComponentFlags::AABB) && (secondFlags & PhysicsComponentFlags::CircleBoundingBox)) {
		// Circle-AABB collision
		normal = this->calculateCircleAABBNormal(positionComponent2, positionComponent1, physicsComponent1);

		// ASSUME: Only the ball movces
		movementComponent2->velocity = glm::reflect(movementComponent2->velocity, normal);
	} else if ((firstFlags & PhysicsComponentFlags::OBB) && (secondFlags & PhysicsComponentFlags::CircleBoundingBox)) {
		// Circle-OBB collision
		// this->testCircleOBBCollision()
	} else if ((firstFlags & PhysicsComponentFlags::CircleBoundingBox) && (secondFlags & PhysicsComponentFlags::OBB)) {
		// Circle-OBB collision
	}
	

	//movementComponnet1->velocity *= -1 / 1.2;
	//movementComponent2->velocity *= -1 / 1.2;
}

glm::vec3 PhysicsSystem::calculateCircleAABBNormal(const PositionComponent* circlePosition, const PositionComponent* aabbPosition, const PhysicsComponent* aabbPhysicsComponent) {
	// Find which part of the circle centre is not in the bounds of the AABB
	glm::vec3 aabbMinPos = aabbPosition->WorldPosition + aabbPhysicsComponent->aabbPoints.min;
	glm::vec3 aabbMaxPos = aabbPosition->WorldPosition + aabbPhysicsComponent->aabbPoints.max;
	// This might only work for objects which the circle has two of the 3 axis in the same bounds as the AABB
	// https://gamedev.net/forums/topic/649000-intersection-between-a-circle-and-an-aabb/5101896/
	// Normal is the circle centre - collision location

	glm::vec3 closestPoint{};

	if (circlePosition->WorldPosition.x > aabbMaxPos.x) {
		closestPoint.x = aabbMaxPos.x;
	} else if (circlePosition->WorldPosition.x < aabbMinPos.x) {
		closestPoint.x = aabbMinPos.x;
	} else {
		closestPoint.x = circlePosition->WorldPosition.x;
	}

	if (circlePosition->WorldPosition.y > aabbMaxPos.y) {
		closestPoint.y = aabbMaxPos.y;
	} else if (circlePosition->WorldPosition.y < aabbMinPos.y) {
		closestPoint.y = aabbMinPos.y;
	} else {
		closestPoint.y = circlePosition->WorldPosition.y;
	}

	if (circlePosition->WorldPosition.z > aabbMaxPos.z) {
		closestPoint.z = aabbMaxPos.z;
	} else if (circlePosition->WorldPosition.z < aabbMinPos.z) {
		closestPoint.z = aabbMinPos.z;
	} else {
		closestPoint.z = circlePosition->WorldPosition.z;
	}

	glm::vec3 normal = circlePosition->WorldPosition - closestPoint;

	return glm::normalize(normal);
}

void PhysicsSystem::addEntity(size_t id) {
	this->physicsComponents.push_back(id);
}

void PhysicsSystem::update(const std::vector<PhysicsComponent>* physicComponents, std::vector<MovementComponent>* movementComponents, 
						   std::vector<PositionComponent>* positionComponents, float delta_s) {
	this->updateSpatialStructure(positionComponents);

	auto gravity = GRAVITY * delta_s;

	std::vector<glm::vec3> oldPositions;
	oldPositions.resize(positionComponents->size());

	// Apply acceleration to velocity
	for (auto& id : this->physicsComponents) {
		// If not static
		if ((physicComponents->at(id).flags & PhysicsComponentFlags::Static) == 0) {
			oldPositions[id] = positionComponents->at(id).WorldPosition;
			movementComponents->at(id).velocity += gravity;
			positionComponents->at(id).WorldPosition += (movementComponents->at(id).velocity * delta_s);
		}
	}

	for (auto i = 0; i < this->physicsComponents.size(); i++) {
		for (auto j = i + 1; j < this->physicsComponents.size(); j++) {
			auto firstIndex = this->physicsComponents.at(i);
			auto secondIndex = this->physicsComponents.at(j);

			auto* firstPosition = &positionComponents->at(firstIndex);
			auto* secondPosition = &positionComponents->at(secondIndex);

			auto* firstPhysicsComponent = &physicComponents->at(firstIndex);
			auto* secondPhysicsComponent = &physicComponents->at(secondIndex);

			if (this->testForCollision(firstPhysicsComponent, secondPhysicsComponent, firstPosition, secondPosition)) {
				auto* firstMovementComponent = &movementComponents->at(firstIndex);
				auto* secondMovementComponent = &movementComponents->at(secondIndex);

				this->resolveCollision(firstPhysicsComponent, secondPhysicsComponent, firstPosition, secondPosition, firstMovementComponent, secondMovementComponent);
				// For testing, normal is straight up therefore velocity is reversed

				if ((physicComponents->at(firstIndex).flags & PhysicsComponentFlags::Static) == 0) {
					firstPosition->WorldPosition = oldPositions.at(firstIndex);
				}

				if ((physicComponents->at(secondIndex).flags & PhysicsComponentFlags::Static) == 0) {
					secondPosition->WorldPosition = oldPositions.at(secondIndex);
				}
			}
		}
	}
}

PhysicsComponent PhysicsSystem::generatePhysicsComponent(CreatePhysicsComponentOptions* options) {
	if (options->flags & PhysicsComponentFlags::CircleBoundingBox) {
		return this->createCircleBoundingBox(options);
	} else if (options->flags & PhysicsComponentFlags::AABB) {
		return this->createAABB(options);
	}

	return PhysicsComponent{};
}

