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

#include "ShaderResource.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace pgg {

ShaderResource::ShaderResource(ShaderResource::Type type)
: mType(type)
, mLoaded(false) {
}

ShaderResource::~ShaderResource() {
}

void ShaderResource::load() {
    assert(!mLoaded && "Attempted to load shader that is already loaded");

    std::ifstream loader(this->getFile().string().c_str());
    std::stringstream ss;
    ss << loader.rdbuf();
    loader.close();

    std::string shaderSrcStr = ss.str();
    const GLchar* shaderSrc = shaderSrcStr.c_str();

    switch(mType) {
        case COMPUTE: {
            mHandle = glCreateShader(GL_COMPUTE_SHADER);
            break;
        }
        case VERTEX: {
            mHandle = glCreateShader(GL_VERTEX_SHADER);
            break;
        }
        case TESS_CONTROL: {
            mHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
            break;
        }
        case TESS_EVALUATION: {
            mHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
            break;
        }
        case GEOMETRY: {
            mHandle = glCreateShader(GL_GEOMETRY_SHADER);
            break;
        }
        case FRAGMENT: {
            mHandle = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        }
        default: {
            assert(false && "Unknown shader type?");
            break; // what
        }
    }
    glShaderSource(mHandle, 1, &shaderSrc, 0);
    glCompileShader(mHandle);
    
    GLint compileStatus;
    glGetShaderiv(mHandle, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE) {
        std::cout << "Error while compiling ";
        switch(mType) {
            case COMPUTE: {
                std::cout << "COMPUTE";
                break;
            }
            case VERTEX: {
                std::cout << "VERTEX";
                break;
            }
            case TESS_CONTROL: {
                std::cout << "TESS CONTROL";
                break;
            }
            case TESS_EVALUATION: {
                std::cout << "TESS EVALUATION";
                break;
            }
            case GEOMETRY: {
                std::cout << "GEOMETRY";
                break;
            }
            case FRAGMENT: {
                std::cout << "FRAGMENT";
                break;
            }
            default: {
                break; // what
            }
        }
        std::cout << " shader " << this->getName() << std::endl;
        char infoLog[512];
        glGetShaderInfoLog(mHandle, 512, 0, infoLog);
        std::cout << infoLog;
        std::cout << shaderSrcStr;
    } else {
        std::cout << "Compiled shader " << this->getName() << std::endl;
    }

    mLoaded = true;
}

void ShaderResource::unload() {
    assert(mLoaded && "Attempted to unload shader before loading it");
    glDeleteShader(mHandle);
    mLoaded = false;
}

GLuint ShaderResource::getHandle() {
    return mHandle;
}

}
