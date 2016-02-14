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

#ifndef PGG_VertexShaderResource_HPP
#define PGG_VertexShaderResource_HPP

#include <OpenGLStuff.hpp>

#include "ShaderResource.hpp"

namespace pgg {

class VertexShaderResource : public ShaderResource {
private:
    GLuint mVertShader;
    bool mLoaded;
public:
    VertexShaderResource();
    virtual ~VertexShaderResource();
    
    bool load();
    bool unload();
    
    GLuint getHandle();
};

}

#endif // VertexShaderResource_HPP
