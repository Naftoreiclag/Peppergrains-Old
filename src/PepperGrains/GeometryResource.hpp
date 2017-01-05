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

#ifndef PGG_GEOMETRYRESOURCE_HPP
#define PGG_GEOMETRYRESOURCE_HPP

#include <stdint.h>

#include "GraphicsApiStuff.hpp"
#include "Geometry.hpp"
#include "Resource.hpp"

namespace pgg {

class GeometryResource : public Geometry, public Resource {
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

    // Offsets in float array
    uint32_t mPositionOff;
    uint32_t mColorOff;
    uint32_t mUVOff;
    uint32_t mNormalOff;
    uint32_t mTangentOff;
    uint32_t mBitangentOff;
    uint32_t mBoneWeightOff;
    uint32_t mFloatVertexSize;
    
    // Offsets in byte array
    uint32_t mBoneIndexOff;
    uint32_t mByteVertexSize;

    uint32_t mNumVertices;
    uint32_t mNumTriangles;
    
    Geometry::Armature mArmature;
    std::vector<Geometry::Lightprobe> mLightprobes;

    GLuint mFloatVertexBufferObject;
    GLuint mByteVertexBufferObject;
    GLuint mIndexBufferObject;

    bool mLoaded;
    
public:
    GeometryResource();
    virtual ~GeometryResource();
    
    static Geometry* gallop(Resource* resource);

    void load();
    void unload();

    void drawElements() const;
    void drawElementsInstanced(uint32_t num) const;

    // Bind vertex and index buffers to the underlying vertex array object
    void bindBuffers();

    // These methods are used during vertex array object intialization
    // They tell OpenGL how to read attribute data from the buffers
    // If the geometry lacks a specific attribute, these methods will skip
    void enablePositionAttrib(GLuint posAttrib);
    void enableColorAttrib(GLuint colorAttrib);
    void enableUVAttrib(GLuint textureAttrib);
    void enableNormalAttrib(GLuint normalAttrib);
    void enableTangentAttrib(GLuint tangentAttrib);
    void enableBitangentAttrib(GLuint bitangentAttrib);
    void enableBoneAttrib(GLuint boneWeightAttrib, GLuint boneIndexAttrib);

    GLuint getFloatVertexBufferObjectHandle() const;
    GLuint getByteVertexBufferObjectHandle() const;
    GLuint getIndexBufferObjectHandle() const;
    
    bool usesByteVBO() const;
};

}

#endif // GEOMETRYRESOURCE_HPP
