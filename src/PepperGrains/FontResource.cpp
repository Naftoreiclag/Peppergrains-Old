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

#include "FontResource.hpp"

#include <cassert>
#include <fstream>
#include <vector>

#include "StreamStuff.hpp"
#include "Resources.hpp"
#include "ShaderProgramResource.hpp"
#include "TextureResource.hpp"

namespace pgg {

FontResource::FontResource()
: mLoaded(false)
, Resource(Resource::Type::FONT) {
}

FontResource::~FontResource() {
}

void FontResource::load() {
    assert(!mLoaded && "Attempted to load font that is already loaded");

    std::ifstream input(this->getFile().string().c_str(), std::ios::in | std::ios::binary);

    std::string textureName;
    readString(input, textureName);

    readF32(input, mBaseline);
    readF32(input, mPadding);

    mGlyphs = new GlyphData[256];
    for(uint32_t i = 0; i < 256; ++ i) {
        mGlyphs[i].width = readF32(input);
        mGlyphs[i].startX = readF32(input);
    }

    mShaderProg = ShaderProgramResource::gallop(Resources::find("Font.shaderProgram"));
    mShaderProg->grab();

    const std::vector<ShaderProgramResource::Control>& sampler2DControls = mShaderProg->getUniformSampler2Ds();

    for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
        const ShaderProgramResource::Control& entry = *iter;

        mTextureHandle = entry.handle;

        // (Might want other samplers in the future)
        break;
    }

    mTexture = TextureResource::gallop(Resources::find(textureName));
    mTexture->grab();

    mLoaded = true;
}
void FontResource::unload() {
    assert(mLoaded && "Attempted to unload font before loading it");

    mTexture->drop();
    mShaderProg->drop();

    mLoaded = false;
}

void FontResource::bindTextures() {
    unsigned int index = 0;
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, mTexture->getHandle());
    glUniform1i(mTextureHandle, index);
}

}
