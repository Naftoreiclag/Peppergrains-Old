/*
   Copyright 2016-2017 James Fong

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

#ifndef PGG_GEOMETRY_HPP
#define PGG_GEOMETRY_HPP

#include <vector>

#include <GraphicsApiLibrary.hpp>

#include "ReferenceCounted.hpp"
#include "Resource.hpp"
#include "Vec3.hpp"

namespace pgg {

/* A purely mathematical description of the properties of a 3D object related to rendering, including:
 *      Surface data:
 *          Vertex groups:
 *              Per-vertex position, color, uv, normal, tangent, bitangent, bone weights, light receiver weights
 *          Flex data 
 *      Armature: Bone heiarchy, intial transform matrices
 *      Light probe: per-probe position and bone weights
 */

class Geometry : virtual public ReferenceCounted {
public:
    struct Lightprobe {
        Vec3 mLocation;
        uint8_t mBone;
    };
    struct Armature {
        struct Bone {
            std::string mName;
            glm::mat4 mLocalTransform;
            glm::mat4 mTransform;
            
            bool mHasParent = false;
            uint8_t mParent;
            std::vector<uint8_t> mChildren;
        };
        std::vector<Bone> mBones;
    };
public:
    static Geometry* getFallback();
    
    virtual bool hasLightprobes() const;
    virtual const std::vector<Lightprobe>& getLightprobes() const;
    virtual bool hasArmature() const;
    virtual const Armature& getArmature() const;

    #ifdef PGG_OPENGL
    virtual void drawElements() const = 0;
    virtual void drawElementsInstanced(uint32_t num) const = 0;

    // Bind vertex and index buffers to the underlying vertex array object
    virtual void bindBuffers() = 0;

    // These methods are used during vertex array object intialization
    // They tell OpenGL how to read attribute data from the buffers
    // If the geometry lacks a specific attribute, these methods will skip
    virtual void enablePositionAttrib(GLuint posAttrib);
    virtual void enableColorAttrib(GLuint colorAttrib);
    virtual void enableUVAttrib(GLuint textureAttrib);
    virtual void enableNormalAttrib(GLuint normalAttrib);
    virtual void enableTangentAttrib(GLuint tangentAttrib);
    virtual void enableBitangentAttrib(GLuint bitangentAttrib);
    virtual void enableBoneAttrib(GLuint boneWeightAttrib, GLuint boneIndexAttrib);
    #endif
};

}

#endif // PGG_GEOMETRY_HPP
