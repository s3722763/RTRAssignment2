#pragma once
#include "../Components/ModelComponent.h"

#include <assimp/scene.h>

#include <map>
#include <string>

enum ModelFlags {

};

class ModelSystem {
	void processNode(aiNode* node, const aiScene* scene, ModelComponent* modelComponent, const std::string& directory, std::map<std::string, GLuint>* loadedTextures);
	void processMesh(aiMesh* mesh, const aiNode* node, const aiScene* scene, ModelComponent* modelComponent, const std::string& directory, std::map<std::string, GLuint>* loadedTextures);
	void processTexture(ModelComponent* modelComponent, aiString textureFile, TextureType type, const std::string& directory, std::map<std::string, GLuint>* loadedTextures);
	void createVAO(ModelComponent* modelComponent, size_t id);
public:
	ModelSystem();
	
	ModelComponent addModel(const std::string& directory, const std::string& modelFileName);
	void removeModel(size_t id);
};