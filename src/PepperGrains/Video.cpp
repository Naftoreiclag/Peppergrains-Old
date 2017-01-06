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
        std::vector<VkExtensionProperties> extensionProperties;
        const std::vector<VkExtensionProperties>& getAvailableExtensions() { return extensionProperties; }
        std::vector<VkLayerProperties> layerProperties;
        const std::vector<VkLayerProperties>& getAvailableLayers() { return layerProperties; }
        std::vector<VkPhysicalDevice> physicalDevices;
        const std::vector<VkPhysicalDevice>& getPhysicalDevices() { return physicalDevices; }
    }
    void queryVulkanExtensionsAndLayers() {
        using namespace Video::Vulkan;
        Logger::Out vout = Logger::log(Logger::VERBOSE);
        
        uint32_t numExt;
        vkEnumerateInstanceExtensionProperties(nullptr, &numExt, nullptr);
        extensionProperties.resize(numExt);
        vkEnumerateInstanceExtensionProperties(nullptr, &numExt, extensionProperties.data());
        vout << "Supported extensions: " << extensionProperties.size() << std::endl;
        vout.indent();
        for(const VkExtensionProperties& props : extensionProperties) {
            vout << props.extensionName << '\t' << props.specVersion << std::endl;
        }
        vout.unindent();
        
        uint32_t numLayers;
        vkEnumerateInstanceLayerProperties(&numLayers, nullptr);
        layerProperties.resize(numLayers);
        vkEnumerateInstanceLayerProperties(&numLayers, layerProperties.data());
        vout << "Supported validation layers: " << extensionProperties.size() << std::endl;
        vout.indent();
        for(const VkLayerProperties& props : layerProperties) {
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
    void queryVulkanPhysicalDevices(VkInstance instance) {
        using namespace Video::Vulkan;
        Logger::Out vout = Logger::log(Logger::VERBOSE);
        
        uint32_t numDevices;
        vkEnumeratePhysicalDevices(instance, &numDevices, nullptr);
        physicalDevices.resize(numDevices);
        vkEnumeratePhysicalDevices(instance, &numDevices, physicalDevices.data());
        vout << "Available physical devices: " << physicalDevices.size() << std::endl;
        vout.indent();
        for(const VkPhysicalDevice& device : physicalDevices) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(device, &features);
            vout << properties.deviceName << std::endl;
            vout.indent();
                vout << "API version: " << properties.apiVersion << std::endl;
                vout << "Driver version: " << properties.driverVersion << std::endl;
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

