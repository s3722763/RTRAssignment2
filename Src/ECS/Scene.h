#pragma once
#include "../Utility/Camera.h"
#include "Systems/ModelSystem.h"
#include "../Render/Pipeline.h"
#include "Entities.h"

#include <sdl/SDL_video.h>
#include <queue>
#include "Systems/RenderSystem.h"
#include "Systems/MovementSystem.h"
#include "../Utility/Window.h"
#include "Systems/PhysicsSystem.h"

typedef uint64_t SceneUpdateResult;

typedef enum SceneUpdateResultFlags {
	Quit = 1 << 0
} SceneUpdateResultFlags;

typedef enum EntityCreateInfoFlags {
	Renderable = 1 << 0,
	HasModel = 1 << 1,
	HasPosition = 1 << 2,
	Moves = 1 << 3,
	Physicalised = 1 << 4
} EntityCreateInfoFlags;

struct EntityCreateInfo {
	uint64_t flags;
	uint64_t physicaliseFlags;
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
	PhysicsSystem physicsSystem;

	size_t createEntity(EntityCreateInfo* info);

	void addBall();
public:
	void init(Window* window);
	void load(Camera camera);
	void update(float delta_s, SceneUpdateResult* result);
	// TODO: Remove to own system
	Pipeline pipeline;
	size_t backpackID;
	void draw(SDL_Window* window);
};
