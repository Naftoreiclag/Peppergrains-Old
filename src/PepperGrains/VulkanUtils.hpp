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

#ifndef PGG_VULKANUTILS_HPP
#define PGG_VULKANUTILS_HPP

#ifdef PGG_VULKAN

#include <stdint.h>

#include <GraphicsApiLibrary.hpp>

namespace pgg {
namespace VulkanUtils { 

bool oneTimeUseCmdBufferAllocateAndBegin(VkCommandPool cmdPool, VkCommandBuffer* cmdBuff);
void oneTimeUseCmdBufferFreeAndEndAndSubmitAndSynchronizeExecution(VkQueue queue, VkCommandPool cmdPool, VkCommandBuffer* cmdBuff);

// imm = Immediately (Creates, uses, and destroys a disposable command buffer)

void immCopyBuffer(VkBuffer src, VkBuffer dest, VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize destOffset = 0);
void immCopyImage(VkImage src, VkImageLayout srcLayout, VkImage dest, VkImageLayout destLayout, uint32_t imgWidth, uint32_t imgHeight);

void immChangeImageLayout(VkImage img, VkImageLayout oldLayout, VkImageLayout newLayout);

bool findSuitableMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags requiredProperties, uint32_t* memTypeIndex);
bool bufferCreateAndAllocate(
    VkDeviceSize size, 
    VkBufferUsageFlags usage, 
    VkMemoryPropertyFlags requiredProperties, 
    VkBuffer* buffer, VkDeviceMemory* bufferMemory);
bool imageCreateAndAllocate(
    uint32_t width, uint32_t height, 
    VkFormat format, 
    VkImageTiling tilingType, 
    VkImageUsageFlags usage, 
    VkMemoryPropertyFlags requiredProperties, 
    VkImage* imageHandle, VkDeviceMemory* imageMemory);

/* case 2: return VK_INDEX_TYPE_UINT16;
 * case 4: return VK_INDEX_TYPE_UINT32;
 * default: return VK_INDEX_TYPE_END_RANGE;
 */
VkIndexType indexTypeFromSize(uint8_t size);

// Returns true if the physical device supports a particular format / image tiling type / format features combination
bool physDeviceSupportsFormat(VkFormat format, VkImageTiling imageTilingType, VkFormatFeatureFlags requiredFormatFeatures);

// Returns true if the given format has a stencil component
bool formatHasStencilComponent(VkFormat format);

} // VulkanUtils
} // pgg

#endif // PGG_VULKAN

#endif // PGG_VULKANUTILS_HPP
