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

#include "ImageResource.hpp"

#include <cassert>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace pgg {

ImageResource::ImageResource()
: mLoaded(false)
, mIsErrorResource(false) {
}

ImageResource::~ImageResource() {
}
void ImageResource::loadError() {
    mWidth = 8;
    mHeight = 8;
    mComponents = 3;
    mImage = new uint8_t[mWidth * mHeight * mComponents];
    
    float amt = 255.f / ((float) mWidth);
    
    for(uint32_t y = 0; y < mHeight; ++ y) {
        for(uint32_t x = 0; x < mWidth; ++ x) {
            float fx = x;
            float fy = y;
            
            mImage[((y * mWidth) + x) * mComponents + 0] = ((x + y) % 2 ) == 0 ? fx * amt : fx * amt;
            mImage[((y * mWidth) + x) * mComponents + 1] = ((x + y) % 2 ) == 0 ? fy * amt : fy * amt;
            mImage[((y * mWidth) + x) * mComponents + 2] = ((x + y) % 2 ) == 0 ? 0 : 255;
        }
    }
    mLoaded = true;
    mIsErrorResource = true;
    
    std::cout << "Image error: [" << this->getName() << "]" << std::endl;
}

void ImageResource::unloadError() {
    assert(mLoaded && "Attempted to unload image before loading it");
    delete[] mImage;
    mLoaded = false;
    mIsErrorResource = false;
}

void ImageResource::load() {
    assert(!mLoaded && "Attempted to load image that has already been loaded");
    
    if(this->isFallback()) {
        loadError();
        return;
    }
    
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
    
    if(mIsErrorResource) {
        unloadError();
        return;
    }
    
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
