#include "ImageResource.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace pgg {

ImageResource::ImageResource()
: mLoaded(false) {
}

ImageResource::~ImageResource() {
}

bool ImageResource::load() {
    if(mLoaded) {
        return true;
    }

    int width;
    int height;
    int components;
    mImage = stbi_load(this->getFile().c_str(), &width, &height, &components, 0);
    mWidth = width;
    mHeight = height;
    mComponents = components;
    mLoaded = true;
    return true;
}

bool ImageResource::unload() {
    stbi_image_free(mImage);
    mLoaded = false;
    return true;
}

const uint8_t* ImageResource::getImage() const {
    return mImage;
}

uint32_t ImageResource::getWidth() const {
    return mWidth;
}
uint32_t ImageResource::getHeight() const {
    return mHeight;
}
uint32_t ImageResource::getNumComponents() const {
    return mComponents;
}

}
