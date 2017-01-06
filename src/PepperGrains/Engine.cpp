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

#include <cstring>
#include <sstream>
#include <vector>

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

namespace pgg {
namespace Video {
    extern uint32_t mWindowWidth;
    extern uint32_t mWindowHeight;
}
    
namespace Engine {
    
    const char* mName = "Peppergrains";

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
        mSdlWindow = SDL_CreateWindow(mName, 
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
        #ifndef PGG_OPENGL
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        #endif
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        mGlfwWindow = glfwCreateWindow(
            Video::getWindowWidth(), Video::getWindowHeight(), 
            mName, nullptr, nullptr);
        if(!mGlfwWindow) {
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(mGlfwWindow);
        glfwSetKeyCallback(mGlfwWindow, glfwKeyCallback);
        glfwSetCursorPosCallback(mGlfwWindow, glfwCursorPositionCallback);
        glfwSetMouseButtonCallback(mGlfwWindow, glfwMouseButtonCallback);
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
        // Initialize GLEW, using OpenGL experimental drivers
        glewExperimental = GL_TRUE;
        glewInit();
        
        // Enable OpenGL debug output
        glEnable(GL_DEBUG_OUTPUT);
        
        // Query info about our OpenGL environment
        Video::queryOpenGL();
        
        // Everything successful
        Logger::log(Logger::INFO) << "OpenGL initialized successfully" << std::endl;
        return true;
    }
    bool gapiCleanup() {
        
        // Everything successful
        return true;
    }
    #endif
    
    #ifdef PGG_VULKAN
    VkApplicationInfo mAppDesc;
    VkInstance mVkInstance;
    
    #ifndef NDEBUG
    VkDebugReportCallbackEXT vkDebugReportCallback;
    VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objecType,
        uint64_t object,
        size_t location,
        int32_t code,
        const char* layer,
        const char* message,
        void* userData) {
        
        Logger::Channel* out;
        if(flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) out = Logger::VERBOSE;
        if(flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) out = Logger::INFO;
        if(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || 
            flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) out = Logger::WARN;
        if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) out = Logger::SEVERE;
        
        Logger::log(out) << "Vk Validation: " << message << std::endl;
        
