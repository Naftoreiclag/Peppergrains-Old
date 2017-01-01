/*
   Copyright 2015-2017 James Fong

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

#include "ImageResource.hpp"

#include <cassert>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Logger.hpp"
#include "Resources.hpp"

namespace pgg {

ImageResource::ImageResource()
: mLoaded(false)
, Resource(Resource::Type::IMAGE) {
}

ImageResource::~ImageResource() {
}

Image* ImageResource::gallop(Resource* resource) {
    if(!resource || resource->mResourceType != Resource::Type::IMAGE) {
        Logger::log(Logger::WARN) << "Failed to cast " << (resource ? resource->getName() : "nullptr") << " to image!" << std::endl;
        return Image::getFallback();
    } else {
        return static_cast<ImageResource*>(resource);
    }
}

void ImageResource::load() {
    assert(!mLoaded && "Attempted to load image that has already been loaded");
    
    int width;
    int height;
    int components;
    mImage = stbi_load(this->getFile().string().c_str(), &width, &height, &components, 0);
    mWidth = width;
    mHeight = height;
    mComponents = components;
    mLoaded = true;
}

void ImageResource::unload() {
    assert(mLoaded && "Attempted to unload image before loading it");
    
    stbi_image_free(mImage);
    mLoaded = false;
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
