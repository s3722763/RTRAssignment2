#pragma once
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

class Camera {
	glm::vec3 Position;
	glm::vec3 LookAt;
	glm::vec3 Up;
	float yaw;
	float pitch;

public:
	void init(glm::vec3 initialPosition, glm::vec3 initialLookAt, glm::vec3 initialUp, float initialYaw, float initialPitch);
	void updateCamera(float delta_s, const uint8_t* currentKeyStates);
	void updateLookDirection(float pitchChange, float yawChange);
	glm::mat4 generateView();
	glm::vec3 getPosition();
};