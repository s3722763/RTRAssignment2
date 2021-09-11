#include "MovementSystem.h"

void MovementSystem::addEntity(size_t id, size_t flags) {
	this->entityIds.push_back(id);
	this->flags.push_back(flags);
}

void MovementSystem::update(std::vector<PositionComponent>* positionComponents, std::vector<MovementComponent>* moveComponents, float delta_s) {
	for (auto& id : this->entityIds) {
		// TODO: Handle acceleration and rotation
		positionComponents->at(id).WorldPosition += (moveComponents->at(id).velocity * delta_s);
	}
}
