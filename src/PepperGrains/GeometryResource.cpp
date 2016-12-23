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

#include "GeometryResource.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

#include "OpenGLStuff.hpp"
#include "StreamStuff.hpp"
#include "Logger.hpp"

namespace pgg {

GeometryResource::GeometryResource()
: mLoaded(false)
, Resource(Resource::Type::GEOMETRY) {
}

GeometryResource::~GeometryResource() {
}

Geometry* GeometryResource::gallop(Resource* resource) {
    if(!resource || resource->mResourceType != Resource::Type::GEOMETRY) {
        Logger::log(Logger::WARN) << "Failed to cast " << (resource ? resource->getName() : "nullptr") << " to geometry!" << std::endl;
        return getFallback();
    } else {
        return static_cast<GeometryResource*>(resource);
    }
}

void GeometryResource::load() {
    assert(!mLoaded && "Attempted to load geometry that is already loaded");

    if(this->isFallback()) {
        //loadError();
        return;
    }

    std::ifstream input(this->getFile().string().c_str(), std::ios::in | std::ios::binary);
    if(input.fail()) {
        //loadError();
        return;
    }

    mUsePosition = readBool(input);
    mUseColor = readBool(input);
    mUseUV = readBool(input);
    mUseNormal = readBool(input);
    mUseTangent = readBool(input);
    mUseBitangent = readBool(input);

    mNumVertices = readU32(input);
    mNumTriangles = readU32(input);

    mPositionOff = 0;
    mColorOff = mPositionOff + (mUsePosition ? 3 : 0);
    mUVOff = mColorOff + (mUseColor ? 3 : 0);
    mNormalOff = mUVOff + (mUseUV ? 2 : 0);
    mTangentOff = mNormalOff + (mUseNormal ? 3 : 0);
    mBitangentOff = mTangentOff + (mUseTangent ? 3 : 0);
    
    mVertexSize = mBitangentOff + (mUseBitangent ? 3 : 0);

    GLfloat vertices[mNumVertices * mVertexSize];

    for(uint32_t i = 0; i < mNumVertices; ++ i) {
        if(mUsePosition) {
            vertices[(i * mVertexSize) + mPositionOff + 0] = readF32(input);
            vertices[(i * mVertexSize) + mPositionOff + 1] = readF32(input);
            vertices[(i * mVertexSize) + mPositionOff + 2] = readF32(input);
        }
        if(mUseColor) {
            vertices[(i * mVertexSize) + mColorOff + 0] = readF32(input);
            vertices[(i * mVertexSize) + mColorOff + 1] = readF32(input);
            vertices[(i * mVertexSize) + mColorOff + 2] = readF32(input);
        }
        if(mUseUV) {
            vertices[(i * mVertexSize) + mUVOff + 0] = readF32(input);
            vertices[(i * mVertexSize) + mUVOff + 1] = readF32(input);
        }
        if(mUseNormal) {
            vertices[(i * mVertexSize) + mNormalOff + 0] = readF32(input);
            vertices[(i * mVertexSize) + mNormalOff + 1] = readF32(input);
            vertices[(i * mVertexSize) + mNormalOff + 2] = readF32(input);
        }
        if(mUseTangent) {
            vertices[(i * mVertexSize) + mTangentOff + 0] = readF32(input);
            vertices[(i * mVertexSize) + mTangentOff + 1] = readF32(input);
            vertices[(i * mVertexSize) + mTangentOff + 2] = readF32(input);
        }
        if(mUseBitangent) {
            vertices[(i * mVertexSize) + mBitangentOff + 0] = readF32(input);
            vertices[(i * mVertexSize) + mBitangentOff + 1] = readF32(input);
            vertices[(i * mVertexSize) + mBitangentOff + 2] = readF32(input);
        }
    }

    GLuint indices[mNumTriangles * 3];
    for(uint32_t i = 0; i < mNumTriangles; ++ i) {
        indices[(i * 3) + 0] = readU32(input);
        indices[(i * 3) + 1] = readU32(input);
        indices[(i * 3) + 2] = readU32(input);
    }

    input.close();

    glGenBuffers(1, &mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    mLoaded = true;
}

void GeometryResource::unload() {
    assert(mLoaded && "Attempted to unload geometry before loading it");
    
    /*
    if(mIsErrorResource) {
        unloadError();
        return;
    }
    */
    
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);

    mLoaded = false;
}

void GeometryResource::drawElements() const {
    glDrawElements(GL_TRIANGLES, mNumTriangles * 3, GL_UNSIGNED_INT, 0);
}
void GeometryResource::drawElementsInstanced(const uint32_t& num) const {
    glDrawElementsInstanced(GL_TRIANGLES, mNumTriangles * 3, GL_UNSIGNED_INT, 0, num);
}

void GeometryResource::bindBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
}
void GeometryResource::enablePositionAttrib(GLuint posAttrib) {
    if(mUsePosition) {
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, mVertexSize * sizeof(GLfloat), (void*) (mPositionOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableColorAttrib(GLuint colorAttrib) {
    if(mUseColor) {
        glEnableVertexAttribArray(colorAttrib);
        glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, mVertexSize * sizeof(GLfloat), (void*) (mColorOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableUVAttrib(GLuint textureAttrib) {
    if(mUseUV) {
        glEnableVertexAttribArray(textureAttrib);
        glVertexAttribPointer(textureAttrib, 2, GL_FLOAT, GL_FALSE, mVertexSize * sizeof(GLfloat), (void*) (mUVOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableNormalAttrib(GLuint normalAttrib) {
    if(mUseNormal) {
        glEnableVertexAttribArray(normalAttrib);
        glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, mVertexSize * sizeof(GLfloat), (void*) (mNormalOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableTangentAttrib(GLuint tangentAttrib) {
    if(mUseTangent) {
        glEnableVertexAttribArray(tangentAttrib);
        glVertexAttribPointer(tangentAttrib, 3, GL_FLOAT, GL_FALSE, mVertexSize * sizeof(GLfloat), (void*) (mTangentOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableBitangentAttrib(GLuint bitangentAttrib) {
    if(mUseBitangent) {
        glEnableVertexAttribArray(bitangentAttrib);
        glVertexAttribPointer(bitangentAttrib, 3, GL_FLOAT, GL_FALSE, mVertexSize * sizeof(GLfloat), (void*) (mBitangentOff * sizeof(GLfloat)));
    }
}

const GLuint& GeometryResource::getVertexBufferObjectHandle() const { return mVertexBufferObject; }
const GLuint& GeometryResource::getIndexBufferObjectHandle() const { return mIndexBufferObject; }

}
