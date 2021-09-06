#pragma once
#include "../Utility/Camera.h"
#include "Systems/ModelSystem.h"
#include "../Render/Pipeline.h"

#include <sdl/SDL_video.h>
#include <queue>

typedef uint64_t SceneUpdateResult;

typedef enum SceneUpdateResultFlags {
	Quit = 1 << 0
} SceneUpdateResultFlags;

typedef enum EntityCreateInfoFlags {
	Renderable = 1 << 0,
	HasModel = 1 << 1
};

struct EntityCreateInfo {
	uint64_t flags;
	std::string directory;
	std::string model;
};

class Scene {
private:
	Camera camera;
	std::queue<size_t> reusableIds;
	std::vector<size_t> usedIds;

	// Systems
	ModelSystem modelSystem;

	size_t createEntity(EntityCreateInfo* info);

public:
	void load(Camera camera);
	void update(float delta_s, SceneUpdateResult* result);
	// TODO: Remove to own system
	Pipeline pipeline;
	size_t backpackID;
	void draw(SDL_Window* window);
};
