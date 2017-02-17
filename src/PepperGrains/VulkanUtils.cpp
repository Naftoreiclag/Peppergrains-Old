/*
   Copyright 2017 James Fong

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

#ifdef PGG_VULKAN

#include "VulkanUtils.hpp"

#include "Video.hpp"
#include "Logger.hpp"

namespace pgg {
namespace Video {
namespace Vulkan { 
namespace Utils {

bool immediateCmdBufferBegin(VkCommandPool cmdPool, VkCommandBuffer* cmdBuffer) {
    VkCommandBufferAllocateInfo cbaArgs; {
        cbaArgs.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cbaArgs.pNext = nullptr;
        cbaArgs.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cbaArgs.commandPool = cmdPool;
        cbaArgs.commandBufferCount = 1;
    }
    
    VkResult result;
    
    result = vkAllocateCommandBuffers(Video::Vulkan::getLogicalDevice(), &cbaArgs, cmdBuffer);
    if(result != VK_SUCCESS) {
        Logger::log(Logger::WARN) << "Could not allocate one-time-use command buffer" << std::endl;
        (*cmdBuffer) = VK_NULL_HANDLE;
        return false;
    }
    
    VkCommandBufferBeginInfo cbbArgs; {
        cbbArgs.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cbbArgs.pNext = nullptr;
        cbbArgs.pInheritanceInfo = nullptr;
        cbbArgs.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }
    
    vkBeginCommandBuffer(*cmdBuffer, &cbbArgs);
    
    return cmdBuffer;
}
void immediateCmdBufferEnd(VkQueue queue, VkCommandPool cmdPool, VkCommandBuffer* cmdBuff) {
    
    vkEndCommandBuffer(*cmdBuff);
    
    VkSubmitInfo sArgs; {
        sArgs.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        sArgs.pNext = nullptr;
        sArgs.commandBufferCount = 1;
        sArgs.pCommandBuffers = cmdBuff;
        sArgs.signalSemaphoreCount = 0;
        sArgs.waitSemaphoreCount = 0;
    }
    
    vkQueueSubmit(queue, 1, &sArgs, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    
    vkFreeCommandBuffers(Video::Vulkan::getLogicalDevice(), cmdPool, 1, cmdBuff);
    
    (*cmdBuff) = VK_NULL_HANDLE;
}


void cmdCopyBuffer(VkCommandBuffer cmdBuff, VkBuffer src, VkBuffer dest, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize destOffset) {
    VkBufferCopy buffCopy; {
        buffCopy.size = size;
        buffCopy.srcOffset = srcOffset;
        buffCopy.dstOffset = destOffset;
    }
    
    vkCmdCopyBuffer(cmdBuff, src, dest, 1, &buffCopy);
}
void immCopyBuffer(VkBuffer src, VkBuffer dest, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize destOffset) {
    VkCommandBuffer cmdBuff;
    Video::Vulkan::Utils::immediateCmdBufferBegin(Video::Vulkan::getTransferCommandPool(), &cmdBuff);
    cmdCopyBuffer(cmdBuff, src, dest, size, srcOffset, destOffset);
    Video::Vulkan::Utils::immediateCmdBufferEnd(Video::Vulkan::getTransferQueue(), Video::Vulkan::getTransferCommandPool(), &cmdBuff);
}
void cmdCopyImage(VkCommandBuffer cmdBuff, VkImage src, VkImageLayout srcLayout, VkImage dest, VkImageLayout destLayout, uint32_t imgWidth, uint32_t imgHeight) {
    
    VkImageSubresourceLayers imgSubresLayers; {
        imgSubresLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgSubresLayers.baseArrayLayer = 0;
        imgSubresLayers.layerCount = 1;
        imgSubresLayers.mipLevel = 0;
    }
    
    VkImageCopy imgCopy; {
        imgCopy.srcSubresource = imgSubresLayers;
        imgCopy.dstSubresource = imgSubresLayers;
        imgCopy.srcOffset = {0, 0, 0};
        imgCopy.dstOffset = {0, 0, 0};
        imgCopy.extent = {imgWidth, imgHeight, 1};
    }
    
    vkCmdCopyImage(cmdBuff, src, srcLayout, dest, destLayout, 1, &imgCopy);
    
}
void immCopyImage(VkImage src, VkImageLayout srcLayout, VkImage dest, VkImageLayout destLayout, uint32_t imgWidth, uint32_t imgHeight) {
    VkCommandBuffer cmdBuff;
    Video::Vulkan::Utils::immediateCmdBufferBegin(Video::Vulkan::getTransferCommandPool(), &cmdBuff);
    cmdCopyImage(cmdBuff, src, srcLayout, dest, destLayout, imgWidth, imgHeight);
    Video::Vulkan::Utils::immediateCmdBufferEnd(Video::Vulkan::getTransferQueue(), Video::Vulkan::getTransferCommandPool(), &cmdBuff);
}

void immChangeImageLayout(VkImage img, VkFormat imgFormat, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer cmdBuff;
    
    Video::Vulkan::Utils::immediateCmdBufferBegin(Video::Vulkan::getTransferCommandPool(), &cmdBuff);
    
    VkImageMemoryBarrier imgMemoryBarrier; {
        imgMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imgMemoryBarrier.pNext = nullptr;
        
        // Handled below:
        //imgMemoryBarrier.srcAccessMask = 0;
        //imgMemoryBarrier.dstAccessMask = 0;
        
        imgMemoryBarrier.oldLayout = oldLayout;
        imgMemoryBarrier.newLayout = newLayout;
        
        imgMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imgMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        
        imgMemoryBarrier.image = img;
        //imgMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        imgMemoryBarrier.subresourceRange.baseMipLevel = 0;
        imgMemoryBarrier.subresourceRange.layerCount = 1;
        imgMemoryBarrier.subresourceRange.levelCount = 1;
    }
    
    if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        imgMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if(formatHasStencilComponent(imgFormat)) {
            imgMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        imgMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
        
    switch(oldLayout) {
        case VK_IMAGE_LAYOUT_UNDEFINED: {
            imgMemoryBarrier.srcAccessMask = 0;
            break;
        }
        case VK_IMAGE_LAYOUT_PREINITIALIZED: {
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: {
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: {
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: {
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: {
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: {
            imgMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        }
        default: {
            Logger::log(Logger::WARN)
                << "Unsupported initial image layout during layout transition: "
                << oldLayout << std::endl;
            imgMemoryBarrier.srcAccessMask = VK_IMAGE_LAYOUT_GENERAL;
            break;
        }
    }
    
    switch(newLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: {
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: {
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: {
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: {
            imgMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_UNDEFINED:
        case VK_IMAGE_LAYOUT_PREINITIALIZED: {
            Logger::log(Logger::WARN)
                << "Cannot transition image to layout: "
                << (newLayout == VK_IMAGE_LAYOUT_PREINITIALIZED ? "Preinitialized" : "Undefined") << std::endl;
            imgMemoryBarrier.dstAccessMask = VK_IMAGE_LAYOUT_GENERAL;
            break;
        }
        default: {
            Logger::log(Logger::WARN)
                << "Unsupported final image layout during layout transition: "
                << newLayout << std::endl;
            imgMemoryBarrier.dstAccessMask = VK_IMAGE_LAYOUT_GENERAL;
            break;
        }
    }
    
    vkCmdPipelineBarrier(
        cmdBuff, 
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
        0, // Dependency flags?
        // Generic memory barriers
        0, nullptr, 
        // Buffer memory barriers
        0, nullptr, 
        // Image memory barriers
        1, &imgMemoryBarrier);
    
    
    Video::Vulkan::Utils::immediateCmdBufferEnd(Video::Vulkan::getTransferQueue(), Video::Vulkan::getTransferCommandPool(), &cmdBuff);
    
}

bool findSuitableMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags requiredProperties, uint32_t* memTypeIndex) {
    VkPhysicalDeviceMemoryProperties physMemProps = Video::Vulkan::getPhysicalDeviceMemoryProperties();
    
    for(uint32_t i = 0; i < physMemProps.memoryTypeCount; ++ i) {
        if((allowedTypes & (1 << i)) && 
            ((physMemProps.memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties)) {
            (*memTypeIndex) = i;
            return true;
        }
    }
    
    return false;
}

bool bufferCreateAndAllocate(
    VkDeviceSize size, 
    VkBufferUsageFlags usage, 
    VkMemoryPropertyFlags requiredProperties, 
    VkBuffer* buffer, VkDeviceMemory* bufferMemory) {

    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    VkResult result;
    
    VkBufferCreateInfo buffInfo; {
        buffInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffInfo.pNext = nullptr;
        buffInfo.flags = 0;
        
        buffInfo.size = size;
        buffInfo.usage = usage;
        buffInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    
    result = vkCreateBuffer(Video::Vulkan::getLogicalDevice(), &buffInfo, nullptr, buffer);
    
    if(result != VK_SUCCESS) {
        Logger::log(Logger::WARN) << "Could not create buffer" << std::endl;
        return false;
    }
    
    VkMemoryRequirements bufferMemReq;
    vkGetBufferMemoryRequirements(Video::Vulkan::getLogicalDevice(), *buffer, &bufferMemReq);
    
    uint32_t memoryTypeIndex;
    bool success = findSuitableMemoryTypeIndex(bufferMemReq.memoryTypeBits, 
                requiredProperties,
                &memoryTypeIndex);
                
    if(!success) {
        sout << "Could not find memory type for buffer" << std::endl;
        return false;
    }
    
    VkMemoryAllocateInfo allocArgs; {
        allocArgs.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocArgs.pNext = nullptr;
        
        allocArgs.allocationSize = bufferMemReq.size;
        allocArgs.memoryTypeIndex = memoryTypeIndex;
    }
    
    result = vkAllocateMemory(Video::Vulkan::getLogicalDevice(), &allocArgs, nullptr, bufferMemory);
    
    if(result != VK_SUCCESS) {
        sout << "Could not allocate memory for buffer" << std::endl;
        return false;
    }
    
    vkBindBufferMemory(Video::Vulkan::getLogicalDevice(), *buffer, *bufferMemory, 0);
    
    return true;
}

bool imageCreateAndAllocate(
    uint32_t width, uint32_t height, 
    VkFormat format, 
    VkImageTiling tilingType, 
    VkImageUsageFlags usage, 
    VkMemoryPropertyFlags requiredProperties, 
    VkImage* imageHandle, VkDeviceMemory* imageMemory) {
    
    VkResult result;
    bool success;
    
    VkImageCreateInfo imageInfo; {
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.pNext = nullptr;
        imageInfo.flags = 0;
        
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tilingType;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    }
    
    result = vkCreateImage(Video::Vulkan::getLogicalDevice(), &imageInfo, nullptr, imageHandle);
    
    if(result != VK_SUCCESS) {
        Logger::log(Logger::WARN) << "Could not create image" << std::endl;
        return false;
    }
    
    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(Video::Vulkan::getLogicalDevice(), *imageHandle, &memReq);
    
    uint32_t memoryTypeIndex;
    success = findSuitableMemoryTypeIndex(memReq.memoryTypeBits, requiredProperties, &memoryTypeIndex);
    
    VkMemoryAllocateInfo allocInfo; {
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;
    }
    
    result = vkAllocateMemory(Video::Vulkan::getLogicalDevice(), &allocInfo, nullptr, imageMemory);
    
    if(result != VK_SUCCESS) {
        Logger::log(Logger::WARN) << "Could not allocate memory for image" << std::endl;
        return false;
    }
    
    vkBindImageMemory(Video::Vulkan::getLogicalDevice(), *imageHandle, *imageMemory, 0);
    
    return true;
}

bool imageViewCreate(
    VkImage img, 
    VkFormat imgFormat, 
    VkImageAspectFlags aspectFlags, 
    VkImageView* imgView) {
        VkResult result;
        
        VkImageViewCreateInfo ivCargs; {
            ivCargs.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            ivCargs.pNext = nullptr;
            ivCargs.flags = 0;
            
            ivCargs.image = img;
            ivCargs.viewType = VK_IMAGE_VIEW_TYPE_2D;
            ivCargs.format = imgFormat;
            ivCargs.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivCargs.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivCargs.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivCargs.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            ivCargs.subresourceRange.aspectMask = aspectFlags;
            ivCargs.subresourceRange.baseMipLevel = 0;
            ivCargs.subresourceRange.levelCount = 1;
            ivCargs.subresourceRange.baseArrayLayer = 0;
            ivCargs.subresourceRange.layerCount = 1;
        }
        
        result = vkCreateImageView(Video::Vulkan::getLogicalDevice(), &ivCargs, nullptr, imgView);
        
        if(result != VK_SUCCESS) {
            Logger::log(Logger::WARN) << "Could not create image view" << std::endl;
            return false;
        }
        return true;
    }

bool physDeviceSupportsFormat(VkFormat format, VkImageTiling imageTilingType, VkFormatFeatureFlags requiredFormatFeatures) {
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(Video::Vulkan::getPhysicalDevice(), format, &formatProps);
    
    switch(imageTilingType) {
        case VK_IMAGE_TILING_LINEAR: {
            return (formatProps.linearTilingFeatures & requiredFormatFeatures) == requiredFormatFeatures;
        }
        
        case VK_IMAGE_TILING_OPTIMAL: {
            return (formatProps.optimalTilingFeatures & requiredFormatFeatures) == requiredFormatFeatures;
        }
        
        // Note: future Vulkan specifications may have more tiling types
        
        default: {
            return false;
        }
    }
}

VkIndexType indexTypeFromSize(uint8_t size) {
    switch(size) {
        case 2: return VK_INDEX_TYPE_UINT16;
        case 4: return VK_INDEX_TYPE_UINT32;
        default: return VK_INDEX_TYPE_END_RANGE;
    }
}

bool formatHasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_S8_UINT ||
        format == VK_FORMAT_D16_UNORM_S8_UINT ||
        format == VK_FORMAT_D24_UNORM_S8_UINT ||
        format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}

} // Utils
} // Vulkan
} // Video
} // pgg

#endif
