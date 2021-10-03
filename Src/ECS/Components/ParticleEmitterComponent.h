#pragma once
#include <cstdint>
#include <random>

enum ParticleEmitterType {
	// https://cal.cs.umbc.edu/Courses/CS6967-F08/Papers/Reeves-1983-PSA.pdf
	Smoke = 1 << 0
};

struct ParticleEmitterComponent {
	uint64_t type = 0;
	float radius;
	float ejectionAngle;
	std::uniform_real_distribution<float> timeToLiveDistribution;
	std::uniform_real_distribution<float> velocityDistribution;
	float timeToNextEmittion;
	unsigned int particlesGeneratedPerEmittion;
	float timeBetweenEjections;
};