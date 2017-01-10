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

#include "ShoRendererVulkan.hpp"

#ifdef PGG_VULKAN

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

ShoRenderer::ShoRenderer() { }
    
bool ShoRenderer::initialize() {

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
    
    VkRenderPassCreateInfo rpCstrArgs; {
        rpCstrArgs.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpCstrArgs.pNext = nullptr;
        rpCstrArgs.flags = 0;
        
        rpCstrArgs.attachmentCount = 1;
        rpCstrArgs.pAttachments = &colorAttachDesc;
        rpCstrArgs.subpassCount = 1;
        rpCstrArgs.pSubpasses = &subpassDesc;
        rpCstrArgs.dependencyCount = 0;
        rpCstrArgs.pDependencies = nullptr;
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
}

ShoRenderer::~ShoRenderer() {
}

bool ShoRenderer::cleanup() {
    
    vkDestroyPipeline(Video::Vulkan::getLogicalDevice(), mPipeline, nullptr);
    
    vkDestroyRenderPass(Video::Vulkan::getLogicalDevice(), mRenderPass, nullptr);
    
    vkDestroyPipelineLayout(Video::Vulkan::getLogicalDevice(), mPipelineLayout, nullptr);
    
    vkDestroyShaderModule(Video::Vulkan::getLogicalDevice(), mShaderVertModule, nullptr);
    vkDestroyShaderModule(Video::Vulkan::getLogicalDevice(), mShaderFragModule, nullptr);
    
    return true;
    
}

void ShoRenderer::resize(uint32_t width, uint32_t height) {
    
}
void ShoRenderer::renderFrame() {
    
}
}

#endif // PGG_VULKAN

