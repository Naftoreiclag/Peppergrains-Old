#include "FragmentShaderResource.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

FragmentShaderResource::FragmentShaderResource()
: mLoaded(false) {
}

FragmentShaderResource::~FragmentShaderResource() {
}

bool FragmentShaderResource::load() {
    if(mLoaded) {
        return true;
    }

    std::ifstream loader(this->getFile().c_str());
    std::stringstream ss;
    ss << loader.rdbuf();
    loader.close();

    std::string fragSrcStr = ss.str();
    const GLchar* fragSrc = fragSrcStr.c_str();

    mFragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(mFragShader, 1, &fragSrc, 0);
    glCompileShader(mFragShader);

    mLoaded = true;
    return true;
}

bool FragmentShaderResource::unload() {
    glDeleteShader(mFragShader);
    return true;
}

GLuint FragmentShaderResource::getHandle() {
    return mFragShader;
}
