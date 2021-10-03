#include "ParticleSystem.h"
#include <iostream>
#include <iostream>

const std::array<glm::vec3, 6> QUAD_VERTICES = {
	glm::vec3{-0.005f, -0.005f, 0.0f},
	glm::vec3{0.005f, -0.005f, 0.0f},
	glm::vec3{0.005f, 0.005f, 0.0f},
	glm::vec3{0.005f, 0.005f, 0.0f},
	glm::vec3{-0.005f, 0.005f, 0.0f},
	glm::vec3{-0.005f, -0.005f, 0.0f}
};

void ParticleSystem::updateSmokeEmitter(const PositionComponent* positionComponent, ParticleEmitterComponent* particleEmitterComponent, float delta_s) {
	particleEmitterComponent->timeToNextEmittion -= delta_s;

	if (particleEmitterComponent->timeToNextEmittion <= 0) {
		// Generate particles
		for (int i = 0; i < particleEmitterComponent->particlesGeneratedPerEmittion; i++) {
			float timeToLive = particleEmitterComponent->timeToLiveDistribution(this->gen);
			float velocity = particleEmitterComponent->velocityDistribution(this->gen);

			ParticleComponent particleComponent{};
			particleComponent.color = {1.0, 1.0, 1.0, 1.0};
			particleComponent.size = 1;
			particleComponent.timeToLive = timeToLive;

			float xOffset = this->minusOneToOneDistribution(this->gen);
			float zOffset = this->minusOneToOneDistribution(this->gen);

			float velocityXOffset = std::tanf(particleEmitterComponent->ejectionAngle) * xOffset;
			float velocityZOffset = std::tanf(particleEmitterComponent->ejectionAngle) * zOffset;
			glm::vec3 velocityVector = glm::vec3{ 0.0, 1.0, 0.0 } + glm::vec3{ velocityXOffset, 0, velocityZOffset };
			velocityVector = glm::normalize(velocityVector);

			MovementComponent movementComponent{};
			movementComponent.rotationVelocity = { 0.0, 0.0, 0.0 };
			movementComponent.velocity = velocity * velocityVector;

			PositionComponent positionComponent{};
			positionComponent.WorldPosition = positionComponent.WorldPosition + (particleEmitterComponent->radius * xOffset * glm::vec3{ 1.0, 0.0, 0.0 }) + (particleEmitterComponent->radius * zOffset * glm::vec3{ 0.0, 0.0, 1.0 });

			AddParticleInfo addParticleInfo{};
			addParticleInfo.movementComponent = std::move(movementComponent);
			addParticleInfo.particleComponent = std::move(particleComponent);
			addParticleInfo.positionComponent = std::move(positionComponent);
			
			this->addParticle(std::move(addParticleInfo));
		}

		particleEmitterComponent->timeToNextEmittion = particleEmitterComponent->timeBetweenEjections;
	}
}

void ParticleSystem::addParticle(AddParticleInfo&& addParticle) {
	size_t id = 0;

	if (numberOfParticles >= MAXIMUM_PARTICLES) {
		std::cerr << "AT MAXIMUM PARTICLES. CANNOT CREATE ANY MORE" << std::endl;
		return;
	}

	if (!this->reusableIds.empty()) {
		id = this->reusableIds.front();
		this->reusableIds.pop();
	} else {
		id = numberOfParticles;
	}

	this->usedIds.push_back(id);

	this->particlePositionComponents.rotations.at(id) = std::move(addParticle.positionComponent.rotation);
	this->particlePositionComponents.WorldPositions.at(id) = std::move(addParticle.positionComponent.WorldPosition);

	this->particleMovementComponents.rotationVelocities.at(id) = std::move(addParticle.movementComponent.rotationVelocity);
	this->particleMovementComponents.velocitys.at(id) = std::move(addParticle.movementComponent.velocity);

	this->particleComponents.colors.at(id) = std::move(addParticle.particleComponent.color);
	this->particleComponents.sizes.at(id) = std::move(addParticle.particleComponent.size);
	this->particleComponents.timeToLives.at(id) = std::move(addParticle.particleComponent.timeToLive);

	numberOfParticles += 1;
}

void ParticleSystem::checkDeadParticles() {
	if (this->usedIds.size() == 0) {
		return;
	}

	for (auto iteration = this->usedIds.begin(); iteration != this->usedIds.end(); iteration++) {
		if (this->particleComponents.timeToLives.at(*iteration) <= 0) {
			this->reusableIds.push(*iteration);
			iteration = this->usedIds.erase(iteration);
			this->numberOfParticles -= 1;
			if (iteration == this->usedIds.end()) {
				break;
			}
		}
	}
}

