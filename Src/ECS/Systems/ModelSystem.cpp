#include "ModelSystem.h"

#include <array>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

ModelComponent ModelSystem::addModel(const std::string& directory, const std::string& modelFileName) {
	Assimp::Importer importer;
	std::string modelFilepath = directory;
	modelFilepath = modelFilepath.append("/").append(modelFileName);

	const aiScene* scene = importer.ReadFile(modelFilepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	ModelComponent modelComponent;

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Error : Assimp : " << importer.GetErrorString() << "\n";
		return modelComponent;
	}

	std::map<std::string, GLuint> loadedTextures;

	this->processNode(scene->mRootNode, scene, &modelComponent, directory, &loadedTextures);
	return std::move(modelComponent);
}

void ModelSystem::processNode(aiNode* node, const aiScene* scene, ModelComponent* modelComponent, const std::string& directory, std::map<std::string, GLuint>* loadedTextures) {
	for (auto i = 0; i < node->mNumMeshes; i++) {
		std::cout << "Mesh: " << i << std::endl;
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->processMesh(mesh, scene, modelComponent, directory, loadedTextures);
	}

	for (auto i = 0; i < node->mNumChildren; i++) {
		std::cout << "Node: " << i << std::endl;
		this->processNode(node->mChildren[i], scene, modelComponent, directory, loadedTextures);
	}
}

void ModelSystem::processMesh(aiMesh* mesh, const aiScene* scene, ModelComponent* modelComponent, const std::string& directory, std::map<std::string, GLuint>* loadedTextures) {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec2> texCoords;
	std::vector<GLuint> indices;

	vertices.resize(mesh->mNumVertices);
	normals.resize(mesh->mNumVertices);
	tangents.resize(mesh->mNumVertices);
	texCoords.resize(mesh->mNumVertices);
	// ASSUME: each face is a triangle
	indices.resize(3 * mesh->mNumFaces);

	for (auto i = 0; i < mesh->mNumVertices; i++) {
		glm::vec3 vertex = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		glm::vec3 normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		glm::vec3 tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
		glm::vec2 texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

		// TODO: Texcoords
		vertices[i] = std::move(vertex);
		normals[i] = std::move(normal);
		tangents[i] = std::move(tangent);
		texCoords[i] = std::move(texCoord);
	}

	for (auto i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (auto j = 0; j < face.mNumIndices; j++) {
			auto index = (3 * i) + j;
			indices[index] = face.mIndices[j];
		}
	}

	modelComponent->meshes.indices.push_back(std::move(indices));
	modelComponent->meshes.vertices.push_back(std::move(vertices));
	modelComponent->meshes.normals.push_back(std::move(normals));
	modelComponent->meshes.tangent.push_back(std::move(tangents));
	modelComponent->meshes.texCoords.push_back(std::move(texCoords));

	this->createVAO(modelComponent, modelComponent->meshes.vertices.size() - 1);

	// Material processing
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// Diffuse texture
	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 1 && material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
		std::cerr << "Material " << material->GetName().C_Str() << " contains more than 1 diffuse texture." << std::endl;
	}

	aiString path;
	material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
	this->processTexture(modelComponent, path, TextureType::Diffuse, directory, loadedTextures);
}

void ModelSystem::processTexture(ModelComponent* modelComponent, aiString path, TextureType type, const std::string& directory, std::map<std::string, GLuint>* loadedTextures) {
	std::string fullPath = directory;
	fullPath = fullPath.append("/").append(path.C_Str());

	GLuint textureID;
	auto stored	= loadedTextures->find(fullPath);

	if (stored != loadedTextures->end()) {
		textureID = stored->second;
	} else {
		glGenTextures(1, &textureID);

		int width, height, components;
		unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &components, 0);

		if (data) {
			GLenum format;

			if (components == 1) {
				format = GL_RED;
			} else if (components == 3) {
				format = GL_RGB;
			} else if (components == 4) {
				format = GL_RGBA;
			}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			loadedTextures->insert({fullPath, textureID});
			stbi_image_free(data);
		} else {
			std::cerr << "Texture failed to load from path " << fullPath << std::endl;
			stbi_image_free(data);
		}
	}

	modelComponent->meshes.diffuseTextures.push_back(textureID);
}

void ModelSystem::createVAO(ModelComponent* modelComponent, size_t id) {
	std::array<GLuint, 3> buffers;

	glGenBuffers(3, buffers.data());
	GLuint vertexBuffer = buffers[0], indexBuffer = buffers[1], texCoordBuffer = buffers[2], vertexArray = 0;
	glGenVertexArrays(1, &vertexArray);

	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelComponent->meshes.indices.at(id).size() * sizeof(GLuint), modelComponent->meshes.indices.at(id).data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, modelComponent->meshes.vertices.at(id).size() * 3 * sizeof(GLfloat), modelComponent->meshes.vertices.at(id).data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, modelComponent->meshes.texCoords.at(id).size() * 2 * sizeof(GLfloat), modelComponent->meshes.texCoords.at(id).data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	modelComponent->meshes.VAO.push_back(vertexArray);
	modelComponent->meshes.IndexBufferObjects.push_back(indexBuffer);
	modelComponent->meshes.VertexBufferObjects.push_back(vertexBuffer);
	modelComponent->meshes.TextureCoordBufferObjects.push_back(texCoordBuffer);
}

ModelSystem::ModelSystem() {
	stbi_set_flip_vertically_on_load(true);
}

