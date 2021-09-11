#pragma once
#include <vector>
#include "../Components/ModelComponent.h"
#include "../../Render/Pipeline.h"
#include <sdl/SDL_video.h>
#include "RenderSubSystems/LightingSystem.h"
#include "../../Utility/Camera.h"
#include "../../Utility/Window.h"

struct Quad {
public:
	GLuint VAO;
	GLuint VertexBuffer;
	GLuint TexCoordBuffer;

	void init();
};

class RenderSystem {
private:
	// TODO: Seperate vector for particles and other special renderable entities
	std::vector<size_t> renderableEntities;

	// Standard pipeline
	Pipeline lightingPipeline;
	Pipeline gBufferPipeline;

	// Subsystems
	LightingSystem lightingSystem;

	// Framebuffers
	GLuint gBuffer;
	GLuint positionColorFrameBuffer;
	GLuint normalFrameBuffer;
	GLuint specularColorFrameBuffer;
	GLuint rboDepth;

	// Deferred Rendering quad
	Quad quad;

	void initFramebuffers(Window* window);
public:
	void init(Window* window);

	void addRenderableEntity(size_t id);
	void removeRenderableEntity(size_t id);

	void addLight(LightInfo* info);

	void update(const std::vector<PositionComponent>* positions);
	void render(const std::vector<PositionComponent>* positions, const std::vector<ModelComponent>* modelComponents, glm::mat4 viewProj, Camera* camera);
};