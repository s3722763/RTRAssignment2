#pragma once
#include <vector>
#include "../Components/PositionComponent.h"
#include "../Components/MovementComponent.h"

enum MovementFlags {

};

class MovementSystem {
private:
	std::vector<size_t> entityIds;
	// TODO: Enable changing of flags
	std::vector<uint64_t> flags;

public:
	void addEntity(size_t id, uint64_t flags);
	void update(std::vector<PositionComponent>* positionComponents, std::vector<MovementComponent>* moveComponents, float delta_s);
};