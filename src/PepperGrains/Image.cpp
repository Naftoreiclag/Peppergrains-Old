/*
   Copyright 2016 James Fong

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

#include "Image.hpp"

#include <cassert>

#include "Logger.hpp"
#include "Resources.hpp"
#include "ImageResource.hpp"

namespace pgg {

Image::Image() { }
Image::~Image() { }

Image* Image::getFallback() {
    static FallbackImage fallbackImage;
    return &fallbackImage;
}

FallbackImage::FallbackImage() { }
FallbackImage::~FallbackImage() { }

void FallbackImage::load() {
    assert(!mLoaded && "Attempted to load image that has already been loaded");
    
    mImage = new uint8_t[8 * 8 * 3];
    
    float amt = 255.f / 8.f;
    
    for(uint32_t y = 0; y < 8; ++ y) {
        for(uint32_t x = 0; x < 8; ++ x) {
            float fx = x;
            float fy = y;
            
            mImage[((y * 8) + x) * 3    ] = ((x + y) % 2) == 0 ? fx * amt : fx * amt;
            mImage[((y * 8) + x) * 3 + 1] = ((x + y) % 2) == 0 ? fy * amt : fy * amt;
            mImage[((y * 8) + x) * 3 + 2] = ((x + y) % 2) == 0 ? 0 : 255;
        }
    }
    
    mLoaded = true;
}
void FallbackImage::unload() {
    assert(mLoaded && "Attempted to unload image before loading it");
    delete[] mImage;
    
    mLoaded = false;
}

uint32_t FallbackImage::getWidth() const { return 8; }
uint32_t FallbackImage::getHeight() const { return 8; }
uint32_t FallbackImage::getNumComponents() const { return 3; }
const uint8_t* FallbackImage::getImage() const {
    return mImage;
}

}

