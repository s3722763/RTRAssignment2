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

    if ((info->flags & EntityCreateInfoFlags::Physicalised)) {
        CreatePhysicsComponentOptions cpco{};
        cpco.component = &addEntityInfo.modelComponent;
        cpco.flags = info->physicaliseFlags;
        addEntityInfo.physicsComponent = physicsSystem.generatePhysicsComponent(&cpco);
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

    if ((info->flags & EntityCreateInfoFlags::Physicalised)) {
        this->physicsSystem.addEntity(id);
    }

    return id;
}

void Scene::addBall() {
    EntityCreateInfo info{};
    info.directory = "Resources/models/ball";
    info.model = "ball.obj";
    info.flags = EntityCreateInfoFlags::HasModel | EntityCreateInfoFlags::Renderable | EntityCreateInfoFlags::HasPosition | EntityCreateInfoFlags::Physicalised | EntityCreateInfoFlags::Moves;
    info.physicaliseFlags = PhysicsComponentFlags::AffectedByGravity | PhysicsComponentFlags::CircleBoundingBox;
    info.positionComponent.WorldPosition = glm::vec3{ 1, 1, 0 };

    MovementComponent movementComponent{};
    movementComponent.velocity = { -1.0, 0.0, 0.0 };
    info.movementComponent = movementComponent;

    this->createEntity(&info);

    info.directory = "Resources/models/ball";
    info.model = "ball.obj";
    info.flags = EntityCreateInfoFlags::HasModel | EntityCreateInfoFlags::Renderable | EntityCreateInfoFlags::HasPosition | EntityCreateInfoFlags::Physicalised | EntityCreateInfoFlags::Moves;
    info.physicaliseFlags = PhysicsComponentFlags::AffectedByGravity | PhysicsComponentFlags::CircleBoundingBox;
    info.positionComponent.WorldPosition = glm::vec3{ -1, 1, 0 };

    movementComponent.velocity = { 1.0, 0.0, 0.0 };
    info.movementComponent = movementComponent;

    this->createEntity(&info);
}

void Scene::init(Window* window) {
    this->renderSystem.init(window);
}

