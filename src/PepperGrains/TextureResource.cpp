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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mImage->getWidth(), mImage->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, mImage->getImage());

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mImage->getWidth(), mImage->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, mImage->getImage());

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
