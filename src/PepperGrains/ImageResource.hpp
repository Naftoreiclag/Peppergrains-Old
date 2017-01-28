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

#ifndef PGG_IMAGERESOURCE_HPP
#define PGG_IMAGERESOURCE_HPP

#include <stdint.h>

#include <GraphicsApiLibrary.hpp>

#include "Resource.hpp"
#include "Image.hpp"

namespace pgg {

class ImageResource : public Image, public Resource {
private:
    
    #ifdef PGG_VULKAN
    VkImage mImgHandle = VK_NULL_HANDLE;
    VkDeviceMemory mImgMemory = VK_NULL_HANDLE;
    VkImageView mImgView = VK_NULL_HANDLE;
    VkFormat mImgFormat;
    VkImageLayout mImgLayout;
    #endif // PGG_VULKAN
    
    
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mComponents;
    bool mLoaded;
public:
    ImageResource();
    ~ImageResource();
    
    static Image* gallop(Resource* resource);
    
    void load();
    void unload();
    
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getNumComponents() const;
    
    #ifdef PGG_VULKAN
    VkImage getHandle() const;
    VkDeviceMemory getMemory() const;
    VkImageView getView() const;
    VkFormat getFormat() const;
    VkImageLayout getLayout() const;
    #endif // PGG_VULKAN
};

}

#endif // IMAGERESOURCE_HPP