void Scene::load(Camera camera) {
    EntityCreateInfo info{};
    info.directory = "Resources/models/pinballmachine";
    info.model = "untitled.obj";
    //info.directory = "Resources/models/backpack";
    //info.model = "backpack.obj";
    info.flags = EntityCreateInfoFlags::HasModel | EntityCreateInfoFlags::Renderable | EntityCreateInfoFlags::HasPosition | EntityCreateInfoFlags::Physicalised;
    info.physicaliseFlags |= PhysicsComponentFlags::AABB | PhysicsComponentFlags::Static;
    info.positionComponent.WorldPosition = glm::vec3{ 0.0, 0.0, 0.0 };
    // Pitch, Yaw, Roll
    glm::vec3 initialRotation = { 0.0f,  -glm::pi<float>() / 4, 0.0f };
    //info.positionComponent.rotation = initialRotation;
    //info.movementComponent.velocity = { 0, 0, -10 };
    this->createEntity(&info);

    EntityCreateInfo lightEntityInfo{};
    lightEntityInfo.positionComponent.WorldPosition = glm::vec3{ -20.0, 20.0, 0.0 };
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

    ParticleEmitterComponent particleEmitterComponent;
    particleEmitterComponent.ejectionAngle = 3.1415 / 10;
    particleEmitterComponent.particlesGeneratedPerEmittion = 1000;
    particleEmitterComponent.radius = 0.2;
    particleEmitterComponent.type = ParticleEmitterType::Smoke;
    particleEmitterComponent.timeToLiveDistribution = std::uniform_real_distribution<float>(0, 1);
    particleEmitterComponent.timeBetweenEjections = 0.02;
    particleEmitterComponent.timeToNextEmittion = 1;
    particleEmitterComponent.velocityDistribution = std::uniform_real_distribution<float>(1, 10);

    PositionComponent positionComponent;
    positionComponent.rotation = { 0, 0, 0 };
    positionComponent.WorldPosition = { 0, 0, 0 };

    AddEntityInfo addEntityInfo;
    addEntityInfo.positionComponent = std::move(positionComponent);
    addEntityInfo.particleEmitterComponent = std::move(particleEmitterComponent);

    //id = this->entities.addEntity(&addEntityInfo);
    //this->renderSystem.addParticleEmitter(id);

    //addEntityInfo.positionComponent.WorldPosition = glm::vec3{ 10.0, 2.0, 0.0 };
    //id = this->entities.addEntity(&addEntityInfo);
    //this->renderSystem.addParticleEmitter(id);

    this->addBall();

    info.directory = "Resources/models/pinballmachine";
    info.model = "sidewall.obj";
    info.flags = EntityCreateInfoFlags::HasModel | EntityCreateInfoFlags::Renderable | EntityCreateInfoFlags::HasPosition | EntityCreateInfoFlags::Physicalised;
    info.physicaliseFlags |= PhysicsComponentFlags::AABB | PhysicsComponentFlags::Static;
    info.positionComponent.WorldPosition = glm::vec3{ 5.25, 0.5, 0 };
    this->createEntity(&info);

    info.directory = "Resources/models/pinballmachine";
    info.model = "sidewall.obj";
    info.flags = EntityCreateInfoFlags::HasModel | EntityCreateInfoFlags::Renderable | EntityCreateInfoFlags::HasPosition | EntityCreateInfoFlags::Physicalised;
    info.physicaliseFlags |= PhysicsComponentFlags::AABB | PhysicsComponentFlags::Static;
    info.positionComponent.WorldPosition = glm::vec3{ -5.25, 0.5, 0 };
    this->createEntity(&info);

    info.directory = "Resources/models/pinballmachine";
    info.model = "topbottomwall.obj";
    info.flags = EntityCreateInfoFlags::HasModel | EntityCreateInfoFlags::Renderable | EntityCreateInfoFlags::HasPosition | EntityCreateInfoFlags::Physicalised;
    info.physicaliseFlags |= PhysicsComponentFlags::AABB | PhysicsComponentFlags::Static;
    info.positionComponent.WorldPosition = glm::vec3{ 0, 0.5, -10.25 };
    this->createEntity(&info);

    info.directory = "Resources/models/pinballmachine";
    info.model = "topbottomwall.obj";
    info.flags = EntityCreateInfoFlags::HasModel | EntityCreateInfoFlags::Renderable | EntityCreateInfoFlags::HasPosition | EntityCreateInfoFlags::Physicalised;
    info.physicaliseFlags |= PhysicsComponentFlags::AABB | PhysicsComponentFlags::Static;
    info.positionComponent.WorldPosition = glm::vec3{ 0, 0.5, 10.25 };
    this->createEntity(&info);
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

    //this->movementSystem.update(this->entities.getPositionComponents(), this->entities.getMovementComponents(), delta_s);
    this->renderSystem.update(this->entities.getPositionComponents(), this->entities.getParticleEmitterComponents(), delta_s);
    this->physicsSystem.update(this->entities.getPhysicsComponents(), this->entities.getMovementComponents(), this->entities.getPositionComponents(), delta_s);
    //std::cout << std::endl;
    auto cameraPositon = this->camera.getPosition();
    //std::cout << "Camera position: " << cameraPositon.x << "," << cameraPositon.y << "," << cameraPositon.z << std::endl;
}

void Scene::draw(SDL_Window* window) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    auto view = this->camera.generateView();
    glm::mat4 proj = glm::perspective(glm::radians(FOV), (float)width / (float)height, NEAR_PLANE, FAR_PLANE);;

    glm::mat4 viewProj = proj * view;

    this->renderSystem.render(this->entities.getPositionComponents(), this->entities.getModelComponents(), viewProj, view, proj, &this->camera, width, height);
}
