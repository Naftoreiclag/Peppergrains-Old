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

#include <algorithm>
#include <cstring>
#include <set>
#include <sstream>
#include <vector>
#include <chrono>

#include <GraphicsApiLibrary.hpp>
#include <WindowInputSystemLibrary.hpp>
#include <lua.hpp>

#include "Events.hpp"
#include "MissionGamelayer.hpp"
#include "Video.hpp"
#include "Logger.hpp"
#include "Resources.hpp"
#include "Addons.hpp"
#include "Scripts.hpp"
#include "Input.hpp"

#include "StreamStuff.hpp"

namespace pgg {
namespace Video {
    extern uint32_t mWindowWidth;
    extern uint32_t mWindowHeight;
}
    
namespace Engine {
    
    const char* mEngineName = "Peppergrains";
    const char* mAppName = "Paradise";

    bool mMainLoopRunning;
    void quit() {
        mMainLoopRunning = false;
    }
    
    InputState mInputState;
    
    // Fire resize event only on new frame
    bool mWindowResizeEventQueued = false;
    int32_t mNewWindowWidth;
    int32_t mNewWindowHeight;
    
    // wis = Window/Input system
    
    #ifdef PGG_SDL
    SDL_Window* mSdlWindow;
    void getSdlWindow() { return mSdlWindow; }
    
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
        mSdlWindow = SDL_CreateWindow(mEngineName, 
            SDL_WINDOWPOS_CENTERED, 
            SDL_WINDOWPOS_CENTERED, 
            Video::getWindowWidth(), Video::getWindowHeight(), 
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN);
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
        
