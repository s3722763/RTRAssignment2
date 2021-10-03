#pragma once
#include <vector>
#include "Components/ModelComponent.h"
#include "Components/PositionComponent.h"
#include "Components/MovementComponent.h"
#include <queue>
#include "Components/ParticleEmitterComponent.h"

struct AddEntityInfo {
	ModelComponent modelComponent;
	PositionComponent positionComponent;
	MovementComponent movementComponent;
	ParticleEmitterComponent particleEmitterComponent;
};

class Entities {
	// ID stuff
	std::queue<size_t> reusableIds;
	std::vector<size_t> usedIds;

	// Components
	std::vector<ModelComponent> models;
	std::vector<PositionComponent> positions;
	std::vector<MovementComponent> movementComponents;
	std::vector<ParticleEmitterComponent> particleEmitterComponents;

public:
	size_t addEntity(AddEntityInfo* info);

	std::vector<ModelComponent>* getModelComponents();
	std::vector<PositionComponent>* getPositionComponents();
	std::vector<MovementComponent>* getMovementComponents();
	std::vector<ParticleEmitterComponent>* getParticleEmitterComponents();
};