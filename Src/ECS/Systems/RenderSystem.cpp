#include "RenderSystem.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

constexpr unsigned int LIGHTING_SYSTEM_BINDING_POINT = 0;

void RenderSystem::init() {
    PipelineCreateInfo info{};
    info.fragmentShaderPath = "Resources/Shaders/fragment.frag";
    info.vertexShaderPath = "Resources/Shaders/vertex.vert";
    info.flags |= (PipelineCreateInfoFlags::VertexShader | PipelineCreateInfoFlags::FragmentShader);
    this->pipeline.init(&info);

    this->lightingSystem.createBuffer(LIGHTING_SYSTEM_BINDING_POINT);
}

void RenderSystem::addRenderableEntity(size_t id) {
	this->renderableEntities.push_back(id);
}

void RenderSystem::removeRenderableEntity(size_t id) {
	for (auto storedId = this->renderableEntities.begin(); storedId != this->renderableEntities.end(); storedId++) {
		if (*storedId == id) {
			this->renderableEntities.erase(storedId);
			break;
		}
	}
}

void RenderSystem::addLight(LightInfo* info) {
    this->lightingSystem.addLight(info);
}

void RenderSystem::update(const std::vector<PositionComponent>* positions) {
    this->lightingSystem.update(positions);
}

void RenderSystem::render(const std::vector<PositionComponent>* positions, const std::vector<ModelComponent>* modelComponents, glm::mat4 viewProj, Camera* camera) {
	for (auto renderableId : this->renderableEntities) {
        const ModelComponent* model = &modelComponents->at(renderableId);

        // Setup model matrix
        glm::mat4 modelMatrix{1.0};
        modelMatrix = glm::translate(modelMatrix, positions->at(renderableId).WorldPosition);
       
        // Rotate
        modelMatrix = modelMatrix * glm::toMat4(positions->at(renderableId).rotation);

        this->pipeline.use();
        this->pipeline.setMatrix4x4Uniform("viewProj", viewProj);
        this->pipeline.setMatrix4x4Uniform("model", modelMatrix);
        this->pipeline.setVec3Uniform("viewPos", camera->getPosition());
        this->pipeline.bindUniformBlock("LightData", LIGHTING_SYSTEM_BINDING_POINT);

        GLuint vertexPosition = this->pipeline.getVertexAttribIndex("positionVert");
        GLuint texCoordPosition = this->pipeline.getVertexAttribIndex("texCoordVert");
        GLuint normalPosition = this->pipeline.getVertexAttribIndex("normalVert");

        for (auto i = 0; i < model->meshes.VAO.size(); i++) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model->meshes.diffuseTextures.at(i));

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, model->meshes.specularTextures.at(i));

            glBindVertexArray(model->meshes.VAO.at(i));

            glBindBuffer(GL_ARRAY_BUFFER, model->meshes.VertexBufferObjects.at(i));
            glEnableVertexAttribArray(vertexPosition);
            glVertexAttribPointer(vertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

            glBindBuffer(GL_ARRAY_BUFFER, model->meshes.TextureCoordBufferObjects.at(i));
            glEnableVertexAttribArray(texCoordPosition);
            glVertexAttribPointer(texCoordPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

            glBindBuffer(GL_ARRAY_BUFFER, model->meshes.NormalBufferObjects.at(i));
            glEnableVertexAttribArray(normalPosition);
            glVertexAttribPointer(normalPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

            // First specified texture in shader
            glDrawElements(GL_TRIANGLES, model->meshes.indices.at(i).size(), GL_UNSIGNED_INT, NULL);
            glBindVertexArray(0);
        }
	}
}
