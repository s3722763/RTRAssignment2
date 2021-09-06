#include "Scene.h"
#include <sdl/SDL_events.h>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

constexpr float FOV = 45.0f;
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 1000.0f;

size_t Scene::createEntity(EntityCreateInfo* info) {    
    AddEntityInfo addEntityInfo;

    // Create attributes
    if ((info->flags & EntityCreateInfoFlags::HasModel) != 0) {
       addEntityInfo.modelComponent = std::move(this->modelSystem.addModel(info->directory, info->model));  
    }

    // Store attributes
    size_t id = this->entities.addEntity(&addEntityInfo);

    // Pass ids for atttributes
    if ((info->flags & EntityCreateInfoFlags::Renderable) != 0) {
        this->renderSystem.addRenderableEntity(id);
    }

    return id;
}

void Scene::init() {
    this->renderSystem.init();
}

void Scene::load(Camera camera) {
    EntityCreateInfo info{};
    info.directory = "Resources/models/backpack";
    info.model = "backpack.obj";
    info.flags = EntityCreateInfoFlags::HasModel | EntityCreateInfoFlags::Renderable;
    this->createEntity(&info);

    this->camera = std::move(camera);
}

void Scene::update(float delta_s, SceneUpdateResult* result) {
    SDL_Event e;

    const uint8_t* currentKeyStates = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            *result |= SceneUpdateResultFlags::Quit;
        } else if (e.type == SDL_MOUSEMOTION) {
            // Only move the camera when the window has eaten the mouse
            if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
                const float sensitivity = 0.02f;

                float yawChange = sensitivity * e.motion.xrel;
                float pitchChange = sensitivity * e.motion.yrel;
                //std::cout << e.motion.xrel << " - " << e.motion.yrel << "\n";
                //auto rotation_matrix = glm::eulerAngleXY(glm::radians(this->camera_pitch), glm::radians(this->camera_yaw));
                this->camera.updateLookDirection(pitchChange, yawChange);
            }
        } else if (e.type == SDL_WINDOWEVENT) {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                int width = e.window.data1;
                int height = e.window.data2;

                glViewport(0, 0, width, height);
                break;
            }
        }
    }

    this->camera.updateCamera(delta_s, currentKeyStates);

    if (currentKeyStates[SDL_SCANCODE_Q] == 1) {
        *result |= SceneUpdateResultFlags::Quit;
    }
}

void Scene::draw(SDL_Window* window) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    auto view = this->camera.generateView();
    glm::mat4 proj = glm::perspective(glm::radians(FOV), (float)width / (float)height, NEAR_PLANE, FAR_PLANE);;

    glm::mat4 viewProj = proj * view;

    this->renderSystem.render(this->entities.getModelComponents(), viewProj);
}
