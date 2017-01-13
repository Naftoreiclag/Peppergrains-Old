/*
   Copyright 2015-2017 James Fong

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

#ifndef PGG_ShaderResource_HPP
#define PGG_ShaderResource_HPP

#include <GraphicsApiLibrary.hpp>

#include "Resource.hpp"

namespace pgg {

class ShaderResource : public Resource {
public:
    enum Type {
        COMPUTE,
        VERTEX,
        TESS_CONTROL,
        TESS_EVALUATION,
        GEOMETRY,
        FRAGMENT
    };
    #ifdef PGG_VULKAN
    static VkShaderStageFlagBits toVulkanShaderStageFlagBits(Type type);
    #endif
    
private:
    
    #ifdef PGG_VULKAN
    VkShaderModule mHandle = VK_NULL_HANDLE;
    VkPipelineShaderStageCreateInfo mPipelineShaderStageInfo;
    #endif
    
    #ifdef PGG_OPENGL
    GLuint mHandle;
    #endif
    
    bool mLoaded;
public:
    const Type mType;

    ShaderResource(Type type);
    virtual ~ShaderResource();
    
    static ShaderResource* gallop(Resource* resource);
    
    void load();
    void unload();
    
    #ifdef PGG_VULKAN
    VkShaderModule getHandle();
    VkPipelineShaderStageCreateInfo getPipelineShaderStageInfo();
    VkShaderStageFlagBits getShaderStageFlagBits();
    #endif
    
    #ifdef PGG_OPENGL
    GLuint getHandle();
    #endif
};

}

#endif // ShaderResource_HPP
