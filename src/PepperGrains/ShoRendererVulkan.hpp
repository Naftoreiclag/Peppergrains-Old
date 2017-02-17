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

#ifndef PGG_SHORENDERERVULKAN_HPP
#define PGG_SHORENDERERVULKAN_HPP

#ifdef PGG_VULKAN

#include <stdint.h>
#include <vector>

#include <GraphicsApiLibrary.hpp>

#include "ShaderResource.hpp"
#include "Scenegraph.hpp"
#include "Camera.hpp"
#include "Geometry.hpp"
#include "Texture.hpp"

namespace pgg {

class ShoRendererVk {
private:
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
    VkPipeline mPipeline = VK_NULL_HANDLE;
    
    VkImage mDepthImage = VK_NULL_HANDLE;
    VkDeviceMemory mDepthImageMemory = VK_NULL_HANDLE;
    VkFormat mDepthFormat = VK_FORMAT_END_RANGE;
    VkImageView mDepthImageView = VK_NULL_HANDLE;
    
    VkSemaphore mSemImageAvailable = VK_NULL_HANDLE;
    
    /// Groups together per-swapchain-image objects: a framebuffer, command buffer, and mutexes
    struct FramebufferSquad {
        /// Associated framebuffer
        VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
        
        /// Holds graphics-related commands; i.e. drawing operations
        VkCommandBuffer mGraphicsCmdBuffer = VK_NULL_HANDLE;
        
        /**
         * Triggered when the command buffer completes (a swapchain image has been drawn to)
         * Used to synchronize the completion of a swapchain image with presentation on the screen
         */
        VkSemaphore mSemRenderFinished = VK_NULL_HANDLE;
        
        /**
         * Triggered when the command buffer completes (a swapchain image has been drawn to)
         * Used to synchronize (wait until the command buffer completes) before modifying shared memory
         */
        VkFence mFenceRenderFinished = VK_NULL_HANDLE;
    };
    
    /**
     * Each swapchain image has its own framebuffer, etc.
     * The indices into this vector correspond to the indices returned by Vulkan when querying the next available
     * swapchain image.
     */
    std::vector<FramebufferSquad> mFramebufferSquads;
    
    /// Holds copies of all mFenceRenderFinished members in mFramebufferSquads
    std::vector<VkFence> mAllFenceRenderFinished;
    
    VkBuffer mUniformBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mUniformBufferMemory = VK_NULL_HANDLE;
    
    Geometry* mTestGeom = nullptr;
    Texture* mTestTexture = nullptr;
    
    VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet mDescriptorSet;
    
    bool initializeDepthBuffer();
    bool initializeRenderpass();
    bool initializeFramebuffers();
    bool initializeSemaphores();
    bool setupTestGeometry();
    bool initializePipeline();
    bool populateCommandBuffers();
    
    Scenegraph* mScenegraph = nullptr;
public:
    ShoRendererVk();
    ~ShoRendererVk();
    
    bool initialize();
    bool cleanup();
    
    Camera mCamera;
    void setScenegraph(Scenegraph* scenegraph);

    /**
     * @brief Completely renders a frame and displays it onto the screen.
     * 
     * Does the following internally:
     *  - Acquire the next swapchain image
     *  - Synchronizes with GPU so uniform buffers and such can be safely written to
     */
    void renderFrame();
    
    void modelimapUniformBufferUpdate(ModelInstance* modeli);
    void modelimapDepthPass(ModelInstance* modeli);
    void modelimapLightprobe(ModelInstance* modeli);
    void modelimapOpaque(ModelInstance* modeli);
    void modelimapTransparent(ModelInstance* modeli);
    
    void onModeliAdded(ModelInstance* modeli);
    void onModeliRemoved(ModelInstance* modeli);
    
    void rebuildPipeline();
};
typedef ShoRendererVk ShoRenderer;

}

#endif // PGG_VULKAN

#endif // PGG_SHORENDERERVULKAN_HPP
