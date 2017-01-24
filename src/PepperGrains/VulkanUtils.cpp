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
namespace VulkanUtils {

bool oneTimeUseCmdBufferAllocateAndBegin(VkCommandPool cmdPool, VkCommandBuffer* cmdBuffer) {
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
void oneTimeUseCmdBufferFreeAndEndAndSubmit(VkQueue queue, VkCommandPool cmdPool, VkCommandBuffer* cmdBuff) {
    
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

VkIndexType indexTypeFromSize(uint8_t size) {
    switch(size) {
        case 2: return VK_INDEX_TYPE_UINT16;
        case 4: return VK_INDEX_TYPE_UINT32;
        default: return VK_INDEX_TYPE_END_RANGE;
    }
}

bool physDeviceSupportsFormat(VkFormat format, VkImageTiling imageTilingType, VkFormatFeatureFlags requiredFormatFeatures) {
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(Video::Vulkan::getPhysicalDevice(), format, &formatProps);
    
    switch(imageTilingType) {
        case VK_IMAGE_TILING_LINEAR: {
            if((formatProps.linearTilingFeatures & requiredFormatFeatures) == requiredFormatFeatures) return true;
        }
        
        case VK_IMAGE_TILING_OPTIMAL: {
            if((formatProps.optimalTilingFeatures & requiredFormatFeatures) == requiredFormatFeatures) return true;
        }
        
        // Note: future Vulkan specifications may have more tiling types
    }
    return false;
}

bool formatHasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_S8_UINT ||
        format == VK_FORMAT_D16_UNORM_S8_UINT ||
        format == VK_FORMAT_D24_UNORM_S8_UINT ||
        format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}



} // VulkanUtils
} // pgg

#endif
