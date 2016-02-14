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

    std::ifstream loader(this->getFile().c_str());
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
    glDeleteShader(mVertShader);
    return true;
}

GLuint VertexShaderResource::getHandle() {
    return mVertShader;
}

}
