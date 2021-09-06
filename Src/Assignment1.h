#pragma once

#include <glad/glad.h>
#include <sdl/SDL.h>
#include <glm/vec4.hpp>
#include <array>
#include "ECS/Scene.h"

class Application {
	SDL_Window* window;
	SDL_GLContext context;
	Scene scene;

public:
	void init();
	void run();
	void load();
	void cleanup();
};