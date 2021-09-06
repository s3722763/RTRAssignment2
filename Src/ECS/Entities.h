#pragma once
#include <vector>
#include "Components/ModelComponent.h"
#include <queue>

struct AddEntityInfo {
	ModelComponent modelComponent;
};

class Entities {
	// ID stuff
	std::queue<size_t> reusableIds;
	std::vector<size_t> usedIds;

	// Components
	std::vector<ModelComponent> models;

public:
	size_t addEntity(AddEntityInfo* info);

	std::vector<ModelComponent>* getModelComponents();
};