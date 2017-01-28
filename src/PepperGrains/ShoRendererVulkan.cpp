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

#include "Image.hpp"
#include "ImageResource.hpp"
#include "Resources.hpp"
#include "Video.hpp"
#include "Logger.hpp"
#include "VulkanUtils.hpp"
#include "GeometryResource.hpp"
#include "Engine.hpp"

namespace pgg {

ShoRendererVk::ShoRendererVk() { }
ShoRendererVk::~ShoRendererVk() { }

void ShoRendererVk::setScenegraph(Scenegraph* scenegraph) {
    if(mScenegraph == scenegraph) return;
    if(mScenegraph != nullptr) {
        mScenegraph->setRendererAddListener(nullptr);
        mScenegraph->setRendererRemoveListener(nullptr);
    }
    mScenegraph = scenegraph;
    if(mScenegraph != nullptr) {
        mScenegraph->setRendererAddListener(std::bind(&ShoRendererVk::onModeliAdded, this, std::placeholders::_1));
        mScenegraph->setRendererRemoveListener(std::bind(&ShoRendererVk::onModeliRemoved, this, std::placeholders::_1));
    }
}

bool ShoRendererVk::initialize() {
    return initializeRenderpass() && initializeFramebuffers() && initializeSemaphores() && setupTestGeometry() && initializePipeline() && populateCommandBuffers();
}
bool ShoRendererVk::initializeRenderpass() {

    Logger::Out iout = Logger::log(Logger::INFO);
    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    VkResult result;
    
    VkAttachmentDescription colorAttachDesc; {
        colorAttachDesc.flags = 0;
        colorAttachDesc.format = Video::Vulkan::getSwapchainFormat();
        colorAttachDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        //colorAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
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
        subpassDep.dependencyFlags = 0;
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
    
    return true;
}

bool ShoRendererVk::initializeFramebuffers() {

    Logger::Out iout = Logger::log(Logger::INFO);
    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    VkResult result;
    
    mFramebufferSquads.resize(Video::Vulkan::getSwapchainImageViews().size());
    
    std::vector<VkCommandBuffer> commandBuffers(mFramebufferSquads.size(), VK_NULL_HANDLE);
    VkCommandBufferAllocateInfo cbaArgs; {
        cbaArgs.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cbaArgs.pNext = nullptr;
        cbaArgs.commandPool = Video::Vulkan::getGraphicsCommandPool();
        cbaArgs.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cbaArgs.commandBufferCount = commandBuffers.size();
    }
    result = vkAllocateCommandBuffers(Video::Vulkan::getLogicalDevice(), &cbaArgs, commandBuffers.data());
    if(result != VK_SUCCESS) {
        sout << "Could not allocate command buffers" << std::endl;
        return false;
    }
    
    for(uint32_t index = 0; index < Video::Vulkan::getSwapchainImageViews().size(); ++ index) {
        // Must be reference because data is being modified
        FramebufferSquad& stuff = mFramebufferSquads[index];
        
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
        
        result = vkCreateFramebuffer(Video::Vulkan::getLogicalDevice(), &fbCargs, nullptr, &(stuff.mFramebuffer));
        
        if(result != VK_SUCCESS) {
            sout << "Could not create framebuffer #" << index << std::endl;
            return false;
        }
        
        stuff.mGraphicsCmdBuffer = commandBuffers[index];
    }
    
    VkSemaphoreCreateInfo semaphoreCargs; {
        semaphoreCargs.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCargs.pNext = nullptr;
        semaphoreCargs.flags = 0;
    }
    VkFenceCreateInfo fenceCargs; {
        fenceCargs.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCargs.pNext = nullptr;
        fenceCargs.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }
    
    // Must iterate by reference; modifying values
    for(FramebufferSquad& squad : mFramebufferSquads) {
        result = vkCreateSemaphore(Video::Vulkan::getLogicalDevice(), &semaphoreCargs, nullptr, &(squad.mSemRenderFinished));
        if(result != VK_SUCCESS) {
            sout << "Could not create render completion semaphore" << std::endl;
            return false;
        }
        result = vkCreateFence(Video::Vulkan::getLogicalDevice(), &fenceCargs, nullptr, &(squad.mFenceRenderFinished));
        if(result != VK_SUCCESS) {
            sout << "Could not create render completion fence" << std::endl;
            return false;
        }
    }
    
    return true;
}

bool ShoRendererVk::initializeSemaphores() {

    Logger::Out iout = Logger::log(Logger::INFO);
    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    VkResult result;
    
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
    
    return true;
}

bool ShoRendererVk::setupTestGeometry() {

    Logger::Out iout = Logger::log(Logger::INFO);
    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    Image* img = ImageResource::gallop(Resources::find("GreenJellyfish.image"));
    img->grab();
    img->drop();
    
    mTestGeom = GeometryResource::gallop(Resources::find("Monkey.geometry"));
    mTestGeom->grab();
    
    
    VkResult result;
    
    {
        glm::mat4 geomMVP[1];
        
        VulkanUtils::bufferCreateAndAllocate(sizeof(geomMVP), 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            &mUniformBuffer, &mUniformBufferMemory);
            
        void* memAddr;
        vkMapMemory(Video::Vulkan::getLogicalDevice(), mUniformBufferMemory, 0, sizeof(geomMVP), 0, &memAddr);
        memcpy(memAddr, geomMVP, sizeof(geomMVP));
        vkUnmapMemory(Video::Vulkan::getLogicalDevice(), mUniformBufferMemory);
        
        VkDescriptorSetLayoutBinding uniformBufferLayoutBinding; {
            uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            
            uniformBufferLayoutBinding.binding = 0;
            uniformBufferLayoutBinding.descriptorCount = 1;
            uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uniformBufferLayoutBinding.pImmutableSamplers = nullptr;
        }
        
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCargs; {
            descriptorSetLayoutCargs.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutCargs.pNext = nullptr;
            descriptorSetLayoutCargs.flags = 0;
            
            descriptorSetLayoutCargs.bindingCount = 1;
            descriptorSetLayoutCargs.pBindings = &uniformBufferLayoutBinding;
        }
        
        result = vkCreateDescriptorSetLayout(Video::Vulkan::getLogicalDevice(), &descriptorSetLayoutCargs, nullptr, &mDescriptorSetLayout);
        
        if(result != VK_SUCCESS) {
            sout << "Could not create descriptor set layout" << std::endl;
            return false;
        }
        
        VkDescriptorPoolSize descPoolSize; {
            descPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descPoolSize.descriptorCount = 1;
        }
        
        VkDescriptorPoolCreateInfo descPoolCargs; {
            descPoolCargs.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descPoolCargs.pNext = nullptr;
            descPoolCargs.flags = 0;
            descPoolCargs.poolSizeCount = 1;
            descPoolCargs.pPoolSizes = &descPoolSize;
            descPoolCargs.maxSets = 1;
        }
        
        result = vkCreateDescriptorPool(Video::Vulkan::getLogicalDevice(), &descPoolCargs, nullptr, &mDescriptorPool);
        
        
        if(result != VK_SUCCESS) {
            sout << "Could not create descriptor pool" << std::endl;
            return false;
        }
        
        VkDescriptorSetAllocateInfo descSetAargs; {
            descSetAargs.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descSetAargs.pNext = nullptr;
            descSetAargs.descriptorPool = mDescriptorPool;
            descSetAargs.descriptorSetCount = 1;
            descSetAargs.pSetLayouts = &mDescriptorSetLayout;
        }
        
        result = vkAllocateDescriptorSets(Video::Vulkan::getLogicalDevice(), &descSetAargs, &mDescriptorSet);
        
        
        if(result != VK_SUCCESS) {
            sout << "Could not allocate descriptor sets" << std::endl;
            return false;
        }
        
        VkDescriptorBufferInfo descBufferData; {
            descBufferData.buffer = mUniformBuffer;
            descBufferData.offset = 0;
            descBufferData.range = sizeof(geomMVP);
        }
        
        VkWriteDescriptorSet writeDescSet; {
            writeDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescSet.pNext = nullptr;
            writeDescSet.dstSet = mDescriptorSet;
            writeDescSet.dstBinding = 0;
            writeDescSet.dstArrayElement = 0;
            writeDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescSet.descriptorCount = 1;
            writeDescSet.pBufferInfo = &descBufferData;
            writeDescSet.pImageInfo = nullptr;
            writeDescSet.pTexelBufferView = nullptr;
        }
        
        vkUpdateDescriptorSets(Video::Vulkan::getLogicalDevice(), 1, &writeDescSet, 0, nullptr);
    }
    
    return true;
}

bool ShoRendererVk::initializePipeline() {

    Logger::Out iout = Logger::log(Logger::INFO);
    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    VkResult result;
    
    ShaderResource* shaderVertex = ShaderResource::gallop(Resources::find("TestShader2.vertexShader"));
    shaderVertex->grab();
    ShaderResource* shaderFragment = ShaderResource::gallop(Resources::find("TestShader2.fragmentShader"));
    shaderFragment->grab();
    
    VkPipelineShaderStageCreateInfo pssCstrArgss[] = {
        shaderVertex->getPipelineShaderStageInfo(),
        shaderFragment->getPipelineShaderStageInfo()
    };
    
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
        
        //plCstrArgs.setLayoutCount = 0;
        //plCstrArgs.pSetLayouts = nullptr;
        plCstrArgs.setLayoutCount = 1;
        plCstrArgs.pSetLayouts = &mDescriptorSetLayout;
        plCstrArgs.pushConstantRangeCount = 0;
        plCstrArgs.pPushConstantRanges = nullptr;
    }
    
    result = vkCreatePipelineLayout(Video::Vulkan::getLogicalDevice(), &plCstrArgs, nullptr, &mPipelineLayout);
    
    if(result != VK_SUCCESS) {
        sout << "Could not create pipeline layout" << std::endl;
    
        shaderVertex->drop();
        shaderFragment->drop();
        return false;
    }
    
    VkGraphicsPipelineCreateInfo gpCstrArgs; {
        gpCstrArgs.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        gpCstrArgs.pNext = nullptr;
        gpCstrArgs.flags = 0;
        
        gpCstrArgs.renderPass = mRenderPass;
        
        // Shaders
        gpCstrArgs.stageCount = 2;
        gpCstrArgs.pStages = pssCstrArgss;
        
        gpCstrArgs.layout = mPipelineLayout;
        
        gpCstrArgs.pVertexInputState = mTestGeom->getVertexInputState();
        gpCstrArgs.pInputAssemblyState = mTestGeom->getInputAssemblyState();
        gpCstrArgs.pViewportState = &pvsCstrArgs;
        gpCstrArgs.pRasterizationState = &prsCstrArgs;
        gpCstrArgs.pMultisampleState = &pmsCstrArgs;
        gpCstrArgs.pDepthStencilState = nullptr;
        gpCstrArgs.pColorBlendState = &pcbsCstrArgs;
        gpCstrArgs.pDynamicState = nullptr;
        gpCstrArgs.pTessellationState = nullptr;
        
        gpCstrArgs.subpass = 0;
        
        gpCstrArgs.basePipelineHandle = VK_NULL_HANDLE;
        gpCstrArgs.basePipelineIndex = -1;
    }
    
    result = vkCreateGraphicsPipelines(Video::Vulkan::getLogicalDevice(), VK_NULL_HANDLE, 1, &gpCstrArgs, nullptr, &mPipeline);
    
    shaderVertex->drop();
    shaderVertex = nullptr;
    shaderFragment->drop();
    shaderFragment = nullptr;
    
    if(result != VK_SUCCESS) {
        sout << "Could not create graphics pipeline" << std::endl;
        return false;
    }
    
    return true;
}

bool ShoRendererVk::populateCommandBuffers() {

    Logger::Out iout = Logger::log(Logger::INFO);
    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    VkResult result;
    
    for(FramebufferSquad framebufferSquad : mFramebufferSquads) {
        VkCommandBuffer cmdBuff = framebufferSquad.mGraphicsCmdBuffer;
        VkFramebuffer framebuff = framebufferSquad.mFramebuffer;
        
        VkCommandBufferBeginInfo cbbArgs; {
            cbbArgs.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cbbArgs.pNext = nullptr;
            cbbArgs.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            cbbArgs.pInheritanceInfo = nullptr;
        }
        
        result = vkBeginCommandBuffer(cmdBuff, &cbbArgs);
        
        VkClearValue clearVal; {
            clearVal.color = {0, 1, 1, 1};
        }
        
        VkRenderPassBeginInfo rpbArgs; {
            rpbArgs.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rpbArgs.pNext = nullptr;
            rpbArgs.renderPass = mRenderPass;
            rpbArgs.framebuffer = framebuff;
            rpbArgs.renderArea.offset = {0, 0};
            rpbArgs.renderArea.extent = Video::Vulkan::getSwapchainExtent();
            rpbArgs.clearValueCount = 1;
            rpbArgs.pClearValues = &clearVal;
        }
        
        vkCmdBeginRenderPass(cmdBuff, &rpbArgs, VK_SUBPASS_CONTENTS_INLINE);
        
        vkCmdBindPipeline(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
        
        vkCmdBindDescriptorSets(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSet, 0, nullptr);
        
        mTestGeom->cmdBindBuffers(cmdBuff);
        mTestGeom->cmdDrawIndexed(cmdBuff);
        
        vkCmdEndRenderPass(cmdBuff);
        
        result = vkEndCommandBuffer(cmdBuff);
        if(result != VK_SUCCESS) {
            sout << "Could not record command buffer" << std::endl;
            return false;
        }
        
    }
    return true;
}

bool ShoRendererVk::cleanup() {
    
    mTestGeom->drop();
    
    vkFreeMemory(Video::Vulkan::getLogicalDevice(), mUniformBufferMemory, nullptr);
    vkDestroyBuffer(Video::Vulkan::getLogicalDevice(), mUniformBuffer, nullptr);
    
    // Descriptor sets are automatically cleaned up with the descriptor pool
    vkDestroyDescriptorPool(Video::Vulkan::getLogicalDevice(), mDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(Video::Vulkan::getLogicalDevice(), mDescriptorSetLayout, nullptr);
    
    vkDestroyPipeline(Video::Vulkan::getLogicalDevice(), mPipeline, nullptr);
    vkDestroyPipelineLayout(Video::Vulkan::getLogicalDevice(), mPipelineLayout, nullptr);
    
    vkDestroySemaphore(Video::Vulkan::getLogicalDevice(), mSemImageAvailable, nullptr);
    
    for(FramebufferSquad& framebufferSquad : mFramebufferSquads) {
        vkDestroyFramebuffer(Video::Vulkan::getLogicalDevice(), framebufferSquad.mFramebuffer, nullptr);
        vkDestroySemaphore(Video::Vulkan::getLogicalDevice(), framebufferSquad.mSemRenderFinished, nullptr);
        vkDestroyFence(Video::Vulkan::getLogicalDevice(), framebufferSquad.mFenceRenderFinished, nullptr);
        vkFreeCommandBuffers(Video::Vulkan::getLogicalDevice(), Video::Vulkan::getGraphicsCommandPool(), 1, &(framebufferSquad.mGraphicsCmdBuffer));
    }
    mFramebufferSquads.clear();
    
    vkDestroyRenderPass(Video::Vulkan::getLogicalDevice(), mRenderPass, nullptr);
    
    return true;
}

void ShoRendererVk::renderFrame() {
    glm::mat4 geomMVP[] = {
        mCamera.getProjMatrix() * mCamera.getViewMatrix() * glm::mat4()
    };
    
    //Engine::quit();

    // Warning: this is unsafe behavior, since the uniform buffer could still be in use by a running command buffer
    void* memAddr;
    vkMapMemory(Video::Vulkan::getLogicalDevice(), mUniformBufferMemory, 0, sizeof(geomMVP), 0, &memAddr);
    memcpy(memAddr, geomMVP, sizeof(geomMVP));
    vkUnmapMemory(Video::Vulkan::getLogicalDevice(), mUniformBufferMemory);
    
    //mScenegraph->processAll(std::bind(&ShoRendererVk::modelimapOpaque, this, std::placeholders::_1));
    
    VkResult result;
    
    VkSwapchainKHR swapchain = Video::Vulkan::getSwapchain();
    
    uint32_t imgIndex;
    vkAcquireNextImageKHR(Video::Vulkan::getLogicalDevice(), swapchain, std::numeric_limits<uint64_t>::max(), mSemImageAvailable, VK_NULL_HANDLE, &imgIndex);
    FramebufferSquad& squad = mFramebufferSquads.at(imgIndex);
    
    VkPipelineStageFlags waitFlags[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    
    VkSubmitInfo submitArgs; {
        submitArgs.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitArgs.pNext = nullptr;
        
        submitArgs.waitSemaphoreCount = 1;
        submitArgs.pWaitSemaphores = &mSemImageAvailable;
        submitArgs.pWaitDstStageMask = waitFlags;
        
        submitArgs.commandBufferCount = 1;
        submitArgs.pCommandBuffers = &(squad.mGraphicsCmdBuffer);
        
        submitArgs.signalSemaphoreCount = 1;
        submitArgs.pSignalSemaphores = &(squad.mSemRenderFinished);
    }
    
    vkWaitForFences(Video::Vulkan::getLogicalDevice(), 1, &(squad.mFenceRenderFinished), VK_TRUE, std::numeric_limits<uint64_t>::max());
    vkResetFences(Video::Vulkan::getLogicalDevice(), 1, &(squad.mFenceRenderFinished));
    result = vkQueueSubmit(Video::Vulkan::getGraphicsQueue(), 1, &submitArgs, squad.mFenceRenderFinished);
    
    if(result != VK_SUCCESS) {
        Logger::log(Logger::SEVERE) << "AAAA" << std::endl;
        return;
    }
    
    VkPresentInfoKHR presentArgs; {
        presentArgs.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentArgs.pNext = nullptr;
        
        presentArgs.waitSemaphoreCount = 1;
        presentArgs.pWaitSemaphores = &(squad.mSemRenderFinished);
        
        presentArgs.swapchainCount = 1;
        presentArgs.pSwapchains = &swapchain;
        presentArgs.pImageIndices = &imgIndex;
        presentArgs.pResults = nullptr;
    }
    
    vkQueuePresentKHR(Video::Vulkan::getDisplayQueue(), &presentArgs);
}

void ShoRendererVk::modelimapDepthPass(ModelInstance* modeli) {
}
void ShoRendererVk::modelimapLightprobe(ModelInstance* modeli) {
}

void ShoRendererVk::modelimapOpaque(ModelInstance* modeli) {
    Model* model = modeli->getModel();
    Material* material = model->getMaterial();
    Geometry* geometry = model->getGeometry();
}

void ShoRendererVk::modelimapTransparent(ModelInstance* modeli) {
}
void ShoRendererVk::onModeliAdded(ModelInstance* modeli) {
    Logger::log(Logger::VERBOSE) << "Model added" << std::endl;
    
}
void ShoRendererVk::onModeliRemoved(ModelInstance* modeli) {
    Logger::log(Logger::VERBOSE) << "Model removed" << std::endl;
    
}
void ShoRendererVk::rebuildPipeline() {
    cleanup();
    initialize();
}

}

#endif // PGG_VULKAN