ParticleSystem::ParticleSystem() {
	this->gen = std::mt19937(this->rd());
	this->zeroOneDistribution = std::uniform_real_distribution<float>(0, 1);
	this->minusOneToOneDistribution = std::uniform_real_distribution<float>(-1, 1);

	this->particleComponents.colors.resize(MAXIMUM_PARTICLES);
	this->particleComponents.sizes.resize(MAXIMUM_PARTICLES);
	this->particleComponents.timeToLives.resize(MAXIMUM_PARTICLES);

	this->particleMovementComponents.rotationVelocities.resize(MAXIMUM_PARTICLES);
	this->particleMovementComponents.velocitys.resize(MAXIMUM_PARTICLES);

	this->particlePositionComponents.rotations.resize(MAXIMUM_PARTICLES);
	this->particlePositionComponents.WorldPositions.resize(MAXIMUM_PARTICLES);

	this->particleColorBuffer.resize(MAXIMUM_PARTICLES);
	this->particleWorldPositionBuffer.resize(MAXIMUM_PARTICLES);
}

void ParticleSystem::init() {
	PipelineCreateInfo createInfo{};
	createInfo.fragmentShaderPath = "Resources/Shaders/particle.frag";
	createInfo.vertexShaderPath = "Resources/Shaders/particle.vert";
	createInfo.flags = PipelineCreateInfoFlags::FragmentShader | PipelineCreateInfoFlags::VertexShader;

	this->particlePipeline.init(&createInfo);

	// Setup particle VAO
	GLuint buffers[3];
	glGenBuffers(3, buffers);
	this->particlePositionBufferObject = buffers[0];
	this->particleColorBufferObject = buffers[1];
	this->vertexBufferObject = buffers[2];

	glGenVertexArrays(1, &this->particleVAO);
	glBindVertexArray(this->particleVAO);

	auto a = sizeof(QUAD_VERTICES);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, a, QUAD_VERTICES.data(), GL_STATIC_DRAW);
	GLuint vertexLocation = this->particlePipeline.getVertexAttribIndex("vertexPosition");
	glEnableVertexAttribArray(vertexLocation);
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

	glBindBuffer(GL_ARRAY_BUFFER, this->particlePositionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, this->particleWorldPositionBuffer.size() * sizeof(GLfloat) * 3, this->particleWorldPositionBuffer.data(), GL_STATIC_DRAW);
	GLuint positionLocation = this->particlePipeline.getVertexAttribIndex("worldPosition");
	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
	glVertexAttribDivisor(positionLocation, 1);

	glBindBuffer(GL_ARRAY_BUFFER, this->particleColorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, this->particleColorBuffer.size() * sizeof(GLfloat) * 4, this->particleColorBuffer.data(), GL_STATIC_DRAW);
	GLuint colorLocation = this->particlePipeline.getVertexAttribIndex("color");
	glEnableVertexAttribArray(colorLocation);
	glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
	glVertexAttribDivisor(colorLocation, 1);

	glBindVertexArray(0);
}

void ParticleSystem::addEmitter(size_t id) {
	this->emitterIds.push_back(id);
}

void ParticleSystem::update(const std::vector<PositionComponent>* positionComponents, std::vector<ParticleEmitterComponent>* particleEmitterComponents, float delta_s) {

	for (auto& id : this->usedIds) {
		this->particlePositionComponents.WorldPositions.at(id) += this->particleMovementComponents.velocitys.at(id) * delta_s;
		this->particleComponents.timeToLives.at(id) -= delta_s;
	}

	this->checkDeadParticles();

	for (auto& id : this->emitterIds) {
		if (particleEmitterComponents->at(id).type & ParticleEmitterType::Smoke) {
			this->updateSmokeEmitter(&positionComponents->at(id), &particleEmitterComponents->at(id), delta_s);
		}
	}

	this->particleColorBuffer.clear();
	this->particleWorldPositionBuffer.clear();

	this->particleColorBuffer.resize(MAXIMUM_PARTICLES);
	this->particleWorldPositionBuffer.resize(MAXIMUM_PARTICLES);

	int i = 0;
	for (auto& id : this->usedIds) {
		this->particleColorBuffer.at(i) = this->particleComponents.colors.at(id);
		this->particleWorldPositionBuffer.at(i) = this->particlePositionComponents.WorldPositions.at(id);

		i += 1;
	}

	// Update VAO
	glBindVertexArray(this->particleVAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->particlePositionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, this->particleWorldPositionBuffer.size() * sizeof(GLfloat) * 3, this->particleWorldPositionBuffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, this->particleColorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, this->particleColorBuffer.size() * sizeof(GLfloat) * 4, this->particleColorBuffer.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void ParticleSystem::render(const std::vector<PositionComponent>* positionComponents, glm::mat4 viewProj, glm::mat4 view) {
	//TODO: Sort with camera position
	this->particlePipeline.use();

	this->particlePipeline.setMatrix4x4Uniform("viewProj", viewProj);
	// TODO: Remove this and have seperate uniforms instead
	this->particlePipeline.setMatrix4x4Uniform("view", view);

	glBindVertexArray(this->particleVAO);
	//std::cout << this->numberOfParticles << std::endl;
	glDrawArraysInstanced(GL_TRIANGLES, 0, QUAD_VERTICES.size(), this->numberOfParticles);
	glBindVertexArray(0);
}