        return VK_FALSE;
    }
    VkResult initializeDebugReportCallback() {
        VkDebugReportCallbackCreateInfoEXT reportArgs;
        reportArgs.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        reportArgs.pNext = nullptr;
        reportArgs.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
        reportArgs.pfnCallback = vulkanDebugCallback;
        auto proxCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(mVkInstance, "vkCreateDebugReportCallbackEXT");
        if(proxCreateDebugReportCallbackEXT) {
            return proxCreateDebugReportCallbackEXT(mVkInstance, &reportArgs, nullptr, &vkDebugReportCallback);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
    void cleanupDebugReportCallback() {
        auto proxCreateDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(mVkInstance, "vkDestroyDebugReportCallbackEXT");
        if(proxCreateDebugReportCallbackEXT) {
            proxCreateDebugReportCallbackEXT(mVkInstance, vkDebugReportCallback, nullptr);
        }
    }
    #endif // !NDEBUG
    
    bool gapiInitialize() {
        Logger::Out iout = Logger::log(Logger::INFO);
        Logger::Out vout = Logger::log(Logger::VERBOSE);
        Logger::Out sout = Logger::log(Logger::SEVERE);
        
        VkResult result;
        
        mAppDesc.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        mAppDesc.pNext = nullptr;
        mAppDesc.pApplicationName = "Application Name Here";
        mAppDesc.applicationVersion = VK_MAKE_VERSION(0, 0, 2);
        mAppDesc.pEngineName = "Peppergrains";
        mAppDesc.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        mAppDesc.apiVersion = VK_API_VERSION_1_0;
        
        // Required extensions
        std::vector<const char*> requiredExtensions;
        
        // Populate list with GLFW requirements
        {
            uint32_t numGlfwReqExt;
            const char** glfwReqExts = glfwGetRequiredInstanceExtensions(&numGlfwReqExt);
            requiredExtensions.reserve(numGlfwReqExt);
            for(uint32_t i = 0; i < numGlfwReqExt; ++ i) {
                requiredExtensions.push_back(glfwReqExts[i]);
            }
        }
        
        #ifndef NDEBUG
        requiredExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        #endif
        
        // Print verbose debug information
        vout << "Required extensions: " << requiredExtensions.size() << std::endl;
        for(const char* ext : requiredExtensions) {
            vout << '\t' << ext << std::endl;
        }
        
        // Find supported extensions
        std::vector<VkExtensionProperties> availableExtensions;
        {
            uint32_t numSupportedExt = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &numSupportedExt, nullptr);
            availableExtensions.resize(numSupportedExt);
            vkEnumerateInstanceExtensionProperties(nullptr, &numSupportedExt, availableExtensions.data());
        }
        
        // Print verbose debug information
        vout << "Supported extensions: " << availableExtensions.size() << std::endl;
        for(const VkExtensionProperties& props : availableExtensions) {
            vout << '\t' << props.extensionName << '\t' << props.specVersion << std::endl;
        }
        
        // Ensure that all required extensions are available
        {
            std::vector<const char*> missingExts;
            for(const char* requiredExt : requiredExtensions) {
                bool found = false;
                for(const VkExtensionProperties& props : availableExtensions) {
                    if(std::strcmp(props.extensionName, requiredExt) == 0) {
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    missingExts.push_back(requiredExt);
                }
            }
            if(missingExts.size() > 0) {
                sout << "Missing extensions:" << std::endl;
                for(const char* missing : missingExts) {
                    sout << '\t' << missing << std::endl;
                }
            }
            // Note: do not return yet, as more debug information can be provided
        }
        
        // Validation layers
        std::vector<const char*> requiredLayers;
        
        // Standard validation only in debug mode
        #ifndef NDEBUG
        requiredLayers.push_back("VK_LAYER_LUNARG_standard_validation");
        #endif // !NDEBUG
        
        std::vector<VkLayerProperties> availableLayers;
        
        {
            uint32_t numLayers;
            vkEnumerateInstanceLayerProperties(&numLayers, nullptr);
            availableLayers.resize(numLayers);
            vkEnumerateInstanceLayerProperties(&numLayers, availableLayers.data());
        }
        
        // Check validation layers (debug only)
        #ifndef NDEBUG
        {
            std::vector<const char*> missingLayers;
            for(const char* requiredLayer : requiredLayers) {
                bool found = false;
                for(const VkLayerProperties& props : availableLayers) {
                    if(std::strcmp(props.layerName, requiredLayer) == 0) {
                        found = true;
                        break;
                    }
                }
                if(!found) {
                    missingLayers.push_back(requiredLayer);
                }
            }
            if(missingLayers.size() > 0) {
                sout << "Missing layers:" << std::endl;
                for(const char* missing : missingLayers) {
                    sout << '\t' << missing << std::endl;
                }
            }
            // Note: do not return yet, as more debug information can be provided
        }
        #endif // !NDEBUG
        
        // Request extensions, validation layers and submit application info
        VkInstanceCreateInfo createArgs;
        createArgs.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createArgs.pNext = nullptr;
        createArgs.flags = 0;
        createArgs.pApplicationInfo = &mAppDesc;
        createArgs.enabledLayerCount = requiredLayers.size();
        createArgs.ppEnabledLayerNames = requiredLayers.data();
        createArgs.enabledExtensionCount = requiredExtensions.size();
        createArgs.ppEnabledExtensionNames = requiredExtensions.data();
        
        result = vkCreateInstance(&createArgs, nullptr, &mVkInstance);
        
        if(result != VK_SUCCESS) {
            #ifdef NDEBUG
            sout << "Could not create Vulkan instance" << std::endl;
            #else
            sout << "Could not create Vulkan instance: ";
            switch(result) {
                case VK_ERROR_OUT_OF_HOST_MEMORY: {
                    sout << "Host out of memory";
                    break;
                }
                case VK_ERROR_OUT_OF_DEVICE_MEMORY: {
                    sout << "Device out of memory";
                    break;
                }
                case VK_ERROR_INITIALIZATION_FAILED: {
                    sout << "Initialization failed";
                    break;
                }
                case VK_ERROR_LAYER_NOT_PRESENT: {
                    sout << "Missing Vulkan layer";
                    break;
                }
                case VK_ERROR_EXTENSION_NOT_PRESENT: {
                    sout << "Missing Vulkan extension";
                    break;
                }
                case VK_ERROR_INCOMPATIBLE_DRIVER: {
                    sout << "Incompatible driver";
                    break;
                }
                default: {
                    sout << "Unknown error";
                    break;
                }
            }
            sout << std::endl;
            #endif
            return false;
        }
        
        #ifndef NDEBUG
        result = initializeDebugReportCallback();
        if(result != VK_SUCCESS) {
            sout << "Could not initialize debug report callback" << std::endl;
            return false;
        }
        #endif // !NDEBUG
        
        Logger::log(Logger::INFO) << "Vulkan initialized successfully" << std::endl;
        return true;
    }
    bool gapiCleanup() {
        
        #ifndef NDEBUG
        cleanupDebugReportCallback();
        #endif
        
        vkDestroyInstance(mVkInstance, nullptr);
        
        // Everything successful
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
        
        Scripts::init();
        
        Resources::loadCore("core/data.package");
        Addons::preloadAddonDirectory("addons");
        Addons::bootstrapAddons();
        Addons::logAddonFailures();
        Addons::clearFailedAddons();

        #ifdef PGG_SDL
        uint32_t prev = SDL_GetTicks();
        #endif
        #ifdef PGG_GLFW
        uint32_t prev = 0;
        #endif
        mMainLoopRunning = true;
        InputState mInputState;
        
        float mTps = 0.f;
        float mTpsWeight = 0.85f;
        float mOneSecondTimer;
        
        mGamelayerMachine.addBottom(new MissionGameLayer());
        
        iout << "Finalizing windowing/input system..." << std::endl;
        if(!wisPostInitialize()) {
            sout << "Fatal error finalizing windowing/input system" << std::endl;
            return EXIT_FAILURE;
        }
        while(mMainLoopRunning) {
            mInputState.setMouseDelta(0, 0);
            wisPollEvents();
            
            soundio_flush_events(mSndIo);
            
            
            // It is possible that an event triggered the loop to end
            if(!mMainLoopRunning) {
                
            }
            else {
                #ifdef PGG_SDL
                uint32_t now = SDL_GetTicks();
                #endif
                #ifdef PGG_GLFW
                uint32_t now = 0;
                #endif
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
                    iout << "TPS: " << (uint32_t) mTps << "  \tTick: " << (uint32_t) (tpf * 1000.f) << "ms" << std::endl;
                }
                
                mGamelayerMachine.onTick(tpf, &mInputState);
                mSoundEndpoint.updateSoundThread();

                wisPostRender();
            }
        }
        mGamelayerMachine.removeAll();
        
        Scripts::close();
        
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
