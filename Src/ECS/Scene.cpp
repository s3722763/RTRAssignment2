#include "Scene.h"
#include <sdl/SDL_events.h>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <iostream>

constexpr float FOV = 45.0f;
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 1000.0f;

size_t Scene::createEntity(EntityCreateInfo* info) {    
    AddEntityInfo addEntityInfo;

    // Create attributes
    if ((info->flags & EntityCreateInfoFlags::HasModel) != 0) {
       addEntityInfo.modelComponent = std::move(this->modelSystem.addModel(info->directory, info->model));  
    }

    if ((info->flags & EntityCreateInfoFlags::HasPosition) != 0) {
        addEntityInfo.positionComponent = std::move(info->positionComponent);
    }

    addEntityInfo.movementComponent = std::move(info->movementComponent);
    // Store attributes
    size_t id = this->entities.addEntity(&addEntityInfo);

    // Pass ids for atttributes
    if ((info->flags & EntityCreateInfoFlags::Renderable) != 0) {
        this->renderSystem.addRenderableEntity(id);
    }

    if ((info->flags & EntityCreateInfoFlags::Moves) != 0) {
        this->movementSystem.addEntity(id, 0);
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
    info.flags = EntityCreateInfoFlags::HasModel | EntityCreateInfoFlags::Renderable | EntityCreateInfoFlags::HasPosition | EntityCreateInfoFlags::Moves;
    info.positionComponent.WorldPosition = glm::vec3{ 0.0, 0.0, -20 };
    // Pitch, Yaw, Roll
    glm::vec3 initialRotation = { 0.0f,  -glm::pi<float>() / 4, 0.0f };
    info.positionComponent.rotation = initialRotation;
    info.movementComponent.velocity = { 0, 0, -10 };
    this->createEntity(&info);

    EntityCreateInfo lightEntityInfo{};
    lightEntityInfo.positionComponent.WorldPosition = glm::vec3{ 0.0, 0.0, 10.0 };
    lightEntityInfo.flags = EntityCreateInfoFlags::HasPosition;

    size_t id = this->createEntity(&lightEntityInfo);

    LightInfo lightInfo;
    lightInfo.ambient = glm::vec4{ 0.0 };
    lightInfo.entityId = id;
    lightInfo.direction = glm::vec4{ 0.0 };
    lightInfo.specular = glm::vec4{ 0.2, 0.2, 0.2, 0.2 };
    lightInfo.diffuse = glm::vec4{ 0.5, 0.5, 0.5, 1 };

    this->renderSystem.addLight(&lightInfo);

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

    this->movementSystem.update(this->entities.getPositionComponents(), this->entities.getMovementComponents(), delta_s);
    this->renderSystem.update(this->entities.getPositionComponents());

    auto cameraPositon = this->camera.getPosition();
    //std::cout << "Camera position: " << cameraPositon.x << "," << cameraPositon.y << "," << cameraPositon.z << std::endl;
}

void Scene::draw(SDL_Window* window) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    auto view = this->camera.generateView();
    glm::mat4 proj = glm::perspective(glm::radians(FOV), (float)width / (float)height, NEAR_PLANE, FAR_PLANE);;

    glm::mat4 viewProj = proj * view;

    this->renderSystem.render(this->entities.getPositionComponents(), this->entities.getModelComponents(), viewProj, &this->camera);
}
