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

#ifndef PGG_TEXTURE_HPP
#define PGG_TEXTURE_HPP

#include <stdint.h>

#include "OpenGLStuff.hpp"
#include "ReferenceCounted.hpp"
#include "Image.hpp"

namespace pgg {

// Virtual inheritance to avoid diamond conflict with TextureResource
class Texture : virtual public ReferenceCounted {
public:
    Texture();
    virtual ~Texture();
    
    static Texture* getFallback();
    
    virtual GLuint getHandle() const = 0;
};

class FallbackTexture : public Texture {
private:
    bool mLoaded;
    Image* mImage;
    GLuint mHandle;
    
public:
    FallbackTexture();
    ~FallbackTexture();
    
    void load();
    void unload();
    
    GLuint getHandle() const;
};

}

#endif // PGG_TEXTURE_HPP