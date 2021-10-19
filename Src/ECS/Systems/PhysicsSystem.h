#pragma once
#include <vector>
#include "../Components/PhysicsComponent.h"
#include "../Components/MovementComponent.h"
#include "../Components/PositionComponent.h"
#include "../Components/ModelComponent.h"

struct CreatePhysicsComponentOptions {	
	uint64_t flags;
	const ModelComponent* component;
};

class PhysicsSystem {
	std::vector<size_t> physicsComponents;
	void updateSpatialStructure(const std::vector<PositionComponent>* positionComponents);
	bool testForCollision(const PhysicsComponent* physicsComponent1, const PhysicsComponent* physicsComponent2, 
						  const PositionComponent* positionComponent1, const PositionComponent* positionComponent2);
	bool testCircleCircleCollision(const PhysicsComponent* physicsComponent1, const PhysicsComponent* physicsComponent2, 
								   const PositionComponent* positionComponent1, const PositionComponent* positionComponent2);
	bool testCircleAABBCollision(const PhysicsComponent* circlePhysicsComponent, const PhysicsComponent* aabbPhysicsComponent2, 
								 const PositionComponent* circlePositionComponent1, const PositionComponent* aabbPositionComponent2);
	bool testCircleOBBCollision(const PhysicsComponent* circlePhysicsComponent, const PhysicsComponent* obbPhysicsComponent2,
								const PositionComponent* circlePositionComponent1, const PositionComponent* obbPositionComponent2);
	PhysicsComponent createCircleBoundingBox(CreatePhysicsComponentOptions* options);
	PhysicsComponent createAABB(CreatePhysicsComponentOptions* options);

	void resolveCollision(const PhysicsComponent* physicsComponent1, const PhysicsComponent* physicsComponent2,
						  const PositionComponent* positionComponent1, const PositionComponent* positionComponent2,
						  MovementComponent* movementComponnet1, MovementComponent* movementComponent2);
	glm::vec3 calculateCircleAABBNormal(const PositionComponent* circlePosition, const PositionComponent* aabbPosition, const PhysicsComponent* aabbPhysicsComponent2);

public:
	void addEntity(size_t id);
	void update(const std::vector<PhysicsComponent>* physicComponents, std::vector<MovementComponent>* movementComponents, std::vector<PositionComponent>* positionComponents, float delta_s);

	PhysicsComponent generatePhysicsComponent(CreatePhysicsComponentOptions* options);
};