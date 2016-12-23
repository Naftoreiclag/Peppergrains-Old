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

#ifndef PGG_FONTRESOURCE_HPP
#define PGG_FONTRESOURCE_HPP

#include "Resource.hpp"
#include "Texture.hpp"
#include "ShaderProgramResource.hpp"

namespace pgg {

class FontResource : public Resource {
public:
    struct GlyphData {
        float width;
        float startX;
    };
private:
    bool mLoaded;

    Texture* mTexture;
    ShaderProgramResource* mShaderProg;
    GLuint mTextureHandle;


public:
    FontResource();
    virtual ~FontResource();

    GlyphData* mGlyphs;
    float mBaseline;
    float mPadding;
    void load();
    void unload();

    void bindTextures();
};

}

#endif // PGG_FONTRESOURCE_HPP
