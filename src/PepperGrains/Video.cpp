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

#include "Video.hpp"

#include <cstring>
#include <vector>
#include <set>

#include "Logger.hpp"
#include "Engine.hpp"

namespace pgg {

// Sometimes, we need to be able to signal the engine on graphics-related updates, such as Vulkan swapchain invalidation
// Also needs to call the quit() function to forcefully end upon fatal error
namespace Engine {
    
    void quit();
    
    #ifdef PGG_VULKAN
    void onVulkanSwapchainInvalidated();
    #endif
} // Engine

namespace Video {
    #ifdef PGG_OPENGL
    namespace OpenGL {
        GLint getMajorVersion(GLint x) { static GLint y = x; return y; }
        GLint getMinorVersion(GLint x) { static GLint y = x; return y; }
        GLint getContextFlags(GLint x) { static GLint y = x; return y; }
        
        GLint getMaxAtomicCounterBufferSize(GLint x) { static GLint y = x; return y; } // Min 32
        GLint getMaxAtomicCounterBufferBindings(GLint x) { static GLint y = x; return y; } // Min 1
        GLint getMaxShaderStorageBufferBindings(GLint x) { static GLint y = x; return y; } // Min 8
        GLint getMaxCombinedShaderOutputResources(GLint x) { static GLint y = x; return y; } // Min 8
        GLint getMaxCombinedShaderStorageBlocks(GLint x) { static GLint y = x; return y; } // Min 8
        GLint getMaxUniformBlockSize(GLint x) { static GLint y = x; return y; } // Min 16384
        //GLint getMaxTransformFeedbackBufferBindings(GLint x) { static GLint y = x; return y; }
        GLint getMaxUniformBufferBindings(GLint x) { static GLint y = x; return y; } // Min 84
        GLint getMaxDebugLoggedMessages(GLint x) { static GLint y = x; return y; } // Min 1
        GLint getMaxDebugMessageLength(GLint x) { static GLint y = x; return y; } // Min 1
        GLint getMaxColorAttachments(GLint x) { static GLint y = x; return y; } // Min 8
        GLint getMaxColorTextureSamples(GLint x) { static GLint y = x; return y; } // Min 1
        GLint getMaxDepthTextureSamples(GLint x) { static GLint y = x; return y; } // Min 1
        GLint getMaxDrawBuffers(GLint x) { static GLint y = x; return y; } // Min 8
        GLint getMaxFramebufferHeight(GLint x) { static GLint y = x; return y; } // Min 16384
        GLint getMaxFramebufferWidth(GLint x) { static GLint y = x; return y; } // Min 16384
        GLint getMaxFramebufferLayers(GLint x) { static GLint y = x; return y; } // Min 2048
        GLint getMaxFramebufferSamples(GLint x) { static GLint y = x; return y; } // Min 4
        
        bool isDebugOutputEnabled(bool x) { static bool y = x; return y; }
        
        std::string getVersion(std::string x) { static std::string y = x; return y; }
        std::string getVendor(std::string x) { static std::string y = x; return y; }
        std::string getRenderer(std::string x) { static std::string y = x; return y; }
        std::string getShadingLanguageVersion(std::string x) { static std::string y = x; return y; }
        
        bool supportsGeometryShaders() {
            if(getMajorVersion() > 3) {
                return true;
            }
            if(getMajorVersion() == 3 && getMinorVersion() >= 2) {
                return true;
            }
            return false;
        }
        bool supportsTessellationShaders() {
            return getMajorVersion() >= 4;
        }
        
