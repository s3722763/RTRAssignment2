#include "Entities.h"

size_t Entities::addEntity(AddEntityInfo* info) {
    size_t id = 0;

    if (!this->reusableIds.empty()) {
        id = this->reusableIds.front();
        this->reusableIds.pop();
    } else {
        id = this->usedIds.size();
        // Resize data structures to match new size
        this->models.resize(id + 1);
    }

    this->usedIds.push_back(id);

    this->models.at(id) = std::move(info->modelComponent);

    return id;
}

std::vector<ModelComponent>* Entities::getModelComponents() {
    return &this->models;
}
