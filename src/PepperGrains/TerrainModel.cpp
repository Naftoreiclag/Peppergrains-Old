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

#include "TerrainModel.hpp"

#include <iostream>

#include "ResourceManager.hpp"

namespace pgg {

TerrainModel::TerrainModel()
{
    std::cout << "terrain" << std::endl;
    mMapSize = 32;
    std::cout << mMapSize << std::endl;
    mHorizSize = 2.f;
    std::cout << mHorizSize << std::endl;
    mVertSize = 10.f;
    std::cout << mVertSize << std::endl;
    mComponents = 6;
    std::cout << mComponents << std::endl;
}
TerrainModel::~TerrainModel() {}

void TerrainModel::load() {
    ResourceManager* resman = ResourceManager::getSingleton();
    
    GLfloat vertices[mMapSize * mMapSize * mComponents];
    
    mIndicesSize = (mMapSize - 1) * (mMapSize - 1) * 6;
    GLuint indices[mIndicesSize];
    
    {
        ImageResource* heightmap = resman->findImage("HeightmapWheat.image");
        heightmap->grab();
        
        uint32_t skip = heightmap->getNumComponents();
        uint32_t imgSize = 512;
        
        std::cout << "numcomp " << skip << std::endl;
        
        const uint8_t* image = heightmap->getImage();
        
        for(uint32_t z = 0; z < mMapSize; ++ z) {
            for(uint32_t x = 0; x < mMapSize; ++ x) {
                float oY = image[((z * imgSize) + x) * skip];
                float oX = x;
                float oZ = z;
                oY /= 255.f;
                
                oY *= mVertSize;
                oX *= mHorizSize;
                oZ *= mHorizSize;
                
                // Position
                vertices[((z * mMapSize) + x) * mComponents + 0] = oX;
                vertices[((z * mMapSize) + x) * mComponents + 1] = oY;
                vertices[((z * mMapSize) + x) * mComponents + 2] = oZ;
                
                {
                    uint32_t lx = x > 0 ? x - 1 : x;
                    uint32_t gx = x < (mMapSize - 1) ? x + 1 : x;
                    uint32_t lz = z > 0 ? z - 1 : z;
                    uint32_t gz = z < (mMapSize - 1) ? z + 1 : z;
                    float hA = image[((z * imgSize) + lx) * skip];
                    float hB = image[((z * imgSize) + gx) * skip];
                    float hC = image[((lz * imgSize) + x) * skip];
                    float hD = image[((gz * imgSize) + x) * skip];
                    
                    hA /= 255.f;
                    hB /= 255.f;
                    hC /= 255.f;
                    hD /= 255.f;
                    
                    float dx = hB - hA;
                    float dz = hD - hC;
                    
                    glm::vec3 asdf = glm::normalize(glm::vec3(dx * mVertSize, 2 * mHorizSize, dz * mVertSize));
                    
                    vertices[((z * mMapSize) + x) * mComponents + 3] = asdf.x;
                    vertices[((z * mMapSize) + x) * mComponents + 4] = asdf.y;
                    vertices[((z * mMapSize) + x) * mComponents + 5] = asdf.z;
                }
                
            }
        }
        
        // This stands for index index...
        uint32_t iIndex = 0;
        for(uint32_t z = 0; z < mMapSize - 1; ++ z) {
            for(uint32_t x = 0; x < mMapSize - 1; ++ x) {
                
                /*
                 *   x >
                 * z    
                 * v   A       B
                 * 
                 * 
                 *     C       D
                 * 
                 */

                 
                 uint32_t iA = (z + 0) * mMapSize + x + 0;
                 uint32_t iB = (z + 0) * mMapSize + x + 1;
                 uint32_t iC = (z + 1) * mMapSize + x + 0;
                 uint32_t iD = (z + 1) * mMapSize + x + 1;
                 
                 indices[iIndex ++] = iA;
                 indices[iIndex ++] = iD;
                 indices[iIndex ++] = iB;
                 
                 indices[iIndex ++] = iA;
                 indices[iIndex ++] = iC;
                 indices[iIndex ++] = iD;
            }
        }
        
        heightmap->drop();
    }
    
    std::cout << "sizeof(vertices) = " << sizeof(vertices) << std::endl;
    std::cout << "sizeof(indices) = " << sizeof(indices) << std::endl;
    


    mShaderProg = resman->findShaderProgram("GrassTerrain.shaderProgram");
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
    glVertexAttribPointer(mShaderProg->getPosAttrib(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(mShaderProg->getNormalAttrib());
    glVertexAttribPointer(mShaderProg->getNormalAttrib(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));

    glBindVertexArray(0);
}
void TerrainModel::unload() {
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);

    mShaderProg->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);

    delete this;
}
void TerrainModel::render(Renderable::Pass rendPass, const glm::mat4& modelMat) {

    if(rendPass.mType != Renderable::Pass::Type::GEOMETRY && rendPass.mType != Renderable::Pass::Type::SHADOW) {
        return;
    }
    
    glUseProgram(mShaderProg->getHandle());

    mShaderProg->bindModelViewProjMatrices(modelMat, rendPass.mCamera.getViewMatrix(), rendPass.mCamera.getProjMatrix());

    glBindVertexArray(mVertexArrayObject);

    glDrawElements(GL_TRIANGLES, mIndicesSize, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}

}