        void queryOpenGL() {
            GLint gi;
            
            glGetIntegerv(GL_MAJOR_VERSION, &gi); getMajorVersion(gi);
            glGetIntegerv(GL_MINOR_VERSION, &gi); getMinorVersion(gi);
            glGetIntegerv(GL_CONTEXT_FLAGS, &gi); getContextFlags(gi);
            
            glGetIntegerv(GL_ATOMIC_COUNTER_BUFFER_SIZE, &gi); getMaxAtomicCounterBufferSize(gi);
            glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &gi); getMaxAtomicCounterBufferBindings(gi);
            glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &gi); getMaxShaderStorageBufferBindings(gi);
            glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, &gi); getMaxCombinedShaderOutputResources(gi);
            glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &gi); getMaxCombinedShaderStorageBlocks(gi);
            glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &gi); getMaxUniformBlockSize(gi);
            //glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS, &gi); OpenGL(gi);
            glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &gi); getMaxUniformBufferBindings(gi);
            glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES, &gi); getMaxDebugLoggedMessages(gi);
            glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &gi); getMaxDebugMessageLength(gi);
            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &gi); getMaxColorAttachments(gi);
            glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &gi); getMaxColorTextureSamples(gi);
            glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &gi); getMaxDepthTextureSamples(gi);
            glGetIntegerv(GL_MAX_DRAW_BUFFERS, &gi); getMaxDrawBuffers(gi);
            glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &gi); getMaxFramebufferHeight(gi);
            glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &gi); getMaxFramebufferWidth(gi);
            glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &gi); getMaxFramebufferLayers(gi);
            glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &gi); getMaxFramebufferSamples(gi);
            
            isDebugOutputEnabled(glIsEnabled(GL_DEBUG_OUTPUT) == GL_TRUE);
            
            getVersion(std::string((const char*) glGetString(GL_VERSION)));
            getVendor(std::string((const char*) glGetString(GL_VENDOR)));
            getRenderer(std::string((const char*) glGetString(GL_RENDERER)));
            getShadingLanguageVersion(std::string((const char*) glGetString(GL_SHADING_LANGUAGE_VERSION)));
            
            Logger::Out out = Logger::log(Logger::INFO);
            
            out << "OpenGL Version (Integral): " << getMajorVersion() << "." << getMinorVersion() << std::endl;
            out << "OpenGL Version (String): " << getVersion() << std::endl;
            out << "OpenGL Debug output enabled: " << isDebugOutputEnabled() << std::endl;
            out << "OpenGL Max draw buffers: " << getMaxDrawBuffers() << std::endl;
            out << "OpenGL Max color attachments: " << getMaxColorAttachments() << std::endl;
        }
        
        bool initialize() {
            // Initialize GLEW, using OpenGL experimental drivers
            glewExperimental = GL_TRUE;
            glewInit();
            
            // Enable OpenGL debug output
            glEnable(GL_DEBUG_OUTPUT);
            
            // Query info about our OpenGL environment
            Video::OpenGL::queryOpenGL();
            
            // Everything successful
            Logger::log(Logger::INFO) << "OpenGL initialized successfully" << std::endl;
            return true;
        }
    }
    #endif // PGG_OPENGL
    
    #ifdef PGG_VULKAN
    namespace Vulkan {
        VkApplicationInfo mAppDesc; // Basic struct
        VkFormat mVkIdealSurfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
        VkColorSpaceKHR mVkIdealSurfaceColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        VkPresentModeKHR mVkIdealPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        
        // Determined during initialization
        std::vector<const char*> mRequiredInstanceExts;
        std::vector<const char*> mRequiredInstanceLayers;
        std::vector<const char*> mRequiredPhysDeviceExts;
        
        VkInstance mVkInstance = VK_NULL_HANDLE; // Clean up manually
        VkInstance getInstance() { return mVkInstance; }
        VkSurfaceKHR mVkSurface = VK_NULL_HANDLE; // Clean up manually
        VkSurfaceKHR getSurface() { return mVkSurface; }
        VkPhysicalDevice mVkPhysDevice = VK_NULL_HANDLE; // Cleaned up automatically by mVkInstance
        VkPhysicalDevice getPhysicalDevice() { return mVkPhysDevice; }
        VkDevice mVkLogicalDevice = VK_NULL_HANDLE; // Clean up manually
        VkDevice getLogicalDevice() { return mVkLogicalDevice; }
        
        int32_t mQFICompute = -1;
        int32_t getComputeQueueFamilyIndex() { return mQFICompute; }
        VkQueue mVkComputeQueue = VK_NULL_HANDLE; // Cleaned up automatically by mVkInstance
        VkQueue getComputeQueue() { return mVkComputeQueue; }
        int32_t mQFIDisplay = -1;
        int32_t getDisplayQueueFamilyIndex() { return mQFIDisplay; }
        VkQueue mVkDisplayQueue = VK_NULL_HANDLE; // Cleaned up automatically by mVkInstance
        VkQueue getDisplayQueue() { return mVkDisplayQueue; }
        int32_t mQFIGraphics = -1;
        int32_t getGraphicsQueueFamilyIndex() { return mQFIGraphics; }
        VkQueue mVkGraphicsQueue = VK_NULL_HANDLE; // Cleaned up automatically by mVkInstance
        VkQueue getGraphicsQueue() { return mVkGraphicsQueue; }
        int32_t mQFISparse = -1;
        int32_t getSparseQueueFamilyIndex() { return mQFISparse; }
        VkQueue mVkSparseQueue = VK_NULL_HANDLE; // Cleaned up automatically by mVkInstance
        VkQueue getSparseQueue() { return mVkSparseQueue; }
        int32_t mQFITransfer = -1;
        int32_t getTransferQueueFamilyIndex() { return mQFITransfer; }
        VkQueue mVkTransferQueue = VK_NULL_HANDLE; // Cleaned up automatically by mVkInstance
        VkQueue getTransferQueue() { return mVkTransferQueue; }
        
        VkSurfaceCapabilitiesKHR mSurfaceCapabilities;
        VkSurfaceCapabilitiesKHR getSurfaceCapabilities() { return mSurfaceCapabilities; }
        std::vector<VkSurfaceFormatKHR> mAvailableSurfaceFormats;
        const std::vector<VkSurfaceFormatKHR> getAvailableSurfaceFormats() { return mAvailableSurfaceFormats; }
        std::vector<VkPresentModeKHR> mAvailablePresentModes;
        const std::vector<VkPresentModeKHR> getAvailablePresentModes() { return mAvailablePresentModes; }
        
        VkSwapchainKHR mVkSwapchain = VK_NULL_HANDLE; // Clean up manually before mVkLogicalDevice
        VkSwapchainKHR getSwapchain() { return mVkSwapchain; }
        VkFormat mVkSwapchainFormat; // Basic struct
        VkFormat getSwapchainFormat() { return mVkSwapchainFormat; }
        VkExtent2D mVkSwapchainExtent; // Basic struct
        VkExtent2D getSwapchainExtent() { return mVkSwapchainExtent; }
        std::vector<VkImageView> mSwapchainImageViews; // Clean up manually
        const std::vector<VkImageView>& getSwapchainImageViews() { return mSwapchainImageViews; }
        std::vector<VkImage> mSwapchainImages; // Cleaned up automatically by mVkSwapchain
        const std::vector<VkImage>& getSwapchainImages() { return mSwapchainImages; }
        
        std::vector<VkExtensionProperties> mExtensionProperties;
        const std::vector<VkExtensionProperties>& getAvailableExtensions() { return mExtensionProperties; }
        std::vector<VkLayerProperties> mLayerProperties;
        const std::vector<VkLayerProperties>& getAvailableLayers() { return mLayerProperties; }
        std::vector<VkPhysicalDevice> mPhysicalDevices;
        const std::vector<VkPhysicalDevice>& getAllPhysicalDevices() { return mPhysicalDevices; }
        
        std::vector<VkExtensionProperties> mPhysicalDeviceExts;
        const std::vector<VkExtensionProperties>& getAvailablePhysicalDeviceExtensions() { return mPhysicalDeviceExts; }
        VkPhysicalDeviceProperties mPhysicalDeviceProperties;
        VkPhysicalDeviceProperties getPhysicalDeviceProperties() { return mPhysicalDeviceProperties; }
        VkPhysicalDeviceFeatures mPhysicalDeviceFeatures;
        VkPhysicalDeviceFeatures getPhysicalDeviceFeatures() { return mPhysicalDeviceFeatures; }
        VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
        VkPhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties() { return mPhysicalDeviceMemoryProperties; }
        
        std::vector<VkQueueFamilyProperties> mQueueFamilies;
        const std::vector<VkQueueFamilyProperties>& getQueueFamilies() { return mQueueFamilies; }
        
        // Queries important data from Vulkan
        void queryGlobals() {
            Logger::Out iout = Logger::log(Logger::INFO);
            Logger::Out vout = Logger::log(Logger::VERBOSE);
            
            uint32_t numExt;
            vkEnumerateInstanceExtensionProperties(nullptr, &numExt, nullptr);
            mExtensionProperties.resize(numExt);
            vkEnumerateInstanceExtensionProperties(nullptr, &numExt, mExtensionProperties.data());
            iout << "Supported extensions: " << mExtensionProperties.size() << std::endl;
            vout.indent();
            for(const VkExtensionProperties& props : mExtensionProperties) {
                vout << props.extensionName << '\t' << props.specVersion << std::endl;
            }
            vout.unindent();
            
            uint32_t numLayers;
            vkEnumerateInstanceLayerProperties(&numLayers, nullptr);
            mLayerProperties.resize(numLayers);
            vkEnumerateInstanceLayerProperties(&numLayers, mLayerProperties.data());
            iout << "Supported validation layers: " << mLayerProperties.size() << std::endl;
            vout.indent();
            for(const VkLayerProperties& props : mLayerProperties) {
                vout << props.layerName << std::endl;
                vout.indent();
                vout << "Spec: " << props.specVersion << std::endl;
                vout << "Implementation: " << props.implementationVersion << std::endl;
                std::string desc = props.description;
                if(desc.length() > 0) {
                    vout << "Desc: " << props.description << std::endl;
                }
                vout.unindent();
            }
            vout.unindent();
        }
        void queryInstanceSpecific() {
            Logger::Out iout = Logger::log(Logger::INFO);
            Logger::Out vout = Logger::log(Logger::VERBOSE);
            
            if(mVkInstance == VK_NULL_HANDLE) {
                mPhysicalDevices.clear();
                return;
            }
            
            uint32_t numDevices;
            vkEnumeratePhysicalDevices(mVkInstance, &numDevices, nullptr);
            mPhysicalDevices.resize(numDevices);
            vkEnumeratePhysicalDevices(mVkInstance, &numDevices, mPhysicalDevices.data());
            iout << "Available physical devices: " << mPhysicalDevices.size() << std::endl;
            vout.indent();
            for(const VkPhysicalDevice& device : mPhysicalDevices) {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(device, &properties);
                VkPhysicalDeviceFeatures features;
                vkGetPhysicalDeviceFeatures(device, &features);
                vout << properties.deviceName << std::endl;
                vout.indent();
                    vout << "API version: " << properties.apiVersion << std::endl;
                    vout << "Driver version: " << properties.driverVersion << std::endl;
                    vout << "Device type: ";
                    switch(properties.deviceType) {
                        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: {
                            vout << " Integrated GPU" << std::endl;
                            break;
                        }
                        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: {
                            vout << " Discrete GPU" << std::endl;
                            break;
                        }
                        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: {
                            vout << " Virtual GPI" << std::endl;
                            break;
                        }
                        case VK_PHYSICAL_DEVICE_TYPE_CPU: {
                            vout << " CPU" << std::endl;
                            break;
                        }
                        default: {
                            vout << " Other" << std::endl;
                            break;
                        }
                    }
                    #ifndef NDEBUG
                    vout << "alphaToOne: "                              << features.alphaToOne << std::endl;
                    vout << "depthBiasClamp: "                          << features.depthBiasClamp << std::endl;
                    vout << "depthBounds: "                             << features.depthBounds << std::endl;
                    vout << "depthClamp: "                              << features.depthClamp << std::endl;
                    vout << "drawIndirectFirstInstance: "               << features.drawIndirectFirstInstance << std::endl;
                    vout << "dualSrcBlend: "                            << features.dualSrcBlend << std::endl;
                    vout << "fillModeNonSolid: "                        << features.fillModeNonSolid << std::endl;
                    vout << "fragmentStoresAndAtomics: "                << features.fragmentStoresAndAtomics << std::endl;
                    vout << "fullDrawIndexUint32: "                     << features.fullDrawIndexUint32 << std::endl;
                    vout << "geometryShader: "                          << features.geometryShader << std::endl;
                    vout << "imageCubeArray: "                          << features.imageCubeArray << std::endl;
                    vout << "independentBlend: "                        << features.independentBlend << std::endl;
                    vout << "inheritedQueries: "                        << features.inheritedQueries << std::endl;
                    vout << "largePoints: "                             << features.largePoints << std::endl;
                    vout << "logicOp: "                                 << features.logicOp << std::endl;
                    vout << "multiDrawIndirect: "                       << features.multiDrawIndirect << std::endl;
                    vout << "multiViewport: "                           << features.multiViewport << std::endl;
                    vout << "occlusionQueryPrecise: "                   << features.occlusionQueryPrecise << std::endl;
                    vout << "pipelineStatisticsQuery: "                 << features.pipelineStatisticsQuery << std::endl;
                    vout << "robustBufferAccess: "                      << features.robustBufferAccess << std::endl;
                    vout << "sampleRateShading: "                       << features.sampleRateShading << std::endl;
                    vout << "samplerAnisotropy: "                       << features.samplerAnisotropy << std::endl;
                    vout << "shaderClipDistance: "                      << features.shaderClipDistance << std::endl;
                    vout << "shaderCullDistance: "                      << features.shaderCullDistance << std::endl;
                    vout << "shaderFloat64: "                           << features.shaderFloat64 << std::endl;
                    vout << "shaderImageGatherExtended: "               << features.shaderImageGatherExtended << std::endl;
                    vout << "shaderInt16: "                             << features.shaderInt16 << std::endl;
                    vout << "shaderInt64: "                             << features.shaderInt64 << std::endl;
                    vout << "shaderResourceMinLod: "                    << features.shaderResourceMinLod << std::endl;
                    vout << "shaderResourceResidency: "                 << features.shaderResourceResidency << std::endl;
                    vout << "shaderSampledImageArrayDynamicIndexing: "  << features.shaderSampledImageArrayDynamicIndexing << std::endl;
                    vout << "shaderStorageBufferArrayDynamicIndexing: " << features.shaderStorageBufferArrayDynamicIndexing << std::endl;
                    vout << "shaderStorageImageArrayDynamicIndexing: "  << features.shaderStorageImageArrayDynamicIndexing << std::endl;
                    vout << "shaderStorageImageExtendedFormats: "       << features.shaderStorageImageExtendedFormats << std::endl;
                    vout << "shaderStorageImageMultisample: "           << features.shaderStorageImageMultisample << std::endl;
                    vout << "shaderStorageImageReadWithoutFormat: "     << features.shaderStorageImageReadWithoutFormat << std::endl;
                    vout << "shaderStorageImageWriteWithoutFormat: "    << features.shaderStorageImageWriteWithoutFormat << std::endl;
                    vout << "shaderTessellationAndGeometryPointSize: "  << features.shaderTessellationAndGeometryPointSize << std::endl;
                    vout << "shaderUniformBufferArrayDynamicIndexing: " << features.shaderUniformBufferArrayDynamicIndexing << std::endl;
                    vout << "sparseBinding: "                           << features.sparseBinding << std::endl;
                    vout << "sparseResidency16Samples: "                << features.sparseResidency16Samples << std::endl;
                    vout << "sparseResidency2Samples: "                 << features.sparseResidency2Samples << std::endl;
                    vout << "sparseResidency4Samples: "                 << features.sparseResidency4Samples << std::endl;
                    vout << "sparseResidency8Samples: "                 << features.sparseResidency8Samples << std::endl;
                    vout << "sparseResidencyAliased: "                  << features.sparseResidencyAliased << std::endl;
                    vout << "sparseResidencyBuffer: "                   << features.sparseResidencyBuffer << std::endl;
                    vout << "sparseResidencyImage2D: "                  << features.sparseResidencyImage2D << std::endl;
                    vout << "sparseResidencyImage3D: "                  << features.sparseResidencyImage3D << std::endl;
                    vout << "tessellationShader: "                      << features.tessellationShader << std::endl;
                    vout << "textureCompressionASTC_LDR: "              << features.textureCompressionASTC_LDR << std::endl;
                    vout << "textureCompressionBC: "                    << features.textureCompressionBC << std::endl;
                    vout << "textureCompressionETC2: "                  << features.textureCompressionETC2 << std::endl;
                    vout << "variableMultisampleRate: "                 << features.variableMultisampleRate << std::endl;
                    vout << "vertexPipelineStoresAndAtomics: "          << features.vertexPipelineStoresAndAtomics << std::endl;
                    vout << "wideLines: "                               << features.wideLines << std::endl;
                    #endif // !NDEBUG
                vout.unindent();
            }
            vout.unindent();
        }
        
        void querySurfaceSpecific() {
            if(mQFIDisplay == -1) {
                mSurfaceCapabilities = VkSurfaceCapabilitiesKHR();
                mAvailableSurfaceFormats.clear();
                mAvailablePresentModes.clear();
            } else {
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVkPhysDevice, mVkSurface, &mSurfaceCapabilities);
                
                uint32_t numFormats;
                vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysDevice, mVkSurface, &numFormats, nullptr);
                mAvailableSurfaceFormats.resize(numFormats);
                vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysDevice, mVkSurface, &numFormats, mAvailableSurfaceFormats.data());
                
                uint32_t numPresents;
                vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysDevice, mVkSurface, &numPresents, nullptr);
                mAvailablePresentModes.resize(numPresents);
                vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysDevice, mVkSurface, &numPresents, mAvailablePresentModes.data());
            }
        }
        
        void queryPhysicalDeviceSpecific() {
            Logger::Out iout = Logger::log(Logger::INFO);
            Logger::Out vout = Logger::log(Logger::VERBOSE);
            
            if(mVkPhysDevice == VK_NULL_HANDLE) {
                mPhysicalDeviceExts.clear();
                mQueueFamilies.clear();
                mQFICompute = -1;
                mQFIDisplay = -1;
                mQFIGraphics = -1;
                mQFISparse = -1;
                mQFITransfer = -1;
                mVkComputeQueue = VK_NULL_HANDLE;
                mVkDisplayQueue = VK_NULL_HANDLE;
                mVkGraphicsQueue = VK_NULL_HANDLE;
                mVkSparseQueue = VK_NULL_HANDLE;
                mVkTransferQueue = VK_NULL_HANDLE;
            
                // Surface properties change depending on physical device
                querySurfaceSpecific();
                
                return;
            }
            
            uint32_t numExts;
            vkEnumerateDeviceExtensionProperties(mVkPhysDevice, nullptr, &numExts, nullptr);
            mPhysicalDeviceExts.resize(numExts);
            vkEnumerateDeviceExtensionProperties(mVkPhysDevice, nullptr, &numExts, mPhysicalDeviceExts.data());
            
            vkGetPhysicalDeviceProperties(mVkPhysDevice, &mPhysicalDeviceProperties);
            vkGetPhysicalDeviceFeatures(mVkPhysDevice, &mPhysicalDeviceFeatures);
            vkGetPhysicalDeviceMemoryProperties(mVkPhysDevice, &mPhysicalDeviceMemoryProperties);
            
            uint32_t numFamilies;
            vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysDevice, &numFamilies, nullptr);
            mQueueFamilies.resize(numFamilies);
            vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysDevice, &numFamilies, mQueueFamilies.data());
            iout << "Available queue families: " << mQueueFamilies.size() << std::endl;
            vout.indent();
            int32_t index = -1;
            for(const VkQueueFamilyProperties& family : mQueueFamilies) {
                ++ index;
                
                vout << "Queue of size " << family.queueCount << std::endl;
                
                bool supportGraphics = family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
                bool supportCompute = family.queueFlags & VK_QUEUE_COMPUTE_BIT;
                bool supportTransfer = family.queueFlags & VK_QUEUE_TRANSFER_BIT;
                bool supportSparse = family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT;
                VkBool32 vkBool;
                vkGetPhysicalDeviceSurfaceSupportKHR(mVkPhysDevice, index, mVkSurface, &vkBool);
                bool supportDisplay = vkBool;
                
                vout.indent();
                vout << "Compute operations: " << (supportCompute ? "available" : "unavailable") << std::endl;
                vout << "Display operations (SurfaceKHR): " << (supportDisplay ? "available" : "unavailable") << std::endl;
                vout << "Graphics operations: " << (supportGraphics ? "available" : "unavailable") << std::endl;
                vout << "Sparse memory management operations: " << (supportSparse ? "available" : "unavailable") << std::endl;
                vout << "Transfer operations: " << (supportTransfer ? "available" : "unavailable") << std::endl;
                vout.unindent();
                
                if(family.queueCount == 0) {
                    continue;
                }
                
                if(supportCompute && mQFICompute == -1) mQFICompute = index;
                if(supportDisplay && mQFIDisplay == -1) mQFIDisplay = index;
                if(supportGraphics && mQFIGraphics == -1) mQFIGraphics = index;
                if(supportSparse && mQFISparse == -1) mQFISparse = index;
                if(supportTransfer && mQFITransfer == -1) mQFITransfer = index;
            }
            vout.unindent();
            
            // Surface properties change depending on physical device
            querySurfaceSpecific();
        }
        
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
        
        VkSurfaceFormatKHR findBestSwapSurfaceFormat() {
            const std::vector<VkSurfaceFormatKHR>& formats = Video::Vulkan::getAvailableSurfaceFormats();
            
            assert(formats.size() > 0);
            
            for(VkSurfaceFormatKHR format : formats) {
                if(format.format == mVkIdealSurfaceFormat && format.colorSpace == mVkIdealSurfaceColorSpace) {
                    return format;
                }
            }
            
            VkSurfaceFormatKHR format = formats.at(0);
            if(format.format = VK_FORMAT_UNDEFINED) {
                VkSurfaceFormatKHR retVal; {
                    retVal.format = mVkIdealSurfaceFormat;
                    retVal.colorSpace = mVkIdealSurfaceColorSpace;
                }
                return retVal;
            }
            
            return format;
        }
        
        VkPresentModeKHR findBestSwapPresentMode() {
            const std::vector<VkPresentModeKHR>& presents = Video::Vulkan::getAvailablePresentModes();
            
            assert(presents.size() > 0);
            
            for(VkPresentModeKHR present : presents) {
                if(present == mVkIdealPresentMode) {
                    return present;
                }
            }
            return VK_PRESENT_MODE_FIFO_KHR;
        }
        
        VkExtent2D calcBestSwapExtents() {
            // Supports arbitrary sizes
            if(mSurfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
                VkExtent2D retVal; {
                    retVal.width = std::min(std::max(Video::getWindowWidth(), mSurfaceCapabilities.minImageExtent.width), mSurfaceCapabilities.maxImageExtent.width);
                    retVal.height = std::min(std::max(Video::getWindowHeight(), mSurfaceCapabilities.minImageExtent.height), mSurfaceCapabilities.maxImageExtent.height);
                }
                return retVal;
            }
            
            // 
            else {
                return mSurfaceCapabilities.currentExtent;
            }
        }
        
        bool rebuildSwapchain() {
            Logger::Out iout = Logger::log(Logger::INFO);
            Logger::Out vout = Logger::log(Logger::VERBOSE);
            Logger::Out sout = Logger::log(Logger::SEVERE);
            
            VkResult result;
            
            // Requery the surface data
            // Note: this will unnecessary on the very first call of this function but who cares
            querySurfaceSpecific();
            
            // First, delete any swapchain images that may have existed previously
            for(VkImageView view : mSwapchainImageViews) {
                vkDestroyImageView(mVkLogicalDevice, view, nullptr);
            }
            
            // Init surface swap chain
            {
                // Query / calculate important data
                VkSurfaceFormatKHR surfaceFormat = findBestSwapSurfaceFormat();
                VkPresentModeKHR surfacePresentMode = findBestSwapPresentMode();
                VkExtent2D surfaceExtent = calcBestSwapExtents();
                
                // Hopefully we can get one extra image to use triple buffering
                uint32_t surfaceImageQuantity = mSurfaceCapabilities.minImageCount + 1;
                
                // Has a hard swap chain quantity limit (max != 0) and our reqest exceeds that maximum
                if(mSurfaceCapabilities.maxImageCount > 0 && surfaceImageQuantity > mSurfaceCapabilities.maxImageCount) {
                    surfaceImageQuantity = mSurfaceCapabilities.maxImageCount;
                }
                
                VkSwapchainCreateInfoKHR swapchainCstrArgs;
                
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
                swapchainCstrArgs.preTransform = mSurfaceCapabilities.currentTransform;
                
                // Used for blending output with other operating system elements
                swapchainCstrArgs.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                
                // Use the present mode determined earlier
                swapchainCstrArgs.presentMode = surfacePresentMode;
                
                // Does the integrity of pixels not visible, not matter?
                swapchainCstrArgs.clipped = VK_TRUE;
                
                // If there was a previous swap chain in use (there was not) this must be specified appropriately
                swapchainCstrArgs.oldSwapchain = VK_NULL_HANDLE;
                
                if(mVkSwapchain == VK_NULL_HANDLE) {
                    result = vkCreateSwapchainKHR(mVkLogicalDevice, &swapchainCstrArgs, nullptr, &mVkSwapchain);
                } else {
                    swapchainCstrArgs.oldSwapchain = mVkSwapchain;
                    result = vkCreateSwapchainKHR(mVkLogicalDevice, &swapchainCstrArgs, nullptr, &mVkSwapchain);
                    vkDestroySwapchainKHR(mVkLogicalDevice, swapchainCstrArgs.oldSwapchain, nullptr);
                }
                
                if(result != VK_SUCCESS) {
                    sout << "Could not create swapchain" << std::endl;
                    return false;
                }
                
                mVkSwapchainFormat = surfaceFormat.format;
                mVkSwapchainExtent = surfaceExtent;
            
                uint32_t numImages;
                vkGetSwapchainImagesKHR(mVkLogicalDevice, mVkSwapchain, &numImages, nullptr);
                mSwapchainImages.resize(numImages);
                vkGetSwapchainImagesKHR(mVkLogicalDevice, mVkSwapchain, &numImages, mSwapchainImages.data());
            }
            
            // Init image views
            {
                mSwapchainImageViews.resize(Video::Vulkan::getSwapchainImages().size(), VK_NULL_HANDLE);
                uint32_t index = 0;
                for(VkImage image : Video::Vulkan::getSwapchainImages()) {
                    VkImageViewCreateInfo imageViewCstrArgs; {
                        imageViewCstrArgs.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                        imageViewCstrArgs.pNext = nullptr;
                        imageViewCstrArgs.image = image;
                        imageViewCstrArgs.viewType = VK_IMAGE_VIEW_TYPE_2D;
                        imageViewCstrArgs.format = mVkSwapchainFormat;
                        imageViewCstrArgs.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                        imageViewCstrArgs.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                        imageViewCstrArgs.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                        imageViewCstrArgs.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                        imageViewCstrArgs.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                        imageViewCstrArgs.subresourceRange.baseMipLevel = 0;
                        imageViewCstrArgs.subresourceRange.levelCount = 1;
                        imageViewCstrArgs.subresourceRange.baseArrayLayer = 0;
                        imageViewCstrArgs.subresourceRange.layerCount = 1;
                    }
                    
                    result = vkCreateImageView(mVkLogicalDevice, &imageViewCstrArgs, nullptr, &(mSwapchainImageViews.data()[index]));
                    
                    if(result != VK_SUCCESS) {
                        sout << "Could not create image view #" << index << std::endl;
                        return false;
                    }
                    ++ index;
                }
            }
            return true;
        }
        
        bool initialize() {
            Logger::Out iout = Logger::log(Logger::INFO);
            Logger::Out vout = Logger::log(Logger::VERBOSE);
            Logger::Out sout = Logger::log(Logger::SEVERE);
            
            VkResult result;
            
            // Init globals
            {
                mAppDesc.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                mAppDesc.pNext = nullptr;
                mAppDesc.pApplicationName = Engine::mAppName;
                mAppDesc.applicationVersion = VK_MAKE_VERSION(0, 0, 2);
                mAppDesc.pEngineName = Engine::mEngineName;
                mAppDesc.engineVersion = VK_MAKE_VERSION(0, 0, 1);
                mAppDesc.apiVersion = VK_API_VERSION_1_0;
                
                // Load global vulkan data
                //  Available extensions
                //  Available validation layers
                queryGlobals();
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
                queryInstanceSpecific();
            }
            
            // Init surface
            {
                #ifdef PGG_GLFW
                result = glfwCreateWindowSurface(mVkInstance, Engine::getGlfwWindow(), nullptr, &mVkSurface);
                #else
                result = VK_FAILURE;
                #endif
                
                if(result != VK_SUCCESS) {
                    sout << "Could not create Vulkan surface" << std::endl;
                    return false;
                }
            }
            
            // Init physical device
            {
                assert(mRequiredPhysDeviceExts.empty());
                mRequiredPhysDeviceExts.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
                
                // Search for an appropriate physical device
                {
                    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
                    double bestExtrasScore = -1.0;
                    for(VkPhysicalDevice device : Video::Vulkan::getAllPhysicalDevices()) {
                        // Read physical-device specific data from Vulkan
                        mVkPhysDevice = device;
                        queryPhysicalDeviceSpecific();
                        
                        if(isVulkanPhysicalDeviceSuitable(mRequiredPhysDeviceExts)) {
                            double extraValue = calcVulkanPhysicalDeviceExtraValue();
                            if(extraValue > bestExtrasScore) {
                                bestDevice = device;
                                bestExtrasScore = extraValue;
                            }
                        }
                    }
                    if(mVkPhysDevice != bestDevice) {
                        mVkPhysDevice = bestDevice;
                        queryPhysicalDeviceSpecific();
                    }
                }
                
                // If no physical device is found
                if(mVkPhysDevice == VK_NULL_HANDLE) {
                    sout << "Could not find a GPU meeting requirements" << std::endl;
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
                        mQFICompute,
                        mQFIDisplay,
                        mQFIGraphics,
                        mQFISparse,
                        mQFITransfer,
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
                
                VkDeviceCreateInfo deviceArgs; {
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
                    
                    deviceArgs.pEnabledFeatures = &enabledFeatures;
                }
                
                // Finally create the device
                result = vkCreateDevice(mVkPhysDevice, &deviceArgs, nullptr, &mVkLogicalDevice);
                
                // Make sure that logical device initialization was successful
                if(result != VK_SUCCESS) {
                    sout << "Could not initialize logical device" << std::endl;
                    return false;
                }
                
                // Get the queue family handles for later
                if(mQFIGraphics != -1) vkGetDeviceQueue(mVkLogicalDevice, mQFIGraphics, 0, &mVkGraphicsQueue);
                if(mQFIDisplay != -1) vkGetDeviceQueue(mVkLogicalDevice, mQFIDisplay, 0, &mVkDisplayQueue);
                if(mQFITransfer != -1) vkGetDeviceQueue(mVkLogicalDevice, mQFITransfer, 0, &mVkTransferQueue);
                if(mQFISparse != -1) vkGetDeviceQueue(mVkLogicalDevice, mQFISparse, 0, &mVkSparseQueue);
                if(mQFICompute != -1) vkGetDeviceQueue(mVkLogicalDevice, mQFICompute, 0, &mVkComputeQueue);
            }
            
            if(!rebuildSwapchain()) {
                sout << "Fatal error building swapchain" << std::endl;
                return false;
            }
            
            iout << "Vulkan initialized successfully" << std::endl;
            return true;
        }
        
        bool cleanup() {
            
            Logger::Out sout = Logger::log(Logger::SEVERE);
            Logger::Out wout = Logger::log(Logger::WARN);
            
            #ifndef NDEBUG
            cleanupDebugReportCallback();
            #endif
            
            for(VkImageView view : mSwapchainImageViews) {
                vkDestroyImageView(mVkLogicalDevice, view, nullptr);
            }
            
            vkDestroySwapchainKHR(mVkLogicalDevice, mVkSwapchain, nullptr); // Swapchain must be destroyed before logical device
            vkDestroyDevice(mVkLogicalDevice, nullptr); // Logical device must be destroyed before instance
            vkDestroySurfaceKHR(mVkInstance, mVkSurface, nullptr);
            vkDestroyInstance(mVkInstance, nullptr);
            
            // Everything successful
            return true;
        }
    }
    #endif // PGG_VULKAN
    
    #ifdef PGG_SDL
    namespace SDL {
        std::string getName(std::string x) { static std::string y = x; return y; }
        bool isSoftwareFallback(bool x) { static bool y = x; return y; }
        bool isHardwareAccelerated(bool x) { static bool y = x; return y; }
        
        bool supportsTextureRender(bool x) { static bool y = x; return y; }
        
        void querySDL(SDL_Renderer* renderer) {
            SDL_RendererInfo rendererInfo;
            SDL_GetRendererInfo(renderer, &rendererInfo);
            
            SDL::getName(std::string(rendererInfo.name));
            
            uint32_t flags = rendererInfo.flags;
            SDL::isSoftwareFallback(flags & SDL_RENDERER_SOFTWARE);
            SDL::isHardwareAccelerated(flags & SDL_RENDERER_ACCELERATED);
            SDL::supportsTextureRender(flags & SDL_RENDERER_TARGETTEXTURE);
            
            Logger::Out out = Logger::log(Logger::INFO);
            
            out << "SDL Renderer name: " << SDL::getName() << std::endl;
            out << "SDL Software fallback: " << SDL::isSoftwareFallback() << std::endl;
            out << "SDL Hardware accelerated: " << SDL::isHardwareAccelerated() << std::endl;
            out << "SDL Texture renderering: " << SDL::supportsTextureRender() << std::endl;
        }
    }
    #endif // PGG_SDL
    
    #ifdef PGG_GFLW
    namespace GLFW {
    }
    #endif // PGG_GLFW
    
    uint32_t mWindowWidth = 640;
    uint32_t mWindowHeight = 480;
    
    uint32_t getWindowWidth() { return mWindowWidth; }
    uint32_t getWindowHeight() { return mWindowHeight; }
    float calcWindowAspectRatio() { return (float) mWindowWidth / (float) mWindowHeight; }
    
    void onWindowResize(uint32_t width, uint32_t height) {
        //Logger::log(Logger::VERBOSE) << mWindowWidth << ", " << mWindowHeight << ", " << width << ", " << height << std::endl;
        
        mWindowWidth = width;
        mWindowHeight = height;
        
        #ifdef PGG_VULKAN
        // For some reason images of width or height 1 crash Vulkan
        if(mWindowWidth > 1 && mWindowHeight > 1) {
            vkDeviceWaitIdle(Vulkan::mVkLogicalDevice);
            if(!Vulkan::rebuildSwapchain()) {
                Logger::log(Logger::SEVERE) << "Could not rebuild Vulkan swapchain for window size: " 
                    << mWindowWidth << ", " << mWindowHeight << std::endl;
                Engine::quit();
            }
            Engine::onVulkanSwapchainInvalidated();
        }
        #endif
    }
    
} // Video
} // pgg

