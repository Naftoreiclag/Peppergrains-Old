/*
   Copyright 2016 James Fong
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "PepperGrains.hpp"

#include <iostream>
#include <sstream>

#include "OpenGLStuff.hpp"
#include "SDL2/SDL.h"

#include "ResourceManager.hpp"
#include "SceneNode.hpp"

#include "TextModel.hpp"
#include "ManualModel.hpp"

namespace pgg {

PepperGrains* PepperGrains::getSingleton() {
    static PepperGrains instance;

    return &instance;
}

PepperGrains::PepperGrains() { }
PepperGrains::~PepperGrains() { }

int PepperGrains::run(int argc, char* argv[]) {

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL Error" << std::endl;
        return -1;
    }

    uint32_t windowWidth = 1280;
    uint32_t windowHeight = 720;
    
    SDL_Window* sdlWindow = SDL_CreateWindow("What, you egg?", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
    
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);

    boost::filesystem::path resourceDef = "../../../resources/data.package";
    ResourceManager* resman = ResourceManager::getSingleton();
    resman->mapAll(resourceDef);

    SceneNode rootNode;
    SceneNode friendNode;
    
    ManualModel* manModel = new ManualModel();
    manModel->grab();

    FontResource* rainstormFont = resman->findFont("Rainstorm.font");
    rainstormFont->grab();

    TextModel* textModel = new TextModel(rainstormFont, "All the world's a stage, ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    textModel->grab();

    TextModel* fpsCounter = new TextModel(rainstormFont, "FPS: Calculating...");
    fpsCounter->grab();

    friendNode.grabModel(resman->findModel("JellyUFO.model"));

    rootNode.addChild(&friendNode);

    SceneNode overlayNode;
    overlayNode.grabModel(resman->findModel("JellyUFO.model"));

    glm::mat4 viewMat = glm::lookAt(glm::vec3(0.f, 2.f, -2.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1280.f / 720.f, 1.f, 10.f);

    glm::mat4 viewMatOverlay;
    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) windowWidth, 0.f, (float) windowHeight);
    glm::mat4 testMM;

    uint32_t prev = SDL_GetTicks();

    float fps = 0.f;
    float fpsWeight = 0.85f;

    float oneSecondTimer = 0.f;

    bool running = true;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = false;
            }
        }

        uint32_t now = SDL_GetTicks();
        float tpf = now - prev;
        tpf /= 1000.f;
        prev = now;

        oneSecondTimer += tpf;

        if(tpf > 0) {
            float fpsNew = 1 / tpf;
            fps = (fps * fpsWeight) + (fpsNew * (1.f - fpsWeight));
        }

        if(oneSecondTimer > 1.f) {
            oneSecondTimer -= 1.f;

            fpsCounter->drop();

            std::stringstream ss;
            ss << "FPS: ";
            ss << (uint32_t) fps;

            fpsCounter = new TextModel(rainstormFont, ss.str());
            fpsCounter->grab();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        rootNode.rotate(glm::vec3(0.0f, 1.0f, 0.0f), (float) tpf);
        rootNode.setLocalScale(glm::vec3(0.5f, 0.5f, 0.5f));
        friendNode.move(glm::vec3(0.f, 0.f, (float) (tpf * 0.3)));
        rootNode.render(viewMat, projMat);
        
        //manModel->render(viewMat, projMat, glm::mat4());

        glClear(GL_DEPTH_BUFFER_BIT);
        //overlayNode.render(viewMatOverlay, projMatOverlay);

        fpsCounter->render(viewMatOverlay, projMatOverlay, testMM);

        // Swap buffers (draw everything onto the screen)
        SDL_GL_SwapWindow(sdlWindow);
    }
    fpsCounter->drop();

    textModel->drop();

    rootNode.dropModel();
    
    rainstormFont->drop();
    
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    
    return 0;
}

}

