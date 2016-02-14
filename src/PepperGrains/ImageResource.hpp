#ifndef IMAGERESOURCE_HPP
#define IMAGERESOURCE_HPP

#include <stdint.h>

#include "Resource.hpp"

namespace pgg {

class ImageResource : public Resource {
private:
    uint8_t* mImage;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mComponents;
    bool mLoaded;
public:
    ImageResource();
    virtual ~ImageResource();
    
    bool load();
    bool unload();
    
    const uint8_t* getImage() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getNumComponents() const;
};

}

#endif // IMAGERESOURCE_HPP
