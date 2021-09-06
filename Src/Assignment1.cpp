// Assignment1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Assignment1.h"
#include <iostream>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <numeric>

#include "Utility/Camera.h"

#include <assimp/scene.h>

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;
const char* glsl_version = "#version 330 core";

void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}

void Application::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Error initializing SDL: " << SDL_GetError() << "\n";
    } else {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        this->window = SDL_CreateWindow("Assignment 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    
        if (this->window == nullptr) {
            std::cout << "Could not create SDL window: " << SDL_GetError() << "\n";
            SDL_Quit();
        } else {
            this->context = SDL_GL_CreateContext(this->window);
            
            if (this->context == nullptr) {
                std::cout << "Could not create OpenGL context: " << SDL_GetError() << "\n";

                SDL_DestroyWindow(this->window);
                SDL_Quit();
            } else {
                SDL_GL_MakeCurrent(this->window, this->context);
                SDL_SetRelativeMouseMode(SDL_TRUE);
                if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0) {
                    std::cout << "Could not get ProcAddress for GLAD";

                    SDL_GL_DeleteContext(this->context);
                    SDL_DestroyWindow(this->window);
                    SDL_Quit();
                } else {
                    // Disable VSYNC
                    SDL_GL_SetSwapInterval(0);
                    SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);

                    int width, height;
                    SDL_GetWindowSize(this->window, &width, &height);

                    //glEnable(GL_DEBUG_OUTPUT);
                    //glDebugMessageCallback(MessageCallback, 0);

                    glViewport(0, 0, width, height);
                }
            }
        }
    }
}

void Application::run() {
    bool quit = false;
    SceneUpdateResult sceneUpdateResult{};


    while (!quit) {
        std::chrono::steady_clock::time_point static time = std::chrono::steady_clock::now();
        auto current_time = std::chrono::steady_clock::now();
        float duration_seconds = std::chrono::duration<float>(current_time - time).count();
        time = current_time;

        // duration_seconds is equal to the last time it took for the last draw call 

        this->scene.update(duration_seconds, &sceneUpdateResult);

        if ((sceneUpdateResult & SceneUpdateResultFlags::Quit) != 0) {
            quit = true;
            break;
        }

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        // Draw
        this->scene.draw(this->window);
        SDL_GL_SwapWindow(this->window);
    }
}

void Application::load() {

    glm::vec3 initialPosition = { 5, 5, 20 };
    glm::vec3 initialLookAt = { 0, 0, -1 };
    glm::vec3 initialUp = { 0, 1, 0 };
    float initialYaw = -90;
    float initialPitch = 0;

    Camera camera;
    camera.init(std::move(initialPosition), std::move(initialLookAt), std::move(initialUp), initialYaw, initialPitch);

    this->scene.load(std::move(camera));
}

void Application::cleanup() {
    SDL_GL_DeleteContext(this->context);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

int main(int argc, char** argv) {
    Application application{};
    application.init();
    application.load();
    application.run();
    application.cleanup();

    return 0;
}
