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

#include "TerrainModel.hpp"

#include <iostream>

#include "ResourceManager.hpp"

namespace pgg {

TerrainModel::TerrainModel()
{
    std::cout << "terrain" << std::endl;
    mMapSize = 512;
    std::cout << mMapSize << std::endl;
    mHorizSize = 16.f;
    std::cout << mHorizSize << std::endl;
    mVertSize = 8.f;
    std::cout << mVertSize << std::endl;
    mComponents = 6;
    std::cout << mComponents << std::endl;
}
TerrainModel::~TerrainModel() {}

void TerrainModel::load() {
    ResourceManager* resman = ResourceManager::getSingleton();

    
    
    GLfloat vertices[] = {
        -50.f, 0.f,  50.f,
         50.f, 0.f,  50.f,
        -50.f, 0.f, -50.f,
         50.f, 0.f, -50.f
    };
    GLuint indices[] = {
        0, 1, 2,
        1, 3, 2,
    };
    
    mIndicesSize = 6;


    mShaderProg = resman->findShaderProgram("Red.shaderProgram");
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
    glVertexAttribPointer(mShaderProg->getPosAttrib(), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

    glBindVertexArray(0);
}
void TerrainModel::unload() {
    glDeleteBuffers(1, &mIndexBufferObject);
    glDeleteBuffers(1, &mVertexBufferObject);

    mShaderProg->drop();

    glDeleteVertexArrays(1, &mVertexArrayObject);

    delete this;
}
void TerrainModel::render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::mat4& modelMat) {

    glUseProgram(mShaderProg->getHandle());

    if(mShaderProg->needsModelMatrix()) {
        glUniformMatrix4fv(mShaderProg->getModelMatrixUnif(), 1, GL_FALSE, glm::value_ptr(modelMat));
    }
    if(mShaderProg->needsViewMatrix()) {
        glUniformMatrix4fv(mShaderProg->getViewMatrixUnif(), 1, GL_FALSE, glm::value_ptr(viewMat));
    }
    if(mShaderProg->needsProjMatrix()) {
        glUniformMatrix4fv(mShaderProg->getProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(projMat));
    }

    glBindVertexArray(mVertexArrayObject);

    glDrawElements(GL_TRIANGLES, mIndicesSize, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glUseProgram(0);
}

}
