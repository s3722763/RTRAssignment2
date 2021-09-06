#include "RenderSystem.h"

void RenderSystem::addRenderableEntity(size_t id) {
	this->renderableEntities.push_back(id);
}

void RenderSystem::removeRenderableEntity(size_t id) {
	for (auto storedId = this->renderableEntities.begin(); storedId != this->renderableEntities.end(); storedId++) {
		if (*storedId == id) {
			this->renderableEntities.erase(storedId);
			break;
		}
	}
}

void RenderSystem::render(const std::vector<ModelComponent>* modelComponents) {

}
