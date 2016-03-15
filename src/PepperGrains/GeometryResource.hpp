/*
   Copyright 2015-2016 James Fong
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

#include <OpenGLStuff.hpp>

#include "Resource.hpp"

namespace pgg {

class GeometryResource : public Resource {
private:
    struct Vertex {
        // Position
        float x;
        float y;
        float z;

        // Color
        float r;
        float g;
        float b;
        float a;

        // Texture
        float u;
        float v;

        // Normal
        float nx;
        float ny;
        float nz;
        
        // Tangent
        float tx;
        float ty;
        float tz;
        float btx;
        float bty;
        float btz;

        Vertex()
        : x(0.f)
        , y(0.f)
        , z(0.f)
        , r(1.f)
        , g(1.f)
        , b(1.f)
        , a(1.f)
        , u(0.f)
        , v(0.f)
        , nx(1.f)
        , ny(0.f)
        , nz(0.f) {}
    };

    struct Triangle {
        uint32_t a;
        uint32_t b;
        uint32_t c;
    };

    typedef std::vector<Vertex> VertexBuffer;
    typedef std::vector<Triangle> TriangleBuffer;

    bool mUsePosition;
    bool mUseColor;
    bool mUseUV;
    bool mUseNormal;
    bool mUseTangent;
    bool mUseBitangent;

    uint32_t mPositionOff;
    uint32_t mColorOff;
    uint32_t mUVOff;
    uint32_t mNormalOff;
    uint32_t mTangentOff;
    uint32_t mBitangentOff;
    
    uint32_t mVertexSize;

    uint32_t mNumVertices;
    uint32_t mNumTriangles;

    GLuint mVertexBufferObject;
    GLuint mIndexBufferObject;

    bool mLoaded;
    
    void loadError();
    void unloadError();
    bool mIsErrorResource;
public:
    GeometryResource();
    virtual ~GeometryResource();

    void load();
    void unload();

    void drawElements() const;
    void drawElementsInstanced(const uint32_t& num) const;

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
    //void enableTangentAttrib(GLuint tangentAttrib, GLuint bitangentAttrib);

    const GLuint& getVertexBufferObjectHandle() const;
    const GLuint& getIndexBufferObjectHandle() const;
};

}

#endif // GEOMETRYRESOURCE_HPP
