#pragma once
#include <vector>
#include "../Components/ModelComponent.h"
#include "../../Render/Pipeline.h"
#include <sdl/SDL_video.h>
#include "RenderSubSystems/LightingSystem.h"
#include "../../Utility/Camera.h"
#include "../../Utility/Window.h"
#include "RenderSubSystems/ParticleSystem.h"

struct Quad {
public:
	GLuint VAO;
	GLuint VertexBuffer;
	GLuint TexCoordBuffer;

	void init();
};

struct Cube {
public:
	GLuint VAO;
	GLuint VertexBuffer;

	void init();
};

class RenderSystem {
private:
	// TODO: Seperate vector for particles and other special renderable entities
	std::vector<size_t> renderableEntities;

	// Standard pipeline
	Pipeline lightingPipeline;
	Pipeline gBufferPipeline;
	// Pipeline for light cubes
	Pipeline lightCubePipeline;
	Pipeline skyboxPipeline;
	// Pipeline for shadows
	Pipeline pointShadowGeneratePipeline;
	Pipeline directionalShadowGeneratePipeline;

	// Subsystems
	LightingSystem lightingSystem;
	ParticleSystem particleSystem;

	// Framebuffers
	GLuint gBuffer;
	GLuint positionColorFrameBuffer;
	GLuint normalFrameBuffer;
	GLuint specularColorFrameBuffer;
	GLuint rboDepth;

	// Skybox
	GLuint skyboxTextureId;
	GLuint skyboxVAO;
	GLuint skyboxVBO;

	// Shadows
	GLuint shadowFrameBuffer;
	GLuint shadowsDepthMap;

	GLuint pointShadowFrameBuffer;
	GLuint pointShadowDepthCube;

	// Deferred Rendering quad
	Quad quad;
	Cube cube;

	void initFramebuffers(Window* window);
	void loadSkybox();

	void drawScene(Pipeline* pipeline, const std::vector<ModelComponent>* modelComponents, const std::vector<PositionComponent>* positions, bool genShadows);

	// Shadows
	void setupDirectionalShadows();
	void setupPointShadows();
	void generateShadows(const std::vector<ModelComponent>* modelComponents, const std::vector<PositionComponent>* positions, glm::mat4 view, Camera* camera, int width, int height);

public:
	void init(Window* window);

	void addRenderableEntity(size_t id);
	void removeRenderableEntity(size_t id);

	void addParticleEmitter(size_t id);

	void addLight(LightInfo* info);

	void update(const std::vector<PositionComponent>* positions, std::vector<ParticleEmitterComponent>* particleEmmitters, float delta_s);
	void render(const std::vector<PositionComponent>* positions, const std::vector<ModelComponent>* modelComponents, glm::mat4 viewProj, glm::mat4 view, glm::mat4 Proj, Camera* camera, int width, int height);
};