        Logger::log(Logger::INFO) << "SDL initialized successfully" << std::endl;
        return true;
    }
    bool wisPostInitialize() {
        SDL_ShowWindow(mSdlWindow);
        return true;
    }
    inline void wisPollEvents() {
        mInputState.updateKeysFromSDL();
        mInputState.updateMouseFromSDL();
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
                            mWindowResizeEventQueued = true;
                            mNewWindowWidth = e.data1;
                            mNewWindowHeight = e.data2;
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
    GLFWwindow* getGlfwWindow() { return mGlfwWindow; }
    void keyCallbackGlfw(GLFWwindow* window, int key, int scancode, int action, int heldKeys) {
        mGamelayerMachine.onKeyboardEvent(KeyboardEvent(key, action));
        
        if(Input::scancodeFromGLFWKey(key) == Input::Scancode::K_Q) {
            quit();
        }
    }
    void cursorPositionCallbackGlfw(GLFWwindow* window, double x, double y) {
        double width = Video::getWindowWidth();
        double height = Video::getWindowHeight();
        mGamelayerMachine.onMouseMove(MouseMoveEvent(x * width, y * height, mInputState.getMouseDX(), mInputState.getMouseDY()));
    }
    void mouseButtonCallbackGlfw(GLFWwindow* window, int button, int action, int heldKeys) {
        mGamelayerMachine.onMouseButton(MouseButtonEvent(button, action, mInputState.getMouseX(), mInputState.getMouseY()));
    }
    void windowResizeCallbackGlfw(GLFWwindow* window, int width, int height) {
        mWindowResizeEventQueued = true;
        mNewWindowWidth = width;
        mNewWindowHeight = height;
    }
    bool wisInitialize() {
        if(!glfwInit()) {
            Logger::log(Logger::SEVERE) << "Could not initialize GLFW" << std::endl;
            return false;
        }
        #ifndef PGG_OPENGL
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        #endif
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        mGlfwWindow = glfwCreateWindow(
            Video::getWindowWidth(), Video::getWindowHeight(), 
            mEngineName, nullptr, nullptr);
        if(!mGlfwWindow) {
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(mGlfwWindow);
        glfwSetKeyCallback(mGlfwWindow, keyCallbackGlfw);
        glfwSetCursorPosCallback(mGlfwWindow, cursorPositionCallbackGlfw);
        glfwSetMouseButtonCallback(mGlfwWindow, mouseButtonCallbackGlfw);
        glfwSetWindowSizeCallback(mGlfwWindow, windowResizeCallbackGlfw);
        Logger::log(Logger::INFO) << "GLFW initialized successfully" << std::endl;
        return true;
    }
    bool wisPostInitialize() {
        glfwShowWindow(mGlfwWindow);
        return true;
    }
    inline void wisPollEvents() {
        if(glfwWindowShouldClose(mGlfwWindow)) {
            mGamelayerMachine.onQuit(QuitEvent());
            quit();
        }
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
        return Video::OpenGL::initialize();
    }
    bool gapiCleanup() {
        return true;
    }
    #endif
    
    #ifdef PGG_VULKAN
    void onVulkanSwapchainInvalidated() {
        mGamelayerMachine.onNeedRebuildRenderPipeline();
    }
    bool gapiInitialize() {
        return Video::Vulkan::initialize();
    }
    bool gapiCleanup() {
        return Video::Vulkan::cleanup();
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
        //Logger::VERBOSE->setEnabled(false);
        
        Logger::Out iout = Logger::log(Logger::INFO);
        Logger::Out sout = Logger::log(Logger::SEVERE);
        
        iout << "Initializing windowing/input system..." << std::endl;
        if(!wisInitialize()) {
            sout << "Fatal error initializing windowing/input system" << std::endl;
            return EXIT_FAILURE;
        }
        iout << "Initializing windowing/input graphics API..." << std::endl;
        if(!gapiInitialize()) {
            sout << "Fatal error initializing graphics API" << std::endl;
            return EXIT_FAILURE;
        }
        iout << "Initializing windowing/input sound system..." << std::endl;
        if(!sndsInitialize()) {
            sout << "Fatal error initializing sound system" << std::endl;
            return EXIT_FAILURE;
        }
        iout << "Initializing lua scripting..." << std::endl;
        if(!Scripts::initialize()) {
            sout << "Fatal error initializing lua scripting" << std::endl;
            return EXIT_FAILURE;
        }
        
        Resources::loadCore("core/data.package");
        
        iout << "Loading and initializing addons..." << std::endl;
        if(!Addons::initialize()) {
            sout << "Fatal error loading and initializing addons" << std::endl;
            return EXIT_FAILURE;
        }
        
        double mTps = 0.f;
        double mTpsWeight = 0.85f;
        double mOneSecondTimer;
        
        mGamelayerMachine.addBottom(new MissionGameLayer());
        
        iout << "Finalizing windowing/input system..." << std::endl;
        if(!wisPostInitialize()) {
            sout << "Fatal error finalizing windowing/input system" << std::endl;
            return EXIT_FAILURE;
        }
        
        iout << "Initialization completed successfully" << std::endl;
        iout << "Running..." << std::endl;

        auto timePrev = std::chrono::steady_clock::now();
        mMainLoopRunning = true;
        while(mMainLoopRunning) {
            mInputState.setMouseDelta(0, 0);
            wisPollEvents();
            
            // TODO: use separate threads for game logic and windowing/rendering to avoid pause while dragging/resizing
            if(mWindowResizeEventQueued) {
                if(mNewWindowWidth < 0) mNewWindowWidth = 0;
                if(mNewWindowHeight < 0) mNewWindowHeight = 0;
                if(mNewWindowWidth != Video::getWindowWidth() || mNewWindowHeight != Video::getWindowHeight()) {
                    Video::onWindowResize(mNewWindowWidth, mNewWindowHeight);
                    mGamelayerMachine.onWindowResize(WindowResizeEvent(mNewWindowWidth, mNewWindowHeight));
                    Logger::log(Logger::VERBOSE) << "Window resized to " << mNewWindowWidth << ", " << mNewWindowHeight << std::endl;
                }
                mWindowResizeEventQueued = false;
            }
            
            soundio_flush_events(mSndIo);
            
            // It is possible that an event triggered the loop to end
            if(mMainLoopRunning) {
                auto timeNow = std::chrono::steady_clock::now();
                std::chrono::duration<double> timeDelta = timeNow - timePrev;
                timePrev = timeNow;
                
                // TPF = Time Per Frame (in seconds per frame)
                double tpf = timeDelta.count();
                
                if(tpf > 0) {
                    float fpsNew = 1 / tpf;
                    mTps = (mTps * mTpsWeight) + (fpsNew * (1.f - mTpsWeight));
                }
                mOneSecondTimer += tpf;
                if(mOneSecondTimer > 1.f) {
                    mOneSecondTimer -= 1.f;
                    iout << "TPS: " << (uint32_t) mTps << "  \tTick: " << (uint32_t) (tpf * 1000.f) << "ms" << std::endl;
                }
                
                mGamelayerMachine.onTick(tpf, &mInputState);
                mSoundEndpoint.updateSoundThread();

                wisPostRender();
            }
        }
        iout << "Terminating..." << std::endl;
        
        #ifdef PGG_VULKAN
        vkDeviceWaitIdle(Video::Vulkan::getLogicalDevice());
        #endif
        
        mGamelayerMachine.removeAll();
        
        iout << "Cleaning up scripts..." << std::endl;
        if(!Scripts::cleanup()) {
            sout << "Fatal error cleaning up scripts" << std::endl;
            return EXIT_FAILURE;
        }
        iout << "Cleaning up graphics API..." << std::endl;
        if(!gapiCleanup()) {
            sout << "Fatal error cleaning up graphics API" << std::endl;
            return EXIT_FAILURE;
        }
        iout << "Cleaning up windowing/input system..." << std::endl;
        if(!wisCleanup()) {
            sout << "Fatal error cleaning up windowing/input system" << std::endl;
            return EXIT_FAILURE;
        }
        
        iout << "Application terminated successfully" << std::endl;
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
