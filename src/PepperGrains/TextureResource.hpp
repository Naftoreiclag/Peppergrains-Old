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

#ifndef PGG_TEXTURERESOURCE_HPP
#define PGG_TEXTURERESOURCE_HPP

#include <OpenGLStuff.hpp>

#include "Resource.hpp"
#include "ImageResource.hpp"

namespace pgg {

class TextureResource : public Resource {
private:
    GLuint mHandle;
    bool mLoaded;

    // (Not actually required to keep the image loaded)
    ImageResource* mImage;
public:
    TextureResource();
    virtual ~TextureResource();
    
    void load();
    void unload();
    
    GLuint getHandle() const;

    static GLenum toEnum(const std::string& val, GLenum errorVal);

};

}

#endif // TEXTURERESOURCE_HPP
