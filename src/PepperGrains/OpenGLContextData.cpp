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

#include "OpenGLContextData.hpp"

namespace pgg {

bool OpenGLContextData::OpenGLInfo::supportsGeometryShaders() const {
    if(iMajorVersion > 3) {
        return true;
    }
    if(iMajorVersion == 3 && iMinorVersion >= 2) {
        return true;
    }
    return false;
}

bool OpenGLContextData::OpenGLInfo::supportsTessellationShaders() const {
    if(iMajorVersion >= 4) {
        return true;
    }
    return false;
}
    
OpenGLContextData* OpenGLContextData::getSingleton() {
    static OpenGLContextData instance;
    return &instance;
}
    
OpenGLContextData::OpenGLContextData() {
    glGetIntegerv(GL_MAJOR_VERSION, &mInfo.iMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &mInfo.iMinorVersion);
    glGetIntegerv(GL_CONTEXT_FLAGS, &mInfo.iContextFlags);
    
    glGetIntegerv(GL_ATOMIC_COUNTER_BUFFER_SIZE, &mInfo.iMaxAtomicCounterBufferSize); // Min 32
    glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &mInfo.iMaxAtomicCounterBufferBindings); // Min 1
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &mInfo.iMaxShaderStorageBufferBindings); // Min 8
    glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, &mInfo.iMaxCombinedShaderOutputResources); // Min 8
    glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &mInfo.iMaxCombinedShaderStorageBlocks); // Min 8
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &mInfo.iMaxUniformBlockSize); // Min 16384
    //glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS, &mInfo.iMaxTransformFeedbackBufferBindings);
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &mInfo.iMaxUniformBufferBindings); // Min 84
    glGetIntegerv(GL_MAX_DEBUG_LOGGED_MESSAGES, &mInfo.iMaxDebugLoggedMessages); // Min 1
    glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &mInfo.iMaxDebugMessageLength); // Min 1
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &mInfo.iMaxColorAttachments); // Min 8
    glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &mInfo.iMaxColorTextureSamples); // Min 1
    glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &mInfo.iMaxDepthTextureSamples); // Min 1
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &mInfo.iMaxDrawBuffers); // Min 8
    glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &mInfo.iMaxFramebufferHeight); // Min 16384
    glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &mInfo.iMaxFramebufferWidth); // Min 16384
    glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &mInfo.iMaxFramebufferLayers); // Min 2048
    glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &mInfo.iMaxFramebufferSamples); // Min 4
    
    mInfo.bDebugOutput = (glIsEnabled(GL_DEBUG_OUTPUT) == GL_TRUE);
    
    mInfo.sVersion = std::string((const char*) glGetString(GL_VERSION));
    mInfo.sVendor = std::string((const char*) glGetString(GL_VENDOR));
    mInfo.sRenderer = std::string((const char*) glGetString(GL_RENDERER));
    mInfo.sShadingLanguageVersion = std::string((const char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
}
OpenGLContextData::~OpenGLContextData() { }

const OpenGLContextData::OpenGLInfo& OpenGLContextData::getData() {
    return this->mInfo;
}

}

