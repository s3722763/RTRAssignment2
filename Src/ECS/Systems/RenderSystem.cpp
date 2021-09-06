#include "RenderSystem.h"

void RenderSystem::init() {
    PipelineCreateInfo info{};
    info.fragmentShaderPath = "Resources/Shaders/fragment.frag";
    info.vertexShaderPath = "Resources/Shaders/vertex.vert";
    info.flags |= (PipelineCreateInfoFlags::VertexShader | PipelineCreateInfoFlags::FragmentShader);
    this->pipeline.init(&info);
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

void RenderSystem::render(const std::vector<ModelComponent>* modelComponents, glm::mat4 viewProj) {
	for (auto renderableId : this->renderableEntities) {
        const ModelComponent* model = &modelComponents->at(renderableId);

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
}
