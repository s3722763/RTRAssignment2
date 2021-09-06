#pragma once
#include <glad/glad.h>

template<typename T>
class UniformBlock {
	GLuint uniformBufferObject;

public:
	void createBuffer(T* data, size_t size, unsigned int uniformBindingPoint) {
		glGenBuffers(1, &this->uniformBufferObject);

		glBindBuffer(GL_UNIFORM_BUFFER, this->uniformBufferObject);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, uniformBindingPoint, this->uniformBufferObject, 0, size);
	}

	void update(T* data, size_t size) {
		glBindBuffer(GL_UNIFORM_BUFFER, this->uniformBufferObject);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
};