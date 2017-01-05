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
    #endif
    
    #ifdef PGG_SDL
    namespace SDL {
        std::string getName(std::string x) { static std::string y = x; return y; }
        bool isSoftwareFallback(bool x) { static bool y = x; return y; }
        bool isHardwareAccelerated(bool x) { static bool y = x; return y; }
        
        bool supportsTextureRender(bool x) { static bool y = x; return y; }
    }
    #endif
    
    #ifdef PGG_OPENGL
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
    #endif
    
    #ifdef PGG_SDL
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
    #endif
    
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

