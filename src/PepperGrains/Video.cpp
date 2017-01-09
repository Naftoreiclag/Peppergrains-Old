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

#include "Logger.hpp"

namespace pgg {
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
    }
    void queryOpenGL() {
        GLint gi;
        
        glGetIntegerv(GL_MAJOR_VERSION, &gi); OpenGL::getMajorVersion(gi);
        glGetIntegerv(GL_MINOR_VERSION, &gi); OpenGL::getMinorVersion(gi);
        glGetIntegerv(GL_CONTEXT_FLAGS, &gi); OpenGL::getContextFlags(gi);
        
        glGetIntegerv(GL_ATOMIC_COUNTER_BUFFER_SIZE, &gi); OpenGL::getMaxAtomicCounterBufferSize(gi);
        glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &gi); OpenGL::getMaxAtomicCounterBufferBindings(gi);
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &gi); OpenGL::getMaxShaderStorageBufferBindings(gi);
        glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, &gi); OpenGL::getMaxCombinedShaderOutputResources(gi);
        glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &gi); OpenGL::getMaxCombinedShaderStorageBlocks(gi);
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &gi); OpenGL::getMaxUniformBlockSize(gi);
        //glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS, &gi); OpenGL(gi);
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &gi); OpenGL::getMaxUniformBufferBindings(gi);
        glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES, &gi); OpenGL::getMaxDebugLoggedMessages(gi);
        glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &gi); OpenGL::getMaxDebugMessageLength(gi);
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &gi); OpenGL::getMaxColorAttachments(gi);
        glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &gi); OpenGL::getMaxColorTextureSamples(gi);
        glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &gi); OpenGL::getMaxDepthTextureSamples(gi);
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &gi); OpenGL::getMaxDrawBuffers(gi);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &gi); OpenGL::getMaxFramebufferHeight(gi);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &gi); OpenGL::getMaxFramebufferWidth(gi);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &gi); OpenGL::getMaxFramebufferLayers(gi);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &gi); OpenGL::getMaxFramebufferSamples(gi);
        
        OpenGL::isDebugOutputEnabled(glIsEnabled(GL_DEBUG_OUTPUT) == GL_TRUE);
        
        OpenGL::getVersion(std::string((const char*) glGetString(GL_VERSION)));
        OpenGL::getVendor(std::string((const char*) glGetString(GL_VENDOR)));
        OpenGL::getRenderer(std::string((const char*) glGetString(GL_RENDERER)));
        OpenGL::getShadingLanguageVersion(std::string((const char*) glGetString(GL_SHADING_LANGUAGE_VERSION)));
        
        Logger::Out out = Logger::log(Logger::INFO);
        
        out << "OpenGL Version (Integral): " << OpenGL::getMajorVersion() << "." << OpenGL::getMinorVersion() << std::endl;
        out << "OpenGL Version (String): " << OpenGL::getVersion() << std::endl;
        out << "OpenGL Debug output enabled: " << OpenGL::isDebugOutputEnabled() << std::endl;
        out << "OpenGL Max draw buffers: " << OpenGL::getMaxDrawBuffers() << std::endl;
        out << "OpenGL Max color attachments: " << OpenGL::getMaxColorAttachments() << std::endl;
    }
    #endif // PGG_OPENGL
    
    #ifdef PGG_VULKAN
    namespace Vulkan {
        VkInstance mVulkanInstance = VK_NULL_HANDLE;
        VkInstance getInstance() { return mVulkanInstance; }
        VkSurfaceKHR mVulkanSurface = VK_NULL_HANDLE;
        VkSurfaceKHR getSurface() { return mVulkanSurface; }
        VkPhysicalDevice mVulkanPhysDevice = VK_NULL_HANDLE;
        VkPhysicalDevice getPhysicalDevice() { return mVulkanPhysDevice; }
        VkSwapchainKHR mVulkanSwapchain = VK_NULL_HANDLE;
        VkSwapchainKHR getSwapchain() { return mVulkanSwapchain; }
        VkDevice mVulkanLogicalDevice;
        VkDevice getLogicalDevice() { return mVulkanLogicalDevice; }
        
        int32_t mQFIGraphics = -1;
        int32_t mQFICompute = -1;
        int32_t mQFITransfer = -1;
        int32_t mQFISparse = -1;
        int32_t mQFIDisplay = -1;
        int32_t getGraphicsQueueFamilyIndex() { return mQFIGraphics; }
        int32_t getComputeQueueFamilyIndex() { return mQFICompute; }
        int32_t getTransferQueueFamilyIndex() { return mQFITransfer; }
        int32_t getSparseQueueFamilyIndex() { return mQFISparse; }
        int32_t getDisplayQueueFamilyIndex() { return mQFIDisplay; }
        
        
        VkSurfaceCapabilitiesKHR mSurfaceCapabilities;
        VkSurfaceCapabilitiesKHR getSurfaceCapabilities() { return mSurfaceCapabilities; }
        std::vector<VkSurfaceFormatKHR> mAvailableSurfaceFormats;
        const std::vector<VkSurfaceFormatKHR> getAvailableSurfaceFormats() { return mAvailableSurfaceFormats; }
        std::vector<VkPresentModeKHR> mAvailablePresentModes;
        const std::vector<VkPresentModeKHR> getAvailablePresentModes() { return mAvailablePresentModes; }
        
        std::vector<VkImage> mSwapchainImages;
        const std::vector<VkImage>& getSwapchainImages() { return mSwapchainImages; }
        
        std::vector<VkExtensionProperties> mExtensionProperties;
        const std::vector<VkExtensionProperties>& getAvailableExtensions() { return mExtensionProperties; }
        std::vector<VkLayerProperties> mLayerProperties;
        const std::vector<VkLayerProperties>& getAvailableLayers() { return mLayerProperties; }
        std::vector<VkPhysicalDevice> mPhysicalDevices;
        const std::vector<VkPhysicalDevice>& getAllPhysicalDevices() { return mPhysicalDevices; }
        std::vector<VkExtensionProperties> mPhysicalDeviceExts;
        const std::vector<VkExtensionProperties>& getAvailablePhysicalDeviceExtensions() { return mPhysicalDeviceExts; }
        std::vector<VkQueueFamilyProperties> mQueueFamilies;
        const std::vector<VkQueueFamilyProperties>& getQueueFamilies() { return mQueueFamilies; }
        
        bool findQueueFamilySupporting(VkQueueFlags flags, uint32_t* rIndex) {
            uint32_t index = 0;
            for(const VkQueueFamilyProperties& family : mQueueFamilies) {
                if(family.queueFlags & flags) {
                    uint32_t& overwrite = *rIndex;
                    overwrite = index;
                    return true;
                }
                ++ index;
            }
            return false;
        }
        
        bool findQueueFamilySupportingSurfaceKHR(uint32_t& rIndex) {
            uint32_t index = 0;
        }
        
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
            iout << "Supported validation layers: " << mExtensionProperties.size() << std::endl;
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
        void queryInstanceSpecific(VkInstance instance) {
            Logger::Out iout = Logger::log(Logger::INFO);
            Logger::Out vout = Logger::log(Logger::VERBOSE);
            
            mVulkanInstance = instance;
            
            if(mVulkanInstance == VK_NULL_HANDLE) {
                mPhysicalDevices.clear();
                return;
            }
            
            uint32_t numDevices;
            vkEnumeratePhysicalDevices(mVulkanInstance, &numDevices, nullptr);
            mPhysicalDevices.resize(numDevices);
            vkEnumeratePhysicalDevices(mVulkanInstance, &numDevices, mPhysicalDevices.data());
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
        void querySurfaceSpecific(VkSurfaceKHR surface) {
            
            mVulkanSurface = surface;
        }
        void queryPhysicalDeviceSpecific(VkPhysicalDevice device) {
            Logger::Out iout = Logger::log(Logger::INFO);
            Logger::Out vout = Logger::log(Logger::VERBOSE);
            
            mVulkanPhysDevice = device;
            
            if(mVulkanPhysDevice == VK_NULL_HANDLE) {
                mSurfaceCapabilities = VkSurfaceCapabilitiesKHR();
                mAvailablePresentModes.clear();
                mAvailableSurfaceFormats.clear();
                mPhysicalDeviceExts.clear();
                mQueueFamilies.clear();
                mQFIGraphics = -1;
                mQFICompute = -1;
                mQFITransfer = -1;
                mQFISparse = -1;
                mQFIDisplay = -1;
                return;
            }
            
            uint32_t numExts;
            vkEnumerateDeviceExtensionProperties(mVulkanPhysDevice, nullptr, &numExts, nullptr);
            mPhysicalDeviceExts.resize(numExts);
            vkEnumerateDeviceExtensionProperties(mVulkanPhysDevice, nullptr, &numExts, mPhysicalDeviceExts.data());
            
            /*
            bool hasSwapchain = false;
            for(VkExtensionProperties props : mPhysicalDeviceExts) {
                if(strcmp(props.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
                    hasSwapchain = true;
                    continue;
                }
                
                // More extensions...
            }
            */
            
            uint32_t numFamilies;
            vkGetPhysicalDeviceQueueFamilyProperties(mVulkanPhysDevice, &numFamilies, nullptr);
            mQueueFamilies.resize(numFamilies);
            vkGetPhysicalDeviceQueueFamilyProperties(mVulkanPhysDevice, &numFamilies, mQueueFamilies.data());
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
                vkGetPhysicalDeviceSurfaceSupportKHR(mVulkanPhysDevice, index, mVulkanSurface, &vkBool);
                bool supportSurfaceKHR = vkBool;
                
                vout.indent();
                vout << "Graphics operations: " << (supportGraphics ? "available" : "unavailable") << std::endl;
                vout << "Compute operations: " << (supportCompute ? "available" : "unavailable") << std::endl;
                vout << "Transfer operations: " << (supportTransfer ? "available" : "unavailable") << std::endl;
                vout << "Sparse memory management operations: " << (supportSparse ? "available" : "unavailable") << std::endl;
                vout << "Supports SurfaceKHR: " << (supportSurfaceKHR ? "available" : "unavailable") << std::endl;
                vout.unindent();
                
                if(family.queueCount == 0) {
                    continue;
                }
                
                if(supportGraphics) mQFIGraphics = index;
                if(supportCompute) mQFICompute = index;
                if(supportTransfer) mQFITransfer = index;
                if(supportSparse) mQFISparse = index;
                if(supportSurfaceKHR) mQFIDisplay = index;
            }
            vout.unindent();
            
            if(mQFIDisplay == -1) {
                mSurfaceCapabilities = VkSurfaceCapabilitiesKHR();
                mAvailableSurfaceFormats.clear();
                mAvailablePresentModes.clear();
            } else {
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVulkanPhysDevice, mVulkanSurface, &mSurfaceCapabilities);
                
                uint32_t numFormats;
                vkGetPhysicalDeviceSurfaceFormatsKHR(mVulkanPhysDevice, mVulkanSurface, &numFormats, nullptr);
                mAvailableSurfaceFormats.resize(numFormats);
                vkGetPhysicalDeviceSurfaceFormatsKHR(mVulkanPhysDevice, mVulkanSurface, &numFormats, mAvailableSurfaceFormats.data());
                
                uint32_t numPresents;
                vkGetPhysicalDeviceSurfacePresentModesKHR(mVulkanPhysDevice, mVulkanSurface, &numPresents, nullptr);
                mAvailablePresentModes.resize(numPresents);
                vkGetPhysicalDeviceSurfacePresentModesKHR(mVulkanPhysDevice, mVulkanSurface, &numPresents, mAvailablePresentModes.data());
            }
        }
        void queryLogicalDeviceSpecific(VkDevice logicalDevice) {
            mVulkanLogicalDevice = logicalDevice;
        }
        void querySwapchainSpecific(VkSwapchainKHR swapchain) {
            Logger::Out iout = Logger::log(Logger::INFO);
            Logger::Out vout = Logger::log(Logger::VERBOSE);
            
            mVulkanSwapchain = swapchain;
            
            uint32_t numImages;
            vkGetSwapchainImagesKHR(mVulkanLogicalDevice, mVulkanSwapchain, &numImages, nullptr);
            mSwapchainImages.resize(numImages);
            vkGetSwapchainImagesKHR(mVulkanLogicalDevice, mVulkanSwapchain, &numImages, mSwapchainImages.data());
            
        }
    }
    #endif // PGG_VULKAN
    
    #ifdef PGG_SDL
    namespace SDL {
        std::string getName(std::string x) { static std::string y = x; return y; }
        bool isSoftwareFallback(bool x) { static bool y = x; return y; }
        bool isHardwareAccelerated(bool x) { static bool y = x; return y; }
        
        bool supportsTextureRender(bool x) { static bool y = x; return y; }
    }
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
    #endif // PGG_SDL
    
    uint32_t mWindowWidth = 640;
    uint32_t mWindowHeight = 480;
    
    uint32_t getWindowWidth() { return mWindowWidth; }
    uint32_t getWindowHeight() { return mWindowHeight; }
    
    void resizeWindow(uint32_t width, uint32_t height) {
        mWindowWidth = width;
        mWindowHeight = height;
        
        // todo: actually call the resize func instead
    }
    
} // Video
} // pgg

