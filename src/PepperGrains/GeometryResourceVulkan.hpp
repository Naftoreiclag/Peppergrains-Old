/*
   Copyright 2015-2017 James Fong

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

#ifndef PGG_GEOMETRYRESOURCE_VULKAN_HPP
#define PGG_GEOMETRYRESOURCE_VULKAN_HPP

#include <stdint.h>

#include <GraphicsApiLibrary.hpp>

#include "Geometry.hpp"
#include "Resource.hpp"

namespace pgg {

class GeometryResourceVK : public Geometry, public Resource {
private:
    bool mHasArmature;
    bool mHasLightprobes;
    
    bool mUsePosition;
    bool mUseColor;
    bool mUseUV;
    bool mUseNormal;
    bool mUseTangent;
    bool mUseBitangent;
    bool mUseBoneWeights; // Also for bone indices
    
    
    uint32_t mSizeOfByteVertexArray;
    uint32_t mSizeOfFloatVertexArray;
    uint32_t mSizeOfIndexArray;
    
    // Either sizeof(glm::u16) or sizeof(glm::u32)
    uint8_t mIndexTypeSize;

    // Offsets in float array
    uint32_t mPositionOff;
    uint32_t mColorOff;
    uint32_t mUVOff;
    uint32_t mNormalOff;
    uint32_t mTangentOff;
    uint32_t mBitangentOff;
    uint32_t mBoneWeightOff;
    uint32_t mFloatsPerVertex;
    
    // Offsets in byte array
    uint32_t mBoneIndexOff;
    uint32_t mBytesPerVertex;

    uint32_t mNumVertices;
    uint32_t mNumTriangles;
    
    VkBuffer mVertexIndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mVertexIndexBufferMemory = VK_NULL_HANDLE;
    
    // The data of these vectors are referenced during pipeline creation
    std::vector<VkVertexInputBindingDescription> mVertexInputBindingDescs;
    std::vector<VkVertexInputAttributeDescription> mVertexInputAttributeDescs;
    
    VkPipelineVertexInputStateCreateInfo mVertexInputState;
    VkPipelineInputAssemblyStateCreateInfo mInputAssemblyState;
    
    Geometry::Armature mArmature;
    std::vector<Geometry::Lightprobe> mLightprobes;

    bool mLoaded;
    
public:
    GeometryResourceVK();
    virtual ~GeometryResourceVK();
    
    static Geometry* gallop(Resource* resource);

    void load();
    void unload();
    
    const VkPipelineVertexInputStateCreateInfo* getVertexInputState();
    const VkPipelineInputAssemblyStateCreateInfo* getInputAssemblyState();
    
    void cmdBindBuffers(VkCommandBuffer cmdBuff);
    void cmdDrawIndexed(VkCommandBuffer cmdBuff);
};
typedef GeometryResourceVK GeometryResource;

}

#endif // PGG_GEOMETRYRESOURCE_VULKAN_HPP


#endif // PGG_VULKAN
