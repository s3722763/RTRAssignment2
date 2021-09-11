#pragma once
#include <vector>
#include "Components/ModelComponent.h"
#include "Components/PositionComponent.h"
#include <queue>

struct AddEntityInfo {
	ModelComponent modelComponent;
	PositionComponent positionComponent;
};

class Entities {
	// ID stuff
	std::queue<size_t> reusableIds;
	std::vector<size_t> usedIds;

	// Components
	std::vector<ModelComponent> models;
	std::vector<PositionComponent> positions;

public:
	size_t addEntity(AddEntityInfo* info);

	std::vector<ModelComponent>* getModelComponents();
	std::vector<PositionComponent>* getPositionComponents();
};