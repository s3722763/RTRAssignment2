#pragma once
#include <vector>
#include "../Components/ModelComponent.h"

class RenderSystem {
	std::vector<size_t> renderableEntities;

public:
	void addRenderableEntity(size_t id);
	void removeRenderableEntity(size_t id);

	void render(const std::vector<ModelComponent>* modelComponents);
};