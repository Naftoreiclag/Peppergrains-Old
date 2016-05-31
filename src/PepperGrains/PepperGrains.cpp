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

#include "OverworldGameLayer.hpp"
#include "SandboxGameLayer.hpp"
#include "OpenGLContextData.hpp"
#include "SDLRendererData.hpp"

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
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    int glMajorVersion;
    int glMinorVersion;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &glMajorVersion);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &glMinorVersion);
    
    std::cout << "OpenGL Version (Default): " << glMajorVersion << "." << glMinorVersion << std::endl;
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    SDL_Window* sdlWindow;
    SDL_Renderer* sdlRenderer;
    
    SDL_CreateWindowAndRenderer(windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &sdlWindow, &sdlRenderer);
    SDL_SetWindowTitle(sdlWindow, "Window Title");
    SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    
    if(!sdlWindow) {
        std::cout << "SDL Window Error" << std::endl;
        return -1;
    }
    
    // Enable double-buffers
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Create SDL context
    SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow);
    SDL_GL_SetSwapInterval(1);
    
    // Use experimental drivers #yolo
    glewExperimental = GL_TRUE;
    glewInit();
    
    glEnable(GL_DEBUG_OUTPUT);
    
    {
        SDLRendererData* rendData = SDLRendererData::getSingleton();
        rendData->loadData(sdlRenderer);
        const SDLRendererData::SDLInfo& info = rendData->getData();
        
        std::cout << "SDL Renderer name: " << info.name << std::endl;
        std::cout << "SDL Software fallback: " << info.softwareFallback << std::endl;
        std::cout << "SDL Hardware accelerated: " << info.hardwareAccelerated << std::endl;
        std::cout << "SDL Texture renderering: " << info.supportTextureRender << std::endl;
    }
    {
        OpenGLContextData* context = OpenGLContextData::getSingleton();
        
        const OpenGLContextData::OpenGLInfo& info = context->getData();
        
        std::cout << "OpenGL Version (Integral): " << info.iMinorVersion << "." << info.iMajorVersion << std::endl;
        std::cout << "OpenGL Version (String): " << info.sVersion << std::endl;
        std::cout << "OpenGL Debug output enabled: " << info.bDebugOutput << std::endl;
        std::cout << "OpenGL Max draw buffers: " << info.iMaxDrawBuffers << std::endl;
        std::cout << "OpenGL Max color attachments: " << info.iMaxColorAttachments << std::endl;
    }
    
    boost::filesystem::path resourceDef = "../../../resources/data.package";
    ResourceManager* resman = ResourceManager::getSingleton();
    resman->mapAll(resourceDef);

    mGameLayerMachine = new GameLayerMachine();
    
    mGameLayerMachine->addBottom(new OverworldGameLayer(windowWidth, windowHeight));

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
                case SDL_WINDOWEVENT: {
                    switch(event.window.event) {
                        // This also catches resizing due to API calls
                        case SDL_WINDOWEVENT_SIZE_CHANGED: {
                            mGameLayerMachine->onWindowSizeUpdate(event.window);
                        }
                    }
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
    SDL_DestroyRenderer(sdlRenderer);
    
    SDL_Quit();
    
    return 0;
}

}

