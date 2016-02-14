#ifndef TEXTURERESOURCE_HPP
#define TEXTURERESOURCE_HPP

#include <OpenGLStuff.hpp>

#include "Resource.hpp"

namespace pgg {

class TextureResource : public Resource {
private:
    GLuint mHandle;
    bool mLoaded;
public:
    TextureResource();
    virtual ~TextureResource();
    
    bool load();
    bool unload();
    
    GLuint getHandle() const;

    static GLenum toEnum(const std::string& val, GLenum errorVal);

};

}

#endif // TEXTURERESOURCE_HPP
