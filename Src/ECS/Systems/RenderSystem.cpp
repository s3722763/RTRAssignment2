#include "RenderSystem.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <string_view>
#include <stb/stb_image.h>
constexpr unsigned int LIGHTING_SYSTEM_BINDING_POINT = 0;

constexpr unsigned int SHADOW_DIMENSIONS = 1024;

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

std::array<glm::vec3, 36> SKYBOX_VERTICES = {
    glm::vec3{-1.0f, 1.0f, -1.0f},
    glm::vec3{-1.0f, -1.0f, -1.0f},
    glm::vec3{1.0f, -1.0f, -1.0f},
    glm::vec3{1.0f, -1.0f, -1.0f},
    glm::vec3{1.0f, 1.0f, -1.0f},
    glm::vec3{-1.0f, 1.0f, -1.0f},

    glm::vec3{-1.0f, -1.0f, 1.0f},
    glm::vec3{-1.0f, -1.0f, -1.0f},
    glm::vec3{-1.0f, 1.0f, -1.0f},
    glm::vec3{-1.0f, 1.0f, -1.0f},
    glm::vec3{-1.0f, 1.0f, 1.0f},
    glm::vec3{-1.0f, -1.0f, 1.0f},

    glm::vec3{1.0f, -1.0f, -1.0f},
    glm::vec3{1.0f, -1.0f, 1.0f},
    glm::vec3{1.0f, 1.0f, 1.0f},
    glm::vec3{1.0f, 1.0f, 1.0f},
    glm::vec3{1.0f, 1.0f, -1.0f},
    glm::vec3{1.0f, -1.0f, -1.0f},

    glm::vec3{-1.0f, -1.0f, 1.0f},
    glm::vec3{-1.0f, 1.0f, 1.0f},
    glm::vec3{1.0f, 1.0f, 1.0f},
    glm::vec3{1.0f, 1.0f, 1.0f},
    glm::vec3{1.0f, -1.0f, 1.0f},
    glm::vec3{-1.0f, -1.0f, 1.0f},

    glm::vec3{-1.0f, 1.0f, -1.0f},
    glm::vec3{1.0f, 1.0f, -1.0f},
    glm::vec3{1.0f, 1.0f, 1.0f},
    glm::vec3{1.0f, 1.0f, 1.0f},
    glm::vec3{-1.0f, 1.0f, 1.0f},
    glm::vec3{-1.0f, 1.0f, -1.0f},

    glm::vec3{-1.0f, -1.0f, -1.0f},
    glm::vec3{-1.0f, -1.0f, 1.0f},
    glm::vec3{1.0f, -1.0f, -1.0f},
    glm::vec3{1.0f, -1.0f, -1.0f},
    glm::vec3{-1.0f, -1.0f, 1.0f},
    glm::vec3{1.0f, -1.0f, 1.0f}
};


std::array<std::string, 6> skyboxFaces = {
    "Resources/Skybox/right.jpg",
    "Resources/Skybox/left.jpg",
    "Resources/Skybox/top.jpg",
    "Resources/Skybox/bottom.jpg",
    "Resources/Skybox/front.jpg",
    "Resources/Skybox/back.jpg",
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

void RenderSystem::loadSkybox() {
    stbi_set_flip_vertically_on_load(false);
    glGenTextures(1, &this->skyboxTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyboxTextureId);

    int width, height, numberChannels;

    for (auto i = 0; i < skyboxFaces.size(); i++) {
        unsigned char* data = stbi_load(skyboxFaces[i].c_str(), &width, &height, &numberChannels, 0);

        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "COULD NOT LOAD TEXTURE: " << skyboxFaces[i] << std::endl;
        }

        stbi_image_free(data);
    }
    stbi_set_flip_vertically_on_load(true);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenVertexArrays(1, &this->skyboxVAO);
    glGenBuffers(1, &this->skyboxVBO);

    glBindVertexArray(this->skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_VERTICES), SKYBOX_VERTICES.data(), GL_STATIC_DRAW);
    GLuint vertexPos = this->skyboxPipeline.getVertexAttribIndex("vertexPos");
    glEnableVertexAttribArray(vertexPos);
    glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    glBindVertexArray(0);
}

