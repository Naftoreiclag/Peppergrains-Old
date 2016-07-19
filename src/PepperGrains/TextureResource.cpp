/*
   Copyright 2015-2016 James Fong

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

#include <OpenGLStuff.hpp>

#include "json/json.h"

#include "ResourceManager.hpp"

namespace pgg {

TextureResource::TextureResource()
: mLoaded(false) {
}

TextureResource::~TextureResource() {
}

GLenum TextureResource::toEnumPF(const std::string& val, GLenum errorVal) {
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
        return errorVal;
    }
}

GLenum TextureResource::toEnum(const std::string& val, GLenum errorVal) {
    if(val == "linear") {
        return GL_LINEAR;
    } else if(val == "nearest") {
        return GL_NEAREST;
    } else if(val == "repeat") {
        return GL_REPEAT;
    } else if(val == "mirroredRepeat") {
        return GL_MIRRORED_REPEAT;
    } else if(val == "clampToEdge") {
        return GL_CLAMP_TO_EDGE;
    } else if(val == "clampToBorder") {
        return GL_CLAMP_TO_BORDER;
    } else if(val == "RGB8") {
        return GL_RGB8;
    } else if(val == "R8") {
        return GL_R8;
    } else if(val == "SRGB8") {
        return GL_SRGB8;
    } else {
        return errorVal;
    }
}
void TextureResource::loadError() {
    assert(!mLoaded && "Attempted to load texture that has already been loaded");
    
    ResourceManager* rmgr = ResourceManager::getSingleton();
    
    mImage = rmgr->getFallbackImage();
    mImage->grab();
    
    glGenTextures(1, &mHandle);
    glBindTexture(GL_TEXTURE_2D, mHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mImage->getWidth(), mImage->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, mImage->getImage());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    mLoaded = true;
}

void TextureResource::load() {
    assert(!mLoaded && "Attempted to load texture that has already been loaded");

    if(this->isFallback()) {
        loadError();
        return;
    }

    Json::Value textureData;
    {
        std::ifstream loader(this->getFile().string().c_str());
        loader >> textureData;
        loader.close();
    }

    ResourceManager* rmgr = ResourceManager::getSingleton();
    
    mImage = rmgr->findImage(textureData["image"].asString());
    mImage->grab();

    glGenTextures(1, &mHandle);
    glBindTexture(GL_TEXTURE_2D, mHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, toEnum(textureData["internalFormat"].asString(), GL_RGB8), mImage->getWidth(), mImage->getHeight(), 0, toEnumPF(textureData["pixelFormat"].asString(), GL_RGB), GL_UNSIGNED_BYTE, mImage->getImage());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toEnum(textureData["wrapX"].asString(), GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toEnum(textureData["wrapY"].asString(), GL_CLAMP_TO_EDGE));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toEnum(textureData["minFilter"].asString(), GL_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toEnum(textureData["magFilter"].asString(), GL_LINEAR));

    glBindTexture(GL_TEXTURE_2D, 0);

    mLoaded = true;
}

void TextureResource::unload() {
    assert(mLoaded && "Attempted to unload texture before loading it");
    glDeleteTextures(1, &mHandle);
    mImage->drop();
    mLoaded = false;
}

GLuint TextureResource::getHandle() const {
    return mHandle;
}

}
