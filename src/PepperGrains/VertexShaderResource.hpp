#ifndef VertexShaderResource_HPP
#define VertexShaderResource_HPP

#include <OpenGLStuff.hpp>

#include "ShaderResource.hpp"

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

#endif // VertexShaderResource_HPP
