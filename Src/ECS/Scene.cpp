#include "Scene.h"
#include <sdl/SDL_events.h>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

constexpr float FOV = 45.0f;
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 1000.0f;

size_t Scene::createEntity(EntityCreateInfo* info) {
    size_t id = 0;

    if (!this->reusableIds.empty()) {
        id = this->reusableIds.front();
        this->reusableIds.pop();
    } else {
        id = this->usedIds.size();
    }

    this->usedIds.push_back(id);

    if ((info->flags & EntityCreateInfoFlags::HasModel) != 0) {
        size_t modelId;
        this->modelSystem.addModel(info->directory, info->model, &modelId);
    }

    return id;
}

void Scene::load(Camera camera) {
    size_t id;
    this->modelSystem.addModel("Resources/models/backpack", "backpack.obj", &id);
    this->backpackID = id;

    PipelineCreateInfo info{};
    info.fragmentShaderPath = "Resources/Shaders/fragment.frag";
    info.vertexShaderPath = "Resources/Shaders/vertex.vert";
    info.flags |= (PipelineCreateInfoFlags::VertexShader | PipelineCreateInfoFlags::FragmentShader);
    pipeline.init(&info);

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
    auto* model = &this->modelSystem.models.at(this->backpackID);

    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    auto view = this->camera.generateView();
    glm::mat4 proj = glm::perspective(glm::radians(FOV), (float)width / (float)height, NEAR_PLANE, FAR_PLANE);;

    glm::mat4 viewProj = proj * view;

    this->pipeline.use();
    this->pipeline.setMatrix4x4Uniform("viewProj", viewProj);

    GLuint vertexPosition = this->pipeline.getVertexAttribIndex("positionVert");
    GLuint texCoordPosition = this->pipeline.getVertexAttribIndex("texCoordVert");

    for (auto i = 0; i < model->meshes.VAO.size(); i++) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, model->meshes.diffuseTextures.at(i));
        glBindVertexArray(model->meshes.VAO.at(i));

        glBindBuffer(GL_ARRAY_BUFFER, model->meshes.VertexBufferObjects.at(i));
        glEnableVertexAttribArray(vertexPosition);
        glVertexAttribPointer(vertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        glBindBuffer(GL_ARRAY_BUFFER, model->meshes.TextureCoordBufferObjects.at(i));
        glEnableVertexAttribArray(texCoordPosition);
        glVertexAttribPointer(texCoordPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

        // First specified texture in shader
        glDrawElements(GL_TRIANGLES, model->meshes.indices.at(i).size(), GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);
    }


}
