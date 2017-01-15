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

bool makeBufferAndAllocateMemory(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags requiredProperties, VkBuffer* buffer, VkDeviceMemory* bufferMemory) {

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
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
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

VkIndexType indexTypeFromSize(uint8_t size) {
    switch(size) {
        case 2: return VK_INDEX_TYPE_UINT16;
        case 4: return VK_INDEX_TYPE_UINT32;
        default: return VK_INDEX_TYPE_END_RANGE;
    }
}

} // VulkanUtils
} // pgg

#endif
