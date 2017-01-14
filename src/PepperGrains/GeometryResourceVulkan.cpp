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

#include <cassert>
#include <fstream>
#include <iostream>

#include <GraphicsApiLibrary.hpp>

#include "StreamStuff.hpp"
#include "Logger.hpp"

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
        return getFallback();
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
    bool mHasVertices = bitfield & 0x01;
    bool mHasTriangles = bitfield & 0x02;
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
    mFloatVertexSize = mBoneWeightOff + (mUseBoneWeights ? 4 : 0);
    
    mBoneIndexOff = 0;
    mByteVertexSize = mBoneIndexOff + (mUseBoneWeights ? 4 : 0);

    
    GLfloat floatVertices[mNumVertices * mFloatVertexSize];

    for(uint32_t i = 0; i < mNumVertices; ++ i) {
        if(mUsePosition) {
            floatVertices[(i * mFloatVertexSize) + mPositionOff    ] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mPositionOff + 1] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mPositionOff + 2] = readF32(input);
        }
        if(mUseColor) {
            floatVertices[(i * mFloatVertexSize) + mColorOff    ] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mColorOff + 1] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mColorOff + 2] = readF32(input);
        }
        if(mUseUV) {
            floatVertices[(i * mFloatVertexSize) + mUVOff    ] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mUVOff + 1] = readF32(input);
        }
        if(mUseNormal) {
            floatVertices[(i * mFloatVertexSize) + mNormalOff    ] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mNormalOff + 1] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mNormalOff + 2] = readF32(input);
        }
        if(mUseTangent) {
            floatVertices[(i * mFloatVertexSize) + mTangentOff    ] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mTangentOff + 1] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mTangentOff + 2] = readF32(input);
        }
        if(mUseBitangent) {
            floatVertices[(i * mFloatVertexSize) + mBitangentOff    ] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mBitangentOff + 1] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mBitangentOff + 2] = readF32(input);
        }
        if(mUseBoneWeights) {
            /*
            byteVertices[(i * mByteVertexSize) + mBoneIndexOff    ] = readU8(input);
            byteVertices[(i * mByteVertexSize) + mBoneIndexOff + 1] = readU8(input);
            byteVertices[(i * mByteVertexSize) + mBoneIndexOff + 2] = readU8(input);
            byteVertices[(i * mByteVertexSize) + mBoneIndexOff + 3] = readU8(input);
            */
            floatVertices[(i * mFloatVertexSize) + mBoneWeightOff    ] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mBoneWeightOff + 1] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mBoneWeightOff + 2] = readF32(input);
            floatVertices[(i * mFloatVertexSize) + mBoneWeightOff + 3] = readF32(input);
        }
    }
    
    mNumTriangles = readU32(input);

    GLuint indices[mNumTriangles * 3];
    if(mNumVertices <= 1 << 8) {
        for(uint32_t i = 0; i < mNumTriangles; ++ i) {
            indices[(i * 3)    ] = readU8(input);
            indices[(i * 3) + 1] = readU8(input);
            indices[(i * 3) + 2] = readU8(input);
        }
    }
    else if(mNumVertices <= 1 << 16) {
        for(uint32_t i = 0; i < mNumTriangles; ++ i) {
            indices[(i * 3)    ] = readU16(input);
            indices[(i * 3) + 1] = readU16(input);
            indices[(i * 3) + 2] = readU16(input);
        }
    }
    else {
        for(uint32_t i = 0; i < mNumTriangles; ++ i) {
            indices[(i * 3)    ] = readU32(input);
            indices[(i * 3) + 1] = readU32(input);
            indices[(i * 3) + 2] = readU32(input);
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
    
    mLoaded = true;
}

void GeometryResourceVK::unload() {
    assert(mLoaded && "Attempted to unload geometry before loading it");
    
    mLoaded = false;
}

}

#endif // PGG_VULKAN
