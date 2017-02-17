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
namespace Video {
namespace Vulkan { 
namespace Utils {

/**
 * Creates and allocates a temporary command buffer and then begins recording commands for it. Pretty much like the
 * start of an immediate mode call block in OpenGL. Useful for telling the GPU to do things that should not be done
 * every frame but instead be done only on special occasions, such as copying stuff in allocated memory.
 * 
 * Remember to call immediateCmdBufferEnd() when ready to submit commands and free up command buffer!
 * 
 * @param cmdPool The pool which this command buffer should belong to
 * @param cmdBuff Overwritten with pointer to newly allocated command buffer
 * @return True iff successful
 */
bool immediateCmdBufferBegin(
    VkCommandPool cmdPool, 
    VkCommandBuffer* cmdBuff);

/**
 * Sister method for immediateCmdBufferBegin(). Flushes (submits) queued commands using provided queue, and then
 * synchronizes (waits for execution on GPU to complete) and then cleans up (free up allocated memory)
 * 
 * @param queue The queue to accept the commands held on cmdBuff
 * @param cmdPool The pool to which cmdBuff belongs
 * @param cmdBuff The command buffer to execute and clean up; Overwritten with VK_NULL_HANDLE for safety
 */
void immediateCmdBufferEnd(
    VkQueue queue, 
    VkCommandPool cmdPool, 
    VkCommandBuffer* cmdBuff);

/**
 * Issues a command to the provide command buffer.
 * Copies the data stored in one buffer into another using the GPU.
 * 
 * @param cmdBuff Command buffer to issue command to
 * @param src The preallocated buffer handle from which the data will be copied
 * @param dest The preallocated buffer handle to which the data will be copied
 * @param size How many consecutive bytes will be copied
 * @param srcOffset Offset in src buffer which will be treated as the zeroth index
 * @param destOffset Offset in the dest buffer which will be treated as the zeroth index
 */
void cmdCopyBuffer(
    VkCommandBuffer cmdBuff, 
    VkBuffer src, VkBuffer dest, 
    VkDeviceSize size, 
    VkDeviceSize srcOffset = 0, VkDeviceSize destOffset = 0);

/**
 * Part of the immediate function family (prefix "imm"). Does the same thing as 
 * cmdCopyBuffer() 
 * except it transparently handles immediate command buffer begining and ending, and chooses appropriate VkQueue.
 * 
 * @param src The preallocated buffer handle from which the data will be copied
 * @param dest The preallocated buffer handle to which the data will be copied
 * @param size How many consecutive bytes will be copied
 * @param srcOffset Offset in src buffer which will be treated as the zeroth index
 * @param destOffset Offset in the dest buffer which will be treated as the zeroth index
 */
void immCopyBuffer(
    VkBuffer src, VkBuffer dest, 
    VkDeviceSize size, 
    VkDeviceSize srcOffset = 0, VkDeviceSize destOffset = 0);

/**
 * Issues a command to the provide command buffer.
 * Copies all or part of a 2D image. Lower bounds of copy rectangle are at (0, 0).
 * 
 * @param cmdBuff Command buffer to issue command to
 * @param src The preallocated image handle from which the data will be copied
 * @param srcLayout The layout of the source image
 * @param dest The preallocated image handle to which the data will be copied
 * @param destLayout The layout of the destination image
 * @param imgWidth Width of the portion of the image to be copied
 * @param imgHeight Height of the portion of the image to be copied
 */
void cmdCopyImage(
    VkCommandBuffer cmdBuff, 
    VkImage src, VkImageLayout srcLayout, 
    VkImage dest, VkImageLayout destLayout, 
    uint32_t imgWidth, uint32_t imgHeight);

/**
 * Part of the immediate function family (prefix "imm"). Does the same thing as 
 * cmdCopyImage() 
 * except it transparently handles immediate command buffer begining and ending, and chooses appropriate VkQueue.
 * 
 * @param src The preallocated image handle from which the data will be copied
 * @param srcLayout The layout of the source image
 * @param dest The preallocated image handle to which the data will be copied
 * @param destLayout The layout of the destination image
 * @param imgWidth Width of the portion of the image to be copied
 * @param imgHeight Height of the portion of the image to be copied
 */
void immCopyImage(
    VkImage src, VkImageLayout srcLayout, 
    VkImage dest, VkImageLayout destLayout, 
    uint32_t imgWidth, uint32_t imgHeight);

/**
 * Part of the immediate function family (prefix "imm"). 
 * Transitions an image's layout.
 * Transparently handles immediate command buffer begining and ending, and chooses appropriate VkQueue.
 * There is no "cmd-" counterpart.
 * 
 * @param img The image to change the layout of
 * @param imgFormat Affects choice of aspect flags
 * @param oldLayout Current layout
 * @param newLayout Target layout
 */
void immChangeImageLayout(
    VkImage img, VkFormat imgFormat, 
    VkImageLayout oldLayout, VkImageLayout newLayout);

bool findSuitableMemoryTypeIndex(
    uint32_t allowedTypes, 
    VkMemoryPropertyFlags requiredProperties, 
    uint32_t* memTypeIndex);
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
bool imageViewCreate(
    VkImage img, 
    VkFormat imgFormat, 
    VkImageAspectFlags aspectFlags, 
    VkImageView* imgView);

/**
 * Returns true if the physical device supports a particular format / image tiling type / format features combination
 * @param format Requested format
 * @param requiredTilingType Required image tiling type
 * @param requiredFormatFeatures Required
 * @return True if the physical device supports a particular format / image tiling type / format features combination
 */
bool physDeviceSupportsFormat(
    VkFormat format, 
    VkImageTiling requiredTilingType, VkFormatFeatureFlags requiredFormatFeatures);

/**
 * Returns the correct Vulkan enum for using an index of a particular size.
 * 
 * Currently implemented as:
 *  - case 2: return VK_INDEX_TYPE_UINT16;
 *  - case 4: return VK_INDEX_TYPE_UINT32;
 *  - default: return VK_INDEX_TYPE_END_RANGE;
 * 
 * @param size Requested size in bytes
 * @return Vulkan index type enum
 */
VkIndexType indexTypeFromSize(uint8_t size);

/**
 * Returns true if the given format has a stencil component
 * @param format Format to check
 * @return True if the given format has a stencil component
 */
bool formatHasStencilComponent(VkFormat format);

} // Utils
} // Vulkan
} // Video
} // pgg

#endif // PGG_VULKAN

#endif // PGG_VULKANUTILS_HPP
