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

#include "ShoRendererVulkan.hpp"

#include "StreamStuff.hpp"
#include "Video.hpp"
#include "Logger.hpp"

namespace pgg {
        
bool makeShaderModule(const std::vector<uint8_t>& bytecode, VkShaderModule* module) {
    VkShaderModuleCreateInfo cstrArgs; {
        cstrArgs.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        cstrArgs.pNext = nullptr;
        cstrArgs.flags = 0;
        cstrArgs.codeSize = bytecode.size();
        cstrArgs.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());
    }
    
    VkResult result = vkCreateShaderModule(Video::Vulkan::getLogicalDevice(), &cstrArgs, nullptr, module);
    return result == VK_SUCCESS;
}

ShoRendererVk::ShoRendererVk() { }

bool initializeGraphicsPipeline() {
    
}

bool ShoRendererVk::initialize() {

    Logger::Out iout = Logger::log(Logger::INFO);
    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    VkResult result;
    
    VkAttachmentDescription colorAttachDesc; {
        colorAttachDesc.flags = 0;
        colorAttachDesc.format = Video::Vulkan::getSwapchainFormat();
        colorAttachDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //colorAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
    
    VkAttachmentReference colorAttachRef; {
        colorAttachRef.attachment = 0;
        colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    
    VkSubpassDescription subpassDesc; {
        subpassDesc.flags = 0;
        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        
        subpassDesc.inputAttachmentCount = 0;
        subpassDesc.pInputAttachments = nullptr;
        
        subpassDesc.colorAttachmentCount = 1;
        subpassDesc.pColorAttachments = &colorAttachRef;
        subpassDesc.pResolveAttachments = nullptr;
        subpassDesc.pDepthStencilAttachment = nullptr;
        
        subpassDesc.preserveAttachmentCount = 0;
        subpassDesc.pPreserveAttachments = nullptr;
    }
    
    VkSubpassDependency subpassDep; {
        subpassDep.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDep.dstSubpass = 0;
        subpassDep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDep.srcAccessMask = 0;
        subpassDep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    
    VkRenderPassCreateInfo rpCstrArgs; {
        rpCstrArgs.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpCstrArgs.pNext = nullptr;
        rpCstrArgs.flags = 0;
        
        rpCstrArgs.attachmentCount = 1;
        rpCstrArgs.pAttachments = &colorAttachDesc;
        rpCstrArgs.subpassCount = 1;
        rpCstrArgs.pSubpasses = &subpassDesc;
        rpCstrArgs.dependencyCount = 1;
        rpCstrArgs.pDependencies = &subpassDep;
    }
    
    result = vkCreateRenderPass(Video::Vulkan::getLogicalDevice(), &rpCstrArgs, nullptr, &mRenderPass);
    
    if(result != VK_SUCCESS) {
        sout << "Could not create render pass" << std::endl;
        return false;
    }
    
    std::vector<uint8_t> shaderVertRaw, shaderFragRaw;
    
    if(!readFileToByteBuffer("shader.vert.spv", shaderVertRaw)) {
        sout << "Could not read test vertex shader" << std::endl;
        return false;
    }
    
    vout << "Vert size: " << shaderVertRaw.size() << std::endl;
    
    if(!readFileToByteBuffer("shader.frag.spv", shaderFragRaw)) {
        sout << "Could not read test fragment shader" << std::endl;
        return false;
    }
    
    vout << "Frag size: " << shaderFragRaw.size() << std::endl;
    
    if(!makeShaderModule(shaderVertRaw, &mShaderVertModule)) {
        sout << "Could not create vertex shader module" << std::endl;
        return false;
    }
    
    if(!makeShaderModule(shaderFragRaw, &mShaderFragModule)) {
        sout << "Could not create fragment shader module" << std::endl;
        return false;
    }
    
    
    VkPipelineShaderStageCreateInfo pssVertCstrArgs; {
        pssVertCstrArgs.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pssVertCstrArgs.pNext = nullptr;
        pssVertCstrArgs.flags = 0;
        pssVertCstrArgs.stage = VK_SHADER_STAGE_VERTEX_BIT;
        pssVertCstrArgs.module = mShaderVertModule;
        pssVertCstrArgs.pName = "main";
        pssVertCstrArgs.pSpecializationInfo = nullptr;
    }
    
    
    VkPipelineShaderStageCreateInfo pssFragCstrArgs; {
        pssFragCstrArgs.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pssFragCstrArgs.pNext = nullptr;
        pssFragCstrArgs.flags = 0;
        pssFragCstrArgs.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        pssFragCstrArgs.module = mShaderFragModule;
        pssFragCstrArgs.pName = "main";
        pssFragCstrArgs.pSpecializationInfo = nullptr;
    }
    
    
    VkPipelineShaderStageCreateInfo pssCstrArgss[] = {
        pssVertCstrArgs,
        pssFragCstrArgs
    };
    
    
    VkPipelineVertexInputStateCreateInfo pvisCstrArgs; {
        pvisCstrArgs.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pvisCstrArgs.pNext = nullptr;
        pvisCstrArgs.flags = 0;
        pvisCstrArgs.vertexAttributeDescriptionCount = 0;
        pvisCstrArgs.pVertexAttributeDescriptions = nullptr;
        pvisCstrArgs.vertexBindingDescriptionCount = 0;
        pvisCstrArgs.pVertexBindingDescriptions = nullptr;
    }
    
    
    VkPipelineInputAssemblyStateCreateInfo piasCstrArgs; {
        piasCstrArgs.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        piasCstrArgs.pNext = nullptr;
        piasCstrArgs.flags = 0;
        piasCstrArgs.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        piasCstrArgs.primitiveRestartEnable = VK_FALSE;
    }
    
    
    VkViewport viewport; {
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = Video::Vulkan::getSwapchainExtent().width;
        viewport.height = Video::Vulkan::getSwapchainExtent().height;
        viewport.minDepth = 0;
        viewport.maxDepth = 1;
    }
    
    
    VkOffset2D scissorsOff; {
        scissorsOff.x = 0;
        scissorsOff.y = 0;
    }
    
    
    VkRect2D scissors; {
        scissors.offset = scissorsOff;
        scissors.extent = Video::Vulkan::getSwapchainExtent();
    }
    
    
    VkPipelineViewportStateCreateInfo pvsCstrArgs; {
        pvsCstrArgs.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pvsCstrArgs.pNext = nullptr;
        pvsCstrArgs.flags = 0;
        
        pvsCstrArgs.viewportCount = 1;
        pvsCstrArgs.pViewports = &viewport;
        pvsCstrArgs.scissorCount = 1;
        pvsCstrArgs.pScissors = &scissors;
    }
    
    
    VkPipelineRasterizationStateCreateInfo prsCstrArgs; {
        prsCstrArgs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        prsCstrArgs.pNext = nullptr;
        prsCstrArgs.flags = 0;
        
        prsCstrArgs.depthClampEnable = VK_FALSE;
        prsCstrArgs.rasterizerDiscardEnable = VK_FALSE;
        prsCstrArgs.polygonMode = VK_POLYGON_MODE_FILL;
        prsCstrArgs.lineWidth = 1;
        prsCstrArgs.cullMode = VK_CULL_MODE_BACK_BIT;
        prsCstrArgs.frontFace = VK_FRONT_FACE_CLOCKWISE;
        prsCstrArgs.depthBiasEnable = VK_FALSE;
        prsCstrArgs.depthBiasConstantFactor = 0;
        prsCstrArgs.depthBiasClamp = 0;
        prsCstrArgs.depthBiasSlopeFactor = 0;
    }
    
    
    VkPipelineMultisampleStateCreateInfo pmsCstrArgs; {
        pmsCstrArgs.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pmsCstrArgs.pNext = nullptr;
        pmsCstrArgs.flags = 0;
        pmsCstrArgs.sampleShadingEnable = VK_FALSE;
        pmsCstrArgs.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        pmsCstrArgs.minSampleShading = 1;
        pmsCstrArgs.pSampleMask = nullptr;
        pmsCstrArgs.alphaToCoverageEnable = VK_FALSE;
        pmsCstrArgs.alphaToOneEnable = VK_FALSE;
    }
    
    
    VkPipelineColorBlendAttachmentState pcbas; {
        pcbas.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        pcbas.blendEnable = VK_FALSE;
        
        pcbas.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        pcbas.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        pcbas.colorBlendOp = VK_BLEND_OP_ADD;
        pcbas.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        pcbas.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        pcbas.alphaBlendOp = VK_BLEND_OP_ADD;
    }
    
    
    VkPipelineColorBlendStateCreateInfo pcbsCstrArgs; {
        pcbsCstrArgs.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pcbsCstrArgs.pNext = nullptr;
        pcbsCstrArgs.flags = 0;
        
        pcbsCstrArgs.logicOpEnable = VK_FALSE;
        pcbsCstrArgs.logicOp = VK_LOGIC_OP_COPY;
        pcbsCstrArgs.attachmentCount = 1;
        pcbsCstrArgs.pAttachments = &pcbas;
        
        pcbsCstrArgs.blendConstants[0] = 0;
        pcbsCstrArgs.blendConstants[1] = 0;
        pcbsCstrArgs.blendConstants[2] = 0;
        pcbsCstrArgs.blendConstants[3] = 0;
    }
    
    
    VkPipelineLayoutCreateInfo plCstrArgs; {
        plCstrArgs.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        plCstrArgs.pNext = nullptr;
        plCstrArgs.flags = 0;
        
        plCstrArgs.setLayoutCount = 0;
        plCstrArgs.pSetLayouts = nullptr;
        plCstrArgs.pushConstantRangeCount = 0;
        plCstrArgs.pPushConstantRanges = nullptr;
    }
    
    
    result = vkCreatePipelineLayout(Video::Vulkan::getLogicalDevice(), &plCstrArgs, nullptr, &mPipelineLayout);
    
    
    if(result != VK_SUCCESS) {
        sout << "Could not create pipeline layout" << std::endl;
        return false;
    }
    
    
    VkGraphicsPipelineCreateInfo gpCstrArgs; {
        gpCstrArgs.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        gpCstrArgs.pNext = nullptr;
        gpCstrArgs.flags = 0;
        
        gpCstrArgs.stageCount = 2;
        gpCstrArgs.pStages = pssCstrArgss;
        gpCstrArgs.pVertexInputState = &pvisCstrArgs;
        gpCstrArgs.pInputAssemblyState = &piasCstrArgs;
        gpCstrArgs.pViewportState = &pvsCstrArgs;
        gpCstrArgs.pRasterizationState = &prsCstrArgs;
        gpCstrArgs.pMultisampleState = &pmsCstrArgs;
        gpCstrArgs.pDepthStencilState = nullptr;
        gpCstrArgs.pColorBlendState = &pcbsCstrArgs;
        gpCstrArgs.pDynamicState = nullptr;
        gpCstrArgs.layout = mPipelineLayout;
        gpCstrArgs.renderPass = mRenderPass;
        gpCstrArgs.subpass = 0;
        gpCstrArgs.basePipelineHandle = VK_NULL_HANDLE;
        gpCstrArgs.basePipelineIndex = -1;
    }
    
    result = vkCreateGraphicsPipelines(Video::Vulkan::getLogicalDevice(), VK_NULL_HANDLE, 1, &gpCstrArgs, nullptr, &mPipeline);
    
    if(result != VK_SUCCESS) {
        sout << "Could not create graphics pipeline" << std::endl;
        return false;
    }
    
    iout << "Graphics pipeline created" << std::endl;
    
    
    mSwapchainFramebuffers.resize(Video::Vulkan::getSwapchainImageViews().size(), VK_NULL_HANDLE);
    
    for(uint32_t index = 0; index < Video::Vulkan::getSwapchainImageViews().size(); ++ index) {
        VkImageView imgViewAttachments[] = {
            Video::Vulkan::getSwapchainImageViews().at(index)
        };
        
        VkFramebufferCreateInfo fbCargs; {
            fbCargs.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbCargs.pNext = nullptr;
            fbCargs.flags = 0;
            
            fbCargs.renderPass = mRenderPass;
            
            fbCargs.attachmentCount = 1;
            fbCargs.pAttachments = imgViewAttachments;
            fbCargs.width = Video::Vulkan::getSwapchainExtent().width;
            fbCargs.height = Video::Vulkan::getSwapchainExtent().height;
            fbCargs.layers = 1;
        }
        
        result = vkCreateFramebuffer(Video::Vulkan::getLogicalDevice(), &fbCargs, nullptr, &(mSwapchainFramebuffers[index]));
        
        if(result != VK_SUCCESS) {
            sout << "Could not create framebuffer #" << index << std::endl;
            return false;
        }
    }
    
    VkCommandPoolCreateInfo cpCargs; {
        cpCargs.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cpCargs.pNext = nullptr;
        cpCargs.flags = 0;
        
        cpCargs.queueFamilyIndex = Video::Vulkan::getGraphicsQueueFamilyIndex();
    }
    
    result = vkCreateCommandPool(Video::Vulkan::getLogicalDevice(), &cpCargs, nullptr, &mCommandPool);
    if(result != VK_SUCCESS) {
        sout << "Could not create command pool" << std::endl;
        return false;
    }
    
    mCommandBuffers.resize(mSwapchainFramebuffers.size());
    
    VkCommandBufferAllocateInfo cbaArgs; {
        cbaArgs.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cbaArgs.pNext = nullptr;
        cbaArgs.commandPool = mCommandPool;
        cbaArgs.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cbaArgs.commandBufferCount = mCommandBuffers.size();
    }
    
    result = vkAllocateCommandBuffers(Video::Vulkan::getLogicalDevice(), &cbaArgs, mCommandBuffers.data());
    
    if(result != VK_SUCCESS) {
        sout << "Could not allocate command buffers" << std::endl;
        return false;
    }
    
    for(uint32_t i = 0; i < mCommandBuffers.size(); ++ i) {
        VkCommandBuffer cmdBuff = mCommandBuffers.at(i);
        VkCommandBufferBeginInfo cbbArgs; {
            cbbArgs.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cbbArgs.pNext = nullptr;
            cbbArgs.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            cbbArgs.pInheritanceInfo = nullptr;
        }
        
        result = vkBeginCommandBuffer(cmdBuff, &cbbArgs);
        
        VkClearValue color = {0.f, 1.f, 1.f, 1.f};
        
        VkRenderPassBeginInfo rpbArgs; {
            rpbArgs.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rpbArgs.pNext = nullptr;
            rpbArgs.renderPass = mRenderPass;
            rpbArgs.framebuffer = mSwapchainFramebuffers.at(i);
            rpbArgs.renderArea.offset = {0, 0};
            rpbArgs.renderArea.extent = Video::Vulkan::getSwapchainExtent();
            rpbArgs.clearValueCount = 1;
            rpbArgs.pClearValues = &color;
        }
        
        vkCmdBeginRenderPass(cmdBuff, &rpbArgs, VK_SUBPASS_CONTENTS_INLINE);
        
        vkCmdBindPipeline(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
        
        vkCmdDraw(cmdBuff, 3, 1, 0, 0);
        
        vkCmdEndRenderPass(cmdBuff);
        
        result = vkEndCommandBuffer(cmdBuff);
        if(result != VK_SUCCESS) {
            sout << "Could not record command buffer" << std::endl;
            return false;
        }
        
    }
    
    VkSemaphoreCreateInfo whyVulkan; {
        whyVulkan.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        whyVulkan.pNext = nullptr;
        whyVulkan.flags = 0;
    }
    
    result = vkCreateSemaphore(Video::Vulkan::getLogicalDevice(), &whyVulkan, nullptr, &mSemImageAvailable);
    if(result != VK_SUCCESS) {
        sout << "Could not create image availability semaphore" << std::endl;
        return false;
    }
    
    result = vkCreateSemaphore(Video::Vulkan::getLogicalDevice(), &whyVulkan, nullptr, &mSemRenderFinished);
    if(result != VK_SUCCESS) {
        sout << "Could not create render finishing semaphore" << std::endl;
        return false;
    }
    
    return true;
}

ShoRendererVk::~ShoRendererVk() {
}

bool ShoRendererVk::cleanup() {

    vkDeviceWaitIdle(Video::Vulkan::getLogicalDevice());
    
    vkDestroySemaphore(Video::Vulkan::getLogicalDevice(), mSemImageAvailable, nullptr);
    vkDestroySemaphore(Video::Vulkan::getLogicalDevice(), mSemRenderFinished, nullptr);
    
    vkDestroyCommandPool(Video::Vulkan::getLogicalDevice(), mCommandPool, nullptr);
    
    for(VkFramebuffer fb : mSwapchainFramebuffers) {
        vkDestroyFramebuffer(Video::Vulkan::getLogicalDevice(), fb, nullptr);
    }
    
    vkDestroyPipeline(Video::Vulkan::getLogicalDevice(), mPipeline, nullptr);
    
    vkDestroyRenderPass(Video::Vulkan::getLogicalDevice(), mRenderPass, nullptr);
    
    vkDestroyPipelineLayout(Video::Vulkan::getLogicalDevice(), mPipelineLayout, nullptr);
    
    vkDestroyShaderModule(Video::Vulkan::getLogicalDevice(), mShaderVertModule, nullptr);
    vkDestroyShaderModule(Video::Vulkan::getLogicalDevice(), mShaderFragModule, nullptr);
    
    return true;
    
}

void ShoRendererVk::resize(uint32_t width, uint32_t height) {
    
}
void ShoRendererVk::renderFrame() {
    
    VkResult result;
    
    uint32_t imgIndex;
    vkAcquireNextImageKHR(Video::Vulkan::getLogicalDevice(), Video::Vulkan::getSwapchain(), std::numeric_limits<uint64_t>::max(), mSemImageAvailable, VK_NULL_HANDLE, &imgIndex);
    
    VkSemaphore waitSems[] = {
        mSemImageAvailable
    };
    
    VkPipelineStageFlags waitFlags[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    
    VkSemaphore signalSems[] = {
        mSemRenderFinished
    };
    
    VkSubmitInfo submitArgs; {
        submitArgs.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitArgs.pNext = nullptr;
        submitArgs.waitSemaphoreCount = 1;
        submitArgs.pWaitSemaphores = waitSems;
        submitArgs.pWaitDstStageMask = waitFlags;
        submitArgs.commandBufferCount = 1;
        submitArgs.pCommandBuffers = &(mCommandBuffers.at(imgIndex));
        submitArgs.signalSemaphoreCount = 1;
        submitArgs.pSignalSemaphores = signalSems;
    }
    
    result = vkQueueSubmit(Video::Vulkan::getGraphicsQueue(), 1, &submitArgs, VK_NULL_HANDLE);
    
    if(result != VK_SUCCESS) {
        Logger::log(Logger::SEVERE) << "AAAA" << std::endl;
        return;
    }
    
    VkSwapchainKHR swapchains[] = {
        Video::Vulkan::getSwapchain()
    };
    
    VkPresentInfoKHR presentArgs; {
        presentArgs.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentArgs.pNext = nullptr;
        presentArgs.waitSemaphoreCount = 1;
        presentArgs.pWaitSemaphores = signalSems;
        presentArgs.swapchainCount = 1;
        presentArgs.pSwapchains = swapchains;
        presentArgs.pImageIndices = &imgIndex;
        presentArgs.pResults = nullptr;
    }
    
    vkQueuePresentKHR(Video::Vulkan::getDisplayQueue(), &presentArgs);
}
}

#endif // PGG_VULKAN

