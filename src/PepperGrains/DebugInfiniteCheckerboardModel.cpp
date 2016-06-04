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
    mSize = 32;
}

void InfiniteCheckerboardModel::load() {
    GLfloat vertices[(mSize + 1) * (mSize + 1) * 9];
    
    mIndicesSize = mSize * mSize * 6;
    GLuint indices[mIndicesSize];
    
    // Generate model
    {
        // Generate vertices
        {
            uint32_t iVertex = 0;
            float originX = -mSize;
            float originZ = -mSize;
            originX /= 2.f;
            originZ /= 2.f;
            
            for(uint32_t x = 0; x < mSize + 1; ++ x) {
                for(uint32_t z = 0; z < mSize + 1; ++ z) {
                    vertices[iVertex ++] = originX + x;
                    vertices[iVertex ++] = 0.f;
                    vertices[iVertex ++] = originZ + z;
                    Vec3 color;
                    
                    if(((x & 1) ^ (z & 1)) != 0) {
                        color = Vec3(0, 0, 0);
                    } else {
                        color = Vec3(1, 1, 1);
                    }
                    vertices[iVertex ++] = color.x;
                    vertices[iVertex ++] = color.y;
                    vertices[iVertex ++] = color.z;
                    vertices[iVertex ++] = 0.f;
                    vertices[iVertex ++] = 1.f;
                    vertices[iVertex ++] = 0.f;
                }
            }
            std::cout << iVertex << std::endl;
        }
        
        // Generate indices
        {
            uint32_t iIndex = 0;
            
            for(uint32_t x = 0; x < mSize; ++ x) {
                for(uint32_t z = 0; z < mSize; ++ z) {
                    
                    /*
                     *   x >
                     * z    
                     * v   A       B
                     * 
                     * 
                     *     C       D
                     * 
                     */

                     
                     uint32_t iA = ((z + 0) * (mSize + 1)) + (x + 0);
                     uint32_t iB = ((z + 0) * (mSize + 1)) + (x + 1);
                     uint32_t iC = ((z + 1) * (mSize + 1)) + (x + 0);
                     uint32_t iD = ((z + 1) * (mSize + 1)) + (x + 1);
                     
                     indices[iIndex ++] = iA;
                     indices[iIndex ++] = iD;
                     indices[iIndex ++] = iB;
                     
                     indices[iIndex ++] = iA;
                     indices[iIndex ++] = iC;
                     indices[iIndex ++] = iD;
                }
            }
            std::cout << iIndex << std::endl;
        }
    }
    
    std::cout << "sizeof(vertices) = " << (sizeof(vertices) / 4) << std::endl;
    std::cout << "sizeof(indices) = " << (sizeof(indices) / 4) << std::endl;
    
    ResourceManager* resman = ResourceManager::getSingleton();
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

    std::cout << mShaderProg->isFallback() << std::endl;

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

void InfiniteCheckerboardModel::render(const Model::RenderPassConfiguration& rendPass, const glm::mat4& unused) {

    if(rendPass.type != RenderPassType::GEOMETRY && rendPass.type != RenderPassType::SHADOW) {
        return;
    }
    
    glUseProgram(mShaderProg->getHandle());
    
    float offsetX = fmod(fmod(mFocus.x, 2) + 2, 2);
    float offsetZ = fmod(fmod(mFocus.z, 2) + 2, 2);

    glm::mat4 modelMat = glm::translate(glm::mat4(1.f), glm::vec3(offsetX, 0, offsetZ));

    mShaderProg->bindModelViewProjMatrices(unused, rendPass.viewMat, rendPass.projMat);

    glBindVertexArray(mVertexArrayObject);

    glDrawElements(GL_TRIANGLES, mIndicesSize, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}

void InfiniteCheckerboardModel::setFocus(Vec3 location) {
    mFocus = location;
}

}

