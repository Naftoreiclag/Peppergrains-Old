#ifndef FragmentShaderResource_HPP
#define FragmentShaderResource_HPP

#include <OpenGLStuff.hpp>

#include "ShaderResource.hpp"

namespace pgg {

class FragmentShaderResource : public ShaderResource {
private:
    GLuint mFragShader;
    bool mLoaded;
public:
    FragmentShaderResource();
    virtual ~FragmentShaderResource();

    bool load();
    bool unload();

    GLuint getHandle();
};

}

#endif // FragmentShaderResource_HPP