void RenderSystem::drawScene(Pipeline* pipeline, const std::vector<ModelComponent>* modelComponents, const std::vector<PositionComponent>* positions) {
    GLuint vertexPosition = pipeline->getVertexAttribIndex("positionVert");
    GLuint texCoordPosition = pipeline->getVertexAttribIndex("texCoordVert");
    GLuint normalPosition = pipeline->getVertexAttribIndex("normalVert");

    for (auto renderableId : this->renderableEntities) {
        const ModelComponent* model = &modelComponents->at(renderableId);

        // Setup model matrix
        for (auto i = 0; i < model->meshes.VAO.size(); i++) {
            //glm::mat4 modelMatrix = modelComponents->at(renderableId).meshes.modelMatrixes.at(i);
            glm::mat4 modelMatrix{ 1.0 };
            modelMatrix = glm::translate(modelMatrix, positions->at(renderableId).WorldPosition);

            // Rotate
            //modelMatrix = modelMatrix * glm::toMat4(positions->at(renderableId).rotation);

            pipeline->setMatrix4x4Uniform("model", modelMatrix);

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

void RenderSystem::setupShadows() {
    glGenFramebuffers(1, &this->shadowFrameBuffer);

    glGenTextures(1, &this->shadowsDepthMap);
    glBindTexture(GL_TEXTURE_2D, this->shadowsDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_DIMENSIONS, SHADOW_DIMENSIONS, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    PipelineCreateInfo info{};
    info.fragmentShaderPath = "Resources/Shaders/shadow.frag";
    info.vertexShaderPath = "Resources/Shaders/shadow.vert";
    info.flags |= (PipelineCreateInfoFlags::VertexShader | PipelineCreateInfoFlags::FragmentShader);
    this->shadowGenerationPipeline.init(&info);

    // Bind depth buffer to texture
    glBindFramebuffer(GL_FRAMEBUFFER, this->shadowFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowsDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSystem::generateShadows(Pipeline* pipeline, const std::vector<ModelComponent>* modelComponents, const std::vector<PositionComponent>* positions, glm::mat4 view, Camera* camera, int width, int height) {
    // Ortho for direction, perspective for point
    std::array<glm::mat4, MAX_LIGHTS> lightSpaceTransforms = this->lightingSystem.generateLightSpaceTransforms(positions);

    glViewport(0, 0, SHADOW_DIMENSIONS, SHADOW_DIMENSIONS);
    glBindFramebuffer(GL_FRAMEBUFFER, this->shadowFrameBuffer);

    glClear(GL_DEPTH_BUFFER_BIT);
    pipeline->use();

    // For each light
    for (auto& lightSpaceMatrix : lightSpaceTransforms) {
        pipeline->setMatrix4x4Uniform("lightSpaceMatrix", lightSpaceMatrix);
        this->drawScene(pipeline, modelComponents, positions);
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

    info.fragmentShaderPath = "Resources/Shaders/skybox.frag";
    info.vertexShaderPath = "Resources/Shaders/skybox.vert";
    info.flags |= (PipelineCreateInfoFlags::VertexShader | PipelineCreateInfoFlags::FragmentShader);
    this->skyboxPipeline.init(&info);

    this->lightingSystem.createBuffer(LIGHTING_SYSTEM_BINDING_POINT);

    this->loadSkybox();
    
    this->quad.init();
    this->cube.init();
    this->particleSystem.init();
    this->setupShadows();
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

void RenderSystem::render(const std::vector<PositionComponent>* positions, const std::vector<ModelComponent>* modelComponents, glm::mat4 viewProj, glm::mat4 view, glm::mat4 proj, Camera* camera, int width, int height) {
    //this->generateShadows(&this->shadowGenerationPipeline, modelComponents, positions, view, camera, width, height);
    glViewport(0, 0, width, height);
    // GEOMETRY PART OF DEFERRED PIPELINE
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->particleSystem.render(positions, viewProj, view);

    this->gBufferPipeline.use();
    this->gBufferPipeline.setMatrix4x4Uniform("viewProj", viewProj);

    this->drawScene(&this->gBufferPipeline, modelComponents, positions);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
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

    // Draw skybox
    glDepthFunc(GL_LEQUAL);
    this->skyboxPipeline.use();
    //this->skyboxPipeline.setMatrix4x4Uniform("viewProj", viewProj);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    glm::mat4 skyboxViewProj = proj * skyboxView;
    this->skyboxPipeline.setMatrix4x4Uniform("viewProj", skyboxViewProj);
    this->skyboxPipeline.setIntUniform("skybox", 0);
    glBindVertexArray(this->skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyboxTextureId);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
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
