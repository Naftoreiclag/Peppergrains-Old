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

#include "DebugInfiniteCheckerboardModel.hpp"

#include <iostream>
#include <math.h>

#include "ResourceManager.hpp"

namespace pgg {

InfiniteCheckerboardModel::InfiniteCheckerboardModel() {
    mSize = 64;
    mCellSize = 1.f;
}

void InfiniteCheckerboardModel::load() {
    ResourceManager* resman = ResourceManager::getSingleton();
    GLfloat vertices[mSize * mSize * 4 * 9];
    
    mIndicesSize = mSize * mSize * 6;
    GLuint indices[mIndicesSize];
    
    // Generate model
    {
        // Generate vertices
        {
            uint32_t iVertex = 0;
            float originX = ((float) mSize) * -0.5f;
            float originZ = ((float) mSize) * -0.5f;
            originX *= mCellSize;
            originZ *= mCellSize;
            
            for(uint32_t z = 0; z < mSize; ++ z) {
                for(uint32_t x = 0; x < mSize; ++ x) {
                    Vec3 color;
                    
                    if(((x & 1) ^ (z & 1)) != 0) {
                        color = Vec3(0.5, 0.5, 0.5);
                    } else {
                        color = Vec3(1, 1, 1);
                    }
                    
                    for(uint32_t q = 0; q < 4; ++ q) {
                        
                        float lx = x + (q % 2);
                        float lz = z + (q > 1 ? 1 : 0);
                        lx *= mCellSize;
                        lz *= mCellSize;
                        vertices[iVertex ++] = originX + lx;
                        vertices[iVertex ++] = 0.f;
                        vertices[iVertex ++] = originZ + lz;
                        vertices[iVertex ++] = color.x;
                        vertices[iVertex ++] = color.y;
                        vertices[iVertex ++] = color.z;
                        vertices[iVertex ++] = 0.f;
                        vertices[iVertex ++] = 1.f;
                        vertices[iVertex ++] = 0.f;
                    }
                }
            }
        }
        
        // Generate indices
        {
            uint32_t iIndex = 0;
            for(uint32_t z = 0; z < mSize; ++ z) {
                for(uint32_t x = 0; x < mSize; ++ x) {
                    
                    /*
                     *   x >
                     * z    
                     * v   A       B
                     * 
                     * 
                     *     C       D
                     * 
                     */

                     
                     uint32_t iA = ((z * mSize) + x) * 4 + 0;
                     uint32_t iB = ((z * mSize) + x) * 4 + 1;
                     uint32_t iC = ((z * mSize) + x) * 4 + 2;
                     uint32_t iD = ((z * mSize) + x) * 4 + 3;
                     
                     indices[iIndex ++] = iA;
                     indices[iIndex ++] = iD;
                     indices[iIndex ++] = iB;
                     
                     indices[iIndex ++] = iA;
                     indices[iIndex ++] = iC;
                     indices[iIndex ++] = iD;
                }
            }
        }
    }
    
    mShaderProg = resman->findShaderProgram("MinimalCN.shaderProgram");
    mShaderProg->grab();

    glGenBuffers(1, &mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mIndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);

    glEnableVertexAttribArray(mShaderProg->getPosAttrib());
    glVertexAttribPointer(mShaderProg->getPosAttrib(), 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(mShaderProg->getColorAttrib());
    glVertexAttribPointer(mShaderProg->getColorAttrib(), 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(mShaderProg->getNormalAttrib());
    glVertexAttribPointer(mShaderProg->getNormalAttrib(), 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*) (6 * sizeof(GLfloat)));

    glBindVertexArray(0);
}

void InfiniteCheckerboardModel::unload() {
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);
    glDeleteVertexArrays(1, &mVertexArrayObject);
    
    mShaderProg->drop();
}

InfiniteCheckerboardModel::~InfiniteCheckerboardModel() {
}

void InfiniteCheckerboardModel::render(Renderable::Pass rendPass, const glm::mat4& unused) {

    if(rendPass.mType != Renderable::Pass::Type::GEOMETRY && rendPass.mType != Renderable::Pass::Type::SHADOW) {
        return;
    }
    
    glUseProgram(mShaderProg->getHandle());
    
    float offsetX = mFocus.x - fmod(fmod(mFocus.x, 2 * mCellSize) + (2 * mCellSize), 2 * mCellSize);
    float offsetZ = mFocus.z - fmod(fmod(mFocus.z, 2 * mCellSize) + (2 * mCellSize), 2 * mCellSize);

    glm::mat4 modelMat = glm::translate(glm::mat4(1.f), glm::vec3(offsetX, 0, offsetZ));

    mShaderProg->bindModelViewProjMatrices(modelMat, rendPass.mCamera.getViewMatrix(), rendPass.mCamera.getProjMatrix());

    glBindVertexArray(mVertexArrayObject);

    glDrawElements(GL_TRIANGLES, mIndicesSize, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}

void InfiniteCheckerboardModel::setFocus(Vec3 location) {
    mFocus = location;
}

}

