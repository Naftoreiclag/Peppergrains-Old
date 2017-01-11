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

#include "ShaderProgramResource.hpp"
#include "Scenegraph.hpp"
#include "Camera.hpp"

namespace pgg {

class ShoRendererVk {
private:
    VkShaderModule mShaderVertModule = VK_NULL_HANDLE;
    VkShaderModule mShaderFragModule = VK_NULL_HANDLE;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
    VkPipeline mPipeline = VK_NULL_HANDLE;
    
    VkSemaphore mSemImageAvailable = VK_NULL_HANDLE;
    VkSemaphore mSemRenderFinished = VK_NULL_HANDLE;
    
    std::vector<VkFramebuffer> mSwapchainFramebuffers;
    
    VkCommandPool mCommandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> mCommandBuffers;
public:
    ShoRendererVk();
    ~ShoRendererVk();
    
    bool initialize();
    bool cleanup();
    
    Camera mCamera;
    Scenegraph* mScenegraph;

    void resize(uint32_t width, uint32_t height);
    void renderFrame();
    
    void modelimapDepthPass(ModelInstance* modeli);
    void modelimapLightprobe(ModelInstance* modeli);
    void modelimapOpaque(ModelInstance* modeli);
    void modelimapTransparent(ModelInstance* modeli);
    
    void rebuildPipeline();
};
typedef ShoRendererVk ShoRenderer;

}

#endif // PGG_VULKAN

#endif // PGG_SHORENDERERVULKAN_HPP
