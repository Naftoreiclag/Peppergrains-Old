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

#include "OpenGLStuff.hpp"
#include "ReferenceCounted.hpp"
#include "Resource.hpp"
#include "Vec3.hpp"

#define PGG_BONE_NONE 0xFF

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
            
            uint8_t mParent; // PGG_BONE_NONE for no parent. (There can be multiple roots in one armature)
            std::vector<uint8_t> mChildren;
        };
        std::vector<Bone> mBones;
    };
public:
    static Geometry* getFallback();

    virtual void drawElements() const = 0;
    virtual void drawElementsInstanced(uint32_t num) const = 0;

    // Bind vertex and index buffers to the underlying vertex array object
    virtual void bindBuffers() = 0;

    // These methods are used during vertex array object intialization
    // They tell OpenGL how to read attribute data from the buffers
    // If the geometry lacks a specific attribute, these methods will skip
    virtual void enablePositionAttrib(GLuint posAttrib) = 0;
    virtual void enableColorAttrib(GLuint colorAttrib) = 0;
    virtual void enableUVAttrib(GLuint textureAttrib) = 0;
    virtual void enableNormalAttrib(GLuint normalAttrib) = 0;
    virtual void enableTangentAttrib(GLuint tangentAttrib) = 0;
    virtual void enableBitangentAttrib(GLuint bitangentAttrib) = 0;

    virtual const GLuint& getVertexBufferObjectHandle() const = 0;
    virtual const GLuint& getIndexBufferObjectHandle() const = 0;
    
    virtual const std::vector<Lightprobe>& getLightprobes() const;
    virtual const Armature& getArmature() const;
};

}

#endif // PGG_GEOMETRY_HPP
