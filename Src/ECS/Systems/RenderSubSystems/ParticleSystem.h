#pragma once
#include "../../Components/ParticleComponent.h"
#include "../../Components/PositionComponent.h"
#include "../../Components/MovementComponent.h"
#include <array>
#include "../../Components/ParticleEmitterComponent.h"
#include <queue>
#include "../../../Render/Pipeline.h"

constexpr size_t MAXIMUM_PARTICLES = 100000;

struct AddParticleInfo {
	ParticleComponent particleComponent;
	MovementComponent movementComponent;
	PositionComponent positionComponent;
};

struct ParticleQuad {
public:
	GLuint VAO;
	GLuint VertexBuffer;
	GLuint TexCoordBuffer;

	void init();
};


class ParticleSystem {
private:
	ParticleComponents particleComponents;
	MovementComponents particleMovementComponents;
	PositionComponents particlePositionComponents;
	std::vector<size_t> emitterIds;

	unsigned int numberOfParticles = 0;
	std::queue<size_t> reusableIds;
	std::vector<size_t> usedIds;

	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<float> zeroOneDistribution;
	std::uniform_real_distribution<float> minusOneToOneDistribution;

	Pipeline particlePipeline;

	GLuint particleVAO;
	GLuint particlePositionBufferObject;
	GLuint particleColorBufferObject;
	GLuint vertexBufferObject;
	std::vector<glm::vec3> particleWorldPositionBuffer;
	std::vector<glm::vec4> particleColorBuffer;
	//std::vector<glm::vec3> vertexBuffer;

	void updateSmokeEmitter(const PositionComponent* positionComponent, ParticleEmitterComponent* particleEmitterComponent, float delta_s);
	void addParticle(AddParticleInfo&& addParticle);
	void checkDeadParticles();

public:
	ParticleSystem();

	void init();
	void addEmitter(size_t id);

	void update(const std::vector<PositionComponent>* positionComponents, std::vector<ParticleEmitterComponent>* particleEmitterComponents, float delta_s);
	void render(const std::vector<PositionComponent>* positionComponents, glm::mat4 viewProj, glm::mat4 view);
};