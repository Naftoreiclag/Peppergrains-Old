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

#include "SandboxGameLayer.hpp"

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
    
    boost::filesystem::path resourceDef = "../../../resources/data.package";
    ResourceManager* resman = ResourceManager::getSingleton();
    resman->mapAll(resourceDef);

    mGameLayerMachine = new GameLayerMachine();
    
    mGameLayerMachine->addBottom(new SandboxGameLayer());

    uint32_t prev = SDL_GetTicks();
    mMainLoopRunning = true;
    while(mMainLoopRunning) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: {
                    mGameLayerMachine->removeAll();
                    mMainLoopRunning = false;
                    break;
                }
                case SDL_TEXTINPUT: {
                    mGameLayerMachine->onTextInput(event.text);
                    break;
                }
                case SDL_KEYDOWN: {
                    mGameLayerMachine->onKeyPress(event.key, event.key.repeat);
                    break;
                }
                case SDL_KEYUP: {
                    mGameLayerMachine->onKeyRelease(event.key);
                    break;
                }
                case SDL_MOUSEMOTION: {
                    mGameLayerMachine->onMouseMove(event.motion);
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    mGameLayerMachine->onMousePress(event.button);
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    mGameLayerMachine->onMouseRelease(event.button);
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    mGameLayerMachine->onMouseWheel(event.wheel);
                    break;
                }
                default: {
                    break;
                }
            }
        }
        
        // It is possible that an event triggered the loop to end
        if(!mMainLoopRunning) {
            
        }
        else {
            uint32_t now = SDL_GetTicks();
            float tpf = now - prev;
            tpf /= 1000.f;
            prev = now;
            
            mGameLayerMachine->onTick(tpf, SDL_GetKeyboardState(NULL));

            // Swap buffers (draw everything onto the screen)
            SDL_GL_SwapWindow(sdlWindow);
        }
    }
    
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    
    return 0;
}

}

