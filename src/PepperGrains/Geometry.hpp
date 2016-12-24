/*
   Copyright 2016 James Fong

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

#include "OpenGLStuff.hpp"
#include "Resource.hpp"
#include "ReferenceCounted.hpp"

namespace pgg {

class Geometry : virtual public ReferenceCounted {
public:
    Geometry();
    virtual ~Geometry();
    
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
};

}

#endif // PGG_GEOMETRY_HPP
