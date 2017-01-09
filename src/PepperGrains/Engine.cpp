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
    VkInstance mVkInstance = VK_NULL_HANDLE; // Clean up manually
    VkSurfaceKHR mVkSurface = VK_NULL_HANDLE; // Clean up manually
    VkPhysicalDevice mVkPhysDevice = VK_NULL_HANDLE; // Cleaned up automatically by mVkInstance
    VkQueue mVkGraphicsQueue = VK_NULL_HANDLE; // Cleaned up automatically by mVkInstance
    VkQueue mVkDisplayQueue = VK_NULL_HANDLE; // Cleaned up automatically by mVkInstance
    VkDevice mVkLogicalDevice = VK_NULL_HANDLE; // Clean up manually
    VkSwapchainKHR mVkSwapchain = VK_NULL_HANDLE; // Clean up manually before mVkLogicalDevice
    
    #ifndef NDEBUG
    VkDebugReportCallbackEXT vkDebugReportCallback;
    VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
        VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objecType,
        uint64_t object, size_t location, int32_t code,
        const char* layer, const char* message,
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
        auto proxCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(mVkInstance, "vkCreateDebugReportCallbackEXT"));
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

    // True if this device can be used at all
    bool isVulkanPhysicalDeviceSuitable(const std::vector<const char*>& mRequiredPhysDeviceExts) {
        if(Video::Vulkan::getGraphicsQueueFamilyIndex() == -1 || 
            Video::Vulkan::getDisplayQueueFamilyIndex() == -1) return false;
        
        for(const char* reqExt : mRequiredPhysDeviceExts) {
            bool found = false;
            for(VkExtensionProperties ext : Video::Vulkan::getAvailablePhysicalDeviceExtensions()) {
                if(strcmp(ext.extensionName, reqExt) == 0) {
                    found = true;
                    break;
                }
            }
            if(!found) return false;
        }
        
        // Note: Check after extensions
        if(Video::Vulkan::getAvailablePresentModes().empty() || 
            Video::Vulkan::getAvailableSurfaceFormats().empty()) return false;
        
        // Nothing contradicted the assumption that it is suitable
        return true;
    }
    
    // Used for comparing multiple devices. Scores only nonessential features
    double calcVulkanPhysicalDeviceExtraValue() {
        return 1.0;
    }
    
    VkFormat mIdealVulkanSurfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
    VkColorSpaceKHR mIdealVulkanSurfaceColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkSurfaceFormatKHR findBestSwapSurfaceFormat() {
        const std::vector<VkSurfaceFormatKHR>& formats = Video::Vulkan::getAvailableSurfaceFormats();
        
        assert(formats.size() > 0);
        
        for(VkSurfaceFormatKHR format : formats) {
            if(format.format == mIdealVulkanSurfaceFormat && format.colorSpace == mIdealVulkanSurfaceColorSpace) {
                return format;
            }
        }
        
        VkSurfaceFormatKHR format = formats.at(0);
        if(format.format = VK_FORMAT_UNDEFINED) {
            VkSurfaceFormatKHR retVal; {
                retVal.format = mIdealVulkanSurfaceFormat;
                retVal.colorSpace = mIdealVulkanSurfaceColorSpace;
            }
            return retVal;
        }
        
        return format;
    }
    
    VkPresentModeKHR mIdealVulkanPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    VkPresentModeKHR findBestSwapPresentMode() {
        const std::vector<VkPresentModeKHR>& presents = Video::Vulkan::getAvailablePresentModes();
        
        assert(presents.size() > 0);
        
        for(VkPresentModeKHR present : presents) {
            if(present == mIdealVulkanPresentMode) {
                return present;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    
    VkExtent2D calcBestSwapExtents() {
        VkSurfaceCapabilitiesKHR cap = Video::Vulkan::getSurfaceCapabilities();
        
        // Supports arbitrary sizes
        if(cap.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
            VkExtent2D retVal; {
                retVal.width = std::min(std::max(Video::getWindowWidth(), cap.minImageExtent.width), cap.maxImageExtent.width);
                retVal.height = std::min(std::max(Video::getWindowHeight(), cap.minImageExtent.height), cap.maxImageExtent.height);
            }
            return retVal;
        }
        
        // 
        else {
            return cap.currentExtent;
        }
    }
    
    std::vector<const char*> mRequiredInstanceExts;
    std::vector<const char*> mRequiredInstanceLayers;
    std::vector<const char*> mRequiredPhysDeviceExts;
    bool gapiInitialize() {
        Logger::Out iout = Logger::log(Logger::INFO);
        Logger::Out vout = Logger::log(Logger::VERBOSE);
        Logger::Out sout = Logger::log(Logger::SEVERE);
        
        VkResult result;
        
        // Init globals
        {
            mAppDesc.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            mAppDesc.pNext = nullptr;
            mAppDesc.pApplicationName = "Application Name Here";
            mAppDesc.applicationVersion = VK_MAKE_VERSION(0, 0, 2);
            mAppDesc.pEngineName = "Peppergrains";
            mAppDesc.engineVersion = VK_MAKE_VERSION(0, 0, 1);
            mAppDesc.apiVersion = VK_API_VERSION_1_0;
            
            // Load global vulkan data
            //  Available extensions
            //  Available validation layers
            Video::Vulkan::queryGlobals();
        }
        
        // Init instance
        {
            assert(mRequiredInstanceExts.empty());
            
            // Populate list with GLFW requirements
            #ifdef PGG_GLFW
            {
                uint32_t numGlfwReqExt;
                const char** glfwReqExts = glfwGetRequiredInstanceExtensions(&numGlfwReqExt);
                mRequiredInstanceExts.reserve(numGlfwReqExt);
                for(uint32_t i = 0; i < numGlfwReqExt; ++ i) {
                    mRequiredInstanceExts.push_back(glfwReqExts[i]);
                }
            }
            #endif
            
            // Require debug reports in debug mode
            #ifndef NDEBUG
            mRequiredInstanceExts.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            #endif
            
            // Print verbose debug information
            vout << "Required extensions: " << mRequiredInstanceExts.size() << std::endl;
            vout.indent();
            for(const char* ext : mRequiredInstanceExts) {
                vout << ext << std::endl;
            }
            vout.unindent();
            
            // Ensure that all required extensions are available
            {
                std::vector<const char*> missingExts;
                for(const char* requiredExt : mRequiredInstanceExts) {
                    bool found = false;
                    for(const VkExtensionProperties& availableExt : Video::Vulkan::getAvailableExtensions()) {
                        if(std::strcmp(availableExt.extensionName, requiredExt) == 0) {
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
                    sout.indent();
                    for(const char* missing : missingExts) {
                        sout << missing << std::endl;
                    }
                    sout.unindent();
                }
                // Note: do not return yet, as more debug information can be provided
            }
            
            assert(mRequiredInstanceLayers.empty());
            
            // Standard validation only in debug mode
            #ifndef NDEBUG
            mRequiredInstanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
            #endif // !NDEBUG
            
            // Check validation layers (debug only)
            #ifndef NDEBUG
            {
                std::vector<const char*> missingLayers;
                for(const char* requiredLayer : mRequiredInstanceLayers) {
                    bool found = false;
                    for(const VkLayerProperties& availableLayer : Video::Vulkan::getAvailableLayers()) {
                        if(std::strcmp(availableLayer.layerName, requiredLayer) == 0) {
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
                    sout.indent();
                    for(const char* missing : missingLayers) {
                        sout << missing << std::endl;
                    }
                    sout.unindent();
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
            createArgs.enabledLayerCount = mRequiredInstanceLayers.size();
            createArgs.ppEnabledLayerNames = mRequiredInstanceLayers.data();
            createArgs.enabledExtensionCount = mRequiredInstanceExts.size();
            createArgs.ppEnabledExtensionNames = mRequiredInstanceExts.data();
            
            result = vkCreateInstance(&createArgs, nullptr, &mVkInstance);
            
            if(result != VK_SUCCESS) {
                #ifdef NDEBUG
                sout << "Could not create Vulkan instance" << std::endl;
                #else // !NDEBUG
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
                #endif // !NDEBUG
                return false;
            }
            
            #ifndef NDEBUG
            result = initializeDebugReportCallback();
            if(result != VK_SUCCESS) {
                sout << "Could not initialize debug report callback" << std::endl;
                return false;
            }
            #endif // !NDEBUG
            
            // Read instance-specific data from Vulkan
            //  Physical devices
            Video::Vulkan::queryInstanceSpecific(mVkInstance);
        }
        
        // Init surface
        {
            #ifdef PGG_GLFW
            result = glfwCreateWindowSurface(mVkInstance, mGlfwWindow, nullptr, &mVkSurface);
            #else
            result = VK_FAILURE;
            #endif
            
            if(result != VK_SUCCESS) {
                sout << "Could not create Vulkan surface" << std::endl;
                return false;
            }
            
            /*
            if(mVkSurface == VK_NULL_HANDLE) {
                sout << "Undefined error while creating Vulkan surface" << std::endl;
                return false;
            }
            */
            
            // Read surface-specific data from Vulkan
            Video::Vulkan::querySurfaceSpecific(mVkSurface);
        }
        
        // Init physical device
        {
            assert(mRequiredPhysDeviceExts.empty());
            mRequiredPhysDeviceExts.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            
            // Search for an appropriate physical device
            mVkPhysDevice = VK_NULL_HANDLE;
            {
                double prevBest = -1.0;
                for(VkPhysicalDevice device : Video::Vulkan::getAllPhysicalDevices()) {
                    // Read physical-device specific data from Vulkan
                    Video::Vulkan::queryPhysicalDeviceSpecific(device);
                    
                    if(isVulkanPhysicalDeviceSuitable(mRequiredPhysDeviceExts)) {
                        double extraValue = calcVulkanPhysicalDeviceExtraValue();
                        if(extraValue > prevBest) {
                            mVkPhysDevice = device;
                            prevBest = extraValue;
                        }
                    }
                }
            }
            
            // If no physical device is found
            if(mVkPhysDevice == VK_NULL_HANDLE) {
                sout << "Could not find a GPU meeting requirements" << std::endl;
                Video::Vulkan::queryPhysicalDeviceSpecific(VK_NULL_HANDLE);
                return false;
            }
        }
        
        // Init logical device
        {
            std::vector<VkDeviceQueueCreateInfo> queueCreationInfos;
            
            // This variable must remain in scope until logical device initialization finishes
            float priority = 1.f;
            
            // Build a unique list of creation args for queues
            {
                std::set<int32_t> neededQueueFamilies = {
                    Video::Vulkan::getGraphicsQueueFamilyIndex(), 
                    Video::Vulkan::getDisplayQueueFamilyIndex(), 
                    Video::Vulkan::getComputeQueueFamilyIndex(), 
                    Video::Vulkan::getTransferQueueFamilyIndex(), 
                    Video::Vulkan::getSparseQueueFamilyIndex()
                };
                for(int32_t queueFamilyIndex : neededQueueFamilies) {
                    if(queueFamilyIndex == -1) {
                        continue;
                    }
                    VkDeviceQueueCreateInfo queueCreateArgs; {
                        queueCreateArgs.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                        queueCreateArgs.pNext = nullptr;
                        queueCreateArgs.flags = 0;
                        queueCreateArgs.queueFamilyIndex = queueFamilyIndex;
                        queueCreateArgs.queueCount = 1;
                        queueCreateArgs.pQueuePriorities = &priority;
                    }
                    queueCreationInfos.push_back(queueCreateArgs);
                }
            }
            
            
            VkDeviceCreateInfo deviceArgs;
            deviceArgs.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceArgs.pNext = nullptr;
            deviceArgs.flags = 0;
            
            // 
            deviceArgs.queueCreateInfoCount = queueCreationInfos.size();
            deviceArgs.pQueueCreateInfos = queueCreationInfos.data();
            
            // (Deprecated, Vulkan used to have device-level layers but no longer does)
            deviceArgs.enabledLayerCount = 0;
            deviceArgs.ppEnabledLayerNames = nullptr;
            
            // Use the extension list specified earlier
            deviceArgs.enabledExtensionCount = mRequiredPhysDeviceExts.size();
            deviceArgs.ppEnabledExtensionNames = mRequiredPhysDeviceExts.data();
            
            VkPhysicalDeviceFeatures enabledFeatures; {
                enabledFeatures.alphaToOne = VK_FALSE;
                enabledFeatures.depthBiasClamp = VK_FALSE;
                enabledFeatures.depthBounds = VK_FALSE;
                enabledFeatures.depthClamp = VK_FALSE;
                enabledFeatures.drawIndirectFirstInstance = VK_FALSE;
                enabledFeatures.dualSrcBlend = VK_FALSE;
                enabledFeatures.fillModeNonSolid = VK_FALSE;
                enabledFeatures.fragmentStoresAndAtomics = VK_FALSE;
                enabledFeatures.fullDrawIndexUint32 = VK_FALSE;
                enabledFeatures.geometryShader = VK_FALSE;
                enabledFeatures.imageCubeArray = VK_FALSE;
                enabledFeatures.independentBlend = VK_FALSE;
                enabledFeatures.inheritedQueries = VK_FALSE;
                enabledFeatures.largePoints = VK_FALSE;
                enabledFeatures.logicOp = VK_FALSE;
                enabledFeatures.multiDrawIndirect = VK_FALSE;
                enabledFeatures.multiViewport = VK_FALSE;
                enabledFeatures.occlusionQueryPrecise = VK_FALSE;
                enabledFeatures.pipelineStatisticsQuery = VK_FALSE;
                enabledFeatures.robustBufferAccess = VK_FALSE;
                enabledFeatures.sampleRateShading = VK_FALSE;
                enabledFeatures.samplerAnisotropy = VK_FALSE;
                enabledFeatures.shaderClipDistance = VK_FALSE;
                enabledFeatures.shaderCullDistance = VK_FALSE;
                enabledFeatures.shaderFloat64 = VK_FALSE;
                enabledFeatures.shaderImageGatherExtended = VK_FALSE;
                enabledFeatures.shaderInt16 = VK_FALSE;
                enabledFeatures.shaderInt64 = VK_FALSE;
                enabledFeatures.shaderResourceMinLod = VK_FALSE;
                enabledFeatures.shaderResourceResidency = VK_FALSE;
                enabledFeatures.shaderSampledImageArrayDynamicIndexing = VK_FALSE;
                enabledFeatures.shaderStorageBufferArrayDynamicIndexing = VK_FALSE;
                enabledFeatures.shaderStorageImageArrayDynamicIndexing = VK_FALSE;
                enabledFeatures.shaderStorageImageExtendedFormats = VK_FALSE;
                enabledFeatures.shaderStorageImageMultisample = VK_FALSE;
                enabledFeatures.shaderStorageImageReadWithoutFormat = VK_FALSE;
                enabledFeatures.shaderStorageImageWriteWithoutFormat = VK_FALSE;
                enabledFeatures.shaderTessellationAndGeometryPointSize = VK_FALSE;
                enabledFeatures.shaderUniformBufferArrayDynamicIndexing = VK_FALSE;
                enabledFeatures.sparseBinding = VK_FALSE;
                enabledFeatures.sparseResidency16Samples = VK_FALSE;
                enabledFeatures.sparseResidency2Samples = VK_FALSE;
                enabledFeatures.sparseResidency4Samples = VK_FALSE;
                enabledFeatures.sparseResidency8Samples = VK_FALSE;
                enabledFeatures.sparseResidencyAliased = VK_FALSE;
                enabledFeatures.sparseResidencyBuffer = VK_FALSE;
                enabledFeatures.sparseResidencyImage2D = VK_FALSE;
                enabledFeatures.sparseResidencyImage3D = VK_FALSE;
                enabledFeatures.tessellationShader = VK_FALSE;
                enabledFeatures.textureCompressionASTC_LDR = VK_FALSE;
                enabledFeatures.textureCompressionBC = VK_FALSE;
                enabledFeatures.textureCompressionETC2 = VK_FALSE;
                enabledFeatures.variableMultisampleRate = VK_FALSE;
                enabledFeatures.vertexPipelineStoresAndAtomics = VK_FALSE;
                enabledFeatures.wideLines = VK_FALSE;
            }
            deviceArgs.pEnabledFeatures = &enabledFeatures;
            
            // Finally create the device
            result = vkCreateDevice(mVkPhysDevice, &deviceArgs, nullptr, &mVkLogicalDevice);
            
            // Make sure that logical device initialization was successful
            if(result != VK_SUCCESS) {
                sout << "Could not initialize logical device" << std::endl;
                return false;
            }
            
            // Get the queue family handles for later
            vkGetDeviceQueue(mVkLogicalDevice, Video::Vulkan::getGraphicsQueueFamilyIndex(), 0, &mVkGraphicsQueue);
            vkGetDeviceQueue(mVkLogicalDevice, Video::Vulkan::getDisplayQueueFamilyIndex(), 0, &mVkDisplayQueue);
        
            Video::Vulkan::queryLogicalDeviceSpecific(mVkLogicalDevice);
        }
        
        // Init surface swap chain
        {
            // Query / calculate important data
            VkSurfaceCapabilitiesKHR surfaceCapabilities = Video::Vulkan::getSurfaceCapabilities();
            VkSurfaceFormatKHR surfaceFormat = findBestSwapSurfaceFormat();
            VkPresentModeKHR surfacePresentMode = findBestSwapPresentMode();
            VkExtent2D surfaceExtent = calcBestSwapExtents();
            
            // Hopefully we can get one extra image to use triple buffering
            uint32_t surfaceImageQuantity = surfaceCapabilities.minImageCount + 1;
            
            // Has a hard swap chain quantity limit (max != 0) and our reqest exceeds that maximum
            if(surfaceCapabilities.maxImageCount > 0 && surfaceImageQuantity > surfaceCapabilities.maxImageCount) {
                surfaceImageQuantity = surfaceCapabilities.maxImageCount;
            }
            
            VkSwapchainCreateInfoKHR swapchainCstrArgs; {
                swapchainCstrArgs.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                swapchainCstrArgs.pNext = nullptr;
                
                swapchainCstrArgs.flags = 0;
                
                swapchainCstrArgs.surface = mVkSurface;
                
                // How many presentable images the application needs
                swapchainCstrArgs.minImageCount = surfaceImageQuantity;
                
                // Image qualities
                swapchainCstrArgs.imageFormat = surfaceFormat.format;
                swapchainCstrArgs.imageColorSpace = surfaceFormat.colorSpace;
                swapchainCstrArgs.imageExtent = surfaceExtent;
                
                // Number of views. Used for stereo surfaces
                swapchainCstrArgs.imageArrayLayers = 1;
                
                // How the swapchain's presented images will be used
                swapchainCstrArgs.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            }
            
            std::vector<uint32_t> sharingQueueFamilies;
            sharingQueueFamilies.push_back(Video::Vulkan::getGraphicsQueueFamilyIndex());
            if(Video::Vulkan::getGraphicsQueueFamilyIndex() == Video::Vulkan::getDisplayQueueFamilyIndex()) {
                // The swapchain will only ever be used by one queue family at a time
                swapchainCstrArgs.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                // Note that queueFamilyIndexCount and pQueueFamilyIndices are optional at this point
                
            } else {
                // The swapchain will need to be shared by multiple families
                sharingQueueFamilies.push_back(Video::Vulkan::getDisplayQueueFamilyIndex());
                swapchainCstrArgs.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                
                assert(sharingQueueFamilies.size() >= 2);
            }
            swapchainCstrArgs.queueFamilyIndexCount = sharingQueueFamilies.size();
            swapchainCstrArgs.pQueueFamilyIndices = sharingQueueFamilies.data();
            
            // No transformation
            swapchainCstrArgs.preTransform = surfaceCapabilities.currentTransform;
            
            // Used for blending output with other operating system elements
            swapchainCstrArgs.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            
            // Use the present mode determined earlier
            swapchainCstrArgs.presentMode = surfacePresentMode;
            
            // Does the integrity of pixels not visible, not matter?
            swapchainCstrArgs.clipped = VK_TRUE;
            
            // If there was a previous swap chain in use (there was not) this must be specified appropriately
            swapchainCstrArgs.oldSwapchain = VK_NULL_HANDLE;
            
            result = vkCreateSwapchainKHR(mVkLogicalDevice, &swapchainCstrArgs, nullptr, &mVkSwapchain);
            
            if(result != VK_SUCCESS) {
                sout << "Could not create swapchain" << std::endl;
                return false;
            }
            
            Video::Vulkan::querySwapchainSpecific(mVkSwapchain);
        }
        
        iout << "Vulkan initialized successfully" << std::endl;
        return true;
    }
    bool gapiCleanup() {
        
        Logger::Out sout = Logger::log(Logger::SEVERE);
        Logger::Out wout = Logger::log(Logger::WARN);
        
        #ifndef NDEBUG
        cleanupDebugReportCallback();
        #endif
        
        vkDestroySwapchainKHR(mVkLogicalDevice, mVkSwapchain, nullptr); // Swapchain must be destroyed before logical device
        vkDestroyDevice(mVkLogicalDevice, nullptr); // Logical device must be destroyed before instance
        vkDestroySurfaceKHR(mVkInstance, mVkSurface, nullptr);
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
        //Logger::VERBOSE->setEnabled(false);
        
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
