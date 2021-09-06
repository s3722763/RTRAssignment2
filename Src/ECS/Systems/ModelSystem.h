#pragma once
#include "../Components/ModelComponent.h"

#include <assimp/scene.h>

#include <map>
#include <string>

enum ModelFlags {

};

class ModelSystem {
	void processNode(aiNode* node, const aiScene* scene, ModelComponent* modelComponent, const std::string& directory, std::map<std::string, GLuint>* loadedTextures);
	void processMesh(aiMesh* mesh, const aiScene* scene, ModelComponent* modelComponent, const std::string& directory, std::map<std::string, GLuint>* loadedTextures);
	void processTexture(ModelComponent* modelComponent, aiString textureFile, TextureType type, const std::string& directory, std::map<std::string, GLuint>* loadedTextures);
	void createVAO(ModelComponent* modelComponent, size_t id);
public:
	ModelSystem();
	
	std::vector<ModelComponent> models;
	std::vector<uint64_t> modelFlags;

	void addModel(const std::string& directory, const std::string& modelFileName, size_t* id);
	void removeModel(size_t id);
};