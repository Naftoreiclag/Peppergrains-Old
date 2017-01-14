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

#include "GeometryResourceVulkan.hpp"

#include <cstring>
#include <cassert>
#include <fstream>
#include <iostream>

#include <GraphicsApiLibrary.hpp>

#include "StreamStuff.hpp"
#include "Logger.hpp"
#include "Video.hpp"
#include "VulkanUtils.hpp"

namespace pgg {

GeometryResourceVK::GeometryResourceVK()
: mLoaded(false)
, Resource(Resource::Type::GEOMETRY) {
}

GeometryResourceVK::~GeometryResourceVK() {
}

Geometry* GeometryResourceVK::gallop(Resource* resource) {
    if(!resource || resource->mResourceType != Resource::Type::GEOMETRY) {
        Logger::log(Logger::WARN) << "Failed to cast " << (resource ? resource->getName() : "nullptr") << " to geometry!" << std::endl;
        return Geometry::getFallback();
    } else {
        return static_cast<GeometryResourceVK*>(resource);
    }
}

void GeometryResourceVK::load() {
    assert(!mLoaded && "Attempted to load geometry that is already loaded");

    std::ifstream input(this->getFile().string().c_str(), std::ios::in | std::ios::binary);
    if(input.fail()) {
        //loadError();
        return;
    }
    
    uint8_t bitfield = readU8(input);
    //mHasVertices = bitfield & 0x01;
    //mHasTriangles = bitfield & 0x02;
    mHasArmature = bitfield & 0x04;
    mHasLightprobes = bitfield & 0x08;
        
    bitfield = readU8(input);
    mUsePosition = bitfield & 0x01;
    mUseColor = bitfield & 0x02;
    mUseUV = bitfield & 0x04;
    mUseNormal = bitfield & 0x08;
    mUseTangent = bitfield & 0x10;
    mUseBitangent = bitfield & 0x20;
    mUseBoneWeights = bitfield & 0x40;

    // Skinning technique
    readU8(input);

    mNumVertices = readU32(input);

    mPositionOff = 0;
    mColorOff = mPositionOff + (mUsePosition ? 3 : 0);
    mUVOff = mColorOff + (mUseColor ? 3 : 0);
    mNormalOff = mUVOff + (mUseUV ? 2 : 0);
    mTangentOff = mNormalOff + (mUseNormal ? 3 : 0);
    mBitangentOff = mTangentOff + (mUseTangent ? 3 : 0);
    mBoneWeightOff = mBitangentOff + (mUseBitangent ? 3 : 0);
    mFloatsPerVertex = mBoneWeightOff + (mUseBoneWeights ? 4 : 0);
    
    mBoneIndexOff = 0;
    mBytesPerVertex = mBoneIndexOff + (mUseBoneWeights ? 4 : 0);
    
    glm::f32* floatVertices = nullptr;
    if(mFloatsPerVertex > 0) floatVertices = new glm::f32[mNumVertices * mFloatsPerVertex];
    glm::u8* byteVertices = nullptr;
    if(mBytesPerVertex > 0) byteVertices = new glm::u8[mNumVertices * mBytesPerVertex];

    for(uint32_t i = 0; i < mNumVertices; ++ i) {
        if(mUsePosition) {
            floatVertices[(i * mFloatsPerVertex) + mPositionOff    ] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mPositionOff + 1] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mPositionOff + 2] = readF32(input);
        }
        if(mUseColor) {
            floatVertices[(i * mFloatsPerVertex) + mColorOff    ] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mColorOff + 1] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mColorOff + 2] = readF32(input);
        }
        if(mUseUV) {
            floatVertices[(i * mFloatsPerVertex) + mUVOff    ] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mUVOff + 1] = readF32(input);
        }
        if(mUseNormal) {
            floatVertices[(i * mFloatsPerVertex) + mNormalOff    ] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mNormalOff + 1] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mNormalOff + 2] = readF32(input);
        }
        if(mUseTangent) {
            floatVertices[(i * mFloatsPerVertex) + mTangentOff    ] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mTangentOff + 1] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mTangentOff + 2] = readF32(input);
        }
        if(mUseBitangent) {
            floatVertices[(i * mFloatsPerVertex) + mBitangentOff    ] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mBitangentOff + 1] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mBitangentOff + 2] = readF32(input);
        }
        if(mUseBoneWeights) {
            byteVertices[(i * mBytesPerVertex) + mBoneIndexOff    ] = readU8(input);
            byteVertices[(i * mBytesPerVertex) + mBoneIndexOff + 1] = readU8(input);
            byteVertices[(i * mBytesPerVertex) + mBoneIndexOff + 2] = readU8(input);
            byteVertices[(i * mBytesPerVertex) + mBoneIndexOff + 3] = readU8(input);
            floatVertices[(i * mFloatsPerVertex) + mBoneWeightOff    ] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mBoneWeightOff + 1] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mBoneWeightOff + 2] = readF32(input);
            floatVertices[(i * mFloatsPerVertex) + mBoneWeightOff + 3] = readF32(input);
        }
    }
    
    mNumTriangles = readU32(input);
    
    if(mNumTriangles == 0) {
        //loadError();
        return;
    }

    glm::u8* indices8 = nullptr;
    glm::u16* indices16 = nullptr;
    glm::u32* indices32 = nullptr;
    if(mNumVertices <= 1 << 8) {
        mIndexTypeSize = sizeof(glm::u8);
        indices8 = new glm::u8[mNumTriangles * 3];
        for(uint32_t i = 0; i < mNumTriangles; ++ i) {
            indices8[(i * 3)    ] = readU8(input);
            indices8[(i * 3) + 1] = readU8(input);
            indices8[(i * 3) + 2] = readU8(input);
        }
    }
    else if(mNumVertices <= 1 << 16) {
        mIndexTypeSize = sizeof(glm::u16);
        indices16 = new glm::u16[mNumTriangles * 3];
        for(uint32_t i = 0; i < mNumTriangles; ++ i) {
            indices16[(i * 3)    ] = readU16(input);
            indices16[(i * 3) + 1] = readU16(input);
            indices16[(i * 3) + 2] = readU16(input);
        }
    }
    else {
        mIndexTypeSize = sizeof(glm::u32);
        indices32 = new glm::u32[mNumTriangles * 3];
        for(uint32_t i = 0; i < mNumTriangles; ++ i) {
            indices32[(i * 3)    ] = readU32(input);
            indices32[(i * 3) + 1] = readU32(input);
            indices32[(i * 3) + 2] = readU32(input);
        }
    }
    
    if(mHasArmature) {
        uint16_t numBones = readU8(input);
        ++ numBones;
        mArmature.mBones.reserve(numBones);
        for(uint16_t i = 0; i < numBones; ++ i) {
            mArmature.mBones.push_back(Geometry::Armature::Bone());
            Geometry::Armature::Bone& bone = mArmature.mBones.back();
            
            bone.mName = readString(input);
            bone.mHasParent = readBool(input);
            if(bone.mHasParent) bone.mParent = readU8(input);
            
            uint8_t numChildren = readU8(input);
            bone.mChildren.reserve(numChildren);
            for(uint8_t j = 0; j < numChildren; ++ j) bone.mChildren.push_back(readU8(input));
        }
    }
    
    if(mHasLightprobes) {
        // Skinning technique
        readU8(input);
        
        uint16_t numProbes = readU8(input);
        ++ numProbes;
        
        for(uint16_t i = 0; i < numProbes; ++ i) {
            readF32(input); // x
            readF32(input); // y
            readF32(input); // z
            
            bool hasWeights = readBool(input);
            
            if(hasWeights) {
                readU8(input);
                readU8(input);
                readU8(input);
                readU8(input);
                readF32(input);
                readF32(input);
                readF32(input);
                readF32(input);
            }
        }
    }

    input.close();
    
    uint32_t sizeOfFloatVertexArray = mNumVertices * mFloatsPerVertex * sizeof(glm::f32);
    uint32_t sizeOfByteVertexArray = mNumVertices * mBytesPerVertex * sizeof(glm::u8);
    uint32_t sizeOfIndexArray = mNumTriangles * 3 * mIndexTypeSize;
    
    // TODO: upload byte vertex array also
    
    VulkanUtils::makeBufferAndAllocateMemory(sizeOfFloatVertexArray + sizeOfIndexArray, 
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        &mVertexIndexBuffer, &mVertexIndexBufferMemory);
    {
        void* memAddr;
        
        vkMapMemory(Video::Vulkan::getLogicalDevice(), mVertexIndexBufferMemory, 0, sizeOfFloatVertexArray, 0, &memAddr);
        std::memcpy(memAddr, floatVertices, sizeOfFloatVertexArray);
        vkUnmapMemory(Video::Vulkan::getLogicalDevice(), mVertexIndexBufferMemory);
        
        vkMapMemory(Video::Vulkan::getLogicalDevice(), mVertexIndexBufferMemory, sizeOfFloatVertexArray, sizeOfIndexArray, 0, &memAddr);
        if(mIndexTypeSize == sizeof(glm::u8)) {
            std::memcpy(memAddr, indices8, sizeOfIndexArray);
        } else if(mIndexTypeSize == sizeof(glm::u16)) {
            std::memcpy(memAddr, indices16, sizeOfIndexArray);
        } else if(mIndexTypeSize == sizeof(glm::u32)) {
            std::memcpy(memAddr, indices32, sizeOfIndexArray);
        }
        vkUnmapMemory(Video::Vulkan::getLogicalDevice(), mVertexIndexBufferMemory);
    }
    
    // TODO: make sure memory is cleaned up even in the event of error
    if(floatVertices) delete[] floatVertices;
    if(byteVertices) delete[] byteVertices;
    if(indices8) delete[] indices8;
    if(indices16) delete[] indices16;
    if(indices32) delete[] indices32;
    
    mLoaded = true;
}

void GeometryResourceVK::unload() {
    assert(mLoaded && "Attempted to unload geometry before loading it");
    
    vkFreeMemory(Video::Vulkan::getLogicalDevice(), mVertexIndexBufferMemory, nullptr);
    vkDestroyBuffer(Video::Vulkan::getLogicalDevice(), mVertexIndexBuffer, nullptr);
    
    mLoaded = false;
}

}

#endif // PGG_VULKAN
