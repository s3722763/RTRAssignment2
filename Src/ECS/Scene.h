#pragma once
#include "../Utility/Camera.h"
#include "Systems/ModelSystem.h"
#include "../Render/Pipeline.h"
#include "Entities.h"

#include <sdl/SDL_video.h>
#include <queue>
#include "Systems/RenderSystem.h"
#include "Systems/MovementSystem.h"

typedef uint64_t SceneUpdateResult;

typedef enum SceneUpdateResultFlags {
	Quit = 1 << 0
} SceneUpdateResultFlags;

typedef enum EntityCreateInfoFlags {
	Renderable = 1 << 0,
	HasModel = 1 << 1,
	HasPosition = 1 << 2,
	Moves = 1 << 3
} EntityCreateInfoFlags;

struct EntityCreateInfo {
	uint64_t flags;
	std::string directory;
	std::string model;
	PositionComponent positionComponent;
	MovementComponent movementComponent;
};

class Scene {
private:
	Camera camera;
	Entities entities;

	// Systems
	ModelSystem modelSystem;
	RenderSystem renderSystem;
	MovementSystem movementSystem;

	size_t createEntity(EntityCreateInfo* info);

public:
	void init();
	void load(Camera camera);
	void update(float delta_s, SceneUpdateResult* result);
	// TODO: Remove to own system
	Pipeline pipeline;
	size_t backpackID;
	void draw(SDL_Window* window);
};
