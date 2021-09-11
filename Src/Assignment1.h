#pragma once

#include <glad/glad.h>
#include <sdl/SDL.h>
#include <glm/vec4.hpp>
#include <array>
#include "ECS/Scene.h"
#include "Utility/Window.h"

class Application {
	SDL_Window* window;
	SDL_GLContext context;
	Scene scene;
	Window windowInfo;

public:
	void init();
	void run();
	void load();
	void cleanup();
};