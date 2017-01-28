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

#ifndef PGG_IMAGE_HPP
#define PGG_IMAGE_HPP

#include <stdint.h>

#include <GraphicsApiLibrary.hpp>

#include "ReferenceCounted.hpp"

namespace pgg {

// Virtual inheritance to avoid diamond conflict with ImageResource
class Image : virtual public ReferenceCounted {
public:
    Image();
    virtual ~Image();
    
    static Image* getFallback();
    
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual uint32_t getNumComponents() const = 0;
    
    #ifdef PGG_VULKAN
    virtual VkImage getHandle() const = 0;
    virtual VkDeviceMemory getMemory() const = 0;
    virtual VkImageView getView() const = 0;
    virtual VkFormat getFormat() const = 0;
    virtual VkImageLayout getLayout() const = 0;
    #endif // PGG_VULKAN
};

class FallbackImage : public Image {
private:
    bool mLoaded;
    
public:
    FallbackImage();
    ~FallbackImage();
    
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

#endif // PGG_IMAGE_HPP
