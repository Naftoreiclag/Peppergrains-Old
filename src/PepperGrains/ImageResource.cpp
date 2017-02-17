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

#include <GraphicsApiLibrary.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Video.hpp"
#include "Logger.hpp"
#include "Resources.hpp"
#include "VulkanUtils.hpp"

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

#ifdef PGG_VULKAN
void ImageResource::load() {
    assert(!mLoaded && "Attempted to load image that has already been loaded");

    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out wout = Logger::log(Logger::WARN);
    
    uint8_t* rawImgData = nullptr;
    
    // Read image using stbi
    {
        int width;
        int height;
        int components;
        rawImgData = stbi_load(this->getFile().string().c_str(), &width, &height, &components, 0);
        mWidth = width;
        mHeight = height;
        mComponents = components;
        vout << "width: " << width << std::endl;
        vout << "height: " << height << std::endl;
    }
    
    #ifdef PGG_VULKAN
    
    VkResult result;
    bool success;
    
    
    if(mComponents == 1) {
        mImgFormat = VK_FORMAT_R8_UNORM;
    }
    else if(mComponents == 2) {
        mImgFormat = VK_FORMAT_R8G8_UNORM;
    }
    else if(mComponents == 3) {
        mImgFormat = VK_FORMAT_R8G8B8_UNORM;
    }
    else if(mComponents == 4) {
        mImgFormat = VK_FORMAT_R8G8B8A8_UNORM;
    } else {
        wout << "Unsupported image component count: " << mComponents << std::endl;
        mImgFormat = VK_FORMAT_R8G8B8A8_UNORM;
        // crash?
    }
    
        
    uint32_t mImageSize = mWidth * mHeight * mComponents * sizeof(uint8_t);
    
    VkImage stagingImgHandle = VK_NULL_HANDLE;
    VkDeviceMemory stagingImgMemory = VK_NULL_HANDLE;
    success = Video::Vulkan::Utils::imageCreateAndAllocate(
        mWidth, mHeight, 
        mImgFormat, 
        VK_IMAGE_TILING_LINEAR, 
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        &stagingImgHandle, &stagingImgMemory);
    
    if(!success) {
        wout << "Unable to create image and allocate memory for staging image" << std::endl;
        // crash?
    }
    
    void* memAddr;
    vkMapMemory(Video::Vulkan::getLogicalDevice(), stagingImgMemory, 0, mImageSize, 0, &memAddr);
    
    VkImageSubresource imgSubresQuery; {
        imgSubresQuery.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgSubresQuery.mipLevel = 0;
        imgSubresQuery.arrayLayer = 0;
    }
    VkSubresourceLayout stagingImgSubresLayout;
    vkGetImageSubresourceLayout(Video::Vulkan::getLogicalDevice(), stagingImgHandle, &imgSubresQuery, &stagingImgSubresLayout);
    
    // Image as represented in RAM exactly matches how it is represented in GPU memory, so just copy it
    if(stagingImgSubresLayout.rowPitch == mWidth * mComponents * sizeof(uint8_t)) {
        memcpy(memAddr, rawImgData, mImageSize);
    }
    
    // Image as represented in RAM has some extra padding between rows, requiring multiple calls to memcpy
    else {
        uint8_t* destImgMem = reinterpret_cast<uint8_t*>(memAddr);
        for(uint32_t y = 0; y < mHeight; ++ y) {
            memcpy(
                &(destImgMem[y * stagingImgSubresLayout.rowPitch]), 
                &(rawImgData[y * mWidth * mComponents]), 
                mWidth * mComponents);
        }
    }
    
    vkUnmapMemory(Video::Vulkan::getLogicalDevice(), stagingImgMemory);
    
    // Free up image from ram, unneeded now
    stbi_image_free(rawImgData);
    
    
    success = Video::Vulkan::Utils::imageCreateAndAllocate(
        mWidth, mHeight, 
        mImgFormat,
        VK_IMAGE_TILING_OPTIMAL, // Tiling can differ, in this case just use whatever is optimal for the GPU since we won't read from this anyway
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        &mImgHandle, &mImgMemory);
    
    if(!success) {
        wout << "Unable to create image and allocate memory for destination image" << std::endl;
        // crash?
    }
    
    /*
    VkCommandBuffer imgUploadCmdBuffer;
    Video::Vulkan::Utils::oneTimeUseCmdBufferAllocateAndBegin(Video::Vulkan::getGraphicsCommandPool(), &imgUploadCmdBuffer);
    //Video::Vulkan::Utils::oneTimeUseCmdBufferAllocateAndBegin(Video::Vulkan::getTransferCommandPool(), &imgUploadCmdBuffer);
    Video::Vulkan::Utils::oneTimeUseCmdBufferFreeAndEndAndSubmitAndSynchronizeExecution(Video::Vulkan::getGraphicsQueue(), Video::Vulkan::getGraphicsCommandPool(), &imgUploadCmdBuffer);
    //Video::Vulkan::Utils::oneTimeUseCmdBufferFreeAndEndAndSubmitAndSynchronizeExecution(Video::Vulkan::getTransferQueue(), Video::Vulkan::getTransferCommandPool(), &imgUploadCmdBuffer);
    */
    
    // TODO: perform in a single immediate command buffer
    
    Video::Vulkan::Utils::immChangeImageLayout(
        stagingImgHandle, 
        mImgFormat, 
        VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    Video::Vulkan::Utils::immChangeImageLayout(
        mImgHandle, 
        mImgFormat, 
        VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        //VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DEST_OPTIMAL);
    Video::Vulkan::Utils::immCopyImage(
        stagingImgHandle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        mImgHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        mWidth,
        mHeight);
    mImgLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    Video::Vulkan::Utils::immChangeImageLayout(
        mImgHandle, 
        mImgFormat, 
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mImgLayout);
    
    vkFreeMemory(Video::Vulkan::getLogicalDevice(), stagingImgMemory, nullptr);
    vkDestroyImage(Video::Vulkan::getLogicalDevice(), stagingImgHandle, nullptr);
    
    VkImageViewCreateInfo imageViewCstrArgs; {
        imageViewCstrArgs.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCstrArgs.pNext = nullptr;
        imageViewCstrArgs.flags = 0;
        imageViewCstrArgs.image = mImgHandle;
        imageViewCstrArgs.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCstrArgs.format = mImgFormat;
        imageViewCstrArgs.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCstrArgs.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCstrArgs.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCstrArgs.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCstrArgs.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCstrArgs.subresourceRange.baseMipLevel = 0;
        imageViewCstrArgs.subresourceRange.levelCount = 1;
        imageViewCstrArgs.subresourceRange.baseArrayLayer = 0;
        imageViewCstrArgs.subresourceRange.layerCount = 1;
    }
    
    result = vkCreateImageView(Video::Vulkan::getLogicalDevice(), &imageViewCstrArgs, nullptr, &mImgView);
    if(result != VK_SUCCESS) {
        wout << "Could not create image view for image" << std::endl;
        return;
    }
    
    #endif // PGG_VULKAN
    
    mLoaded = true;
}
void ImageResource::unload() {
    assert(mLoaded && "Attempted to unload image before loading it");
    
    #ifdef PGG_VULKAN
    vkDestroyImageView(Video::Vulkan::getLogicalDevice(), mImgView, nullptr);
    mImgView = VK_NULL_HANDLE;
    vkFreeMemory(Video::Vulkan::getLogicalDevice(), mImgMemory, nullptr);
    mImgMemory = VK_NULL_HANDLE;
    vkDestroyImage(Video::Vulkan::getLogicalDevice(), mImgHandle, nullptr);
    mImgHandle = VK_NULL_HANDLE;
    #endif // PGG_VULKAN
    
    mLoaded = false;
}
#endif // PGG_VULKAN

uint32_t ImageResource::getWidth() const { return mWidth; }
uint32_t ImageResource::getHeight() const { return mHeight; }
uint32_t ImageResource::getNumComponents() const { return mComponents; }

#ifdef PGG_VULKAN
VkImage ImageResource::getHandle() const { return mImgHandle; }
VkDeviceMemory ImageResource::getMemory() const { return mImgMemory; }
VkImageView ImageResource::getView() const { return mImgView; }
VkFormat ImageResource::getFormat() const { return mImgFormat; }
VkImageLayout ImageResource::getLayout() const { return mImgLayout; }
#endif // PGG_VULKAN

}
