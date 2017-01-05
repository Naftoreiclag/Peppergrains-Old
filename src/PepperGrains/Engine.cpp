/*
   Copyright 2016-2017 James Fong

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

#include <GraphicsApiStuff.hpp>
#include <WindowInputSystemStuff.hpp>
#include <lua.hpp>

#include "Events.hpp"
#include "MissionGamelayer.hpp"
#include "Video.hpp"
#include "Logger.hpp"
#include "Resources.hpp"
#include "Addons.hpp"
#include "Scripts.hpp"

namespace pgg {
namespace Video {
    extern uint32_t mWindowWidth;
    extern uint32_t mWindowHeight;
}
    
namespace Engine {

    bool mMainLoopRunning;
    void quit() {
        mMainLoopRunning = false;
    }
    
    InputState mInputState;
    
    // wis = Window/Input system
    
    #ifdef PGG_SDL
    SDL_Window* mSdlWindow;
    SDL_Renderer* mSdlRenderer;
    SDL_GLContext mGlContext;
    bool wisInitialize() {
        if(SDL_Init(SDL_INIT_VIDEO) < 0) {
            Logger::log(Logger::SEVERE) << "Could not initalize SDL video" << std::endl;
            return false;
        }
        
        // These attributes must be set before creating any SDL windows
        // Note: the major and minor version are only hints (not guranteed to use 4.3)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        
        // Create the window
        mSdlWindow = SDL_CreateWindow("SDL Window", 
            SDL_WINDOWPOS_CENTERED, 
            SDL_WINDOWPOS_CENTERED, 
            Video::getWindowWidth(), Video::getWindowHeight(), 
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        if(!mSdlWindow) {
            Logger::log(Logger::SEVERE) << "Could not create SDL window" << std::endl;
            return false;
        }
        
        // Create OpenGL context for the renderer
        mGlContext = SDL_GL_CreateContext(mSdlWindow);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetSwapInterval(1);
        
        // Create SDL renderer
        mSdlRenderer = SDL_CreateRenderer(mSdlWindow, -1, 0);
        if(!mSdlRenderer) {
            Logger::log(Logger::SEVERE) << "Could not create SDL renderer" << std::endl;
            return false;
        }
        Video::querySDL(mSdlRenderer);
        
        if(SDL_Init(SDL_INIT_EVENTS) < 0) {
            std::cout << "Could not initialize SDL events" << std::endl;
            return false;
        }
        
        return true;
    }
    inline void wisPollEvents() {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: {
                    mGamelayerMachine.onQuit(QuitEvent(event.quit));
                    quit();
                    break;
                }
                case SDL_TEXTINPUT: {
                    mGamelayerMachine.onTextInput(TextInputEvent(event.text));
                    break;
                }
                
                // Both press and release should trigger the same event
                case SDL_KEYUP:
                case SDL_KEYDOWN: {
                    mGamelayerMachine.onKeyboardEvent(KeyboardEvent(event.key));
                    break;
                }
                case SDL_MOUSEMOTION: {
                    MouseMoveEvent mme(event.motion);
                    mInputState.setMouseDelta(mme.dx, mme.dy);
                    mGamelayerMachine.onMouseMove(MouseMoveEvent(event.motion));
                    break;
                }
                
                // Both press and release should trigger the same event
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN: {
                    mGamelayerMachine.onMouseButton(MouseButtonEvent(event.button));
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    mGamelayerMachine.onMouseWheel(MouseWheelMoveEvent(event.wheel));
                    break;
                }
                case SDL_WINDOWEVENT: {
                    switch(event.window.event) {
                        // This also catches resizing due to API calls
                        case SDL_WINDOWEVENT_SIZE_CHANGED: {
                            WindowResizeEvent windowResizeEvent(event.window);
                            Video::mWindowWidth = windowResizeEvent.width;
                            Video::mWindowHeight = windowResizeEvent.height;
                            mGamelayerMachine.onWindowSizeUpdate(WindowResizeEvent(event.window));
                        }
                    }
                }
                default: break;
            }
        }
    }
    inline void wisPostRender() {
        // Swap buffers (draw everything onto the screen)
        SDL_GL_SwapWindow(mSdlWindow);
    }
    bool wisCleanup() {
        SDL_GL_DeleteContext(mGlContext);
        
        SDL_DestroyWindow(mSdlWindow);
        SDL_DestroyRenderer(mSdlRenderer);
        
        SDL_Quit();
        return true;
    }
    #endif

    #ifdef PGG_GLFW
    GLFWwindow* mGlfwWindow;
    
    void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int heldKeys) {
        mGamelayerMachine.onKeyboardEvent(KeyboardEvent(key, action));
    }
    void glfwCursorPositionCallback(GLFWwindow* window, double x, double y) {
        double width = Video::getWindowWidth();
        double height = Video::getWindowHeight();
        mGamelayerMachine.onMouseMove(MouseMoveEvent(x * width, y * height, mInputState.getMouseDX(), mInputState.getMouseDY()));
    }
    void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int heldKeys) {
        mGamelayerMachine.onMouseButton(MouseButtonEvent(button, action, mInputState.getMouseX(), mInputState.getMouseY()));
    }
    bool wisInitialize() {
        if(!glfwInit()) {
            Logger::log(Logger::SEVERE) << "Could not initialize GLFW" << std::endl;
            return false;
        }
        mGlfwWindow = glfwCreateWindow(
            Video::getWindowWidth(), Video::getWindowHeight(), 
            "GLFW Window", 0, 0);
        if(!mGlfwWindow) {
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(mGlfwWindow);
        glfwSetKeyCallback(mGlfwWindow, glfwKeyCallback);
        glfwSetCursorPosCallback(mGlfwWindow, glfwCursorPositionCallback);
        glfwSetMouseButtonCallback(mGlfwWindow, glfwMouseButtonCallback);
        return true;
    }
    inline void wisPollEvents() {
        glfwPollEvents();
    }
    inline void wisPostRender() {
        // Swap buffers (draw everything onto the screen)
        glfwSwapBuffers(mGlfwWindow);
    }
    bool wisCleanup() {
        glfwTerminate();
        return true;
    }
    #endif
    
    #ifdef PGG_OPENGL
    bool gapiInitialize() {
        // Initialize GLEW, using OpenGL experimental drivers
        glewExperimental = GL_TRUE;
        glewInit();
        
        // Enable OpenGL debug output
        glEnable(GL_DEBUG_OUTPUT);
        
        Video::queryOpenGL();
        
        return true;
    }
    bool gapiCleanup() {
        return true;
    }
    #endif
    
    #ifdef PGG_VULKAN
    bool gapiInitialize() {
        return true;
        
    }
    bool gapiCleanup() {
        return true;
        
    }
    #endif
    
    
    Sound::Endpoint mSoundEndpoint;
    SoundIo* mSndIo;
    bool sndsInitialize() {
        mSndIo = soundio_create();
        soundio_connect(mSndIo); // Connect to the first available backend
        soundio_flush_events(mSndIo); // Initial event flush
        SoundIoDevice* mSndDevice = soundio_get_output_device(mSndIo, soundio_default_output_device_index(mSndIo)); // Use default device
        mSoundEndpoint.setDevice(mSndDevice);
        
        return true;
    }

    GamelayerMachine mGamelayerMachine;
    uint32_t mTotalTicks;
    int run(int argc, char* argv[]) {
        
        Video::mWindowWidth = 1280;
        Video::mWindowHeight = 960;
        
        if(!wisInitialize()) {
            Logger::log(Logger::SEVERE) << "Fatal error initializing windowing/input system" << std::endl;
            return EXIT_FAILURE;
        }
        if(!gapiInitialize()) {
            Logger::log(Logger::SEVERE) << "Fatal error initializing graphics API" << std::endl;
            return EXIT_FAILURE;
        }
        
        if(!sndsInitialize()) {
            Logger::log(Logger::SEVERE) << "Fatal error initializing sound system" << std::endl;
            return EXIT_FAILURE;
        }
        
        Scripts::init();
        
        Resources::loadCore("core/data.package");
        Addons::preloadAddonDirectory("addons");
        Addons::bootstrapAddons();
        Addons::logAddonFailures();
        Addons::clearFailedAddons();

        uint32_t prev = SDL_GetTicks();
        mMainLoopRunning = true;
        InputState mInputState;
        
        float mTps = 0.f;
        float mTpsWeight = 0.85f;
        float mOneSecondTimer;
        
        //mGamelayerMachine.addBottom(new MissionGameLayer(windowWidth, windowHeight));
        
        while(mMainLoopRunning) {
            mInputState.setMouseDelta(0, 0);
            wisPollEvents();
            
            soundio_flush_events(mSndIo);
            
            
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
                
                mInputState.updateKeysFromSDL();
                mInputState.updateMouseFromSDL();
                
                mGamelayerMachine.onTick(tpf, &mInputState);
                mSoundEndpoint.updateSoundThread();

                wisPostRender();
            }
        }
        mGamelayerMachine.removeAll();
        
        Scripts::close();
        
        if(!gapiCleanup()) {
            Logger::log(Logger::SEVERE) << "Fatal error cleaning up graphics API" << std::endl;
            return EXIT_FAILURE;
        }
        
        if(!wisCleanup()) {
            Logger::log(Logger::SEVERE) << "Fatal error cleaning up windowing/input system" << std::endl;
            return EXIT_FAILURE;
        }
        
        return EXIT_SUCCESS;
    }

} // Engine
} // pgg

int main(int argc, char* argv[]) {
    using namespace pgg;
    try {
        return Engine::run(argc, argv);
    } catch(const std::runtime_error& e) {
        Logger::log(Logger::SEVERE) << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
