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

#include "TextureResource.hpp"

#include <cassert>
#include <fstream>

#include <GraphicsApiLibrary.hpp>
#include <json/json.h>

#include "Logger.hpp"
#include "Video.hpp"
#include "Resources.hpp"
#include "ImageResource.hpp"

namespace pgg {

TextureResource::TextureResource()
: mLoaded(false)
, Resource(Resource::Type::TEXTURE) {
}

TextureResource::~TextureResource() {
}

Texture* TextureResource::gallop(Resource* resource) {
    if(!resource || resource->mResourceType != Resource::Type::TEXTURE) {
        Logger::log(Logger::WARN) << "Failed to cast " << (resource ? resource->getName() : "nullptr") << " to texture!" << std::endl;
        return Texture::getFallback();
    } else {
        return static_cast<TextureResource*>(resource);
    }
}

#ifdef PGG_OPENGL
GLenum toEnumPF(const Json::Value& jVal, GLenum defaultVal) {
    if(!jVal.isString()) return defaultVal;
    const std::string val = jVal.asString();
    
    if(val == "RED") {
        return GL_RED;
    } else if(val == "GREEN") {
        return GL_GREEN;
    } else if(val == "BLUE") {
        return GL_BLUE;
    } else if(val == "RED_INTEGER") {
        return GL_RED_INTEGER;
    } else if(val == "GREEN_INTEGER") {
        return GL_GREEN_INTEGER;
    } else if(val == "BLUE_INTEGER") {
        return GL_BLUE_INTEGER;
    } else if(val == "RG") {
        return GL_RG;
    } else if(val == "RG_INTEGER") {
        return GL_RG_INTEGER;
    } else if(val == "RGB") {
        return GL_RGB;
    } else if(val == "RGB_INTEGER") {
        return GL_RGB_INTEGER;
    } else if(val == "RGBA") {
        return GL_RGBA;
    } else if(val == "RGBA_INTEGER") {
        return GL_RGBA_INTEGER;
    } else {
        return defaultVal;
    }
}
GLenum toEnum(const Json::Value& jVal, GLenum defaultVal) {
    if(!jVal.isString()) return defaultVal;
    const std::string val = jVal.asString();
    
    if(val == "linear") {
        return GL_LINEAR;
    } else if(val == "nearest") {
        return GL_NEAREST;
    } else if(val == "repeat") {
        return GL_REPEAT;
    } else if(val == "mirrored-repeat") {
        return GL_MIRRORED_REPEAT;
    } else if(val == "clamp-to-edge") {
        return GL_CLAMP_TO_EDGE;
    } else if(val == "clamp-to-border") {
        return GL_CLAMP_TO_BORDER;
    } else if(val == "RGB8") {
        return GL_RGB8;
    } else if(val == "R8") {
        return GL_R8;
    } else if(val == "SRGB8") {
        return GL_SRGB8;
    } else {
        return defaultVal;
    }
}
#endif // PGG_OPENGL

#ifdef PGG_VULKAN
VkSamplerAddressMode toSamplerAddressMode(const Json::Value& jVal, VkSamplerAddressMode defaultVal) {
    if(!jVal.isString()) return defaultVal;
    const std::string val = jVal.asString();
    
    if(val == "repeat") {
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    } else if(val == "mirrored-repeat") {
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    } else if(val == "clamp-to-edge") {
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    } else if(val == "clamp-to-border") {
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    } else {
        return defaultVal;
    }
}
#endif // PGG_VULKAN

void TextureResource::load() {
    assert(!mLoaded && "Attempted to load texture that has already been loaded");
    Json::Value textureData;
    {
        std::ifstream loader(this->getFile().string().c_str());
        loader >> textureData;
        loader.close();
    }
    
    Logger::Out wout = Logger::log(Logger::WARN);
    
    mImage = ImageResource::gallop(Resources::find(textureData["image"].asString()));
    mImage->grab();

    #ifdef PGG_OPENGL
    glGenTextures(1, &mHandle);
    glBindTexture(GL_TEXTURE_2D, mHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, toEnum(textureData["internalFormat"], GL_RGB8), mImage->getWidth(), mImage->getHeight(), 0, toEnumPF(textureData["pixelFormat"], GL_RGB), GL_UNSIGNED_BYTE, mImage->getImage());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toEnum(textureData["wrapX"], GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toEnum(textureData["wrapY"], GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toEnum(textureData["minFilter"], GL_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toEnum(textureData["magFilter"], GL_LINEAR));

    glBindTexture(GL_TEXTURE_2D, 0);
    #endif // PGG_OPENGL
    
    #ifdef PGG_VULKAN
    
    VkResult result;
    
    VkSamplerCreateInfo samplerCargs; {
        samplerCargs.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCargs.pNext = nullptr;
        samplerCargs.flags = 0;
        samplerCargs.addressModeU = toSamplerAddressMode(textureData["wrapX"], VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        samplerCargs.addressModeV = toSamplerAddressMode(textureData["wrapY"], VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        samplerCargs.addressModeW = toSamplerAddressMode(textureData["wrapZ"], VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
        samplerCargs.anisotropyEnable = VK_FALSE;
        samplerCargs.maxAnisotropy = 0;
        samplerCargs.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerCargs.unnormalizedCoordinates = VK_FALSE;
        samplerCargs.compareEnable = VK_FALSE;
        samplerCargs.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerCargs.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCargs.mipLodBias = 0.f;
        samplerCargs.minLod = 0.f;
        samplerCargs.maxLod = 0.f;
    }
    
    result = vkCreateSampler(Video::Vulkan::getLogicalDevice(), &samplerCargs, nullptr, &mSamplerHandle);
    if(result != VK_SUCCESS) {
        wout << "Could not create texture sampler" << std::endl;
        return;
    }
    
    
    #endif // PGG_VULKAN

    mLoaded = true;
}

void TextureResource::unload() {
    assert(mLoaded && "Attempted to unload texture before loading it");
    #ifdef PGG_OPENGL
    glDeleteTextures(1, &mHandle);
    #endif
    #ifdef PGG_VULKAN
    vkDestroySampler(Video::Vulkan::getLogicalDevice(), mSamplerHandle, nullptr);
    #endif // PGG_VULKAN
    mImage->drop();
    mLoaded = false;
}


#ifdef PGG_OPENGL
GLuint TextureResource::getHandle() const {
    return mHandle;
}
#endif

}
