#include "Camera.h"
#include <cmath>
#include <glm/glm.hpp>
#include <sdl/SDL_keyboard.h>

void Camera::init(glm::vec3 initialPosition, glm::vec3 initialLookAt, glm::vec3 initialUp, float initialYaw, float initialPitch) {
    this->Position = std::move(initialPosition);
    this->LookAt = std::move(initialLookAt);
    this->Up = std::move(initialUp);
    this->yaw = initialYaw;
    this->pitch = initialPitch;
}

void Camera::updateCamera(float delta_s, const uint8_t* currentKeyStates) {
    const float speed = 10 * delta_s;

    if (currentKeyStates[SDL_SCANCODE_W] == 1) {
        this->Position += speed * this->LookAt;
    } else if (currentKeyStates[SDL_SCANCODE_S] == 1) {
        this->Position -= speed * this->LookAt;
    }

    if (currentKeyStates[SDL_SCANCODE_A] == 1) {
        this->Position -= glm::normalize(glm::cross(this->LookAt, this->Up)) * speed;
    } else if (currentKeyStates[SDL_SCANCODE_D] == 1) {
        this->Position += glm::normalize(glm::cross(this->LookAt, this->Up)) * speed;
    }
}

void Camera::updateLookDirection(float pitchChange, float yawChange) {
    this->pitch -= pitchChange;
    this->yaw += yawChange;

    auto new_look_direction = glm::vec3(std::cosf(glm::radians(this->yaw)) * std::cosf(glm::radians(this->pitch)),
        std::sinf(glm::radians(this->pitch)),
        std::sin(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch)));

    this->LookAt = glm::normalize(new_look_direction);
}

glm::mat4 Camera::generateView() {
    return glm::lookAt(this->Position, this->LookAt + this->Position, this->Up);
}

glm::vec3 Camera::getPosition() {
    return this->Position;
}
