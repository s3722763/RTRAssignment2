#include "RenderSystem.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
constexpr unsigned int LIGHTING_SYSTEM_BINDING_POINT = 0;

const std::array<glm::vec3, 4> QUAD_VERTICES = {
    glm::vec3{-1.0f, 1.0f, 0.0f},
    glm::vec3{-1.0f, -1.0f, 0.0f},
    glm::vec3{1.0f, 1.0f, 0.0f},
    glm::vec3{1.0f, -1.0f, 0.0f}
};

const std::array<glm::vec2, 4> QUAD_TEX_COORDS = {
    glm::vec2{0.0f, 1.0f},
    glm::vec2{0.0f, 0.0f},
    glm::vec2{1.0f, 1.0f},
    glm::vec2{1.0f, 0.0f}
};


std::array<glm::vec3, 36> ORIGINAL_SQUARE_VERTICES = {
    // Front
    glm::vec3{0, 0, 0},
    glm::vec3{1, 0, 0},
    glm::vec3{1, 1, 0},
    glm::vec3{1, 1, 0},
    glm::vec3{0, 1, 0},
    glm::vec3{0, 0, 0},
    // Right
    glm::vec3{1, 0, 0},
    glm::vec3{1, 0, -1},
    glm::vec3{1, 1, -1},
    glm::vec3{1, 1, -1},
    glm::vec3{1, 1, 0},
    glm::vec3{1, 0, 0},
    // Back
    glm::vec3{1, 0, -1},
    glm::vec3{0, 0, -1},
    glm::vec3{0, 1, -1},
    glm::vec3{0, 1, -1},
    glm::vec3{1, 1, -1},
    glm::vec3{1, 0, -1},
    // Left
    glm::vec3{0, 0, -1},
    glm::vec3{0, 0, 0},
    glm::vec3{0, 1, 0},
    glm::vec3{0, 1, 0},
    glm::vec3{0, 1, -1},
    glm::vec3{0, 0, -1},
    // Top
    glm::vec3{0, 1, 0},
    glm::vec3{1, 1, 0},
    glm::vec3{1, 1, -1},
    glm::vec3{1, 1, -1},
    glm::vec3{0, 1, -1},
    glm::vec3{0, 1, 0},
    // Bottom
    glm::vec3{0, 0, 0},
    glm::vec3{0, 0, -1},
    glm::vec3{1, 0, -1},
    glm::vec3{1, 0, -1},
    glm::vec3{1, 0, 0},
    glm::vec3{0, 0, 0}
};

