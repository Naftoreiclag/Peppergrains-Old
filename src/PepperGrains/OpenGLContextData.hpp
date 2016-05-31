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

#ifndef PGG_OPENGLCONTEXTDATA_HPP
#define PGG_OPENGLCONTEXTDATA_HPP

#include <string>

#include <GL/glew.h>

namespace pgg {

class OpenGLContextData {
public:
    struct OpenGLInfo {
        GLint iMajorVersion;
        GLint iMinorVersion;
        GLint iContextFlags;
        
        GLint iMaxAtomicCounterBufferSize; // Min 32
        GLint iMaxAtomicCounterBufferBindings; // Min 1
        GLint iMaxShaderStorageBufferBindings; // Min 8
        GLint iMaxCombinedShaderOutputResources; // Min 8
        GLint iMaxCombinedShaderStorageBlocks; // Min 8
        GLint iMaxUniformBlockSize; // Min 16384
        //GLint iMaxTransformFeedbackBufferBindings;
        GLint iMaxUniformBufferBindings; // Min 84
        GLint iMaxDebugLoggedMessages; // Min 1
        GLint iMaxDebugMessageLength; // Min 1
        GLint iMaxColorAttachments; // Min 8
        GLint iMaxColorTextureSamples; // Min 1
        GLint iMaxDepthTextureSamples; // Min 1
        GLint iMaxDrawBuffers; // Min 8
        GLint iMaxFramebufferHeight; // Min 16384
        GLint iMaxFramebufferWidth; // Min 16384
        GLint iMaxFramebufferLayers; // Min 2048
        GLint iMaxFramebufferSamples; // Min 4
        
        bool bDebugOutput;
        
        std::string sVersion;
        std::string sVendor;
        std::string sRenderer;
        std::string sShadingLanguageVersion;
    };
private:
    OpenGLInfo mInfo;
public:
    static OpenGLContextData* getSingleton();
public:
    OpenGLContextData();
    ~OpenGLContextData();

    const OpenGLInfo& getData();
    
};

}

#endif // PGG_OPENGLCONTEXTDATA_HPP
