#pragma once
#include <vector>
#include "../Components/ModelComponent.h"
#include "../../Render/Pipeline.h"
#include <sdl/SDL_video.h>
#include "RenderSubSystems/LightingSystem.h"

class RenderSystem {
	// TODO: Seperate vector for particles and other special renderable entities
	std::vector<size_t> renderableEntities;

	// Standard pipeline
	Pipeline pipeline;

	// Subsystems
	LightingSystem lightingSystem;
public:
	void init();

	void addRenderableEntity(size_t id);
	void removeRenderableEntity(size_t id);

	void addLight(LightInfo* info);

	void update(const std::vector<PositionComponent>* positions);
	void render(const std::vector<PositionComponent>* positions, const std::vector<ModelComponent>* modelComponents, glm::mat4 viewProj);
};