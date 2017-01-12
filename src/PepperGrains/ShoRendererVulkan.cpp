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
ShoRendererVk::~ShoRendererVk() { }

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
        colorAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
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
    
    mCommandBuffers.resize(mSwapchainFramebuffers.size(), VK_NULL_HANDLE);
    
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
    
    result = vkCreateSemaphore(Video::Vulkan::getLogicalDevice(), &whyVulkan, nullptr, &mSemRenderFinished);
    if(result != VK_SUCCESS) {
        sout << "Could not create render finishing semaphore" << std::endl;
        return false;
    }
    
    return true;
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

bool ShoRendererVk::setupTestGeometry() {

    Logger::Out iout = Logger::log(Logger::INFO);
    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    VkResult result;
    
    {
        glm::f32 geomVerticies[] = {
            -0.5, -0.5, 1.0, 0.0, 0.0,
            0.5, -0.5, 0.0, 1.0, 0.0,
            -0.5, 0.5, 0.0, 0.0, 1.0,
            0.5, 0.5, 1.0, 1.0, 0.0
        };
        
        makeBufferAndAllocateMemory(sizeof(geomVerticies), 
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            &mVertexBuffer, &mVertexBufferMemory);
        
        void* memAddr;
        vkMapMemory(Video::Vulkan::getLogicalDevice(), mVertexBufferMemory, 0, sizeof(geomVerticies), 0, &memAddr);
        memcpy(memAddr, geomVerticies, sizeof(geomVerticies));
        vkUnmapMemory(Video::Vulkan::getLogicalDevice(), mVertexBufferMemory);
    }
    
    {
        glm::u16 geomIndices[] = {
            0, 1, 3, 0, 3, 2
        };
        
        makeBufferAndAllocateMemory(sizeof(geomIndices), 
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            &mIndexBuffer, &mIndexBufferMemory);
            
        void* memAddr;
        vkMapMemory(Video::Vulkan::getLogicalDevice(), mIndexBufferMemory, 0, sizeof(geomIndices), 0, &memAddr);
        memcpy(memAddr, geomIndices, sizeof(geomIndices));
        vkUnmapMemory(Video::Vulkan::getLogicalDevice(), mIndexBufferMemory);
    }
    
    /*
    {
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
    }
    */
    
    
    
    
    
    return true;
}

bool ShoRendererVk::initializePipeline() {

    Logger::Out iout = Logger::log(Logger::INFO);
    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    VkResult result;
    
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
    
    VkVertexInputBindingDescription bindDesc; {
        bindDesc.binding = 0;
        bindDesc.stride = (3 + 2) * sizeof(glm::f32);
        bindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }
    
    VkVertexInputAttributeDescription locAttribDesc; {
        locAttribDesc.binding = 0;
        locAttribDesc.location = 0;
        locAttribDesc.format = VK_FORMAT_R32G32_SFLOAT;
        locAttribDesc.offset = (0) * sizeof(glm::f32);
    }
    
    VkVertexInputAttributeDescription colorAttribDesc; {
        colorAttribDesc.binding = 0;
        colorAttribDesc.location = 1;
        colorAttribDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
        colorAttribDesc.offset = (0 + 2) * sizeof(glm::f32);
    }
    
    VkVertexInputAttributeDescription attribDescs[] = {
        locAttribDesc,
        colorAttribDesc
    };
    
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
        pvisCstrArgs.vertexBindingDescriptionCount = 1;
        pvisCstrArgs.pVertexBindingDescriptions = &bindDesc;
        pvisCstrArgs.vertexAttributeDescriptionCount = 2;
        pvisCstrArgs.pVertexAttributeDescriptions = attribDescs;
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
        //plCstrArgs.setLayoutCount = 1;
        //plCstrArgs.pSetLayouts = &mDescriptorSetLayout;
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
        gpCstrArgs.pTessellationState = nullptr;
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
    
    return true;
}