void RenderSystem::initFramebuffers(Window* window) {
    glGenFramebuffers(1, &this->gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);

    // GL_RGBA16F as GPIs generally prefer 4-component formats over 3 (due to byte alignment)

    // Position color buffer
    glGenTextures(1, &this->positionColorFrameBuffer);
    glBindTexture(GL_TEXTURE_2D, this->positionColorFrameBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window->width, window->height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->positionColorFrameBuffer, 0);

    // Normal buffer
    glGenTextures(1, &this->normalFrameBuffer);
    glBindTexture(GL_TEXTURE_2D, this->normalFrameBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window->width, window->height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->normalFrameBuffer, 0);

    // Specular color buffer
    glGenTextures(1, &this->specularColorFrameBuffer);
    glBindTexture(GL_TEXTURE_2D, this->specularColorFrameBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window->width, window->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->specularColorFrameBuffer, 0);

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // Depth buffer
    glGenRenderbuffers(1, &this->rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window->width, window->height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::init(Window* window) {
    this->initFramebuffers(window);

    PipelineCreateInfo info{};
    info.fragmentShaderPath = "Resources/Shaders/lighting.frag";
    info.vertexShaderPath = "Resources/Shaders/lighting.vert";
    info.flags |= (PipelineCreateInfoFlags::VertexShader | PipelineCreateInfoFlags::FragmentShader);
    this->lightingPipeline.init(&info);

    //PipelineCreateInfo info{};
    info.fragmentShaderPath = "Resources/Shaders/deferred.frag";
    info.vertexShaderPath = "Resources/Shaders/deferred.vert";
    info.flags |= (PipelineCreateInfoFlags::VertexShader | PipelineCreateInfoFlags::FragmentShader);
    this->gBufferPipeline.init(&info);

    info.fragmentShaderPath = "Resources/Shaders/cube.frag";
    info.vertexShaderPath = "Resources/Shaders/cube.vert";
    info.flags |= (PipelineCreateInfoFlags::VertexShader | PipelineCreateInfoFlags::FragmentShader);
    this->lightCubePipeline.init(&info);

    this->lightingSystem.createBuffer(LIGHTING_SYSTEM_BINDING_POINT);
    
    this->quad.init();
    this->cube.init();
    this->particleSystem.init();
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

void RenderSystem::addParticleEmitter(size_t id) {
    this->particleSystem.addEmitter(id);
}

void RenderSystem::addLight(LightInfo* info) {
    this->lightingSystem.addLight(info);
}

void RenderSystem::update(const std::vector<PositionComponent>* positions, std::vector<ParticleEmitterComponent>* particleEmmitters, float delta_s) {
    this->lightingSystem.update(positions);
    this->particleSystem.update(positions, particleEmmitters, delta_s);
}

void RenderSystem::render(const std::vector<PositionComponent>* positions, const std::vector<ModelComponent>* modelComponents, glm::mat4 viewProj, glm::mat4 view, Camera* camera) {  
    // GEOMETRY PART OF DEFERRED PIPELINE
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    this->particleSystem.render(positions, viewProj, view);

    this->gBufferPipeline.use();
    this->gBufferPipeline.setMatrix4x4Uniform("viewProj", viewProj);

    GLuint vertexPosition = this->gBufferPipeline.getVertexAttribIndex("positionVert");
    GLuint texCoordPosition = this->gBufferPipeline.getVertexAttribIndex("texCoordVert");
    GLuint normalPosition = this->gBufferPipeline.getVertexAttribIndex("normalVert");

    for (auto renderableId : this->renderableEntities) {
        const ModelComponent* model = &modelComponents->at(renderableId);

        // Setup model matrix
        for (auto i = 0; i < model->meshes.VAO.size(); i++) {
            glm::mat4 modelMatrix = modelComponents->at(renderableId).meshes.modelMatrixes.at(i);
            //glm::mat4 modelMatrix{ 1.0 };
            //modelMatrix = glm::translate(modelMatrix, positions->at(renderableId).WorldPosition);
            
            // Rotate
            //modelMatrix = modelMatrix * glm::toMat4(positions->at(renderableId).rotation);

            this->gBufferPipeline.setMatrix4x4Uniform("model", modelMatrix);

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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // LIGHTING PART OF DEFERRED PIPELINE
    this->lightingPipeline.use();
    this->lightingPipeline.setIntUniform("gPosition", 0);
    this->lightingPipeline.setIntUniform("gNormal", 1);
    this->lightingPipeline.setIntUniform("gAlbedoSpec", 2);

    this->lightingPipeline.bindUniformBlock("LightData", LIGHTING_SYSTEM_BINDING_POINT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->positionColorFrameBuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->normalFrameBuffer);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->specularColorFrameBuffer);

    this->gBufferPipeline.setVec3Uniform("viewPos", camera->getPosition());

    glBindVertexArray(this->quad.VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, 2048, 1152, 0, 0, 2048, 1152, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Draw light cubes
    this->lightCubePipeline.use();
    this->lightCubePipeline.setMatrix4x4Uniform("viewProj", viewProj);
    auto lightIds = lightingSystem.getLightEntities();
    glBindVertexArray(this->cube.VAO);

    for (auto& id : *lightIds) {
        glm::mat4 modelMatrix(1.0);
        modelMatrix = glm::translate(modelMatrix, positions->at(id).WorldPosition);
        this->lightCubePipeline.setMatrix4x4Uniform("model", modelMatrix);
       
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
}

void Quad::init() {
    glGenVertexArrays(1, &this->VAO);
    GLuint buffers[2];

    glGenBuffers(2, buffers);
    this->VertexBuffer = buffers[0];
    this->TexCoordBuffer = buffers[1];
    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTICES), &QUAD_VERTICES, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    glBindBuffer(GL_ARRAY_BUFFER, this->TexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_TEX_COORDS), &QUAD_TEX_COORDS, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

    glBindVertexArray(0);
}

void Cube::init() {
    glGenVertexArrays(1, &this->VAO);
    GLuint buffers[1];

    glGenBuffers(1, buffers);
    this->VertexBuffer = buffers[0];
    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ORIGINAL_SQUARE_VERTICES), &ORIGINAL_SQUARE_VERTICES, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    glBindVertexArray(0);
}
