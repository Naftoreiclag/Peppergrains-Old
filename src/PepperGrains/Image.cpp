/*
   Copyright 2016-2017 James Fong

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
    Resource* lookup = Resources::find("Error.image");
    
    if(lookup && lookup->mResourceType == Resource::Type::IMAGE) {
        return static_cast<ImageResource*>(lookup);
    } else {
        static FallbackImage fallbackImage;
        return &fallbackImage;
    }
}

FallbackImage::FallbackImage() { }
FallbackImage::~FallbackImage() { }

void FallbackImage::load() {
    assert(!mLoaded && "Attempted to load image that has already been loaded");
    
    uint8_t* mImage = new uint8_t[8 * 8 * 3];
    
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
    
    // TODO: upload to gpu memory
    delete[] mImage;
    
    mLoaded = true;
}
void FallbackImage::unload() {
    assert(mLoaded && "Attempted to unload image before loading it");
    
    mLoaded = false;
}

uint32_t FallbackImage::getWidth() const { return 8; }
uint32_t FallbackImage::getHeight() const { return 8; }
uint32_t FallbackImage::getNumComponents() const { return 3; }

#ifdef PGG_VULKAN
VkImage FallbackImage::getHandle() const { return VK_NULL_HANDLE; }
VkDeviceMemory FallbackImage::getMemory() const { return VK_NULL_HANDLE; }
VkImageView FallbackImage::getView() const { return VK_NULL_HANDLE; }
VkFormat FallbackImage::getFormat() const { return VK_FORMAT_R8G8B8A8_UNORM; }
VkImageLayout FallbackImage::getLayout() const { return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }
#endif // PGG_VULKAN
}

