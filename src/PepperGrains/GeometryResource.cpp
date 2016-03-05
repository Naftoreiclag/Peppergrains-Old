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

#include <OpenGLStuff.hpp>

#include "StreamStuff.hpp"

namespace pgg {

GeometryResource::GeometryResource()
: mLoaded(false)
, mIsErrorResource(false) {
}

GeometryResource::~GeometryResource() {
}

void GeometryResource::loadError() {
    assert(!mLoaded && "Attempted to load geometry that is already loaded");
    
    std::cout << "Error data" << std::endl;
    
    mUsePositions = true;
    mUseColor = true;
    mUseUV = true;
    mUseNormals = true;
    
    mNumVertices = 8;
    mNumTriangles = 12;
    
    // pppcccttnnn
    mPositionOff = 0;
    mColorOff = 3;
    mTextureOff = 6;
    mNormalOff = 8;
    mSizeVertices = 11;
    
    GLfloat vertices[mNumVertices * mSizeVertices];
    GLuint indices[mNumTriangles * 3];
    
    // Generate cube
    {
        /*                ^
         *                y
         * 
         *                2---------3
         *               /|        /|
         *              6---------7 |
         *              | |       | | 
         *              | 0-------|-1   x >
         *              |/        |/
         *              4---------5
         *             
         *            z
         *           L
         * 
         *  X+ face = left
         *  X- face = right
         *  Y+ face = top
         *  Y- face = bottom
         *  Z+ face = front
         *  Z- face = back
         * 
         */
        
        // Indices
        {
            uint32_t io = 0;
            
            indices[io ++] = 0;
            indices[io ++] = 2;
            indices[io ++] = 1;
            indices[io ++] = 2; // Back
            indices[io ++] = 3;
            indices[io ++] = 1;
            
            indices[io ++] = 4;
            indices[io ++] = 5;
            indices[io ++] = 6;
            indices[io ++] = 5; // Font
            indices[io ++] = 7;
            indices[io ++] = 6;
            
            indices[io ++] = 6;
            indices[io ++] = 7;
            indices[io ++] = 2;
            indices[io ++] = 7; // Top
            indices[io ++] = 3;
            indices[io ++] = 2;
             
            indices[io ++] = 1;
            indices[io ++] = 5;
            indices[io ++] = 0;
            indices[io ++] = 5; // Bottom
            indices[io ++] = 4;
            indices[io ++] = 0;
            
            indices[io ++] = 4;
            indices[io ++] = 6;
            indices[io ++] = 0;
            indices[io ++] = 6; // Right
            indices[io ++] = 2;
            indices[io ++] = 0;
            
            indices[io ++] = 5;
            indices[io ++] = 1;
            indices[io ++] = 7;
            indices[io ++] = 1; // Left
            indices[io ++] = 3;
            indices[io ++] = 7;
        }
        
        // Vertices
        {
            float size = 1.f;
            uint32_t vert = 0;
            float dylan = 0.57735f; // 1 / sqrt(3)
            for(uint32_t z = 0; z < 2; ++ z) {
                for(uint32_t y = 0; y < 2; ++ y) {
                    for(uint32_t x = 0; x < 2; ++ x) {
                        // Position
                        vertices[vert ++] = x == 0 ? -size : size;
                        vertices[vert ++] = y == 0 ? -size : size;
                        vertices[vert ++] = z == 0 ? -size : size;
                        
                        // Color
                        vertices[vert ++] = x == 0 ? -size : size;
                        vertices[vert ++] = y == 0 ? -size : size;
                        vertices[vert ++] = z == 0 ? -size : size;
                        
                        // Texture
                        vertices[vert ++] = x;
                        vertices[vert ++] = y;
                        
                        // Normal
                        vertices[vert ++] = x == 0 ? -dylan : dylan;
                        vertices[vert ++] = y == 0 ? -dylan : dylan;
                        vertices[vert ++] = z == 0 ? -dylan : dylan;
                    }
                }
            }
            
            std::cout << vert << std::endl;
        }
    }

    glGenBuffers(1, &mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    mLoaded = true;
    mIsErrorResource = true;
}
void GeometryResource::unloadError() {
    assert(mLoaded && "Attempted to unload geometry before loading it");
    
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);
    
    mLoaded = false;
    mIsErrorResource = false;
}
void GeometryResource::load() {
    assert(!mLoaded && "Attempted to load geometry that is already loaded");

    if(this->isFallback()) {
        loadError();
        return;
    }

    std::ifstream input(this->getFile().string().c_str(), std::ios::in | std::ios::binary);
    if(input.fail()) {
        loadError();
        return;
    }

    mUsePositions = readBool(input);
    mUseColor = readBool(input);
    mUseUV = readBool(input);
    mUseNormals = readBool(input);

    mNumVertices = readU32(input);
    mNumTriangles = readU32(input);

    mPositionOff = 0;
    mColorOff = mPositionOff + (mUsePositions ? 3 : 0);
    mTextureOff = mColorOff + (mUseColor ? 3 : 0);
    mNormalOff = mTextureOff + (mUseUV ? 2 : 0);
    mSizeVertices = mNormalOff + (mUseNormals ? 3 : 0);

    GLfloat vertices[mNumVertices * mSizeVertices];

    for(uint32_t i = 0; i < mNumVertices; ++ i) {
        if(mUsePositions) {
            vertices[(i * mSizeVertices) + mPositionOff + 0] = readF32(input);
            vertices[(i * mSizeVertices) + mPositionOff + 1] = readF32(input);
            vertices[(i * mSizeVertices) + mPositionOff + 2] = readF32(input);
        }
        if(mUseColor) {
            vertices[(i * mSizeVertices) + mColorOff + 0] = readF32(input);
            vertices[(i * mSizeVertices) + mColorOff + 1] = readF32(input);
            vertices[(i * mSizeVertices) + mColorOff + 2] = readF32(input);
        }
        if(mUseUV) {
            vertices[(i * mSizeVertices) + mTextureOff + 0] = readF32(input);
            vertices[(i * mSizeVertices) + mTextureOff + 1] = readF32(input);
        }
        if(mUseNormals) {
            vertices[(i * mSizeVertices) + mNormalOff + 0] = readF32(input);
            vertices[(i * mSizeVertices) + mNormalOff + 1] = readF32(input);
            vertices[(i * mSizeVertices) + mNormalOff + 2] = readF32(input);
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
    
    if(mIsErrorResource) {
        unloadError();
        return;
    }
    
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);

    mLoaded = false;
}

void GeometryResource::render() {
    glDrawElements(GL_TRIANGLES, mNumTriangles * 3, GL_UNSIGNED_INT, 0);
}

void GeometryResource::bindBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
}
void GeometryResource::enablePositionAttrib(GLuint posAttrib) {
    if(mUsePositions) {
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, mSizeVertices * sizeof(GLfloat), (void*) (mPositionOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableColorAttrib(GLuint colorAttrib) {
    if(mUseColor) {
        glEnableVertexAttribArray(colorAttrib);
        glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, mSizeVertices * sizeof(GLfloat), (void*) (mColorOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableUVAttrib(GLuint textureAttrib) {
    if(mUseUV) {
        glEnableVertexAttribArray(textureAttrib);
        glVertexAttribPointer(textureAttrib, 2, GL_FLOAT, GL_FALSE, mSizeVertices * sizeof(GLfloat), (void*) (mTextureOff * sizeof(GLfloat)));
    }
}
void GeometryResource::enableNormalAttrib(GLuint normalAttrib) {
    if(mUseNormals) {
        glEnableVertexAttribArray(normalAttrib);
        glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, mSizeVertices * sizeof(GLfloat), (void*) (mNormalOff * sizeof(GLfloat)));
    }
}

}
