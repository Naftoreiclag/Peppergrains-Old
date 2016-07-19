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

#include "AxesModel.hpp"

#include "ResourceManager.hpp"

namespace pgg
{

AxesModel::AxesModel()
{
}

AxesModel::~AxesModel()
{
}

void AxesModel::load() {
    
    // 3 cubes * 8 vertices * 6 floats = 144
    // 3 cubes * 6 faces * 2 triangles * 3 indices = 108
    GLfloat vertices[3 * 8 * 6];
    GLuint indices[3 * 6 * 2 * 3];
    {
        float rad = 0.1f;
        float org = rad;
        float siz = 2.f;
        
        uint32_t vo = 0;
        uint32_t io = 0;
        uint32_t zo = 0;
        for(uint32_t cube = 0; cube < 3; ++ cube) {
            Cuboid cuboid;
            
            if(cube == 0) {
                cuboid.pos1 = glm::vec3(org, -rad, -rad);
                cuboid.pos2 = glm::vec3(siz, rad, rad);
            } else if(cube == 1) {
                cuboid.pos1 = glm::vec3(-rad, org, -rad);
                cuboid.pos2 = glm::vec3(rad, siz, rad);
            } else if(cube == 2) {
                cuboid.pos1 = glm::vec3(-rad, -rad, org);
                cuboid.pos2 = glm::vec3(rad, rad, siz);
            }
    
            float r = cube == 0 ? 1.f : 0.f;
            float g = cube == 1 ? 1.f : 0.f;
            float b = cube == 2 ? 1.f : 0.f;
            
            float rmod = 0.5f;
            float gmod = 0.5f;
            float bmod = 0.5f;
            
            indices[io ++] = zo + 0;
            indices[io ++] = zo + 2;
            indices[io ++] = zo + 1;
            indices[io ++] = zo + 2; // Back
            indices[io ++] = zo + 3;
            indices[io ++] = zo + 1;
            
            indices[io ++] = zo + 4;
            indices[io ++] = zo + 5;
            indices[io ++] = zo + 6;
            indices[io ++] = zo + 5; // Font
            indices[io ++] = zo + 7;
            indices[io ++] = zo + 6;
            
            indices[io ++] = zo + 6;
            indices[io ++] = zo + 7;
            indices[io ++] = zo + 2;
            indices[io ++] = zo + 7; // Top
            indices[io ++] = zo + 3;
            indices[io ++] = zo + 2;
                
            /*
             *            2----------3
             *     6----------7      |
             *     |      |   |      |
             * right      |   |      | left
             *     |      |   |      |
             *     |      0---|------1
             *     4----------5
             * 
             */
             
            indices[io ++] = zo + 1;
            indices[io ++] = zo + 5;
            indices[io ++] = zo + 0;
            indices[io ++] = zo + 5; // Bottom
            indices[io ++] = zo + 4;
            indices[io ++] = zo + 0;
            
            indices[io ++] = zo + 4;
            indices[io ++] = zo + 6;
            indices[io ++] = zo + 0;
            indices[io ++] = zo + 6; // Right
            indices[io ++] = zo + 2;
            indices[io ++] = zo + 0;
            
            indices[io ++] = zo + 5;
            indices[io ++] = zo + 1;
            indices[io ++] = zo + 7;
            indices[io ++] = zo + 1; // Left
            indices[io ++] = zo + 3;
            indices[io ++] = zo + 7;
            
            zo += 8;
            
            vertices[vo ++] = cuboid.pos1.x;
            vertices[vo ++] = cuboid.pos1.y;
            vertices[vo ++] = cuboid.pos1.z;
            vertices[vo ++] = r * rmod;
            vertices[vo ++] = g * gmod;
            vertices[vo ++] = b * bmod;
            
            vertices[vo ++] = cuboid.pos2.x;
            vertices[vo ++] = cuboid.pos1.y;
            vertices[vo ++] = cuboid.pos1.z;
            vertices[vo ++] = r;
            vertices[vo ++] = g * gmod;
            vertices[vo ++] = b * bmod;
            
            vertices[vo ++] = cuboid.pos1.x;
            vertices[vo ++] = cuboid.pos2.y;
            vertices[vo ++] = cuboid.pos1.z;
            vertices[vo ++] = r * rmod;
            vertices[vo ++] = g;
            vertices[vo ++] = b * bmod;
            
            vertices[vo ++] = cuboid.pos2.x;
            vertices[vo ++] = cuboid.pos2.y;
            vertices[vo ++] = cuboid.pos1.z;
            vertices[vo ++] = r;
            vertices[vo ++] = g;
            vertices[vo ++] = b * bmod;
            
            vertices[vo ++] = cuboid.pos1.x;
            vertices[vo ++] = cuboid.pos1.y;
            vertices[vo ++] = cuboid.pos2.z;
            vertices[vo ++] = r * rmod;
            vertices[vo ++] = g * gmod;
            vertices[vo ++] = b;
            
            vertices[vo ++] = cuboid.pos2.x;
            vertices[vo ++] = cuboid.pos1.y;
            vertices[vo ++] = cuboid.pos2.z;
            vertices[vo ++] = r;
            vertices[vo ++] = g * gmod;
            vertices[vo ++] = b;
            
            vertices[vo ++] = cuboid.pos1.x;
            vertices[vo ++] = cuboid.pos2.y;
            vertices[vo ++] = cuboid.pos2.z;
            vertices[vo ++] = r * rmod;
            vertices[vo ++] = g;
            vertices[vo ++] = b;
            
            vertices[vo ++] = cuboid.pos2.x;
            vertices[vo ++] = cuboid.pos2.y;
            vertices[vo ++] = cuboid.pos2.z;
            vertices[vo ++] = r;
            vertices[vo ++] = g;
            vertices[vo ++] = b;
            
        }
    }
    
    

    ResourceManager* resman = ResourceManager::getSingleton();

    mShaderProg = resman->findShaderProgram("SimpleVertexColor.shaderProgram");
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
    glEnableVertexAttribArray(mShaderProg->getColorAttrib());
    glVertexAttribPointer(mShaderProg->getColorAttrib(), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));

    glBindVertexArray(0);
}
void AxesModel::unload() {
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);

    mShaderProg->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);

    delete this;
}
void AxesModel::render(const Model::RenderPass& rendPass, const glm::mat4& modelMat) {

    if(rendPass.type != RenderPass::Type::GEOMETRY && rendPass.type != RenderPass::Type::SHADOW) {
        return;
    }
    
    glUseProgram(mShaderProg->getHandle());

    mShaderProg->bindModelViewProjMatrices(modelMat, rendPass.viewMat, rendPass.projMat);

    glBindVertexArray(mVertexArrayObject);

    glDrawElements(GL_TRIANGLES, 108, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}

}

