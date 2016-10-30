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

#include "Engine.hpp"

#include <sstream>

#include "OpenGLStuff.hpp"
#include "SDL2/SDL.h"
#include "lua.hpp"

#include "ResourceManager.hpp"

#include "DesignerGameLayer.hpp"
#include "OverworldGameLayer.hpp"
#include "MissionGamelayer.hpp"
#include "Video.hpp"
#include "Logger.hpp"
#include "Resources.hpp"
#include "Addons.hpp"
#include "Scripts.hpp"

namespace pgg {
namespace Engine {

    Sound::Endpoint soundEndpoint;

    GamelayerMachine gamelayerMachine;
    uint32_t mTotalTicks;

    bool mMainLoopRunning;
    void quit() {
        gamelayerMachine.removeAll();
        mMainLoopRunning = false;
    }

    double calcRunTimeSeconds() {
        return SDL_GetTicks();
    }
    uint64_t calcRunTimeMilliseconds() {
        return ((double) SDL_GetTicks()) * 0.001;
    }

    int run(int argc, char* argv[]) {
        if(SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "Could not initalize SDL video" << std::endl;
            return -1;
        }
        
        float mTps;
        float mTpsWeight;
        float mOneSecondTimer;
        
        mTps = 0.f;
        mTpsWeight = 0.85f;
        
        uint32_t windowWidth = 1280;
        uint32_t windowHeight = 960;
        
        // Initialize sound system
        SoundIo* mSndIo = soundio_create();
        
        // Connect to the first available backend
        soundio_connect(mSndIo);
        
        // Initial event flush
        soundio_flush_events(mSndIo);
        
        // Use default device
        SoundIoDevice* mSndDevice = soundio_get_output_device(mSndIo, soundio_default_output_device_index(mSndIo));
        
        soundEndpoint.setDevice(mSndDevice);
        
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
        
        // Initialize GLEW, using OpenGL experimental drivers
        glewExperimental = GL_TRUE;
        glewInit();
        
        // Enable OpenGL debug output
        glEnable(GL_DEBUG_OUTPUT);
        
        Video::queryDriverData(sdlRenderer);
        
        // Initialize SDL events
        if(SDL_Init(SDL_INIT_EVENTS) < 0) {
            std::cout << "Could not initialize SDL events" << std::endl;
            return -1;
        }
        
        Scripts::init();
        
        Scripts::RegRef env = Scripts::newEnvironment();
        Scripts::RegRef ref = Scripts::loadFunc("hello.lua", env);
        Scripts::pushRef(ref);
        Scripts::popCallFuncArgs(0, 0);
        Scripts::unref(ref);
        Scripts::unref(env);
        
        /*
        ref = Scripts::loadFile("hello.lua", false);
        Scripts::pushFunc(ref);
        Scripts::callFunc(0, 0);
        Scripts::unref(ref);
        */
        
        Resources::loadCore("core/data.package");
        Addons::preloadAddonDirectory("addons");
        Addons::bootstrapAddons();
        Addons::logAddonFailures();

        gamelayerMachine.addBottom(new MissionGameLayer(windowWidth, windowHeight));

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
                        gamelayerMachine.onQuit(QuitEvent(event.quit));
                        quit();
                        break;
                    }
                    case SDL_TEXTINPUT: {
                        gamelayerMachine.onTextInput(TextInputEvent(event.text));
                        break;
                    }
                    
                    // Both press and release should trigger the same event
                    case SDL_KEYUP:
                    case SDL_KEYDOWN: {
                        gamelayerMachine.onKeyboardEvent(KeyboardEvent(event.key));
                        break;
                    }
                    case SDL_MOUSEMOTION: {
                        MouseMoveEvent mme(event.motion);
                        inputState.setMouseDelta(mme.dx, mme.dy);
                        gamelayerMachine.onMouseMove(MouseMoveEvent(event.motion));
                        break;
                    }
                    
                    // Both press and release should trigger the same event
                    case SDL_MOUSEBUTTONUP:
                    case SDL_MOUSEBUTTONDOWN: {
                        gamelayerMachine.onMouseButton(MouseButtonEvent(event.button));
                        break;
                    }
                    case SDL_MOUSEWHEEL: {
                        gamelayerMachine.onMouseWheel(MouseWheelMoveEvent(event.wheel));
                        break;
                    }
                    case SDL_WINDOWEVENT: {
                        switch(event.window.event) {
                            // This also catches resizing due to API calls
                            case SDL_WINDOWEVENT_SIZE_CHANGED: {
                                gamelayerMachine.onWindowSizeUpdate(WindowResizeEvent(event.window));
                            }
                        }
                    }
                    default: break;
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
                    Logger::log(Logger::INFO) << "TPS: " << (uint32_t) mTps << "  \tTick: " << (uint32_t) (tpf * 1000.f) << "ms" << std::endl;
                }
                
                inputState.updateKeysFromSDL();
                inputState.updateMouseFromSDL();
                
                gamelayerMachine.onTick(tpf, &inputState);
                soundEndpoint.updateSoundThread();

                // Swap buffers (draw everything onto the screen)
                SDL_GL_SwapWindow(sdlWindow);
            }
        }
        
        Scripts::close();
        
        SDL_GL_DeleteContext(glContext);
        
        SDL_DestroyWindow(sdlWindow);
        SDL_DestroyRenderer(sdlRenderer);
        
        SDL_Quit();
        
        return 0;
    }

} // Engine
} // pgg

int main(int argc, char* argv[]) {
    return pgg::Engine::run(argc, argv);
}
