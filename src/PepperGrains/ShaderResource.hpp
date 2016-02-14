#ifndef ShaderResource_HPP
#define ShaderResource_HPP

#include <OpenGLStuff.hpp>

#include "Resource.hpp"

namespace pgg {

class ShaderResource : public Resource {
public:
    ShaderResource();
    virtual ~ShaderResource();
    
    virtual bool load() = 0;
    virtual bool unload() = 0;
    
    virtual GLuint getHandle() = 0;
};

}

#endif // ShaderResource_HPP