bool ShoRendererVk::populateCommandBuffers() {

    Logger::Out iout = Logger::log(Logger::INFO);
    Logger::Out vout = Logger::log(Logger::VERBOSE);
    Logger::Out sout = Logger::log(Logger::SEVERE);
    
    VkResult result;
    
    for(uint32_t i = 0; i < mCommandBuffers.size(); ++ i) {
        VkCommandBuffer cmdBuff = mCommandBuffers.at(i);
        VkCommandBufferBeginInfo cbbArgs; {
            cbbArgs.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cbbArgs.pNext = nullptr;
            cbbArgs.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            cbbArgs.pInheritanceInfo = nullptr;
        }
        
        result = vkBeginCommandBuffer(cmdBuff, &cbbArgs);
        
        VkRenderPassBeginInfo rpbArgs; {
            rpbArgs.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rpbArgs.pNext = nullptr;
            rpbArgs.renderPass = mRenderPass;
            rpbArgs.framebuffer = mSwapchainFramebuffers.at(i);
            rpbArgs.renderArea.offset = {0, 0};
            rpbArgs.renderArea.extent = Video::Vulkan::getSwapchainExtent();
            rpbArgs.clearValueCount = 0;
            rpbArgs.pClearValues = nullptr;
        }
        
        vkCmdBeginRenderPass(cmdBuff, &rpbArgs, VK_SUBPASS_CONTENTS_INLINE);
        
        vkCmdBindPipeline(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
        
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmdBuff, 0, 1, &mVertexBuffer, &offset);
        
        vkCmdBindIndexBuffer(cmdBuff, mIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
        
        vkCmdDrawIndexed(cmdBuff, 6, 1, 0, 0, 0);
        
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
    
    //vkDestroyDescriptorSetLayout(Video::Vulkan::getLogicalDevice(), mDescriptorSetLayout, nullptr);
    
    vkFreeMemory(Video::Vulkan::getLogicalDevice(), mVertexBufferMemory, nullptr);
    vkFreeMemory(Video::Vulkan::getLogicalDevice(), mIndexBufferMemory, nullptr);
    vkDestroyBuffer(Video::Vulkan::getLogicalDevice(), mVertexBuffer, nullptr);
    vkDestroyBuffer(Video::Vulkan::getLogicalDevice(), mIndexBuffer, nullptr);
    
    vkDestroyPipeline(Video::Vulkan::getLogicalDevice(), mPipeline, nullptr);
    vkDestroyPipelineLayout(Video::Vulkan::getLogicalDevice(), mPipelineLayout, nullptr);
    
    vkDestroySemaphore(Video::Vulkan::getLogicalDevice(), mSemImageAvailable, nullptr);
    vkDestroySemaphore(Video::Vulkan::getLogicalDevice(), mSemRenderFinished, nullptr);
    
    if(!mCommandBuffers.empty()) {
        vkFreeCommandBuffers(Video::Vulkan::getLogicalDevice(), mCommandPool, mCommandBuffers.size(), mCommandBuffers.data());
    }
    vkDestroyCommandPool(Video::Vulkan::getLogicalDevice(), mCommandPool, nullptr);
    
    for(VkFramebuffer fb : mSwapchainFramebuffers) {
        vkDestroyFramebuffer(Video::Vulkan::getLogicalDevice(), fb, nullptr);
    }
    
    vkDestroyRenderPass(Video::Vulkan::getLogicalDevice(), mRenderPass, nullptr);
    
    vkDestroyShaderModule(Video::Vulkan::getLogicalDevice(), mShaderVertModule, nullptr);
    vkDestroyShaderModule(Video::Vulkan::getLogicalDevice(), mShaderFragModule, nullptr);
    
    return true;
}

void ShoRendererVk::renderFrame() {
    
    mScenegraph->render(std::bind(&ShoRendererVk::modelimapOpaque, this, std::placeholders::_1));
    
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
void ShoRendererVk::rebuildPipeline() {
    cleanup();
    initialize();
}

}

#endif // PGG_VULKAN

