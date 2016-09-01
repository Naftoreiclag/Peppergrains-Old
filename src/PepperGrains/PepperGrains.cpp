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

#include "DesignerGameLayer.hpp"
#include "OverworldGameLayer.hpp"
#include "OpenGLContextData.hpp"
#include "SDLRendererData.hpp"
#include "MissionGameLayer.hpp"

namespace pgg {

PepperGrains* PepperGrains::getSingleton() {
    static PepperGrains instance;

    return &instance;
}

PepperGrains::PepperGrains() { }
PepperGrains::~PepperGrains() { }

int PepperGrains::run(int argc, char* argv[]) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Could not initalize SDL video" << std::endl;
        return -1;
    }
    
    mTps = 0.f;
    mTpsWeight = 0.85f;
    
    uint32_t windowWidth = 1280;
    uint32_t windowHeight = 960;
    
    // Initialize sound system
    mSndIo = soundio_create();
    
    // Connect to the first available backend
    soundio_connect(mSndIo);
    
    // Initial event flush
    soundio_flush_events(mSndIo);
    
    // Use default device
    mSndDevice = soundio_get_output_device(mSndIo, soundio_default_output_device_index(mSndIo));
    
    mSndEndpoint = new Sound::Endpoint();
    mSndEndpoint->setDevice(mSndDevice);
    
    // These attributes must be set before creating any SDL windows
    // Note: the major and minor version are only hints (not guranteed to use 4.3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // Create the window
    SDL_Window* sdlWindow = SDL_CreateWindow("Window Title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if(!sdlWindow) {
        std::cout << "Could not create SDL window" << std::endl;
        return -1;
    }
    
    // Create OpenGL context for the renderer
    SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);
    
    // Create SDL renderer
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    if(!sdlRenderer) {
        std::cout << "Could not create SDL renderer" << std::endl;
        return -1;
    }
    
    // Query SDL renderer data
    {
        SDLRendererData* rendData = SDLRendererData::getSingleton();
        rendData->loadData(sdlRenderer);
        const SDLRendererData::SDLInfo& info = rendData->getData();
        
        std::cout << "SDL Renderer name: " << info.name << std::endl;
        std::cout << "SDL Software fallback: " << info.softwareFallback << std::endl;
        std::cout << "SDL Hardware accelerated: " << info.hardwareAccelerated << std::endl;
        std::cout << "SDL Texture renderering: " << info.supportTextureRender << std::endl;
    }
    
    // Initialize GLEW, using OpenGL experimental drivers
    glewExperimental = GL_TRUE;
    glewInit();
    
    // Enable OpenGL debug output
    glEnable(GL_DEBUG_OUTPUT);
    
    // Query OpenGL context data
    {
        OpenGLContextData* context = OpenGLContextData::getSingleton();
        
        const OpenGLContextData::OpenGLInfo& info = context->getData();
        
        std::cout << "OpenGL Version (Integral): " << info.iMinorVersion << "." << info.iMajorVersion << std::endl;
        std::cout << "OpenGL Version (String): " << info.sVersion << std::endl;
        std::cout << "OpenGL Debug output enabled: " << info.bDebugOutput << std::endl;
        std::cout << "OpenGL Max draw buffers: " << info.iMaxDrawBuffers << std::endl;
        std::cout << "OpenGL Max color attachments: " << info.iMaxColorAttachments << std::endl;
    }
    
    // Initialize SDL events
    if(SDL_Init(SDL_INIT_EVENTS) < 0) {
        std::cout << "Could not initialize SDL events" << std::endl;
        return -1;
    }
    
    lua_State* luaState = luaL_newstate();
    
    luaL_openlibs(luaState);
    
    int testError = luaL_loadfile(luaState, "test.lua");
    testError = lua_pcall(luaState, 0, LUA_MULTRET, 0);
    
    boost::filesystem::path resourceDef = "../../../resources/data.package";
    ResourceManager* resman = ResourceManager::getSingleton();
    resman->mapAll(resourceDef);

    mGameLayerMachine = new GameLayerMachine();
    
    mGameLayerMachine->addBottom(new MissionGameLayer());

    uint32_t prev = SDL_GetTicks();
    mMainLoopRunning = true;
    InputState inputState;
    
    while(mMainLoopRunning) {
        inputState.setMouseDelta(0, 0);
        
        soundio_flush_events(mSndIo);
        
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: {
                    mGameLayerMachine->onQuit(QuitEvent(event.quit));
                    mGameLayerMachine->removeAll();
                    mMainLoopRunning = false;
                    break;
                }
                case SDL_TEXTINPUT: {
                    mGameLayerMachine->onTextInput(TextInputEvent(event.text));
                    break;
                }
                
                // Both press and release should trigger the same event
                case SDL_KEYUP:
                case SDL_KEYDOWN: {
                    mGameLayerMachine->onKeyboardEvent(KeyboardEvent(event.key));
                    break;
                }
                case SDL_MOUSEMOTION: {
                    MouseMoveEvent mme(event.motion);
                    inputState.setMouseDelta(mme.dx, mme.dy);
                    mGameLayerMachine->onMouseMove(MouseMoveEvent(event.motion));
                    break;
                }
                
                // Both press and release should trigger the same event
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN: {
                    mGameLayerMachine->onMouseButton(MouseButtonEvent(event.button));
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    mGameLayerMachine->onMouseWheel(MouseWheelMoveEvent(event.wheel));
                    break;
                }
                case SDL_WINDOWEVENT: {
                    switch(event.window.event) {
                        // This also catches resizing due to API calls
                        case SDL_WINDOWEVENT_SIZE_CHANGED: {
                            mGameLayerMachine->onWindowSizeUpdate(WindowResizeEvent(event.window));
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
            
            if(tpf > 0) {
                float fpsNew = 1 / tpf;
                mTps = (mTps * mTpsWeight) + (fpsNew * (1.f - mTpsWeight));
            }
            mOneSecondTimer += tpf;
            if(mOneSecondTimer > 1.f) {
                mOneSecondTimer -= 1.f;
                std::cout << "TPS: " << (uint32_t) mTps << "  \tTick: " << (uint32_t) (tpf * 1000.f) << "ms" << std::endl;
            }
            
            inputState.updateKeysFromSDL();
            inputState.updateMouseFromSDL();
            
            mGameLayerMachine->onTick(tpf, &inputState);
            mSndEndpoint->updateSoundThread();

            // Swap buffers (draw everything onto the screen)
            SDL_GL_SwapWindow(sdlWindow);
        }
    }
    
    
    lua_close(luaState);
    
    SDL_GL_DeleteContext(glContext);
    
    SDL_DestroyWindow(sdlWindow);
    SDL_DestroyRenderer(sdlRenderer);
    
    SDL_Quit();
    
    return 0;
}

uint32_t PepperGrains::getRunningTimeMilliseconds() const {
    return SDL_GetTicks();
}

double PepperGrains::getRunningTimeSeconds() const {
    return ((double) SDL_GetTicks()) * 0.001;
}

}

