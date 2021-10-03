#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>

typedef GLuint TextureID;

enum TextureType {
	Diffuse,
	Specular,
	Height,
	Ambient
};

typedef struct MeshComponents {
	std::vector<std::vector<glm::vec3>> vertices;
	std::vector<std::vector<glm::vec3>> normals;
	std::vector<std::vector<glm::vec2>> texCoords;
	std::vector<std::vector<glm::vec3>> tangent;
	std::vector<std::vector<GLuint>> indices;
	// Assuming only one diffuse texture for a mesh
	std::vector<TextureID> diffuseTextures;
	std::vector<TextureID> specularTextures;

	std::vector<glm::mat4> modelMatrixes;

	std::vector<GLuint> VAO;
	std::vector<GLuint> VertexBufferObjects;
	std::vector<GLuint> IndexBufferObjects;
	std::vector<GLuint> TextureCoordBufferObjects;
	std::vector<GLuint> NormalBufferObjects;
} MeshComponents;

struct ModelComponent {
	MeshComponents meshes;
};