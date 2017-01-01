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

#include "Texture.hpp"

#include <cassert>

#include "Logger.hpp"
#include "Resources.hpp"
#include "TextureResource.hpp"

namespace pgg {

Texture::Texture() { }
Texture::~Texture() { }

Texture* Texture::getFallback() {
    Resource* lookup = Resources::find("Error.texture");
    
    if(lookup && lookup->mResourceType == Resource::Type::TEXTURE) {
        return static_cast<TextureResource*>(lookup);
    } else {
        static FallbackTexture fallbackTexture;
        return &fallbackTexture;
    }
}

FallbackTexture::FallbackTexture() { }
FallbackTexture::~FallbackTexture() { }

void FallbackTexture::load() {
    assert(!mLoaded && "Attempted to load texture that has already been loaded");
    
    mImage = Image::getFallback();
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
void FallbackTexture::unload() {
    assert(mLoaded && "Attempted to unload texture before loading it");
    glDeleteTextures(1, &mHandle);
    mImage->drop();
    mLoaded = false;
}
GLuint FallbackTexture::getHandle() const {
    return mHandle;
}

}

