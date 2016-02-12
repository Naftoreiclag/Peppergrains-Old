#include <iostream>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include <OpenGLStuff.hpp>
#include <SDL2/SDL.h>

#include "ResourceManager.hpp"
#include "SceneNode.hpp"
    
int main(int argc, char* argv[]) {

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL Error" << std::endl;
        return -1;
    }
    
    SDL_Window* sdlWindow = SDL_CreateWindow("What, you egg?", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    
    if(!sdlWindow) {
        std::cout << "SDL Window Error" << std::endl;
        return -1;
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // Use OpenGL 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    // Enable double-buffers
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Create SDL context
    SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow);
    SDL_GL_SetSwapInterval(1);
    
    // Use experimental drivers #yolo
    glewExperimental = GL_TRUE;
    glewInit();

    glClearColor(0.098f, 0.486f, 0.502f, 1.f);
    glEnable(GL_DEPTH_TEST);

    boost::filesystem::path resourceDef = "../../../resources/data.package";
    ResourceManager* resman = ResourceManager::getSingleton();
    resman->mapAll(resourceDef);

    SceneNode rootNode;

    rootNode.grabModel(resman->findModel("RoseCube.model"));

    glm::mat4 viewMat = glm::lookAt(glm::vec3(0.f, 2.f, -2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1280.f / 720.f, 1.f, 10.f);
    glm::mat4 modelMat;

    uint32_t prev = SDL_GetTicks();

    bool running = true;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }

        uint32_t now = SDL_GetTicks();
        double tps = now - prev;
        tps /= 1000;
        prev = now;


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        modelMat = glm::rotate(modelMat, (float) tps, glm::vec3(0.0f, 1.0f, 0.0f));

        rootNode.rotate(glm::vec3(0.0f, 1.0f, 0.0f), (float) tps);
        rootNode.move(glm::vec3(0.f, 0.f, (float) (tps * 0.3)));
        rootNode.render(viewMat, projMat);


        SDL_GL_SwapWindow(sdlWindow);
    }

    rootNode.dropModel();
    
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    
    return 0;
}
