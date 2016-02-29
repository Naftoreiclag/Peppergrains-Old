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

#include "VertexShaderResource.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace pgg {

VertexShaderResource::VertexShaderResource()
: mLoaded(false) {
}

VertexShaderResource::~VertexShaderResource() {
}

bool VertexShaderResource::load() {
    if(mLoaded) {
        return true;
    }

    std::ifstream loader(this->getFile().string().c_str());
    std::stringstream ss;
    ss << loader.rdbuf();
    loader.close();

    std::string vertSrcStr = ss.str();
    const GLchar* vertSrc = vertSrcStr.c_str();

    mVertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(mVertShader, 1, &vertSrc, 0);
    glCompileShader(mVertShader);

    mLoaded = true;
    return true;
}

bool VertexShaderResource::unload() {
    assert(mLoaded && "Attempted to unload vertex shader before loading it");
    glDeleteShader(mVertShader);
    mLoaded = false;
    return true;
}

GLuint VertexShaderResource::getHandle() {
    return mVertShader;
}

}
