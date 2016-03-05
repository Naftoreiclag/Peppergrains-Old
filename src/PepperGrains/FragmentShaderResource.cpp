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

#include "FragmentShaderResource.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

namespace pgg {

FragmentShaderResource::FragmentShaderResource()
: mLoaded(false) {
}

FragmentShaderResource::~FragmentShaderResource() {
}

void FragmentShaderResource::load() {
    assert(!mLoaded && "Attempted to load fragment shader that is already loaded");

    std::ifstream loader(this->getFile().string().c_str());
    std::stringstream ss;
    ss << loader.rdbuf();
    loader.close();

    std::string fragSrcStr = ss.str();
    const GLchar* fragSrc = fragSrcStr.c_str();

    mFragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(mFragShader, 1, &fragSrc, 0);
    glCompileShader(mFragShader);
    
    GLint compileStatus;
    glGetShaderiv(mFragShader, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE) {
        std::cout << "Error while compiling shader " << this->getName() << std::endl;
        char infoLog[512];
        glGetShaderInfoLog(mFragShader, 512, 0, infoLog);
        std::cout << infoLog;
    }

    mLoaded = true;
}

void FragmentShaderResource::unload() {
    assert(mLoaded && "Attempted to unload fragment shader before loading it");
    glDeleteShader(mFragShader);
    mLoaded = false;
}

GLuint FragmentShaderResource::getHandle() {
    return mFragShader;
}

}
